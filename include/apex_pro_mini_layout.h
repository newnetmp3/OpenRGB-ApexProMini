#pragma once
#include "apex_pro_mini_protocol.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace apex_mini
{
// Provisional ANSI-like presentation only: physical coordinates and regional
// variants still require on-device validation. This does NOT change HID writes.
constexpr unsigned int layout_rows = 5;
constexpr unsigned int layout_columns = 16;
constexpr unsigned int unmapped = 0xFFFFFFFFu;

struct Placement { std::uint8_t usage; unsigned int row; unsigned int column; };
// OpenRGB's native matrix paints uniform square LED cells. Keep the 68 LEDs
// in five compact rows instead of showing a sparse 32-column checkerboard.
// Alternative firmware/ISO usages remain visible as their own cells; no claim
// is made that these represent 68 independent physical keycaps.
constexpr std::array<Placement, key_count> positions = {{
    {41,0,0},{30,0,1},{31,0,2},{32,0,3},{33,0,4},{34,0,5},{35,0,6},{36,0,7},{37,0,8},{38,0,9},{39,0,10},{45,0,11},{46,0,12},{137,0,13},{42,0,14},
    {43,1,1},{20,1,2},{26,1,3},{8,1,4},{21,1,5},{23,1,6},{28,1,7},{24,1,8},{12,1,9},{18,1,10},{19,1,11},{47,1,12},{48,1,13},{49,1,14},
    {57,2,1},{4,2,2},{22,2,3},{7,2,4},{9,2,5},{10,2,6},{11,2,7},{13,2,8},{14,2,9},{15,2,10},{51,2,11},{52,2,12},{50,2,13},{40,2,14},
    {225,3,1},{100,3,2},{29,3,3},{27,3,4},{6,3,5},{25,3,6},{5,3,7},{17,3,8},{16,3,9},{54,3,10},{55,3,11},{56,3,12},{135,3,13},{229,3,14},
    {224,4,0},{227,4,1},{226,4,2},{139,4,3},{44,4,7},{138,4,10},{136,4,11},{230,4,12},{231,4,13},{240,4,14},{228,4,15}
}};

inline std::array<unsigned int, layout_rows * layout_columns> keyboard_matrix()
{
    std::array<unsigned int, layout_rows * layout_columns> map{};
    map.fill(unmapped);
    std::array<bool, key_count> seen{};
    for(const auto& key : positions)
    {
        if(key.row >= layout_rows || key.column >= layout_columns)
            throw std::logic_error("keyboard layout out of bounds");
        std::size_t led_index = key_count;
        for(std::size_t i = 0; i < key_count; ++i)
            if(hid_codes[i] == key.usage) { led_index = i; break; }
        if(led_index == key_count || seen[led_index])
            throw std::logic_error("unknown or duplicate keyboard usage");
        const auto cell = key.row * layout_columns + key.column;
        if(map[cell] != unmapped) throw std::logic_error("overlapping keyboard positions");
        seen[led_index] = true;
        map[cell] = static_cast<unsigned int>(led_index);
    }
    for(bool value : seen)
        if(!value) throw std::logic_error("unmapped keyboard LED");
    return map;
}

inline std::string key_label(std::uint8_t usage)
{
    switch(usage)
    {
        case 41: return "Escape"; case 42: return "Backspace";
        case 43: return "Tab"; case 44: return "Space"; case 40: return "Enter";
        case 57: return "Caps Lock"; case 225: return "Left Shift";
        case 229: return "Right Shift"; case 224: return "Left Ctrl";
        case 228: return "Right Ctrl"; case 226: return "Left Alt";
        case 230: return "Right Alt"; case 227: return "Left GUI";
        case 231: return "Right GUI"; case 100: return "ISO Backslash";
        case 50: return "Non-US Hash"; case 137: return "Alternate Backspace";
        case 135: return "Alternate Shift"; case 139: return "International 4";
        case 138: return "International 3"; case 136: return "International 1";
        case 240: return "Keyboard LANG 1";
        default: break;
    }
    if(usage >= 4 && usage <= 29) return std::string(1, static_cast<char>('A' + usage - 4));
    if(usage >= 30 && usage <= 38) return std::string(1, static_cast<char>('1' + usage - 30));
    if(usage == 39) return "0";
    switch(usage)
    {
        case 45: return "Minus"; case 46: return "Equals";
        case 47: return "Left Bracket"; case 48: return "Right Bracket";
        case 49: return "Backslash"; case 51: return "Semicolon";
        case 52: return "Apostrophe"; case 54: return "Comma";
        case 55: return "Period"; case 56: return "Slash";
        default: return "HID " + std::to_string(usage);
    }
}
} // namespace apex_mini
