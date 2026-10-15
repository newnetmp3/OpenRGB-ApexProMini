#pragma once
// Independent GG-style scene composition primitives. No vendor shader code.
// Pure C++17: no Qt, HID, evdev or hardware access.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "apex_pro_mini_effects.h"

namespace apex_mini {
namespace scene {
enum class Blend { Normal, Add, Multiply, Screen };
struct Layer {
    Color color;
    double opacity = 1.0;
    Blend blend = Blend::Normal;
};
inline std::uint8_t channel(double value) {
    return static_cast<std::uint8_t>(std::lround(std::clamp(value, 0.0, 255.0)));
}
inline Color combine(Color background, Layer layer) {
    const double a = std::clamp(layer.opacity, 0.0, 1.0);
    const auto blend_channel = [&](std::uint8_t b, std::uint8_t f) {
        double result = f;
        switch(layer.blend) {
        case Blend::Normal: break;
        case Blend::Add: result = std::min(255.0, double(b) + f); break;
        case Blend::Multiply: result = double(b) * f / 255.0; break;
        case Blend::Screen: result = 255.0 - (255.0-b)*(255.0-f)/255.0; break;
        }
        return channel(double(b)*(1.0-a) + result*a);
    };
    return {blend_channel(background.r, layer.color.r),
            blend_channel(background.g, layer.color.g),
            blend_channel(background.b, layer.color.b)};
}
inline double aperture(double x, double y, double center_x, double center_y,
                       double radius, double softness) {
    const double distance = std::hypot(x-center_x, y-center_y);
    if(radius <= 0) return 0;
    if(softness <= 0) return distance <= radius ? 1.0 : 0.0;
    return std::clamp((radius-distance)/softness, 0.0, 1.0);
}
} // namespace scene
} // namespace apex_mini
