#include "apex_pro_mini_catalog.h"
#include <cassert>
#include <cstring>

int main()
{
    using namespace apex_mini;
    static_assert(factory_presets.size() == 31, "Historical catalog contains 31 entries");
    static_assert(region_variants.size() == 36, "36 historical regional variants");
    assert(region_for("pride",0.25,0) != nullptr);
    assert(region_for("pride",0.25,4) != nullptr);
    assert(region_for("prism",0.25,2) == nullptr);
    static_assert(key_count == 68, "Wired Mini exposes 68 HID usages");
    for(int mode = 0; mode <= last_basic_effect; ++mode)
    {
        const auto frame = full_effect_frame(mode, 0.75, 1.0);
        assert(frame.size() == key_count);
        for(std::size_t i=0; i<key_count; ++i)
            assert(frame[i].hid == hid_codes[i]);
        const auto report = encode_direct(frame);
        assert(report.size() == feature_bytes && report[2] == key_count);
    }
    for(std::size_t preset = 0; preset < factory_presets.size(); ++preset)
    {
        const auto frame = full_effect_frame(factory_effect_base+static_cast<int>(preset), 1.5, 1.25);
        assert(frame.size() == key_count);
        for(std::size_t i=0;i<key_count;++i)
            assert(frame[i].hid == hid_codes[i]);
        const auto report = encode_direct(frame);
        assert(report[2] == key_count);
    }
    assert(std::strcmp(factory_presets[20].id,"faze") == 0);
    assert(std::strcmp(factory_presets[30].id,"redPulse") == 0);
    const auto a = full_effect_frame(factory_effect_base+20, 0.5, 1.0);
    const auto b = full_effect_frame(factory_effect_base+30, 0.5, 1.0);
    for(std::size_t i=0;i<key_count;++i)
    {
        assert(a[i].color.r == b[i].color.r);
        assert(a[i].color.g == b[i].color.g);
        assert(a[i].color.b == b[i].color.b);
    }
    const auto quiet = full_effect_frame(9, 0.05, 1.0);
    const auto pressed = full_effect_frame(9, 0.05, 1.0, {{4,0.0}});
    for(std::size_t i=0;i<key_count;++i)
    {
        if(hid_codes[i] == 4)
            assert(pressed[i].color.g > quiet[i].color.g);
        else
            assert(pressed[i].color.g == quiet[i].color.g);
    }
    const auto ripple = full_effect_frame(8, 0.05, 1.0, {{4,0.0}});
    bool saw_change = false;
    for(std::size_t i=0;i<key_count;++i)
        if(ripple[i].color.b > quiet[i].color.b) saw_change = true;
    assert(saw_change);
}
