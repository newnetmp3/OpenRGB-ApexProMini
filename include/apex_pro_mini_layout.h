#pragma once
#include "apex_pro_mini_protocol.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace apex_mini
{
// Owner-provided photo: physical ANSI keyboard with 61 visible keycaps.
// The firmware exposes 68 RGB HID addresses; seven represent variants / extra
// slots and must NOT be drawn as additional keycaps on this keyboard.
// The native OpenRGB grid cannot represent different keycap widths.
constexpr unsigned int layout_rows = 5;
constexpr unsigned int layout_columns = 15;
constexpr unsigned int unmapped = 0xFFFFFFFFu;
constexpr std::size_t physical_key_count = 61;

struct Placement { std::uint8_t usage; unsigned int row; unsigned int column; };
constexpr std::array<Placement, physical_key_count> positions = {{
    // Esc through Backspace: 14 physical keys.
    {41,0,0},{30,0,1},{31,0,2},{32,0,3},{33,0,4},{34,0,5},{35,0,6},
    {36,0,7},{37,0,8},{38,0,9},{39,0,10},{45,0,11},{46,0,12},{42,0,13},
    // Tab, QWERTY, brackets, backslash: 14.
    {43,1,0},{20,1,1},{26,1,2},{8,1,3},{21,1,4},{23,1,5},{28,1,6},
    {24,1,7},{12,1,8},{18,1,9},{19,1,10},{47,1,11},{48,1,12},{49,1,13},
    // Caps, ASDF, punctuation, Enter: 13.
    {57,2,0},{4,2,1},{22,2,2},{7,2,3},{9,2,4},{10,2,5},{11,2,6},
    {13,2,7},{14,2,8},{15,2,9},{51,2,10},{52,2,11},{40,2,12},
    // Both shifts, ZXCV, comma, period, slash: 12.
    {225,3,0},{29,3,1},{27,3,2},{6,3,3},{25,3,4},{5,3,5},
    {17,3,6},{16,3,7},{54,3,8},{55,3,9},{56,3,10},{229,3,11},
    // Ctrl / Win / Alt / Space / Alt / Win / SteelSeries / Ctrl: 8.
    {224,4,0},{227,4,1},{226,4,2},{44,4,7},
    {230,4,10},{231,4,11},{240,4,12},{228,4,13}
}};

// These firmware/ISO usages are intentionally absent from the 61-key image;
// they remain separate addressable LEDs in OpenRGB's dropdown and transport.
constexpr std::array<std::uint8_t, key_count - physical_key_count> variant_usages = {{
    137, 50, 100, 135, 139, 138, 136
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
    for(const auto hid : variant_usages)
    {
        std::size_t idx = key_count;
        for(std::size_t i = 0; i < key_count; ++i)
            if(hid_codes[i] == hid) { idx = i; break; }
        if(idx == key_count || seen[idx])
            throw std::logic_error("variant usage overlaps physical matrix");
        seen[idx] = true;
    }
    for(bool value : seen)
        if(!value) throw std::logic_error("missing physical or variant LED");
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
        case 240: return "SteelSeries Key (provisional HID)";
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
