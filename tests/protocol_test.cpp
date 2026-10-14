#include "apex_pro_mini_protocol.h"
#include <cassert>
#include <stdexcept>

int main()
{
    using namespace apex_mini;
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
