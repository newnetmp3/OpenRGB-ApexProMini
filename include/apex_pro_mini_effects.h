#pragma once
#include "apex_pro_mini_protocol.h"
#include "apex_pro_mini_layout.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace apex_mini
{
enum class Effect : int { Off = 0, Rainbow = 1, Wave = 2, Breathing = 3,
                          Gradient = 4, ColorShift = 5, Comet = 6, Sparkle = 7 };
constexpr double pi = 3.14159265358979323846;

inline Color hsv(double hue, double saturation, double value)
{
    hue -= std::floor(hue);
    saturation = std::max(0.0, std::min(1.0, saturation));
    value = std::max(0.0, std::min(1.0, value));
    double h = hue * 6.0;
    int sector = static_cast<int>(std::floor(h));
    double f = h - sector;
    double p = value * (1.0 - saturation);
    double q = value * (1.0 - f * saturation);
    double t = value * (1.0 - (1.0 - f) * saturation);
    double r=0,g=0,b=0;
    switch(sector % 6)
    {
        case 0: r=value; g=t; b=p; break;
        case 1: r=q; g=value; b=p; break;
        case 2: r=p; g=value; b=t; break;
        case 3: r=p; g=q; b=value; break;
        case 4: r=t; g=p; b=value; break;
        default: r=value; g=p; b=q; break;
    }
    return {static_cast<std::uint8_t>(std::lround(r * 255)),
            static_cast<std::uint8_t>(std::lround(g * 255)),
            static_cast<std::uint8_t>(std::lround(b * 255))};
}

// Stable integer noise independent of render frequency and frame history.
inline std::uint32_t sparkle_hash(std::uint32_t value)
{
    value ^= value >> 16;
    value *= 0x7feb352du;
    value ^= value >> 15;
    value *= 0x846ca68bu;
    value ^= value >> 16;
    return value;
}

inline std::vector<KeyColor> effect_frame(Effect effect, double elapsed_seconds,
                                           double speed = 1.0)
{
    std::vector<KeyColor> result;
    result.reserve(key_count);
    const double time = elapsed_seconds * std::max(0.1, std::min(4.0, speed));
    for(std::size_t i=0; i<key_count; ++i)
    {
        const std::uint8_t hid = hid_codes[i];
        double x = static_cast<double>(i) / key_count;
        double y = 0.5;
        for(const auto& position : positions)
            if(position.usage == hid)
            {
                x = (position.column + 0.5) / layout_columns;
                y = (position.row + 0.5) / layout_rows;
                break;
            }
        Color color{0,0,0};
        switch(effect)
        {
            case Effect::Rainbow:
                color = hsv(x - time * 0.12, 1.0, 1.0);
                break;
            case Effect::Wave:
                color = hsv(x - time * 0.22, 1.0,
                    0.25 + 0.75 * (0.5 + 0.5 * std::sin(2.0 * pi * (x - time * 0.5))));
                break;
            case Effect::Breathing:
                color = hsv(0.62, 0.95,
                    0.08 + 0.92 * (0.5 + 0.5 * std::cos(2.0 * pi * time * 0.35)));
                break;
            case Effect::Gradient:
                color = hsv(0.76 - 0.64 * x, 0.9, 1.0);
                break;
            case Effect::ColorShift:
                color = hsv(time * 0.10, 0.95, 1.0);
                break;
            case Effect::Comet:
            {
                const double center = std::fmod(time * 0.38, 1.4) - 0.2;
                const double trail = x - center;
                const double brightness = trail < 0.0
                    ? std::exp(trail * 13.0)
                    : std::exp(-trail * 55.0);
                color = hsv(0.54 + y * 0.1, 0.95, brightness);
                break;
            }
            case Effect::Sparkle:
            {
                // Six discrete sparkle states per second; no RNG/device input.
                const auto slot = static_cast<std::uint32_t>(std::floor(time * 6.0));
                const auto random = sparkle_hash(
                    static_cast<std::uint32_t>(hid) * 0x9e3779b9u
                    ^ (slot + 1u) * 0x85ebca6bu);
                const bool bright = (random % 17u) == 0u;
                color = hsv(0.59 + x * 0.14, bright ? 0.15 : 0.90,
                            bright ? 1.0 : 0.08);
                break;
            }
            default: break;
        }
        result.push_back({hid, color});
    }
    return result;
}
} // namespace apex_mini
