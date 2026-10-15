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
    assert(reactive_kind(factory_effect_base+5) == 14);
    assert(reactive_kind(factory_effect_base+6) == 8);
    assert(reactive_kind(factory_effect_base+7) == 9);
    assert(reactive_kind(factory_effect_base+29) == 13);
    for(const int id : {factory_effect_base+5, factory_effect_base+6,
                        factory_effect_base+7, factory_effect_base+29})
    {
        const auto idle = full_effect_frame(id, 0.10, 1.0);
        const auto hit = full_effect_frame(id, 0.10, 1.0, {{4,0.0}});
        bool changed = false;
        for(std::size_t i=0; i<key_count; ++i)
        {
            if(idle[i].color.r != hit[i].color.r ||
               idle[i].color.g != hit[i].color.g ||
               idle[i].color.b != hit[i].color.b)
                changed = true;
        }
        assert(changed && "Every Prism reactive preset must respond to key presses");
    }
    const auto heat_single = full_effect_frame(factory_effect_base+29,0.12,1.0,{{4,0.0}});
    const auto heat_double = full_effect_frame(factory_effect_base+29,0.12,1.0,
                                              {{4,0.0},{4,0.05}});
    std::size_t a_index = 0;
    while(a_index < key_count && hid_codes[a_index] != 4) ++a_index;
    assert(a_index < key_count);
    assert(heat_double[a_index].color.r > heat_single[a_index].color.r);
    EffectControls controls;
    controls.brightness=0.0;
    for(const int mode : {1,2,3,4,5,6,7,8,9,10,11,12,13,14,104,129})
    {
        const auto dark=full_effect_frame(mode,0.3,1.0,{},controls);
        for(const auto& key : dark)
            assert(key.color.r==0 && key.color.g==0 && key.color.b==0);
    }
    controls.brightness=1.0;
    controls.base={250,10,20};
    const auto solid=full_effect_frame(12,0.0,1.0,{},controls);
    assert(solid[0].color.r==250 && solid[0].color.g==10);
    controls.accent={10,230,80};
    const auto grad=full_effect_frame(5,0.0,1.0,{},controls);
    assert(grad[0].color.r!=grad[10].color.r);
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
