#pragma once
#include "apex_pro_mini_factory.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
namespace apex_mini
{
struct RegionPreset
{
    const char* id;
    unsigned int row_mask;
    const unsigned char* columns;
    std::size_t column_count;
    unsigned int period_ms;
    int wavelength;
    unsigned int geometry;
    const PaletteStop* stops;
    std::size_t stop_count;
};
constexpr std::array<PaletteStop, 3> region_stops_0 = {{{0,0xff00e1u},{30,0x000000u},{61,0xff0400u}}};
constexpr std::array<PaletteStop, 3> region_stops_1 = {{{0,0xff0000u},{30,0x000000u},{61,0xffea00u}}};
constexpr std::array<PaletteStop, 3> region_stops_2 = {{{0,0xffea00u},{30,0x000000u},{61,0x26ff00u}}};
constexpr std::array<PaletteStop, 3> region_stops_3 = {{{0,0x1eff00u},{30,0x000000u},{61,0x001effu}}};
constexpr std::array<PaletteStop, 3> region_stops_4 = {{{0,0x0000ffu},{30,0x000000u},{61,0x7b00ffu}}};
constexpr std::array<PaletteStop, 3> region_stops_5 = {{{0,0x9500ffu},{30,0x000000u},{61,0xff00ffu}}};
constexpr std::array<PaletteStop, 3> region_stops_6 = {{{3,0xff0000u},{105,0xff00e6u},{218,0xffea00u}}};
constexpr std::array<PaletteStop, 3> region_stops_7 = {{{0,0xff0000u},{127,0xea00ffu},{220,0xffea00u}}};
constexpr std::array<PaletteStop, 7> region_stops_8 = {{{0,0x009dffu},{51,0x0026ffu},{89,0x005e80u},{122,0x00bbffu},{183,0x00b1f2u},{201,0x000000u},{249,0x000000u}}};
constexpr std::array<PaletteStop, 8> region_stops_9 = {{{0,0x009dffu},{63,0x0026ffu},{76,0x000000u},{102,0x000000u},{160,0x00bbffu},{183,0x00b1f2u},{201,0x000000u},{249,0x000000u}}};
constexpr std::array<PaletteStop, 8> region_stops_10 = {{{0,0x009dffu},{63,0x0026ffu},{76,0x000000u},{102,0x000000u},{160,0x00bbffu},{183,0x00b1f2u},{201,0x000000u},{249,0x000000u}}};
constexpr std::array<PaletteStop, 8> region_stops_11 = {{{0,0x009dffu},{63,0x0026ffu},{76,0x000000u},{102,0x000000u},{160,0x00bbffu},{183,0x00b1f2u},{201,0x000000u},{249,0x000000u}}};
constexpr std::array<PaletteStop, 8> region_stops_12 = {{{0,0x009dffu},{63,0x0026ffu},{76,0x000000u},{102,0x000000u},{160,0x00bbffu},{183,0x00b1f2u},{201,0x000000u},{249,0x000000u}}};
constexpr std::array<PaletteStop, 3> region_stops_13 = {{{80,0x000000u},{125,0xff0000u},{170,0x000000u}}};
constexpr std::array<PaletteStop, 3> region_stops_14 = {{{80,0x000000u},{125,0xffff00u},{170,0x000000u}}};
constexpr std::array<PaletteStop, 3> region_stops_15 = {{{80,0x000000u},{125,0x00ff00u},{170,0x000000u}}};
constexpr std::array<PaletteStop, 3> region_stops_16 = {{{80,0x000000u},{125,0x00ffffu},{170,0x000000u}}};
constexpr std::array<PaletteStop, 3> region_stops_17 = {{{80,0x000000u},{125,0x0000ffu},{170,0x000000u}}};
constexpr std::array<PaletteStop, 3> region_stops_18 = {{{80,0x000000u},{125,0xb700ffu},{170,0x000000u}}};
constexpr std::array<PaletteStop, 4> region_stops_19 = {{{190,0x000000u},{210,0xe890fcu},{230,0xffffffu},{255,0x000000u}}};
constexpr std::array<PaletteStop, 4> region_stops_20 = {{{150,0x000000u},{164,0x000000u},{199,0x00ffe9u},{230,0xffffffu}}};
constexpr std::array<PaletteStop, 4> region_stops_21 = {{{20,0x000000u},{40,0x909bffu},{80,0xffffffu},{120,0x000000u}}};
constexpr std::array<PaletteStop, 4> region_stops_22 = {{{80,0x000000u},{110,0xffa81fu},{130,0xffd796u},{170,0x000000u}}};
constexpr std::array<PaletteStop, 4> region_stops_23 = {{{130,0x000000u},{155,0x96ffd7u},{190,0x9dd75au},{207,0x000000u}}};
constexpr std::array<PaletteStop, 3> region_stops_24 = {{{0,0x7f00ffu},{77,0xff0000u},{170,0x7f00ffu}}};
constexpr std::array<PaletteStop, 3> region_stops_25 = {{{0,0x0000ffu},{83,0x7f00ffu},{168,0x0000ffu}}};
constexpr std::array<PaletteStop, 3> region_stops_26 = {{{14,0x00ff00u},{84,0x0000ffu},{181,0x00ff00u}}};
constexpr std::array<PaletteStop, 3> region_stops_27 = {{{0,0xffff00u},{96,0x00ff00u},{177,0xffff00u}}};
constexpr std::array<PaletteStop, 3> region_stops_28 = {{{0,0xff0000u},{93,0xff6600u},{181,0xff0000u}}};
constexpr std::array<PaletteStop, 3> region_stops_29 = {{{0,0xff6600u},{85,0xffea00u},{172,0xff6600u}}};
constexpr std::array<PaletteStop, 3> region_stops_30 = {{{0,0xd60071u},{87,0x9b59b6u},{166,0xd60071u}}};
constexpr std::array<PaletteStop, 3> region_stops_31 = {{{0,0x9b59b6u},{80,0x0038a8u},{171,0x9b59b6u}}};
constexpr std::array<PaletteStop, 3> region_stops_32 = {{{0,0x0038a8u},{79,0xd60071u},{172,0x0038a8u}}};
constexpr std::array<unsigned char, 6> region_columns_33 = {{1,6,8,10,13,16}};
constexpr std::array<PaletteStop, 8> region_stops_33 = {{{0,0x326400u},{40,0x081000u},{51,0x4e9b00u},{102,0x80ff00u},{127,0x000000u},{153,0x408000u},{204,0x80ff00u},{242,0x0b1600u}}};
constexpr std::array<unsigned char, 5> region_columns_34 = {{2,7,11,14,17}};
constexpr std::array<PaletteStop, 8> region_stops_34 = {{{0,0x050a00u},{51,0x081000u},{64,0x50a000u},{77,0x6fde00u},{102,0x1c3700u},{153,0x408000u},{204,0x64c800u},{242,0x000000u}}};
constexpr std::array<unsigned char, 5> region_columns_35 = {{3,5,9,12,19}};
constexpr std::array<PaletteStop, 8> region_stops_35 = {{{0,0x000000u},{51,0x081000u},{64,0x50a000u},{77,0x285000u},{102,0x6edc00u},{153,0x408000u},{204,0x000000u},{242,0x80ff00u}}};
constexpr std::array<RegionPreset, 36> region_variants = {{
    {"rainbow", 2u, nullptr, 0, 6120u, 89, 0u, region_stops_0.data(), 3},
    {"rainbow", 4u, nullptr, 0, 6120u, 89, 0u, region_stops_1.data(), 3},
    {"rainbow", 8u, nullptr, 0, 6120u, 89, 0u, region_stops_2.data(), 3},
    {"rainbow", 16u, nullptr, 0, 6120u, 89, 0u, region_stops_3.data(), 3},
    {"rainbow", 32u, nullptr, 0, 6120u, 89, 0u, region_stops_4.data(), 3},
    {"rainbow", 1u, nullptr, 0, 6120u, 89, 0u, region_stops_5.data(), 3},
    {"colorFusion", 7u, nullptr, 0, 1679u, 64, 0u, region_stops_6.data(), 3},
    {"colorFusion", 56u, nullptr, 0, 1679u, -64, 0u, region_stops_7.data(), 3},
    {"aqua", 2u, nullptr, 0, 4000u, -64, 0u, region_stops_8.data(), 7},
    {"aqua", 4u, nullptr, 0, 4000u, -64, 1u, region_stops_9.data(), 8},
    {"aqua", 8u, nullptr, 0, 4000u, -64, 1u, region_stops_10.data(), 8},
    {"aqua", 16u, nullptr, 0, 4000u, -64, 1u, region_stops_11.data(), 8},
    {"aqua", 32u, nullptr, 0, 4000u, -64, 1u, region_stops_12.data(), 8},
    {"freeway", 1u, nullptr, 0, 2000u, 64, 0u, region_stops_13.data(), 3},
    {"freeway", 2u, nullptr, 0, 2000u, -64, 0u, region_stops_14.data(), 3},
    {"freeway", 4u, nullptr, 0, 2000u, 64, 0u, region_stops_15.data(), 3},
    {"freeway", 8u, nullptr, 0, 2000u, -64, 0u, region_stops_16.data(), 3},
    {"freeway", 16u, nullptr, 0, 1679u, 64, 0u, region_stops_17.data(), 3},
    {"freeway", 32u, nullptr, 0, 1679u, -64, 0u, region_stops_18.data(), 3},
    {"comet", 2u, nullptr, 0, 1679u, -64, 0u, region_stops_19.data(), 4},
    {"comet", 4u, nullptr, 0, 1679u, 64, 0u, region_stops_20.data(), 4},
    {"comet", 8u, nullptr, 0, 1679u, 64, 0u, region_stops_21.data(), 4},
    {"comet", 16u, nullptr, 0, 1679u, 64, 0u, region_stops_22.data(), 4},
    {"comet", 32u, nullptr, 0, 1679u, -64, 0u, region_stops_23.data(), 4},
    {"pride", 32u, nullptr, 0, 4690u, 64, 0u, region_stops_24.data(), 3},
    {"pride", 16u, nullptr, 0, 4690u, 64, 0u, region_stops_25.data(), 3},
    {"pride", 8u, nullptr, 0, 4690u, 64, 0u, region_stops_26.data(), 3},
    {"pride", 4u, nullptr, 0, 4690u, 64, 0u, region_stops_27.data(), 3},
    {"pride", 1u, nullptr, 0, 4690u, 64, 0u, region_stops_28.data(), 3},
    {"pride", 2u, nullptr, 0, 4690u, 64, 0u, region_stops_29.data(), 3},
    {"twilight", 7u, nullptr, 0, 4690u, 64, 0u, region_stops_30.data(), 3},
    {"twilight", 8u, nullptr, 0, 4690u, 64, 0u, region_stops_31.data(), 3},
    {"twilight", 48u, nullptr, 0, 4690u, 64, 0u, region_stops_32.data(), 3},
    {"wabashAndLake", 63u, region_columns_33.data(), 6, 2100u, -16, 3u, region_stops_33.data(), 8},
    {"wabashAndLake", 63u, region_columns_34.data(), 5, 2100u, -19, 3u, region_stops_34.data(), 8},
    {"wabashAndLake", 63u, region_columns_35.data(), 5, 2100u, -6, 3u, region_stops_35.data(), 8},
}};
inline const RegionPreset* region_for(const char* id, double x, unsigned int physical_row)
{
    const auto source_row = physical_row + 1; // Source row 0 is absent on Mini.
    const auto source_column = static_cast<unsigned int>(x*21.0+0.5);
    for(const auto& region : region_variants)
    {
        if(std::strcmp(region.id,id) != 0 || !(region.row_mask & (1u<<source_row)))
            continue;
        if(region.column_count)
        {
            bool has_column = false;
            for(std::size_t i=0;i<region.column_count;++i)
                if(region.columns[i] == source_column) { has_column = true; break; }
            if(!has_column) continue;
        }
        return &region;
    }
    return nullptr;
}
} // namespace apex_mini
