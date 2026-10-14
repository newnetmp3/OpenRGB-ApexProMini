#pragma once
#include <QObject>
#include <OpenRGBPluginInterface.h>
#include <hidapi.h>
#include <mutex>

class ApexProMiniPlugin final : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.openrgb.OpenRGBPluginInterface" FILE "plugin-metadata.json")
    Q_INTERFACES(OpenRGBPluginInterface)
public:
    OpenRGBPluginInfo GetPluginInfo() override;
    unsigned int GetPluginAPIVersion() override { return OPENRGB_PLUGIN_API_VERSION; }
    void Load(OpenRGBPluginAPIInterface* api) override;
    QWidget* GetWidget() override { return nullptr; }
    QMenu* GetTrayMenu() override { return nullptr; }
    void Unload() override;
    void OnProfileAboutToLoad() override {}
    void OnProfileLoad(nlohmann::json) override {}
    nlohmann::json OnProfileSave() override { return nlohmann::json::object(); }
    unsigned char* OnSDKCommand(unsigned int, unsigned char*, unsigned int*) override { return nullptr; }
    void ProfileManagerUpdated(unsigned int) override {}
    void ResourceManagerUpdated(unsigned int) override {}
    void SettingsManagerUpdated(unsigned int) override {}
private:
    static void UpdateLEDs(void* context);
    static void UpdateMode(void* context);
    void SendColors();
    void SendRelease();
    OpenRGBPluginAPIInterface* api_ = nullptr;
    RGBControllerInterface* controller_ = nullptr;
    hid_device* device_ = nullptr;
    bool hid_initialized_ = false;
    std::mutex io_mutex_;
};
