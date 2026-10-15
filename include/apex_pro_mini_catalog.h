#pragma once
#include "apex_pro_mini_effects.h"
#include "apex_pro_mini_factory.h"
#include "apex_pro_mini_regions.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace apex_mini
{
struct KeyHit { std::uint8_t hid; double when; };
constexpr int factory_effect_base = 100;
constexpr int last_basic_effect = 14;

inline int reactive_kind(int effect)
{
    if(effect == 8 || effect == 9 || effect == 13 || effect == 14) return effect;
    if(effect == factory_effect_base + 5) return 14; // reactiveLine
    if(effect == factory_effect_base + 6) return 8;  // reactiveRipple
    if(effect == factory_effect_base + 7) return 9;  // reactiveSingleKey
    if(effect == factory_effect_base + 29) return 13; // heatmap
    return 0;
}


inline Color from_hex(std::uint32_t rgb)
{
    return {static_cast<std::uint8_t>((rgb >> 16) & 255),
            static_cast<std::uint8_t>((rgb >> 8) & 255),
            static_cast<std::uint8_t>(rgb & 255)};
}
inline Color lerp(Color a, Color b, double weight)
{
    weight = std::max(0.0, std::min(1.0, weight));
    return {static_cast<std::uint8_t>(std::lround(a.r * (1.0 - weight) + b.r * weight)),
            static_cast<std::uint8_t>(std::lround(a.g * (1.0 - weight) + b.g * weight)),
            static_cast<std::uint8_t>(std::lround(a.b * (1.0 - weight) + b.b * weight))};
}
inline Color palette(const FactoryPreset& preset, double offset)
{
    if(!preset.stop_count) return {0,0,0};
    const auto& stops = preset.stops;
    offset = std::max(0.0, std::min(255.0, offset));
    if(offset <= stops[0].at) return from_hex(stops[0].rgb);
    for(std::size_t i=1; i<preset.stop_count; ++i)
    {
        if(offset <= stops[i].at)
        {
            if(stops[i].at <= stops[i-1].at) return from_hex(stops[i].rgb);
            return lerp(from_hex(stops[i-1].rgb), from_hex(stops[i].rgb),
                        (offset - stops[i-1].at)/(stops[i].at-stops[i-1].at));
        }
    }
    return from_hex(stops[preset.stop_count-1].rgb);
}

inline std::array<double, 2> key_position(std::uint8_t hid)
{
    for(const auto& p : positions)
        if(p.usage == hid)
            return {{(p.column + 0.5)/layout_columns, (p.row+0.5)/layout_rows}};
    // Seven firmware/ISO variants remain rendered, but their precise
    // physical positions are unknown and must not be presented as calibrated.
    return {{0.5, 0.5}};
}
inline double hit_intensity(int effect, std::uint8_t hid, double x, double y,
                            double seconds, double speed,
                            const std::vector<KeyHit>& hits)
{
    double intensity = 0;
    double accumulated_heat = 0;
    for(const auto& hit : hits)
    {
        const double age = seconds - hit.when;
        if(age < 0 || age > 2.5) continue;
        const auto origin = key_position(hit.hid);
        const double dx = x-origin[0], dy = y-origin[1];
        const double radius = std::sqrt(dx*dx+dy*dy);
        double value = 0;
        if(effect == 9) // reactive single-key
            value = hid == hit.hid ? std::exp(-age * 3.0 * speed) : 0;
        else if(effect == 8) // ripple
        {
            const double distance = radius - age * 0.6 * speed;
            value = std::exp(-distance*distance/0.008) * (1.0-age/2.5);
        }
        else if(effect == 13) // accumulating heatmap
            value = std::exp(-radius * 9.0) * std::exp(-age * 0.60);
        else if(effect == 14) // reactive line
        {
            const double front = std::abs(dx) - age * 0.5 * speed;
            value = std::exp(-front*front/0.008) * std::exp(-dy*dy/0.05)
                    * (1.0-age/2.5);
        }
        intensity = std::max(intensity, value);
        if(effect == 13) accumulated_heat += value;
    }
    if(effect == 13) intensity = 1.0 - std::exp(-accumulated_heat * 0.85);
    return std::max(0.0, std::min(1.0, intensity));
}

// Effects 8..14 fill out the 14-mode source catalogue (including Solid).
// Factory 100..130 correspond to independently reconstructed Prism presets.
// The firmware command stream remains unchanged (all 68 HID usages in order).
inline std::vector<KeyColor> full_effect_frame(int effect, double seconds,
                          double speed, const std::vector<KeyHit>& hits = {})
{
    if(effect >= 1 && effect <= 7)
        return effect_frame(static_cast<Effect>(effect), seconds, speed);

    std::vector<KeyColor> frame;
    frame.reserve(key_count);
    const double time = seconds * std::max(0.1, std::min(4.0, speed));
    const bool is_factory = effect >= factory_effect_base &&
                            effect < factory_effect_base + static_cast<int>(factory_presets.size());
    const FactoryPreset* spec = is_factory ? &factory_presets[effect - factory_effect_base] : nullptr;
    for(auto hid : hid_codes)
    {
        const auto pos = key_position(hid);
        const double x=pos[0], y=pos[1];
        Color color{0,0,0};
        switch(effect)
        {
            case 0: color = {0,0,0}; break;
            case 8: case 9: case 13: case 14:
                color = lerp({6,12,25}, effect == 13 ? Color{255,78,0} : Color{0,230,255},
                    hit_intensity(effect,hid,x,y,seconds,speed,hits));
                break;
            case 10: color = hsv(std::abs(x-0.5)*2.0 + y*0.12 + time*0.16,1.0,1.0); break;
            case 11: color = hsv(x*1.2+y*0.35+std::sin(time*0.19)*0.4-time*0.07,1.0,1.0); break;
            case 12: color = {18,30,65}; break; // Solid
            default:
                if(spec)
                {
                    if(spec->stop_count == 0)
                    {
                        // "steelseriesOrange" was a no-stop preset in the
                        // migration data; use a named fallback, "disabled" is off.
                        if(effect == factory_effect_base)
                            color = {255,95,0};
                        else if(reactive_kind(effect) == 13)
                            color = lerp({6,12,25}, {255,68,0},
                                hit_intensity(13,hid,x,y,seconds,speed,hits));
                        else color = {0,0,0};
                    }
                    else
                    {
                        // The historical Prism catalog includes region-specific
                        // palette parameters; the physical-to-source grid is
                        // approximate, not a vendor-accurate compositor.
                        unsigned int source_row = 2;
                        for(const auto& p : positions)
                            if(p.usage == hid) { source_row = p.row; break; }
                        const auto* region = region_for(spec->id, x, source_row);
                        const unsigned int period_ms = region ? region->period_ms : spec->period_ms;
                        const int wavelength_param = region ? region->wavelength : spec->wavelength;
                        const unsigned int geometry = region ? region->geometry : spec->geometry;
                        const double period = period_ms > 0 ? period_ms/1000.0 : 0;
                        const double phase = period > 0 ? time/period : 0;
                        const double geometric = geometry == 3
                            ? (x+y)*0.5 : geometry == 2 ? y : x;
                        const double wavelength = std::abs(wavelength_param)/64.0;
                        const double direction = wavelength_param < 0 ? -1.0 : 1.0;
                        const double value = period > 0
                            ? std::fmod(geometric*wavelength*255.0+
                                direction*phase*255.0+2048.0*255.0,255.0)
                            : geometric*255.0;
                        if(region)
                        {
                            const FactoryPreset local{spec->id,period_ms,wavelength_param,
                                geometry,1,region->stops,region->stop_count};
                            color = palette(local,value);
                        }
                        else color = palette(*spec, value);
                        if(const int kind = reactive_kind(effect))
                        {
                            const double intensity = hit_intensity(kind,hid,x,y,seconds,speed,hits);
                            // White palette stops are meaningful only as the
                            // reactive foreground; idle keys remain dark.
                            color = lerp({6,12,25}, color, intensity);
                        }
                    }
                }
                break;
        }
        frame.push_back({hid,color});
    }
    return frame;
}
} // namespace apex_mini
