#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace apex_mini
{
constexpr std::uint16_t vendor_id = 0x1038;
constexpr std::uint16_t product_id = 0x161e;
constexpr std::size_t feature_bytes = 513;
constexpr std::size_t output_bytes = 65;
constexpr std::size_t key_count = 68;

constexpr std::array<std::uint8_t, key_count> hid_codes = {{
    41, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 45, 46, 137,
    43, 20, 26, 8, 21, 23, 28, 24, 12, 18, 19, 47, 48, 49,
    57, 4, 22, 7, 9, 10, 11, 13, 14, 15, 51, 52, 50, 40,
    225, 100, 29, 27, 6, 25, 5, 17, 16, 54, 55, 56, 135, 229,
    224, 227, 226, 139, 44, 138, 136, 230, 231, 240, 228, 42
}};

struct Color { std::uint8_t r, g, b; };
struct KeyColor { std::uint8_t hid; Color color; };

inline bool supported(std::uint8_t hid)
{
    for(const auto key : hid_codes)
        if(key == hid) return true;
    return false;
}

inline std::array<std::uint8_t, feature_bytes> encode_direct(const std::vector<KeyColor>& keys)
{
    if(keys.size() > key_count) throw std::invalid_argument("too many keys");
    std::array<std::uint8_t, feature_bytes> report{};
    std::array<bool, 256> used{};
    report[1] = 0x40;
    report[2] = static_cast<std::uint8_t>(keys.size());
    for(std::size_t i = 0; i < keys.size(); ++i)
    {
        const auto& key = keys[i];
        if(!supported(key.hid) || used[key.hid])
            throw std::invalid_argument("unsupported or duplicate HID key");
        used[key.hid] = true;
        const auto offset = 3 + i * 4;
        report[offset] = key.hid;
        report[offset + 1] = key.color.r;
        report[offset + 2] = key.color.g;
        report[offset + 3] = key.color.b;
    }
    return report;
}

inline std::array<std::uint8_t, output_bytes> encode_release()
{
    std::array<std::uint8_t, output_bytes> report{};
    report[1] = 0x41;
    return report;
}
} // namespace apex_mini
