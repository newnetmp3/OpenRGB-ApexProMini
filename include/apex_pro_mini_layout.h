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
constexpr unsigned int layout_columns = 32;
constexpr unsigned int unmapped = 0xFFFFFFFFu;

struct Placement { std::uint8_t usage; unsigned int row; unsigned int column; };
// Grid columns approximate half-key increments, with nearby spare cells used
// to display regional/firmware alternative usage IDs distinctly.
constexpr std::array<Placement, key_count> positions = {{
    {41,0,1},{30,0,3},{31,0,5},{32,0,7},{33,0,9},{34,0,11},
    {35,0,13},{36,0,15},{37,0,17},{38,0,19},{39,0,21},
    {45,0,23},{46,0,25},{137,0,27},{42,0,29},
    {43,1,2},{20,1,4},{26,1,6},{8,1,8},{21,1,10},{23,1,12},
    {28,1,14},{24,1,16},{12,1,18},{18,1,20},{19,1,22},
    {47,1,24},{48,1,26},{49,1,29},
    {57,2,2},{4,2,5},{22,2,7},{7,2,9},{9,2,11},{10,2,13},
    {11,2,15},{13,2,17},{14,2,19},{15,2,21},{51,2,23},
    {52,2,25},{50,2,27},{40,2,29},
    {225,3,2},{100,3,4},{29,3,6},{27,3,8},{6,3,10},{25,3,12},
    {5,3,14},{17,3,16},{16,3,18},{54,3,20},{55,3,22},
    {56,3,24},{135,3,27},{229,3,29},
    {224,4,1},{227,4,4},{226,4,7},{139,4,9},{44,4,15},
    {138,4,21},{136,4,23},{230,4,25},{231,4,27},
    {240,4,29},{228,4,31}
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
