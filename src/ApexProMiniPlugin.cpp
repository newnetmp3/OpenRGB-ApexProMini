#include "ApexProMiniPlugin.h"
#include "apex_pro_mini_protocol.h"
#include <vector>
#include <string>

OpenRGBPluginInfo ApexProMiniPlugin::GetPluginInfo()
{
    OpenRGBPluginInfo info{};
    info.Name = "Apex Pro Mini";
    info.Description = "Wired Apex Pro Mini direct RGB over USB HID";
    info.Version = "0.1.0-dev";
    info.URL = "https://github.com/newnetmp3/OpenRGB-ApexProMini";
    info.Location = OPENRGB_PLUGIN_LOCATION_SETTINGS;
    info.Label = "Apex Pro Mini";
    info.ProtocolVersion = OPENRGB_PLUGIN_API_VERSION;
    return info;
}

void ApexProMiniPlugin::Load(OpenRGBPluginAPIInterface* api)
{
    if(!api || api_ || controller_ || device_) return;
    api_ = api;
    if(hid_init() != 0)
    {
        api_ = nullptr;
        return;
    }
    hid_initialized_ = true;
    // Restrict to the vendor-defined control collection. Do not open the keyboard input endpoint.
    hid_device_info* info = hid_enumerate(apex_mini::vendor_id, apex_mini::product_id);
    for(hid_device_info* it = info; it != nullptr; it = it->next)
    {
        if(it->interface_number == 1 && it->usage_page == 0xFFC0 && it->usage == 1)
        {
            device_ = hid_open_path(it->path);
            break;
        }
    }
    hid_free_enumeration(info);
    if(!device_)
    {
        hid_exit();
        hid_initialized_ = false;
        api_ = nullptr;
        return;
    }

    RGBController_Setup setup{};
    setup.name = "SteelSeries Apex Pro Mini (Plugin)";
    setup.vendor = "SteelSeries";
    setup.description = "Wired Apex Pro Mini USB HID RGB";
    setup.location = "HID: 1038:161e";
    setup.version = "0.1.0-dev";
    setup.type = DEVICE_TYPE_KEYBOARD;
    setup.flags = CONTROLLER_FLAG_VIRTUAL;
    setup.active_mode = 0;
    setup.object_ptr = this;
    setup.DeviceUpdateLEDs = &UpdateLEDs;
    setup.DeviceUpdateZoneLEDs = [](void* p, int) { UpdateLEDs(p); };
    setup.DeviceUpdateSingleLED = [](void* p, int) { UpdateLEDs(p); };
    setup.DeviceUpdateMode = &UpdateMode;

    mode direct{};
    direct.name = "Direct";
    direct.value = 0;
    direct.flags = MODE_FLAG_HAS_PER_LED_COLOR;
    direct.color_mode = MODE_COLORS_PER_LED;
    setup.modes.push_back(direct);
    mode onboard{};
    onboard.name = "Onboard";
    onboard.value = 1;
    onboard.color_mode = MODE_COLORS_NONE;
    setup.modes.push_back(onboard);

    zone keyboard{};
    keyboard.name = "Keyboard";
    keyboard.type = ZONE_TYPE_LINEAR;
    keyboard.leds_min = apex_mini::key_count;
    keyboard.leds_max = apex_mini::key_count;
    keyboard.leds_count = apex_mini::key_count;
    setup.zones.push_back(keyboard);
    for(const auto hid : apex_mini::hid_codes)
    {
        led key{};
        key.name = "HID " + std::to_string(hid);
        key.value = hid;
        setup.leds.push_back(key);
    }
    controller_ = api_->CreateVirtualRGBController(&setup);
    if(controller_)
    {
        api_->RegisterVirtualRGBController(controller_);
    }
    else
    {
        hid_close(device_);
        device_ = nullptr;
        hid_exit();
        hid_initialized_ = false;
        api_ = nullptr;
    }
}

void ApexProMiniPlugin::UpdateLEDs(void* context)
{
    static_cast<ApexProMiniPlugin*>(context)->SendColors();
}

void ApexProMiniPlugin::UpdateMode(void* context)
{
    auto* plugin = static_cast<ApexProMiniPlugin*>(context);
    if(plugin->controller_ && plugin->controller_->GetActiveMode() == 1)
        plugin->SendRelease();
    else
        plugin->SendColors();
}

void ApexProMiniPlugin::SendColors()
{
    std::lock_guard<std::mutex> lock(io_mutex_);
    if(!device_ || !controller_ || controller_->GetActiveMode() != 0) return;
    std::vector<apex_mini::KeyColor> colors;
    for(std::size_t i = 0; i < apex_mini::key_count; ++i)
    {
        const RGBColor rgb = controller_->GetColor(static_cast<unsigned int>(i));
        colors.push_back({apex_mini::hid_codes[i],
            {static_cast<std::uint8_t>(RGBGetRValue(rgb)),
             static_cast<std::uint8_t>(RGBGetGValue(rgb)),
             static_cast<std::uint8_t>(RGBGetBValue(rgb))}});
    }
    auto report = apex_mini::encode_direct(colors);
    hid_send_feature_report(device_, report.data(), report.size());
}

void ApexProMiniPlugin::SendRelease()
{
    std::lock_guard<std::mutex> lock(io_mutex_);
    if(!device_) return;
    auto report = apex_mini::encode_release();
    hid_write(device_, report.data(), report.size());
}

void ApexProMiniPlugin::Unload()
{
    if(api_ && controller_)
    {
        api_->UnregisterVirtualRGBController(controller_);
        api_->DeleteVirtualRGBController(controller_);
        controller_ = nullptr;
    }
    SendRelease();
    {
        std::lock_guard<std::mutex> lock(io_mutex_);
        if(device_) hid_close(device_);
        device_ = nullptr;
    }
    if(hid_initialized_)
    {
        hid_exit();
        hid_initialized_ = false;
    }
    api_ = nullptr;
}
