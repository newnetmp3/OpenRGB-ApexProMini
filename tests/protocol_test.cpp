#include "apex_pro_mini_protocol.h"
#include "apex_pro_mini_layout.h"
#include <cassert>
#include <stdexcept>

int main()
{
    using namespace apex_mini;
    const auto matrix = keyboard_matrix();
    std::array<bool, key_count> visited{};
    for(auto idx : matrix)
    {
        if(idx == unmapped) continue;
        assert(idx < key_count && !visited[idx]);
        visited[idx] = true;
    }
    unsigned int displayed = 0;
    for(bool included : visited) if(included) ++displayed;
    assert(displayed == physical_key_count);
    for(auto hid : variant_usages)
    {
        unsigned int idx = key_count;
        for(unsigned int i = 0; i < key_count; ++i)
            if(hid_codes[i] == hid) { idx = i; break; }
        assert(idx < key_count && !visited[idx]);
    }
    static_assert(layout_rows == 5 && layout_columns == 15, "photo-based keyboard grid expected");
    const std::array<unsigned int, layout_rows> expected_row_leds{{14, 14, 13, 12, 8}};
    for(unsigned int row = 0; row < layout_rows; ++row)
    {
        unsigned int count = 0;
        for(unsigned int col = 0; col < layout_columns; ++col)
            if(matrix[row * layout_columns + col] != unmapped) ++count;
        assert(count == expected_row_leds[row]);
    }
    assert(matrix[4 * layout_columns + 7] != unmapped);
    assert(hid_codes[matrix[4 * layout_columns + 7]] == 44);
    assert(key_label(4) == "A" && key_label(44) == "Space");
    static_assert(hid_codes.size() == 68, "68 HID keys required");
    assert(supported(4) && supported(41) && !supported(0));
    const auto frame = encode_direct({{4, {255, 0, 0}}, {22, {0, 255, 0}}});
    assert(frame.size() == 513);
    assert(frame[0] == 0 && frame[1] == 0x40 && frame[2] == 2);
    assert(frame[3] == 4 && frame[4] == 255 && frame[5] == 0 && frame[6] == 0);
    assert(frame[7] == 22 && frame[8] == 0 && frame[9] == 255 && frame[10] == 0);
    for(std::size_t i = 11; i < frame.size(); ++i) assert(frame[i] == 0);
    std::vector<KeyColor> all;
    for(auto hid : hid_codes) all.push_back({hid, {1, 2, 3}});
    const auto full = encode_direct(all);
    assert(full[2] == 68 && full[3 + 67 * 4] == hid_codes[67]);
    const auto clear = encode_release();
    assert(clear.size() == 65 && clear[0] == 0 && clear[1] == 0x41);
    for(std::size_t i = 2; i < clear.size(); ++i) assert(clear[i] == 0);
    bool threw = false;
    try { encode_direct({{4, {0, 0, 0}}, {4, {0, 0, 0}}}); }
    catch(const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { encode_direct({{0, {0, 0, 0}}}); }
    catch(const std::invalid_argument&) { threw = true; }
    assert(threw);
}
