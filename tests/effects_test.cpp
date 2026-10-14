#include "apex_pro_mini_effects.h"
#include <cassert>
#include <cmath>
#include <set>

int main()
{
    using namespace apex_mini;
    assert(effect_frame(Effect::Off, 0.0).size() == key_count);
    for(const auto effect : {Effect::Rainbow, Effect::Wave, Effect::Breathing})
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
}
