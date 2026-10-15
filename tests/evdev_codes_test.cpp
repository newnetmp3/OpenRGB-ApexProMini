#include "apex_pro_mini_evdev_codes.h"
#include <cassert>
#ifdef __linux__
#include <linux/input-event-codes.h>
int main()
{
    using namespace apex_mini;
    assert(linux_to_hid(KEY_A) == 4);
    assert(linux_to_hid(KEY_S) == 22);
    assert(linux_to_hid(KEY_0) == 39);
    assert(linux_to_hid(KEY_ESC) == 41);
    assert(linux_to_hid(KEY_SPACE) == 44);
    assert(linux_to_hid(KEY_ENTER) == 40);
    assert(linux_to_hid(KEY_LEFTSHIFT) == 225);
    assert(linux_to_hid(KEY_RIGHTSHIFT) == 229);
    assert(linux_to_hid(KEY_LEFTMETA) == 227);
    assert(linux_to_hid(KEY_RIGHTMETA) == 231);
    assert(linux_to_hid(KEY_102ND) == 100);
    assert(linux_to_hid(KEY_F12) == -1);
    for(auto code : {KEY_A, KEY_S, KEY_0, KEY_ENTER, KEY_SPACE})
        assert(supported(static_cast<std::uint8_t>(linux_to_hid(code))));
}
#else
int main() { return 0; }
#endif
