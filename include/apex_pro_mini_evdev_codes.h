#pragma once
#include "apex_pro_mini_protocol.h"
#ifdef __linux__
#include <linux/input-event-codes.h>
namespace apex_mini
{
inline int linux_to_hid(unsigned int code)
{
    switch(code)
    {
        case KEY_ESC: return 41;
        case KEY_1: case KEY_2: case KEY_3: case KEY_4: case KEY_5:
        case KEY_6: case KEY_7: case KEY_8: case KEY_9: return 30 + code - KEY_1;
        case KEY_0: return 39;
        case KEY_MINUS: return 45; case KEY_EQUAL: return 46;
        case KEY_BACKSPACE: return 42; case KEY_TAB: return 43;
        case KEY_Q: return 20; case KEY_W: return 26; case KEY_E: return 8;
        case KEY_R: return 21; case KEY_T: return 23; case KEY_Y: return 28;
        case KEY_U: return 24; case KEY_I: return 12; case KEY_O: return 18;
        case KEY_P: return 19; case KEY_LEFTBRACE: return 47;
        case KEY_RIGHTBRACE: return 48; case KEY_BACKSLASH: return 49;
        case KEY_ENTER: return 40; case KEY_LEFTCTRL: return 224;
        case KEY_A: return 4; case KEY_S: return 22; case KEY_D: return 7;
        case KEY_F: return 9; case KEY_G: return 10; case KEY_H: return 11;
        case KEY_J: return 13; case KEY_K: return 14; case KEY_L: return 15;
        case KEY_SEMICOLON: return 51; case KEY_APOSTROPHE: return 52;
        case KEY_CAPSLOCK: return 57; case KEY_LEFTSHIFT: return 225;
        case KEY_Z: return 29; case KEY_X: return 27; case KEY_C: return 6;
        case KEY_V: return 25; case KEY_B: return 5; case KEY_N: return 17;
        case KEY_M: return 16; case KEY_COMMA: return 54; case KEY_DOT: return 55;
        case KEY_SLASH: return 56; case KEY_RIGHTSHIFT: return 229;
        case KEY_LEFTALT: return 226; case KEY_SPACE: return 44;
        case KEY_RIGHTALT: return 230; case KEY_RIGHTCTRL: return 228;
        case KEY_LEFTMETA: return 227; case KEY_RIGHTMETA: return 231;
        case KEY_102ND: return 100;
        default: return -1;
    }
}

}
#endif
