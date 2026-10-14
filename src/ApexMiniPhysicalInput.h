#pragma once
// Linux-only opt-in physical key-down reader for the wired Apex Pro Mini.
// Never uses EVIOCGRAB, never stores key text or uploads events.
#ifdef __linux__
#include "apex_pro_mini_protocol.h"
#include "apex_pro_mini_evdev_codes.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <functional>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

namespace apex_mini
{
inline bool is_wired_apex_event(const QString& name)
{
    if(!name.startsWith(QStringLiteral("event"))) return false;
    bool valid_number = false;
    name.mid(5).toUInt(&valid_number);
    if(!valid_number) return false;
    QFileInfo sys_info(QStringLiteral("/sys/class/input/") + name);
    QString parent = sys_info.canonicalFilePath();
    // Follow sysfs ancestors to real USB vendor/product, never USB HID label alone.
    bool matching_usb = false;
    while(!parent.isEmpty() && parent != QStringLiteral("/"))
    {
        QFile vendor(parent + QStringLiteral("/idVendor"));
        QFile product(parent + QStringLiteral("/idProduct"));
        if(vendor.open(QIODevice::ReadOnly) && product.open(QIODevice::ReadOnly))
        {
            if(vendor.readAll().trimmed().toLower() == "1038" &&
               product.readAll().trimmed().toLower() == "161e")
            {
                matching_usb = true;
                break;
            }
        }
        parent = QFileInfo(parent).dir().absolutePath();
    }
    if(!matching_usb) return false;
    // KEY_A capability prevents attaching to unrelated media/control interfaces.
    QFile capability(QStringLiteral("/sys/class/input/") + name +
                     QStringLiteral("/device/capabilities/key"));
    if(!capability.open(QIODevice::ReadOnly)) return false;
    const QList<QByteArray> fields = capability.readAll().trimmed().split(' ');
    if(fields.isEmpty()) return false;
    bool ok = false;
    const qulonglong lowest_bits = fields.last().toULongLong(&ok, 16);
    return ok && (lowest_bits & (1ULL << KEY_A)) != 0;
}

class PhysicalKeyInput : public QObject
{
public:
    using OnPress = std::function<void(std::uint8_t)>;
    explicit PhysicalKeyInput(QObject* parent = nullptr) : QObject(parent) {}
    ~PhysicalKeyInput() override { stop(); }

    QString start(OnPress press)
    {
        stop();
        on_press_ = std::move(press);
        const auto nodes = QDir(QStringLiteral("/sys/class/input")).entryList(
            QStringList() << QStringLiteral("event*"), QDir::Dirs | QDir::System | QDir::NoDotAndDotDot);
        QString error;
        for(const auto& name : nodes)
        {
            if(!is_wired_apex_event(name)) continue;
            const QString device = QStringLiteral("/dev/input/") + name;
            const QByteArray path = QFile::encodeName(device);
            const int fd = ::open(path.constData(), O_RDONLY | O_NONBLOCK | O_CLOEXEC);
            if(fd < 0)
            {
                error = QStringLiteral("Cannot open %1: %2. Grant your desktop user read access to the input event node.")
                    .arg(device, QString::fromLocal8Bit(std::strerror(errno)));
                continue;
            }
            fd_ = fd;
            notifier_ = new QSocketNotifier(fd_, QSocketNotifier::Read, this);
            connect(notifier_, &QSocketNotifier::activated, this, [this](int)
            {
                input_event event{};
                while(true)
                {
                    const ssize_t read_count = ::read(fd_, &event, sizeof(event));
                    if(read_count == static_cast<ssize_t>(sizeof(event)))
                    {
                        if(event.type != EV_KEY || event.value != 1) continue;
                        const int hid = linux_to_hid(event.code);
                        if(hid >= 0 && supported(static_cast<std::uint8_t>(hid)) && on_press_)
                            on_press_(static_cast<std::uint8_t>(hid));
                    }
                    else if(read_count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
                        break;
                    else
                    {
                        stop();
                        break;
                    }
                }
            });
            return QStringLiteral("Physical keypresses connected: %1").arg(device);
        }
        return error.isEmpty()
            ? QStringLiteral("No matching wired Apex Pro Mini keyboard input event device found.")
            : error;
    }

    void stop()
    {
        if(notifier_) { delete notifier_; notifier_ = nullptr; }
        if(fd_ >= 0) { ::close(fd_); fd_ = -1; }
        on_press_ = {};
    }
    bool active() const { return fd_ >= 0; }

private:
    int fd_ = -1;
    QSocketNotifier* notifier_ = nullptr;
    OnPress on_press_;
};
} // namespace apex_mini
#endif
