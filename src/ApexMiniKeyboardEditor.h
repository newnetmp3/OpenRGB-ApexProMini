#pragma once

#include "apex_pro_mini_protocol.h"
#include "apex_pro_mini_catalog.h"
#ifdef __linux__
#include "ApexMiniPhysicalInput.h"
#endif
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QElapsedTimer>
#include <QSlider>
#include <QTimer>
#include <QFont>
#include <QHBoxLayout>
#include <QHash>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QRubberBand>
#include <QSet>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <utility>
#include <vector>

namespace apex_mini
{
struct VisualKey { std::uint8_t hid; const char* label; double units; };

// Each key retains the mouse grab while dragging, even if the cursor crosses
// other keys or blank layout space. All coordinates are mapped to the editor.
class KeyButton : public QPushButton
{
public:
    using MouseCallback = std::function<void(const QPoint&, Qt::KeyboardModifiers)>;
    explicit KeyButton(const QString& label, QWidget* parent = nullptr)
        : QPushButton(label, parent) {}
    MouseCallback pressed_at;
    MouseCallback moved_at;
    MouseCallback released_at;
protected:
    void mousePressEvent(QMouseEvent* e) override
    {
        if(e->button() == Qt::LeftButton && pressed_at)
        {
            pressed_at(e->pos(), e->modifiers());
            e->accept();
            return;
        }
        QPushButton::mousePressEvent(e);
    }
    void mouseMoveEvent(QMouseEvent* e) override
    {
        if((e->buttons() & Qt::LeftButton) && moved_at)
        {
            moved_at(e->pos(), e->modifiers());
            e->accept();
            return;
        }
        QPushButton::mouseMoveEvent(e);
    }
    void mouseReleaseEvent(QMouseEvent* e) override
    {
        if(e->button() == Qt::LeftButton && released_at)
        {
            released_at(e->pos(), e->modifiers());
            e->accept();
            return;
        }
        QPushButton::mouseReleaseEvent(e);
    }
};

class KeyboardEditor : public QWidget
{
public:
    using ApplyColor = std::function<bool(const std::vector<std::uint8_t>&, const QColor&)>;
    using ReadColor = std::function<QColor(std::uint8_t)>;
    using ApplyFrame = std::function<bool(const std::vector<KeyColor>&)>;
    explicit KeyboardEditor(ApplyColor apply, ReadColor read, ApplyFrame frame, QWidget* parent = nullptr)
        : QWidget(parent), apply_(std::move(apply)), read_(std::move(read)), frame_(std::move(frame))
    {
        auto* main = new QVBoxLayout(this);
        main->setSpacing(9);
        auto* heading = new QLabel(QStringLiteral("Apex Pro Mini — Key Lighting"));
        QFont title = heading->font();
        title.setBold(true);
        title.setPointSize(title.pointSize() + 2);
        heading->setFont(title);
        main->addWidget(heading);

        auto* note = new QLabel(QStringLiteral(
            "Drag across keycaps to select a rectangle. Click for one key; "
            "Ctrl-click toggles keys, Ctrl-drag adds keys. Choose a color to apply "
            "to the whole selection in Direct mode. Keycaps preview the current OpenRGB colors."));
        note->setWordWrap(true);
        main->addWidget(note);

        auto add_row = [this, main](std::initializer_list<VisualKey> keys)
        {
            auto* line = new QHBoxLayout();
            line->setSpacing(4);
            for(const auto& key : keys)
            {
                auto* button = new KeyButton(QString::fromUtf8(key.label));
                button->setToolTip(QStringLiteral("HID %1").arg(key.hid));
                button->setMinimumHeight(43);
                button->setMinimumWidth(static_cast<int>(45.0 * key.units));
                button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                line->addWidget(button, static_cast<int>(key.units * 100));
                buttons_.push_back({key.hid, button});
                button->pressed_at = [this, button, key](const QPoint& pos, Qt::KeyboardModifiers modifiers)
                {
                    drag_origin_ = button->mapTo(this, pos);
                    drag_modifiers_ = modifiers;
                    pressed_hid_ = key.hid;
                    // Reactive modes are demonstrated using clicks in this editor.
                    // This does not capture global keyboard or evdev events.
                    if(effect_combo_ && effect_timer_ && effect_timer_->isActive())
                    {
                        const double when = effect_clock_.elapsed() / 1000.0;
                        hits_.push_back({key.hid, when});
                    }
                    baseline_ = selected_;
                    dragging_ = false;
                    mouse_pressed_ = true;
                };
                button->moved_at = [this, button](const QPoint& pos, Qt::KeyboardModifiers)
                {
                    if(!mouse_pressed_) return;
                    const QPoint end = button->mapTo(this, pos);
                    if(!dragging_ && (end - drag_origin_).manhattanLength() < 6) return;
                    dragging_ = true;
                    const QRect bounds = QRect(drag_origin_, end).normalized();
                    rubber_band_->setGeometry(bounds);
                    rubber_band_->show();
                    selected_ = (drag_modifiers_ & Qt::ControlModifier) ? baseline_ : QSet<std::uint8_t>{};
                    for(const auto& item : buttons_)
                    {
                        const QRect key_rect(item.second->mapTo(this, QPoint(0, 0)), item.second->size());
                        if(bounds.intersects(key_rect))
                            selected_.insert(item.first);
                    }
                    updateSelection();
                };
                button->released_at = [this](const QPoint&, Qt::KeyboardModifiers)
                {
                    if(!mouse_pressed_) return;
                    mouse_pressed_ = false;
                    rubber_band_->hide();
                    if(!dragging_)
                    {
                        selected_ = (drag_modifiers_ & Qt::ControlModifier) ? baseline_ : QSet<std::uint8_t>{};
                        if((drag_modifiers_ & Qt::ControlModifier) && selected_.contains(pressed_hid_))
                            selected_.remove(pressed_hid_);
                        else
                            selected_.insert(pressed_hid_);
                    }
                    dragging_ = false;
                    updateSelection();
                };
            }
            main->addLayout(line);
        };

        add_row({
            {41,"Esc",1},{30,"1",1},{31,"2",1},{32,"3",1},{33,"4",1},
            {34,"5",1},{35,"6",1},{36,"7",1},{37,"8",1},{38,"9",1},
            {39,"0",1},{45,"-",1},{46,"=",1},{42,"Backspace",2}
        });
        add_row({
            {43,"Tab",1.5},{20,"Q",1},{26,"W",1},{8,"E",1},{21,"R",1},
            {23,"T",1},{28,"Y",1},{24,"U",1},{12,"I",1},{18,"O",1},
            {19,"P",1},{47,"[",1},{48,"]",1},{49,"\\",1.5}
        });
        add_row({
            {57,"Caps",1.75},{4,"A",1},{22,"S",1},{7,"D",1},{9,"F",1},
            {10,"G",1},{11,"H",1},{13,"J",1},{14,"K",1},{15,"L",1},
            {51,";",1},{52,"'",1},{40,"Enter",2.25}
        });
        add_row({
            {225,"Shift",2.25},{29,"Z",1},{27,"X",1},{6,"C",1},{25,"V",1},
            {5,"B",1},{17,"N",1},{16,"M",1},{54,",",1},{55,".",1},
            {56,"/",1},{229,"Shift",2.75}
        });
        add_row({
            {224,"Ctrl",1.25},{227,"Win",1.25},{226,"Alt",1.25},
            {44,"Space",6.25},{230,"Alt",1.25},{231,"Win",1.25},
            {240,"SteelSeries",1.25},{228,"Ctrl",1.25}
        });


        rubber_band_ = new QRubberBand(QRubberBand::Rectangle, this);
        rubber_band_->setAttribute(Qt::WA_TransparentForMouseEvents);

        auto* tools = new QHBoxLayout();
        selection_label_ = new QLabel(QStringLiteral("Selected: 0 keys"));
        tools->addWidget(selection_label_, 1);
        auto* select_all = new QPushButton(QStringLiteral("Select All"));
        tools->addWidget(select_all);
        connect(select_all, &QPushButton::clicked, this, [this]()
        {
            selected_.clear();
            for(const auto& item : buttons_) selected_.insert(item.first);
            updateSelection();
        });
        auto* clear = new QPushButton(QStringLiteral("Clear Selection"));
        tools->addWidget(clear);
        connect(clear, &QPushButton::clicked, this, [this]()
        {
            selected_.clear();
            updateSelection();
        });
        auto* choose = new QPushButton(QStringLiteral("Set selected keys color…"));
        tools->addWidget(choose);
        main->addLayout(tools);
        auto* preview_note = new QLabel(QStringLiteral(
            "Live preview: shows OpenRGB's current LED colors, including edits made in Devices. "
            "Onboard lighting is controlled by the keyboard and cannot be read back."));
        preview_note->setWordWrap(true);
        main->addWidget(preview_note);
        auto* footer = new QLabel(QStringLiteral(
            "The seven additional regional/firmware HID addresses remain accessible "
            "through OpenRGB's standard LED list. SteelSeries key HID is provisional."));
        footer->setWordWrap(true);
        main->addWidget(footer);
        connect(choose, &QPushButton::clicked, this, [this]()
        {
            if(selected_.isEmpty()) return;
            QColor initial = Qt::blue;
            if(read_ && !selected_.isEmpty())
            {
                const QColor current = read_(*selected_.constBegin());
                if(current.isValid()) initial = current;
            }
            const QColor color = QColorDialog::getColor(initial, this, QStringLiteral("Selected keys color"));
            if(!color.isValid() || !apply_) return;
            std::vector<std::uint8_t> selected_hids;
            // Use stable visual-order selection so the output is predictable.
            for(const auto& item : buttons_)
                if(selected_.contains(item.first)) selected_hids.push_back(item.first);
            // A manual color operation takes precedence over running animation.
            if(effect_timer_) effect_timer_->stop();
            if(effect_combo_) effect_combo_->setCurrentIndex(0);
            const bool applied = apply_(selected_hids, color);
            if(applied) refreshPreview();
            selection_label_->setText(applied
                ? QStringLiteral("Applied color to %1 keys").arg(selected_hids.size())
                : QStringLiteral("Cannot apply: select Direct mode and verify device connection"));
        });
        auto* fx = new QHBoxLayout();
        fx->addWidget(new QLabel(QStringLiteral("Animation")));
        auto* effect_box = new QComboBox();
        effect_box->addItem(QStringLiteral("Off"), static_cast<int>(Effect::Off));
        effect_box->addItem(QStringLiteral("Rainbow"), static_cast<int>(Effect::Rainbow));
        effect_box->addItem(QStringLiteral("Color Wave"), static_cast<int>(Effect::Wave));
        effect_box->addItem(QStringLiteral("Breathing"), static_cast<int>(Effect::Breathing));
        effect_box->addItem(QStringLiteral("Gradient"), static_cast<int>(Effect::Gradient));
        effect_box->addItem(QStringLiteral("Color Shift"), static_cast<int>(Effect::ColorShift));
        effect_box->addItem(QStringLiteral("Comet"), static_cast<int>(Effect::Comet));
        effect_box->addItem(QStringLiteral("Sparkle"), static_cast<int>(Effect::Sparkle));
        effect_box->addItem(QStringLiteral("Solid"), 12);
        effect_box->addItem(QStringLiteral("Ripple (click-reactive)"), 8);
        effect_box->addItem(QStringLiteral("Reactive (click-reactive)"), 9);
        effect_box->addItem(QStringLiteral("Rainbow Split"), 10);
        effect_box->addItem(QStringLiteral("Prismatic Sweep"), 11);
        effect_box->addItem(QStringLiteral("Heatmap (click-reactive)"), 13);
        effect_box->addItem(QStringLiteral("Reactive Line (click-reactive)"), 14);
        effect_box->insertSeparator(effect_box->count());
        for(std::size_t i=0; i<factory_presets.size(); ++i)
        {
            const auto& preset = factory_presets[i];
            effect_box->addItem(QStringLiteral("Prism-style: ") + QString::fromLatin1(preset.id),
                                factory_effect_base + static_cast<int>(i));
        }
        fx->addWidget(effect_box, 1);
        effect_combo_ = effect_box;
        fx->addWidget(new QLabel(QStringLiteral("Speed")));
        auto* speed_slider = new QSlider(Qt::Horizontal);
        speed_slider->setMinimum(25);
        speed_slider->setMaximum(300);
        speed_slider->setValue(100);
        speed_slider->setToolTip(QStringLiteral("0.25x – 3.0x"));
        fx->addWidget(speed_slider, 1);
        auto* stop = new QPushButton(QStringLiteral("Stop effect"));
        fx->addWidget(stop);
        main->addLayout(fx);

        effect_timer_ = new QTimer(this);
        effect_timer_->setInterval(50); // 20 frames/s, avoiding excess HID traffic
        connect(effect_timer_, &QTimer::timeout, this, [this, effect_box, speed_slider]()
        {
            if(!frame_) return;
            const auto effect = static_cast<Effect>(effect_box->currentData().toInt());
            if(effect == Effect::Off) return;
            const double seconds = effect_clock_.elapsed() / 1000.0;
            hits_.erase(std::remove_if(hits_.begin(), hits_.end(),
                [seconds](const KeyHit& hit) { return seconds - hit.when > 2.5; }), hits_.end());
            if(!frame_(full_effect_frame(static_cast<int>(effect), seconds,
                     speed_slider->value() / 100.0, hits_)))
            {
                effect_timer_->stop();
                effect_box->setCurrentIndex(0);
                selection_label_->setText(QStringLiteral(
                    "Effect stopped: choose Direct mode and verify device connection"));
                return;
            }
            refreshPreview();
        });
        connect(effect_box, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this, effect_box](int index)
        {
            effect_timer_->stop();
            hits_.clear();
            if(index != 0)
            {
                effect_clock_.start();
                effect_timer_->start();
            }
        });
        connect(stop, &QPushButton::clicked, this, [effect_box]()
        {
            effect_box->setCurrentIndex(0);
        });
#ifdef __linux__
        auto* physical_controls = new QHBoxLayout();
        auto* physical_toggle = new QPushButton(QStringLiteral("Enable physical keypress reactions"));
        physical_toggle->setCheckable(true);
        physical_controls->addWidget(physical_toggle);
        physical_status_ = new QLabel(QStringLiteral("Physical keypress monitoring: off"));
        physical_status_->setWordWrap(true);
        physical_controls->addWidget(physical_status_, 1);
        main->addLayout(physical_controls);
        physical_input_ = new PhysicalKeyInput(this);
        connect(physical_toggle, &QPushButton::toggled, this,
                [this, physical_toggle](bool enabled)
        {
            if(!enabled)
            {
                physical_input_->stop();
                physical_status_->setText(QStringLiteral("Physical keypress monitoring: off"));
                return;
            }
            const QString status = physical_input_->start([this](std::uint8_t hid)
            {
                if(effect_timer_ && effect_timer_->isActive())
                    hits_.push_back({hid, effect_clock_.elapsed() / 1000.0});
            });
            physical_status_->setText(status);
            if(!physical_input_->active())
                physical_toggle->setChecked(false);
        });
#endif
        preview_timer_ = new QTimer(this);
        preview_timer_->setInterval(150);
        connect(preview_timer_, &QTimer::timeout, this, [this]()
        {
            if(isVisible()) refreshPreview();
        });
        preview_timer_->start();
        refreshPreview();
        setMinimumWidth(820);
    }

private:
    void refreshPreview()
    {
        for(const auto& item : buttons_)
        {
            QColor color = read_ ? read_(item.first) : QColor();
            if(color.isValid())
                preview_colors_.insert(item.first, color);
        }
        updateKeyStyles();
    }

