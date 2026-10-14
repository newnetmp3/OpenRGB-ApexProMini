#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace apex_mini
{
// Independently reconstructed Prism-style parameters, NOT vendor-identical.
// Historical "redPulse" is a renamed alias for "faze"; kept as an entry.
struct PaletteStop { unsigned int at; std::uint32_t rgb; };
struct FactoryPreset
{
    const char* id;
    unsigned int period_ms;
    int wavelength;
    unsigned int geometry;
    unsigned int source_variants;
    const PaletteStop* stops;
    std::size_t stop_count;
};
constexpr std::array<PaletteStop, 5> factory_stops_1 = {{{0,0xff0040u},{64,0xff00ffu},{128,0x0000ffu},{192,0x00ffa0u},{255,0xff0040u}}};
constexpr std::array<PaletteStop, 4> factory_stops_2 = {{{0,0xff2000u},{230,0xffaf00u},{245,0x05b1ffu},{255,0x05b1ffu}}};
constexpr std::array<PaletteStop, 1> factory_stops_3 = {{{0,0xff00ffu}}};
constexpr std::array<PaletteStop, 7> factory_stops_4 = {{{0,0xff0000u},{42,0xffff00u},{85,0x00ff00u},{127,0x00ffffu},{170,0x0000ffu},{212,0xff00ffu},{255,0xff0000u}}};
constexpr std::array<PaletteStop, 1> factory_stops_5 = {{{0,0xffffffu}}};
constexpr std::array<PaletteStop, 1> factory_stops_6 = {{{0,0xffffffu}}};
constexpr std::array<PaletteStop, 1> factory_stops_7 = {{{0,0xffffffu}}};
constexpr std::array<PaletteStop, 13> factory_stops_8 = {{{0,0x000000u},{13,0xffd500u},{51,0xffd500u},{64,0x000000u},{107,0x000000u},{128,0xff8000u},{143,0x000000u},{158,0x00eeffu},{173,0x000000u},{189,0xff0000u},{204,0x000000u},{219,0xff66d4u},{255,0x000000u}}};
constexpr std::array<PaletteStop, 4> factory_stops_9 = {{{0,0x000000u},{38,0xffffffu},{77,0xffffffu},{255,0x000000u}}};
constexpr std::array<PaletteStop, 8> factory_stops_10 = {{{0,0x000000u},{26,0x50a000u},{39,0x081000u},{77,0x285000u},{102,0x6edc00u},{153,0x408000u},{204,0x64c800u},{220,0x80ff00u}}};
constexpr std::array<PaletteStop, 7> factory_stops_11 = {{{23,0x01003du},{52,0xffea00u},{93,0x00ccffu},{177,0x00ffffu},{188,0xe1ff00u},{215,0xff0400u},{237,0x66005au}}};
constexpr std::array<PaletteStop, 6> factory_stops_12 = {{{0,0xff0400u},{46,0xf7ff00u},{85,0x04ff00u},{125,0x00ffffu},{170,0x1100ffu},{217,0xb700ffu}}};
constexpr std::array<PaletteStop, 3> factory_stops_13 = {{{0,0x82008fu},{85,0xea0c5fu},{170,0xff8820u}}};
constexpr std::array<PaletteStop, 7> factory_stops_14 = {{{0,0xff0000u},{36,0x00ff00u},{73,0xff3200u},{109,0x00ffffu},{146,0xff00ffu},{182,0x0000ffu},{219,0xff0000u}}};
constexpr std::array<PaletteStop, 3> factory_stops_15 = {{{24,0xffea00u},{118,0xd500ffu},{229,0xff0000u}}};
constexpr std::array<PaletteStop, 9> factory_stops_16 = {{{0,0xff0400u},{20,0xff0400u},{85,0xffffffu},{90,0x001effu},{114,0x001effu},{168,0xffffffu},{173,0xffed1fu},{197,0xfff705u},{249,0xffffffu}}};
constexpr std::array<PaletteStop, 4> factory_stops_17 = {{{1,0x0f0a00u},{75,0xff6900u},{125,0xff3700u},{205,0x0f0a00u}}};
constexpr std::array<PaletteStop, 11> factory_stops_18 = {{{1,0x8a26d9u},{25,0x0085ffu},{48,0x7fff00u},{70,0xffea00u},{90,0xff1900u},{112,0xffffffu},{135,0xff1900u},{155,0xffea00u},{178,0x7fff00u},{200,0x0085ffu},{220,0x8a26d9u}}};
constexpr std::array<PaletteStop, 9> factory_stops_19 = {{{1,0x183d00u},{42,0x80ff00u},{61,0x0dff00u},{85,0x80ff00u},{125,0x183d00u},{173,0xa2ff00u},{196,0xccff00u},{218,0x88ff00u},{252,0x183d00u}}};
constexpr std::array<PaletteStop, 2> factory_stops_20 = {{{0,0xff0000u},{127,0x000000u}}};
constexpr std::array<PaletteStop, 7> factory_stops_21 = {{{0,0x009dffu},{63,0x0026ffu},{112,0x000000u},{160,0x00bdffu},{183,0x00b1f2u},{201,0x000000u},{249,0x000000u}}};
constexpr std::array<PaletteStop, 9> factory_stops_22 = {{{100,0x000000u},{110,0xff0000u},{120,0xff7f00u},{130,0xffff00u},{140,0x00ff00u},{150,0x0000ffu},{160,0x4b0082u},{170,0x9400ffu},{180,0x000000u}}};
constexpr std::array<PaletteStop, 8> factory_stops_23 = {{{60,0x000000u},{80,0xff0000u},{100,0xffff00u},{120,0x00ff00u},{140,0x00ffffu},{160,0x0000ffu},{180,0xff00ffu},{200,0x000000u}}};
constexpr std::array<PaletteStop, 3> factory_stops_24 = {{{80,0x000000u},{125,0x00ff78u},{170,0x000000u}}};
constexpr std::array<PaletteStop, 4> factory_stops_26 = {{{68,0x000000u},{94,0xffffffu},{120,0x00eaffu},{170,0x000000u}}};
constexpr std::array<PaletteStop, 3> factory_stops_27 = {{{0,0xff0000u},{81,0xff6600u},{171,0xff0000u}}};
constexpr std::array<PaletteStop, 3> factory_stops_28 = {{{0,0xd60071u},{78,0x9b59b6u},{174,0xd60071u}}};
constexpr std::array<PaletteStop, 2> factory_stops_30 = {{{0,0xff0000u},{127,0x000000u}}};
constexpr std::array<FactoryPreset, 31> factory_presets = {{
    {"steelseriesOrange", 0u, 0, 0u, 1u, nullptr, 0},
    {"vaporDreams", 30000u, -8, 3u, 1u, factory_stops_1.data(), 5},
    {"westCoast", 0u, -24, 1u, 1u, factory_stops_2.data(), 4},
    {"haze", 5000u, 0, 0u, 1u, factory_stops_3.data(), 1},
    {"prism", 7500u, 32, 0u, 1u, factory_stops_4.data(), 7},
    {"reactiveLine", 500u, 16, 2u, 1u, factory_stops_5.data(), 1},
    {"reactiveRipple", 250u, 3, 1u, 1u, factory_stops_6.data(), 1},
    {"reactiveSingleKey", 1250u, 0, 2u, 1u, factory_stops_7.data(), 1},
    {"chasingGhosts", 9500u, 48, 0u, 1u, factory_stops_8.data(), 13},
    {"warpDrive", 1200u, 6, 1u, 1u, factory_stops_9.data(), 4},
    {"wabashAndLake", 2100u, -25, 3u, 4u, factory_stops_10.data(), 8},
    {"solar", 31679u, 64, 3u, 1u, factory_stops_11.data(), 7},
    {"rainbow", 6120u, 89, 0u, 7u, factory_stops_12.data(), 6},
    {"staticFade", 0u, 12, 1u, 1u, factory_stops_13.data(), 3},
    {"discoMode", 750u, 2, 1u, 1u, factory_stops_14.data(), 7},
    {"colorFusion", 5038u, 64, 3u, 3u, factory_stops_15.data(), 3},
    {"shavedIce", 14071u, -51, 3u, 1u, factory_stops_16.data(), 9},
    {"selfDestruct", 1679u, 64, 0u, 1u, factory_stops_17.data(), 4},
    {"clown", 1679u, 64, 1u, 1u, factory_stops_18.data(), 11},
    {"radioactive", 8000u, 51, 1u, 1u, factory_stops_19.data(), 9},
    {"faze", 3185u, 0, 0u, 1u, factory_stops_20.data(), 2},
    {"aqua", 4000u, 64, 1u, 6u, factory_stops_21.data(), 7},
    {"rainbowSplit", 2400u, 24, 0u, 1u, factory_stops_22.data(), 9},
    {"drain", 2774u, -36, 1u, 1u, factory_stops_23.data(), 8},
    {"freeway", 1679u, 64, 0u, 7u, factory_stops_24.data(), 3},
    {"disabled", 0u, 0, 0u, 1u, nullptr, 0},
    {"comet", 1679u, 64, 0u, 6u, factory_stops_26.data(), 4},
    {"pride", 4690u, 64, 0u, 7u, factory_stops_27.data(), 3},
    {"twilight", 4690u, 64, 0u, 4u, factory_stops_28.data(), 3},
    {"heatmap", 0u, 0, 0u, 1u, nullptr, 0},
    {"redPulse", 3185u, 0, 0u, 1u, factory_stops_30.data(), 2},
}};
} // namespace apex_mini
