#include "apex_pro_mini_catalog.h"
#include <cassert>
#include <cstring>
#include <cmath>

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
    // WabashAndLake's short 2.1s source period needs its own slider
    // compensation, without changing other Prism or core effects.
    const int wabash = factory_effect_base + 10;
    assert(std::strcmp(factory_presets[10].id, "wabashAndLake") == 0);
    assert(std::abs(preset_motion_speed(wabash, 1.0) - 0.35) < 1e-9);
    assert(std::abs(preset_motion_speed(wabash, 1.625) - 0.56875) < 1e-9);
    assert(std::abs(preset_motion_speed(wabash, 3.0) - 1.05) < 1e-9);
    assert(std::abs(preset_motion_speed(factory_effect_base+4, 1.625) - 1.625) < 1e-9);
    const auto slow = full_effect_frame(wabash, 2.0, 1.0);
    const auto faster = full_effect_frame(wabash, 1.0, 2.0);
    for(std::size_t i=0; i<key_count; ++i)
    {
        assert(slow[i].color.r == faster[i].color.r);
        assert(slow[i].color.g == faster[i].color.g);
        assert(slow[i].color.b == faster[i].color.b);
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
    // Effect ID 4 is spatial Gradient; ID 5 is whole-board Color Shift.
    // Compare keys by known HID usage, not by arbitrary transport indices.
    const auto grad=full_effect_frame(static_cast<int>(Effect::Gradient),0.0,1.0,{},controls);
    std::size_t esc=0, backspace=0;
    while(esc<key_count && hid_codes[esc]!=41) ++esc;
    while(backspace<key_count && hid_codes[backspace]!=42) ++backspace;
    assert(esc<key_count && backspace<key_count);
    assert(grad[esc].color.r > grad[backspace].color.r);
    assert(grad[esc].color.g < grad[backspace].color.g);
    const auto shift=full_effect_frame(static_cast<int>(Effect::ColorShift),0.0,1.0,{},controls);
    assert(shift[esc].color.r == shift[backspace].color.r);
    assert(shift[esc].color.g == shift[backspace].color.g);
    // Every non-disabled historical preset must honor a custom palette,
    // including physical-key reactive variants and regional color masks.
    EffectControls recolor;
    recolor.custom_palette=true;
    recolor.base={20,2,180};
    recolor.accent={245,100,14};
    const std::vector<KeyHit> test_hits{{4,0.0},{22,0.15}};
    for(std::size_t preset=0;preset<factory_presets.size();++preset)
    {
        if(preset==25) continue; // disabled intentionally remains dark
        const int id=factory_effect_base+static_cast<int>(preset);
        const auto original=full_effect_frame(id,0.3,1.0,test_hits);
        const auto custom=full_effect_frame(id,0.3,1.0,test_hits,recolor);
        bool different=false;
        for(std::size_t i=0;i<key_count;++i)
        {
            assert(original[i].hid==custom[i].hid);
            different |= original[i].color.r!=custom[i].color.r ||
                         original[i].color.g!=custom[i].color.g ||
                         original[i].color.b!=custom[i].color.b;
        }
        assert(different && "Every non-disabled Prism preset must honor custom colors");
    }
    // Mode 0 must remain dark, regardless of palette selection.
    const auto off=full_effect_frame(0,0.3,1.0,{},recolor);
    for(const auto& k : off)
        assert(k.color.r==0 && k.color.g==0 && k.color.b==0);
    const auto default_rainbow=full_effect_frame(1,0.5,1.0);
    const auto custom_rainbow=full_effect_frame(1,0.5,1.0,{},recolor);
    assert(default_rainbow[0].color.r!=custom_rainbow[0].color.r ||
           default_rainbow[0].color.g!=custom_rainbow[0].color.g ||
           default_rainbow[0].color.b!=custom_rainbow[0].color.b);
    const auto untouched=full_effect_frame(factory_effect_base+4,0.6,1.0);
    EffectControls inactive;
    inactive.base={200,2,3};
    inactive.accent={1,220,40};
    const auto still_original=full_effect_frame(factory_effect_base+4,0.6,1.0,{},inactive);
    for(std::size_t i=0;i<key_count;++i)
    {
        assert(untouched[i].color.r==still_original[i].color.r);
        assert(untouched[i].color.g==still_original[i].color.g);
        assert(untouched[i].color.b==still_original[i].color.b);
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