    void updateKeyStyles()
    {
        for(const auto& item : buttons_)
        {
            const QColor background = preview_colors_.value(item.first, QColor(34, 39, 48));
            const bool selected = selected_.contains(item.first);
            // Render actual RGB fill while selection stays a separate border.
            const int luminance = (background.red() * 299 + background.green() * 587
                                   + background.blue() * 114) / 1000;
            const QString foreground = luminance >= 145 ? QStringLiteral("#111820")
                                                        : QStringLiteral("#f7faff");
            const QString border = selected ? QStringLiteral("3px solid #35baff")
                                            : QStringLiteral("1px solid #697482");
            const QString style = QStringLiteral(
                "QPushButton { background-color: %1; color: %2; border: %3; "
                "border-radius: 5px; font-weight: 600; }")
                .arg(background.name(QColor::HexRgb), foreground, border);
            // Avoid triggering a style/polish pass 61 times every timer tick.
            if(item.second->styleSheet() != style)
                item.second->setStyleSheet(style);
            item.second->setToolTip(QStringLiteral("HID %1 • %2")
                .arg(item.first).arg(background.name(QColor::HexRgb).toUpper()));
        }
    }

    void updateSelection()
    {
        updateKeyStyles();
        selection_label_->setText(QStringLiteral("Selected: %1 keys").arg(selected_.size()));
    }

    ApplyColor apply_;
    ReadColor read_;
    ApplyFrame frame_;
    QComboBox* effect_combo_ = nullptr;
    QTimer* effect_timer_ = nullptr;
    QElapsedTimer effect_clock_;
    std::vector<KeyHit> hits_;
#ifdef __linux__
    PhysicalKeyInput* physical_input_ = nullptr;
    QLabel* physical_status_ = nullptr;
#endif
    QTimer* preview_timer_ = nullptr;
    QHash<std::uint8_t, QColor> preview_colors_;
    std::vector<std::pair<std::uint8_t, KeyButton*>> buttons_;
    QSet<std::uint8_t> selected_;
    QSet<std::uint8_t> baseline_;
    QLabel* selection_label_ = nullptr;
    QRubberBand* rubber_band_ = nullptr;
    QPoint drag_origin_;
    Qt::KeyboardModifiers drag_modifiers_{};
    std::uint8_t pressed_hid_ = 0;
    bool dragging_ = false;
    bool mouse_pressed_ = false;
};
} // namespace apex_mini
