#include "apex_pro_mini_effects.h"
#include <cassert>
#include <cmath>
#include <set>

int main()
{
    using namespace apex_mini;
    assert(effect_frame(Effect::Off, 0.0).size() == key_count);
    for(const auto effect : {Effect::Rainbow, Effect::Wave, Effect::Breathing,
                              Effect::Gradient, Effect::ColorShift, Effect::Comet, Effect::Sparkle})
    {
        for(double t : {0.0, 0.5, 3.0, 10.0})
        {
            auto frame = effect_frame(effect, t);
            assert(frame.size() == key_count);
            for(std::size_t i = 0; i < key_count; ++i)
                assert(frame[i].hid == hid_codes[i]);
        }
    }
    const auto rainbow = effect_frame(Effect::Rainbow, 0.0);
    std::set<unsigned int> colors;
    for(const auto& led : rainbow)
        colors.insert(static_cast<unsigned int>(led.color.r) |
                     (static_cast<unsigned int>(led.color.g) << 8) |
                     (static_cast<unsigned int>(led.color.b) << 16));
    assert(colors.size() >= 8);
    const auto moving = effect_frame(Effect::Rainbow, 3.0);
    assert(moving[0].color.r != rainbow[0].color.r ||
           moving[0].color.g != rainbow[0].color.g ||
           moving[0].color.b != rainbow[0].color.b);
    const auto inhale = effect_frame(Effect::Breathing, 0.0);
    const auto exhale = effect_frame(Effect::Breathing, 1.4285714286);
    assert(inhale[0].color.b > exhale[0].color.b);
    const auto wave0 = effect_frame(Effect::Wave, 0.0);
    const auto wave1 = effect_frame(Effect::Wave, 0.7);
    bool changed = false;
    for(std::size_t i=0; i<key_count; ++i)
        if(wave0[i].color.r != wave1[i].color.r ||
           wave0[i].color.g != wave1[i].color.g ||
           wave0[i].color.b != wave1[i].color.b) changed = true;
    assert(changed);
    const auto gradient0 = effect_frame(Effect::Gradient, 0.0);
    const auto gradient1 = effect_frame(Effect::Gradient, 5.0);
    for(std::size_t i = 0; i < key_count; ++i)
    {
        assert(gradient0[i].color.r == gradient1[i].color.r);
        assert(gradient0[i].color.g == gradient1[i].color.g);
        assert(gradient0[i].color.b == gradient1[i].color.b);
    }
    const auto shift0 = effect_frame(Effect::ColorShift, 0.0);
    const auto shift1 = effect_frame(Effect::ColorShift, 1.0);
    assert(shift0[0].color.r != shift1[0].color.r ||
           shift0[0].color.g != shift1[0].color.g ||
           shift0[0].color.b != shift1[0].color.b);
    const auto comet0 = effect_frame(Effect::Comet, 0.0);
    const auto comet1 = effect_frame(Effect::Comet, 2.0);
    bool comet_changed = false;
    for(std::size_t i=0; i<key_count; ++i)
        if(comet0[i].color.b != comet1[i].color.b) comet_changed = true;
    assert(comet_changed);
    const auto sparkle0 = effect_frame(Effect::Sparkle, 2.0);
    const auto sparkle_repeat = effect_frame(Effect::Sparkle, 2.0);
    const auto sparkle_later = effect_frame(Effect::Sparkle, 4.0);
    bool sparkle_changed = false;
    for(std::size_t i=0; i<key_count; ++i)
    {
        assert(sparkle0[i].color.r == sparkle_repeat[i].color.r);
        assert(sparkle0[i].color.g == sparkle_repeat[i].color.g);
        assert(sparkle0[i].color.b == sparkle_repeat[i].color.b);
        if(sparkle0[i].color.r != sparkle_later[i].color.r ||
           sparkle0[i].color.g != sparkle_later[i].color.g ||
           sparkle0[i].color.b != sparkle_later[i].color.b) sparkle_changed = true;
    }
    assert(sparkle_changed);
}
