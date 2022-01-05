/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "key_event_value_transformation.h"
#include "util.h"

namespace OHOS {
namespace MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
            LOG_CORE, MMI_LOG_DOMAIN, "KeyEventValueTransformations"
        };
    }
const std::multimap<int16_t, KeyEventValueTransformations> MAP_KEY_EVENT_VALUE_TRANSFORMATION = {
    {11, {"KEY_0", 11, 2000, HOS_KEY_0, MMI_NO_SYSTEM_KEY}},
    {2, {"KEY_1", 2, 2001, HOS_KEY_1, MMI_NO_SYSTEM_KEY}},
    {3, {"KEY_2", 3, 2002, HOS_KEY_2, MMI_NO_SYSTEM_KEY}},
    {4, {"KEY_3", 4, 2003, HOS_KEY_3, MMI_NO_SYSTEM_KEY}},
    {5, {"KEY_4", 5, 2004, HOS_KEY_4, MMI_NO_SYSTEM_KEY}},
    {6, {"KEY_5", 6, 2005, HOS_KEY_5, MMI_NO_SYSTEM_KEY}},
    {7, {"KEY_6", 7, 2006, HOS_KEY_6, MMI_NO_SYSTEM_KEY}},
    {8, {"KEY_7", 8, 2007, HOS_KEY_7, MMI_NO_SYSTEM_KEY}},
    {9, {"KEY_8", 9, 2008, HOS_KEY_8, MMI_NO_SYSTEM_KEY}},
    {10, {"KEY_9", 10, 2009, HOS_KEY_9, MMI_NO_SYSTEM_KEY}},
    {522, {"KEY_STAR", 522, 2010, HOS_KEY_STAR, MMI_NO_SYSTEM_KEY}},
    {523, {"KEY_POUND", 523, 2011, HOS_KEY_POUND, MMI_NO_SYSTEM_KEY}},
    {103, {"KEY_DPAD_UP", 103, 2012, HOS_KEY_DPAD_UP, MMI_NO_SYSTEM_KEY}},
    {108, {"KEY_DPAD_DOWN", 108, 2013, HOS_KEY_DPAD_DOWN, MMI_NO_SYSTEM_KEY}},
    {105, {"KEY_DPAD_LEFT", 105, 2014, HOS_KEY_DPAD_LEFT, MMI_NO_SYSTEM_KEY}},
    {106, {"KEY_DPAD_RIGHT", 106, 2015, HOS_KEY_DPAD_RIGHT, MMI_NO_SYSTEM_KEY}},
    {353, {"KEY_DPAD_CENTER", 353, 2016, HOS_KEY_DPAD_CENTER, MMI_NO_SYSTEM_KEY}},
    {30, {"KEY_A", 30, 2017, HOS_KEY_A, MMI_NO_SYSTEM_KEY}},
    {48, {"KEY_B", 48, 2018, HOS_KEY_B, MMI_NO_SYSTEM_KEY}},
    {46, {"KEY_C", 46, 2019, HOS_KEY_C, MMI_NO_SYSTEM_KEY}},
    {32, {"KEY_D", 32, 2020, HOS_KEY_D, MMI_NO_SYSTEM_KEY}},
    {18, {"KEY_E", 18, 2021, HOS_KEY_E, MMI_NO_SYSTEM_KEY}},
    {33, {"KEY_F", 33, 2022, HOS_KEY_F, MMI_NO_SYSTEM_KEY}},
    {34, {"KEY_G", 34, 2023, HOS_KEY_G, MMI_NO_SYSTEM_KEY}},
    {35, {"KEY_H", 35, 2024, HOS_KEY_H, MMI_NO_SYSTEM_KEY}},
    {23, {"KEY_I", 23, 2025, HOS_KEY_I, MMI_NO_SYSTEM_KEY}},
    {36, {"KEY_J", 36, 2026, HOS_KEY_J, MMI_NO_SYSTEM_KEY}},
    {37, {"KEY_K", 37, 2027, HOS_KEY_K, MMI_NO_SYSTEM_KEY}},
    {38, {"KEY_L", 38, 2028, HOS_KEY_L, MMI_NO_SYSTEM_KEY}},
    {50, {"KEY_M", 50, 2029, HOS_KEY_M, MMI_NO_SYSTEM_KEY}},
    {49, {"KEY_N", 49, 2030, HOS_KEY_N, MMI_NO_SYSTEM_KEY}},
    {24, {"KEY_O", 24, 2031, HOS_KEY_O, MMI_NO_SYSTEM_KEY}},
    {25, {"KEY_P", 25, 2032, HOS_KEY_P, MMI_NO_SYSTEM_KEY}},
    {16, {"KEY_Q", 16, 2033, HOS_KEY_Q, MMI_NO_SYSTEM_KEY}},
    {19, {"KEY_R", 19, 2034, HOS_KEY_R, MMI_NO_SYSTEM_KEY}},
    {31, {"KEY_S", 31, 2035, HOS_KEY_S, MMI_NO_SYSTEM_KEY}},
    {20, {"KEY_T", 20, 2036, HOS_KEY_T, MMI_NO_SYSTEM_KEY}},
    {22, {"KEY_U", 22, 2037, HOS_KEY_U, MMI_NO_SYSTEM_KEY}},
    {47, {"KEY_V", 47, 2038, HOS_KEY_V, MMI_NO_SYSTEM_KEY}},
    {17, {"KEY_W", 17, 2039, HOS_KEY_W, MMI_NO_SYSTEM_KEY}},
    {45, {"KEY_X", 45, 2040, HOS_KEY_X, MMI_NO_SYSTEM_KEY}},
    {21, {"KEY_Y", 21, 2041, HOS_KEY_Y, MMI_NO_SYSTEM_KEY}},
    {44, {"KEY_Z", 44, 2042, HOS_KEY_Z, MMI_NO_SYSTEM_KEY}},
    {51, {"KEY_COMMA", 51, 2043, HOS_KEY_COMMA, MMI_NO_SYSTEM_KEY}},
    {52, {"KEY_PERIOD", 52, 2044, HOS_KEY_PERIOD, MMI_NO_SYSTEM_KEY}},
    {56, {"KEY_ALT_LEFT", 56, 2045, HOS_KEY_ALT_LEFT, MMI_NO_SYSTEM_KEY}},
    {100, {"KEY_ALT_RIGHT", 100, 2046, HOS_KEY_ALT_RIGHT, MMI_NO_SYSTEM_KEY}},
    {42, {"KEY_SHIFT_LEFT", 42, 2047, HOS_KEY_SHIFT_LEFT, MMI_NO_SYSTEM_KEY}},
    {54, {"KEY_SHIFT_RIGHT", 54, 2048, HOS_KEY_SHIFT_RIGHT, MMI_NO_SYSTEM_KEY}},
    {15, {"KEY_TAB", 15, 2049, HOS_KEY_TAB, MMI_NO_SYSTEM_KEY}},
    {57, {"KEY_SPACE", 57, 2050, HOS_KEY_SPACE, MMI_NO_SYSTEM_KEY}},
    {150, {"KEY_EXPLORER", 150, 2052, HOS_KEY_EXPLORER, MMI_NO_SYSTEM_KEY}},
    {155, {"KEY_ENVELOPE", 155, 2053, HOS_KEY_ENVELOPE, MMI_NO_SYSTEM_KEY}},
    {28, {"KEY_ENTER", 28, 2054, HOS_KEY_ENTER, MMI_NO_SYSTEM_KEY}},
    {14, {"KEY_DEL", 14, 2055, HOS_KEY_DEL, MMI_NO_SYSTEM_KEY}},
    {41, {"KEY_GRAVE", 41, 2056, HOS_KEY_GRAVE, MMI_NO_SYSTEM_KEY}},
    {12, {"KEY_MINUS", 12, 2057, HOS_KEY_MINUS, MMI_NO_SYSTEM_KEY}},
    {13, {"KEY_EQUALS", 13, 2058, HOS_KEY_EQUALS, MMI_NO_SYSTEM_KEY}},
    {26, {"KEY_LEFT_BRACKET", 26, 2059, HOS_KEY_LEFT_BRACKET, MMI_NO_SYSTEM_KEY}},
    {27, {"KEY_RIGHT_BRACKET", 27, 2060, HOS_KEY_RIGHT_BRACKET, MMI_NO_SYSTEM_KEY}},
    {43, {"KEY_BACKSLASH", 43, 2061, HOS_KEY_BACKSLASH, MMI_NO_SYSTEM_KEY}},
    {39, {"KEY_SEMICOLON", 39, 2062, HOS_KEY_SEMICOLON, MMI_NO_SYSTEM_KEY}},
    {40, {"KEY_APOSTROPHE", 40, 2063, HOS_KEY_APOSTROPHE, MMI_NO_SYSTEM_KEY}},
    {53, {"KEY_SLASH", 53, 2064, HOS_KEY_SLASH, MMI_NO_SYSTEM_KEY}},
    {139, {"KEY_MENU", 139, 2067, HOS_KEY_MENU, MMI_SYSTEM_KEY}},
    {127, {"KEY_MENU", 127, 2067, HOS_KEY_MENU, MMI_SYSTEM_KEY}},
    {104, {"KEY_PAGE_UP", 104, 2068, HOS_KEY_PAGE_UP, MMI_NO_SYSTEM_KEY}},
    {109, {"KEY_PAGE_DOWN", 109, 2069, HOS_KEY_PAGE_DOWN, MMI_NO_SYSTEM_KEY}},
    {1, {"KEY_ESCAPE", 1, 2070, HOS_KEY_ESCAPE, MMI_NO_SYSTEM_KEY}},
    {111, {"KEY_FORWARD_DEL", 111, 2071, HOS_KEY_FORWARD_DEL, MMI_NO_SYSTEM_KEY}},
    {29, {"KEY_CTRL_LEFT", 29, 2072, HOS_KEY_CTRL_LEFT, MMI_NO_SYSTEM_KEY}},
    {97, {"KEY_CTRL_RIGHT", 97, 2073, HOS_KEY_CTRL_RIGHT, MMI_NO_SYSTEM_KEY}},
    {58, {"KEY_CAPS_LOCK", 58, 2074, HOS_KEY_CAPS_LOCK, MMI_NO_SYSTEM_KEY}},
    {70, {"KEY_SCROLL_LOCK", 70, 2075, HOS_KEY_SCROLL_LOCK, MMI_NO_SYSTEM_KEY}},
    {125, {"KEY_META_LEFT", 125, 2076, HOS_KEY_META_LEFT, MMI_NO_SYSTEM_KEY}},
    {126, {"KEY_META_RIGHT", 126, 2077, HOS_KEY_META_RIGHT, MMI_NO_SYSTEM_KEY}},
    {464, {"KEY_FUNCTION", 464, 2078, HOS_KEY_FUNCTION, MMI_NO_SYSTEM_KEY}},
    {99, {"KEY_SYSRQ", 99, 2079, HOS_KEY_SYSRQ, MMI_NO_SYSTEM_KEY}},
    {119, {"KEY_BREAK", 119, 2080, HOS_KEY_BREAK, MMI_NO_SYSTEM_KEY}},
    {102, {"KEY_MOVE_HOME", 102, 2081, HOS_KEY_MOVE_HOME, MMI_NO_SYSTEM_KEY}},
    {107, {"KEY_MOVE_END", 107, 2082, HOS_KEY_MOVE_END, MMI_NO_SYSTEM_KEY}}, // 107 maped into ENDCALL(4)
    {110, {"KEY_INSERT", 110, 2083, HOS_KEY_INSERT, MMI_NO_SYSTEM_KEY}},
    {159, {"KEY_FORWARD", 159, 2084, HOS_KEY_FORWARD, MMI_NO_SYSTEM_KEY}},
    {207, {"KEY_MEDIA_PLAY", 207, 2085, HOS_KEY_MEDIA_PLAY, MMI_SYSTEM_KEY}},
    {200, {"KEY_PLAY", 200, 2643, HOS_KEY_PLAY, MMI_NO_SYSTEM_KEY}},
    {201, {"KEY_MEDIA_PAUSE", 201, 2086, HOS_KEY_MEDIA_PAUSE, MMI_SYSTEM_KEY}},
    {160, {"KEY_MEDIA_CLOSE", 160, 2087, HOS_KEY_MEDIA_CLOSE, MMI_NO_SYSTEM_KEY}},
    {161, {"KEY_MEDIA_EJECT", 161, 2088, HOS_KEY_MEDIA_EJECT, MMI_NO_SYSTEM_KEY}},
    {167, {"KEY_MEDIA_RECORD", 167, 2089, HOS_KEY_MEDIA_RECORD, MMI_SYSTEM_KEY}},
    {59, {"KEY_F1", 59, 2090, HOS_KEY_F1, MMI_NO_SYSTEM_KEY}},
    {60, {"KEY_F2", 60, 2091, HOS_KEY_F2, MMI_NO_SYSTEM_KEY}},
    {61, {"KEY_F3", 61, 2092, HOS_KEY_F3, MMI_NO_SYSTEM_KEY}},
    {62, {"KEY_F4", 62, 2093, HOS_KEY_F4, MMI_NO_SYSTEM_KEY}},
    {63, {"KEY_F5", 63, 2094, HOS_KEY_F5, MMI_NO_SYSTEM_KEY}},
    {64, {"KEY_F6", 64, 2095, HOS_KEY_F6, MMI_NO_SYSTEM_KEY}},
    {65, {"KEY_F7", 65, 2096, HOS_KEY_F7, MMI_NO_SYSTEM_KEY}},
    {66, {"KEY_F8", 66, 2097, HOS_KEY_F8, MMI_NO_SYSTEM_KEY}},
    {67, {"KEY_F9", 67, 2098, HOS_KEY_F9, MMI_NO_SYSTEM_KEY}},
    {68, {"KEY_F10", 68, 2099, HOS_KEY_F10, MMI_NO_SYSTEM_KEY}},
    {87, {"KEY_F11", 87, 2100, HOS_KEY_F11, MMI_NO_SYSTEM_KEY}},
    {88, {"KEY_F12", 88, 2101, HOS_KEY_F12, MMI_NO_SYSTEM_KEY}},
    {69, {"KEY_NUM_LOCK", 69, 2102, HOS_KEY_NUM_LOCK, MMI_NO_SYSTEM_KEY}},
    {82, {"KEY_NUMPAD_0", 82, 2103, HOS_KEY_NUMPAD_0, MMI_NO_SYSTEM_KEY}},
    {79, {"KEY_NUMPAD_1", 79, 2104, HOS_KEY_NUMPAD_1, MMI_NO_SYSTEM_KEY}},
    {80, {"KEY_NUMPAD_2", 80, 2105, HOS_KEY_NUMPAD_2, MMI_NO_SYSTEM_KEY}},
    {81, {"KEY_NUMPAD_3", 81, 2106, HOS_KEY_NUMPAD_3, MMI_NO_SYSTEM_KEY}},
    {75, {"KEY_NUMPAD_4", 75, 2107, HOS_KEY_NUMPAD_4, MMI_NO_SYSTEM_KEY}},
    {76, {"KEY_NUMPAD_5", 76, 2108, HOS_KEY_NUMPAD_5, MMI_NO_SYSTEM_KEY}},
    {77, {"KEY_NUMPAD_6", 77, 2109, HOS_KEY_NUMPAD_6, MMI_NO_SYSTEM_KEY}},
    {71, {"KEY_NUMPAD_7", 71, 2110, HOS_KEY_NUMPAD_7, MMI_NO_SYSTEM_KEY}},
    {72, {"KEY_NUMPAD_8", 72, 2111, HOS_KEY_NUMPAD_8, MMI_NO_SYSTEM_KEY}},
    {73, {"KEY_NUMPAD_9", 73, 2112, HOS_KEY_NUMPAD_9, MMI_NO_SYSTEM_KEY}},
    {98, {"KEY_NUMPAD_DIVIDE", 98, 2113, HOS_KEY_NUMPAD_DIVIDE, MMI_NO_SYSTEM_KEY}},
    {55, {"KEY_NUMPAD_MULTIPLY", 55, 2114, HOS_KEY_NUMPAD_MULTIPLY, MMI_NO_SYSTEM_KEY}},
    {74, {"KEY_NUMPAD_SUBTRACT", 74, 2115, HOS_KEY_NUMPAD_SUBTRACT, MMI_NO_SYSTEM_KEY}},
    {78, {"KEY_NUMPAD_ADD", 78, 2116, HOS_KEY_NUMPAD_ADD, MMI_NO_SYSTEM_KEY}},
    {83, {"KEY_NUMPAD_DOT", 83, 2117, HOS_KEY_NUMPAD_DOT, MMI_NO_SYSTEM_KEY}},
    {95, {"KEY_NUMPAD_COMMA", 95, 2118, HOS_KEY_NUMPAD_COMMA, MMI_NO_SYSTEM_KEY}},
    {121, {"KEY_NUMPAD_COMMA", 121, 2118, HOS_KEY_NUMPAD_COMMA, MMI_NO_SYSTEM_KEY}},
    {96, {"KEY_NUMPAD_ENTER", 96, 2119, HOS_KEY_NUMPAD_ENTER, MMI_NO_SYSTEM_KEY}},
    {117, {"KEY_NUMPAD_EQUALS", 117, 2120, HOS_KEY_NUMPAD_EQUALS, MMI_NO_SYSTEM_KEY}},
    {179, {"KEY_NUMPAD_LEFT_PAREN", 179, 2121, HOS_KEY_NUMPAD_LEFT_PAREN, MMI_NO_SYSTEM_KEY}},
    {180, {"KEY_NUMPAD_RIGHT_PAREN", 180, 2122, HOS_KEY_NUMPAD_RIGHT_PAREN, MMI_NO_SYSTEM_KEY}},

    {272, {"LEFT_BUTTON", 272, 1, HOS_LEFT_BUTTON, MMI_NO_SYSTEM_KEY}},
    {273, {"RIGHT_BUTTON", 273, 2, HOS_RIGHT_BUTTON, MMI_NO_SYSTEM_KEY}},
    {274, {"MIDDLE_BUTTON", 274, 4, HOS_MIDDLE_BUTTON, MMI_NO_SYSTEM_KEY}},
    {275, {"SIDE_BUTTON", 275, 32, HOS_SIDE_BUTTON, MMI_NO_SYSTEM_KEY}},
    {276, {"EXTRA_BUTTON", 276, 64, HOS_EXTRA_BUTTON, MMI_NO_SYSTEM_KEY}},
    {277, {"FORWARD_BUTTON", 277, 16, HOS_FORWARD_BUTTON, MMI_NO_SYSTEM_KEY}},
    {278, {"BACK_BUTTON", 278, 8, HOS_BACK_BUTTON, MMI_NO_SYSTEM_KEY}},
    {279, {"TASK_BUTTON", 279, 128, HOS_TASK_BUTTON, MMI_NO_SYSTEM_KEY}},

    {115, {"KEY_VOLUME_UP", 115, 16, HOS_KEY_VOLUME_UP, MMI_SYSTEM_KEY}},
    {114, {"KEY_VOLUME_DOWN", 114, 17, HOS_KEY_VOLUME_DOWN, MMI_SYSTEM_KEY}},
    {116, {"KEY_POWER", 116, 18, HOS_KEY_POWER, MMI_SYSTEM_KEY}},
    {113, {"KEY_VOLUME_MUTE", 113, 22, HOS_KEY_VOLUME_MUTE, MMI_SYSTEM_KEY}},

    {172, {"KEY_HOME", 172, 1, HOS_KEY_HOME, MMI_SYSTEM_KEY}},
    {158, {"KEY_BACK", 158, 2, HOS_KEY_BACK, MMI_SYSTEM_KEY}},
    {640, {"KEY_VIRTUAL_MULTITASK", 640, 2210, HOS_KEY_VIRTUAL_MULTITASK, MMI_NO_SYSTEM_KEY}},

    {304, {"KEY_BUTTON_A", 304, 2301, HOS_KEY_BUTTON_A, MMI_NO_SYSTEM_KEY}},
    {305, {"KEY_BUTTON_B", 305, 2302, HOS_KEY_BUTTON_B, MMI_NO_SYSTEM_KEY}},
    {306, {"KEY_BUTTON_C", 306, 2303, HOS_KEY_BUTTON_C, MMI_NO_SYSTEM_KEY}},
    {307, {"KEY_BUTTON_X", 307, 2304, HOS_KEY_BUTTON_X, MMI_NO_SYSTEM_KEY}},
    {308, {"KEY_BUTTON_Y", 308, 2305, HOS_KEY_BUTTON_Y, MMI_NO_SYSTEM_KEY}},
    {309, {"KEY_BUTTON_Z", 309, 2306, HOS_KEY_BUTTON_Z, MMI_NO_SYSTEM_KEY}},
    {310, {"KEY_BUTTON_L1", 310, 2307, HOS_KEY_BUTTON_L1, MMI_NO_SYSTEM_KEY}},
    {311, {"KEY_BUTTON_R1", 311, 2308, HOS_KEY_BUTTON_R1, MMI_NO_SYSTEM_KEY}},
    {312, {"KEY_BUTTON_L2", 312, 2309, HOS_KEY_BUTTON_L2, MMI_NO_SYSTEM_KEY}},
    {313, {"KEY_BUTTON_R2", 313, 2310, HOS_KEY_BUTTON_R2, MMI_NO_SYSTEM_KEY}},
    {314, {"KEY_BUTTON_SELECT", 314, 2311, HOS_KEY_BUTTON_SELECT, MMI_NO_SYSTEM_KEY}},
    {315, {"KEY_BUTTON_START", 315, 2312, HOS_KEY_BUTTON_START, MMI_NO_SYSTEM_KEY}},
    {316, {"KEY_BUTTON_MODE", 316, 2313, HOS_KEY_BUTTON_MODE, MMI_NO_SYSTEM_KEY}},
    {317, {"KEY_BUTTON_THUMBL", 317, 2314, HOS_KEY_BUTTON_THUMBL, MMI_NO_SYSTEM_KEY}},
    {318, {"KEY_BUTTON_THUMBR", 318, 2315, HOS_KEY_BUTTON_THUMBR, MMI_NO_SYSTEM_KEY}},

    {288, {"KEY_BUTTON_TRIGGER", 288, 2401, HOS_KEY_BUTTON_TRIGGER, MMI_NO_SYSTEM_KEY}},
    {289, {"KEY_BUTTON_THUMB", 289, 2402, HOS_KEY_BUTTON_THUMB, MMI_NO_SYSTEM_KEY}},
    {290, {"KEY_BUTTON_THUMB2", 290, 2403, HOS_KEY_BUTTON_THUMB2, MMI_NO_SYSTEM_KEY}},
    {291, {"KEY_BUTTON_TOP", 291, 2404, HOS_KEY_BUTTON_TOP, MMI_NO_SYSTEM_KEY}},
    {292, {"KEY_BUTTON_TOP2", 292, 2405, HOS_KEY_BUTTON_TOP2, MMI_NO_SYSTEM_KEY}},
    {293, {"KEY_BUTTON_PINKIE", 293, 2406, HOS_KEY_BUTTON_PINKIE, MMI_NO_SYSTEM_KEY}},
    {294, {"KEY_BUTTON_BASE1", 294, 2407, HOS_KEY_BUTTON_BASE1, MMI_NO_SYSTEM_KEY}},
    {295, {"KEY_BUTTON_BASE2", 295, 2408, HOS_KEY_BUTTON_BASE2, MMI_NO_SYSTEM_KEY}},
    {296, {"KEY_BUTTON_BASE3", 296, 2409, HOS_KEY_BUTTON_BASE3, MMI_NO_SYSTEM_KEY}},
    {297, {"KEY_BUTTON_BASE4", 297, 2410, HOS_KEY_BUTTON_BASE4, MMI_NO_SYSTEM_KEY}},
    {298, {"KEY_BUTTON_BASE5", 298, 2411, HOS_KEY_BUTTON_BASE5, MMI_NO_SYSTEM_KEY}},
    {299, {"KEY_BUTTON_BASE6", 299, 2412, HOS_KEY_BUTTON_BASE6, MMI_NO_SYSTEM_KEY}},
    {300, {"KEY_BUTTON_BASE7", 300, 2413, HOS_KEY_BUTTON_BASE7, MMI_NO_SYSTEM_KEY}},
    {301, {"KEY_BUTTON_BASE8", 301, 2414, HOS_KEY_BUTTON_BASE8, MMI_NO_SYSTEM_KEY}},
    {302, {"KEY_BUTTON_BASE9", 302, 2415, HOS_KEY_BUTTON_BASE9, MMI_NO_SYSTEM_KEY}},
    {303, {"KEY_BUTTON_DEAD", 303, 2416, HOS_KEY_BUTTON_DEAD, MMI_NO_SYSTEM_KEY}},

    {330, {"BUTTON_TOUCH", 330, 2500, HOS_BUTTON_TOUCH, MMI_NO_SYSTEM_KEY}},
    {320, {"BUTTON_TOOL_PEN", 320, 2501, HOS_BUTTON_TOOL_PEN, MMI_NO_SYSTEM_KEY}},
    {321, {"BUTTON_TOOL_RUBBER", 321, 2502, HOS_BUTTON_TOOL_RUBBER, MMI_NO_SYSTEM_KEY}},
    {322, {"BUTTON_TOOL_BRUSH", 322, 2503, HOS_BUTTON_TOOL_BRUSH, MMI_NO_SYSTEM_KEY}},
    {323, {"BUTTON_TOOL_PENCIL", 323, 2504, HOS_BUTTON_TOOL_PENCIL, MMI_NO_SYSTEM_KEY}},
    {324, {"BUTTON_TOOL_AIRBRUSH", 324, 2505, HOS_BUTTON_TOOL_AIRBRUSH, MMI_NO_SYSTEM_KEY}},
    {325, {"BUTTON_TOOL_FINGER", 325, 2506, HOS_BUTTON_TOOL_FINGER, MMI_NO_SYSTEM_KEY}},
    {326, {"BUTTON_TOOL_MOUSE", 326, 2507, HOS_BUTTON_TOOL_MOUSE, MMI_NO_SYSTEM_KEY}},
    {327, {"BUTTON_TOOL_LENS", 327, 2508, HOS_BUTTON_TOOL_LENS, MMI_NO_SYSTEM_KEY}},
    {331, {"BUTTON_STYLUS", 331, 2509, HOS_BUTTON_STYLUS, MMI_NO_SYSTEM_KEY}},
    {332, {"BUTTON_STYLUS2", 332, 2510, HOS_BUTTON_STYLUS2, MMI_NO_SYSTEM_KEY}},
    {329, {"BUTTON_STYLUS3", 329, 2511, HOS_BUTTON_STYLUS3, MMI_NO_SYSTEM_KEY}},
    {333, {"BUTTON_TOOL_DOUBLETAP", 333, 2512, HOS_BUTTON_TOOL_DOUBLETAP, MMI_NO_SYSTEM_KEY}},
    {334, {"BUTTON_TOOL_TRIPLETAP", 334, 2513, HOS_BUTTON_TOOL_TRIPLETAP, MMI_NO_SYSTEM_KEY}},
    {335, {"BUTTON_TOOL_QUADTAP", 335, 2514, HOS_BUTTON_TOOL_QUADTAP, MMI_NO_SYSTEM_KEY}},
    {328, {"BUTTON_TOOL_QUINTTAP", 328, 2515, HOS_BUTTON_TOOL_QUINTTAP, MMI_NO_SYSTEM_KEY}},

    {212, {"KEY_CAMERA", 212, 19, HOS_KEY_CAMERA, MMI_SYSTEM_KEY}},
    {225, {"KEY_BRIGHTNESS_UP", 225, 40, HOS_KEY_BRIGHTNESS_UP, MMI_SYSTEM_KEY}},
    {224, {"KEY_BRIGHTNESS_DOWN", 224, 41, HOS_KEY_BRIGHTNESS_DOWN, MMI_SYSTEM_KEY}},
    {355, {"KEY_CLEAR", 355, 5, HOS_KEY_CLEAR, MMI_NO_SYSTEM_KEY}},
    {528, {"KEY_FOCUS", 528, 7, HOS_KEY_FOCUS, MMI_SYSTEM_KEY}},
    {217, {"KEY_SEARCH", 217, 9, HOS_KEY_SEARCH, MMI_SYSTEM_KEY}},
    {164, {"KEY_MEDIA_PLAY_PAUSE", 164, 10, HOS_KEY_MEDIA_PLAY_PAUSE, MMI_SYSTEM_KEY}},
    {166, {"KEY_MEDIA_STOP", 166, 11, HOS_KEY_MEDIA_STOP, MMI_SYSTEM_KEY}},
    {163, {"KEY_MEDIA_NEXT", 163, 12, HOS_KEY_MEDIA_NEXT, MMI_SYSTEM_KEY}},
    {165, {"KEY_MEDIA_PREVIOUS", 165, 13, HOS_KEY_MEDIA_PREVIOUS, MMI_SYSTEM_KEY}},
    {168, {"KEY_MEDIA_REWIND", 168, 14, HOS_KEY_MEDIA_REWIND, MMI_SYSTEM_KEY}},
    {208, {"KEY_MEDIA_FAST_FORWARD", 208, 15, HOS_KEY_MEDIA_FAST_FORWARD, MMI_SYSTEM_KEY}},
    {582, {"KEY_VOICE_ASSISTANT", 582, 20, HOS_KEY_VOICE_ASSISTANT, MMI_SYSTEM_KEY}},
    {240, {"KEY_UNKNOWN", 240, -1, HOS_KEY_UNKNOWN, MMI_NO_SYSTEM_KEY}},

    {142, {"KEY_SLEEP", 142, 2600, HOS_KEY_SLEEP, MMI_NO_SYSTEM_KEY}},
    {85, {"KEY_ZENKAKU_HANKAKU", 85, 2601, HOS_KEY_ZENKAKU_HANKAKU, MMI_NO_SYSTEM_KEY}},
    {86, {"KEY_102ND", 86, 2602, HOS_KEY_102ND, MMI_NO_SYSTEM_KEY}},
    {89, {"KEY_RO", 89, 2603, HOS_KEY_RO, MMI_NO_SYSTEM_KEY}},
    {90, {"KEY_KATAKANA", 90, 2604, HOS_KEY_KATAKANA, MMI_NO_SYSTEM_KEY}},
    {91, {"KEY_HIRAGANA", 91, 2605, HOS_KEY_HIRAGANA, MMI_NO_SYSTEM_KEY}},
    {92, {"KEY_HENKAN", 92, 2606, HOS_KEY_HENKAN, MMI_NO_SYSTEM_KEY}},
    {93, {"KEY_KATAKANA_HIRAGANA", 93, 2607, HOS_KEY_KATAKANA_HIRAGANA, MMI_NO_SYSTEM_KEY}},
    {94, {"KEY_MUHENKAN", 94, 2608, HOS_KEY_MUHENKAN, MMI_NO_SYSTEM_KEY}},
    {101, {"KEY_LINEFEED", 101, 2609, HOS_KEY_LINEFEED, MMI_NO_SYSTEM_KEY}},
    {112, {"KEY_MACRO", 112, 2610, HOS_KEY_MACRO, MMI_NO_SYSTEM_KEY}},
    {118, {"KEY_NUMPAD_PLUSMINUS", 118, 2611, HOS_KEY_NUMPAD_PLUSMINUS, MMI_NO_SYSTEM_KEY}},
    {120, {"KEY_SCALE", 120, 2612, HOS_KEY_SCALE, MMI_NO_SYSTEM_KEY}},
    {122, {"KEY_HANGUEL", 122, 2613, HOS_KEY_HANGUEL, MMI_NO_SYSTEM_KEY}},
    {123, {"KEY_HANJA", 123, 2614, HOS_KEY_HANJA, MMI_NO_SYSTEM_KEY}},
    {124, {"KEY_YEN", 124, 2615, HOS_KEY_YEN, MMI_NO_SYSTEM_KEY}},
    {128, {"KEY_STOP", 128, 2616, HOS_KEY_STOP, MMI_NO_SYSTEM_KEY}},
    {129, {"KEY_AGAIN", 129, 2617, HOS_KEY_AGAIN, MMI_NO_SYSTEM_KEY}},
    {130, {"KEY_PROPS", 130, 2618, HOS_KEY_PROPS, MMI_NO_SYSTEM_KEY}},
    {131, {"KEY_UNDO", 131, 2619, HOS_KEY_UNDO, MMI_NO_SYSTEM_KEY}},
    {133, {"KEY_COPY", 133, 2620, HOS_KEY_COPY, MMI_NO_SYSTEM_KEY}},
    {134, {"KEY_OPEN", 134, 2621, HOS_KEY_OPEN, MMI_NO_SYSTEM_KEY}},
    {135, {"KEY_PASTE", 135, 2622, HOS_KEY_PASTE, MMI_NO_SYSTEM_KEY}},
    {136, {"KEY_FIND", 136, 2623, HOS_KEY_FIND, MMI_NO_SYSTEM_KEY}},
    {137, {"KEY_CUT", 137, 2624, HOS_KEY_CUT, MMI_NO_SYSTEM_KEY}},
    {138, {"KEY_HELP", 138, 2625, HOS_KEY_HELP, MMI_NO_SYSTEM_KEY}},
    {140, {"KEY_CALC", 140, 2626, HOS_KEY_CALC, MMI_NO_SYSTEM_KEY}},
    {144, {"KEY_FILE", 144, 2627, HOS_KEY_FILE, MMI_NO_SYSTEM_KEY}},
    {156, {"KEY_BOOKMARKS", 156, 2628, HOS_KEY_BOOKMARKS, MMI_NO_SYSTEM_KEY}},
    {163, {"KEY_NEXTSONG", 163, 12, HOS_KEY_MEDIA_NEXT, MMI_NO_SYSTEM_KEY}},
    {164, {"KEY_PLAYPAUSE", 164, 2630, HOS_KEY_PLAYPAUSE, MMI_NO_SYSTEM_KEY}},
    {165, {"KEY_PREVIOUSSONG", 165, 13, HOS_KEY_MEDIA_PREVIOUS, MMI_NO_SYSTEM_KEY}},
    {166, {"KEY_STOPCD", 166, 2632, HOS_KEY_STOPCD, MMI_NO_SYSTEM_KEY}},
    {169, {"KEY_CALL", 169, 3, HOS_KEY_CALL, MMI_SYSTEM_KEY}},
    {171, {"KEY_CONFIG", 171, 2634, HOS_KEY_CONFIG, MMI_NO_SYSTEM_KEY}},
    {173, {"KEY_REFRESH", 173, 2635, HOS_KEY_REFRESH, MMI_NO_SYSTEM_KEY}},
    {174, {"KEY_EXIT", 174, 2636, HOS_KEY_EXIT, MMI_NO_SYSTEM_KEY}},
    {176, {"KEY_EDIT", 176, 2637, HOS_KEY_EDIT, MMI_NO_SYSTEM_KEY}},
    {177, {"KEY_SCROLLUP", 177, 2638, HOS_KEY_SCROLLUP, MMI_NO_SYSTEM_KEY}},
    {178, {"KEY_SCROLLDOWN", 178, 2639, HOS_KEY_SCROLLDOWN, MMI_NO_SYSTEM_KEY}},
    {181, {"KEY_NEW", 181, 2640, HOS_KEY_NEW, MMI_NO_SYSTEM_KEY}},
    {182, {"KEY_REDO", 182, 2641, HOS_KEY_REDO, MMI_NO_SYSTEM_KEY}},
    {206, {"KEY_CLOSE", 206, 2642, HOS_KEY_CLOSE, MMI_NO_SYSTEM_KEY}},
    {209, {"KEY_BASSBOOST", 209, 2644, HOS_KEY_BASSBOOST, MMI_NO_SYSTEM_KEY}},
    {210, {"KEY_PRINT", 210, 2645, HOS_KEY_PRINT, MMI_NO_SYSTEM_KEY}},
    {216, {"KEY_CHAT", 216, 2646, HOS_KEY_CHAT, MMI_NO_SYSTEM_KEY}},
    {219, {"KEY_FINANCE", 219, 2647, HOS_KEY_FINANCE, MMI_NO_SYSTEM_KEY}},
    {223, {"KEY_CANCEL", 223, 2648, HOS_KEY_CANCEL, MMI_NO_SYSTEM_KEY}},
    {228, {"KEY_KBDILLUM_TOGGLE", 228, 2649, HOS_KEY_KBDILLUM_TOGGLE, MMI_NO_SYSTEM_KEY}},
    {229, {"KEY_KBDILLUM_DOWN", 229, 2650, HOS_KEY_KBDILLUM_DOWN, MMI_NO_SYSTEM_KEY}},
    {230, {"KEY_KBDILLUM_UP", 230, 2651, HOS_KEY_KBDILLUM_UP, MMI_NO_SYSTEM_KEY}},
    {231, {"KEY_SEND", 231, 2652, HOS_KEY_SEND, MMI_NO_SYSTEM_KEY}},
    {232, {"KEY_REPLY", 232, 2653, HOS_KEY_REPLY, MMI_NO_SYSTEM_KEY}},
    {233, {"KEY_FORWARDMAIL", 233, 2654, HOS_KEY_FORWARDMAIL, MMI_NO_SYSTEM_KEY}},
    {234, {"KEY_SAVE", 234, 2655, HOS_KEY_SAVE, MMI_NO_SYSTEM_KEY}},
    {235, {"KEY_DOCUMENTS", 235, 2656, HOS_KEY_DOCUMENTS, MMI_NO_SYSTEM_KEY}},
    {241, {"KEY_VIDEO_NEXT", 241, 2657, HOS_KEY_VIDEO_NEXT, MMI_NO_SYSTEM_KEY}},
    {242, {"KEY_VIDEO_PREV", 242, 2658, HOS_KEY_VIDEO_PREV, MMI_NO_SYSTEM_KEY}},
    {243, {"KEY_BRIGHTNESS_CYCLE", 243, 2659, HOS_KEY_BRIGHTNESS_CYCLE, MMI_NO_SYSTEM_KEY}},
    {244, {"KEY_BRIGHTNESS_ZERO", 244, 2660, HOS_KEY_BRIGHTNESS_ZERO, MMI_NO_SYSTEM_KEY}},
    {245, {"KEY_DISPLAY_OFF", 245, 2661, HOS_KEY_DISPLAY_OFF, MMI_NO_SYSTEM_KEY}},
    {256, {"BTN_MISC", 256, 2662, HOS_BTN_MISC, MMI_NO_SYSTEM_KEY}},
    {354, {"KEY_GOTO", 354, 2663, HOS_KEY_GOTO, MMI_NO_SYSTEM_KEY}},
    {358, {"KEY_INFO", 358, 2664, HOS_KEY_INFO, MMI_NO_SYSTEM_KEY}},
    {362, {"KEY_PROGRAM", 362, 2665, HOS_KEY_PROGRAM, MMI_NO_SYSTEM_KEY}},
    {366, {"KEY_PVR", 366, 2666, HOS_KEY_PVR, MMI_NO_SYSTEM_KEY}},
    {370, {"KEY_SUBTITLE", 370, 2667, HOS_KEY_SUBTITLE, MMI_NO_SYSTEM_KEY}},
    {372, {"KEY_FULL_SCREEN", 372, 2668, HOS_KEY_FULL_SCREEN, MMI_NO_SYSTEM_KEY}},
    {374, {"KEY_KEYBOARD", 374, 2669, HOS_KEY_KEYBOARD, MMI_NO_SYSTEM_KEY}},
    {375, {"KEY_ASPECT_RATIO", 375, 2670, HOS_KEY_ASPECT_RATIO, MMI_NO_SYSTEM_KEY}},
    {376, {"KEY_PC", 376, 2671, HOS_KEY_PC, MMI_NO_SYSTEM_KEY}},
    {377, {"KEY_TV", 377, 2672, HOS_KEY_TV, MMI_NO_SYSTEM_KEY}},
    {378, {"KEY_TV2", 378, 2673, HOS_KEY_TV2, MMI_NO_SYSTEM_KEY}},
    {379, {"KEY_VCR", 379, 2674, HOS_KEY_VCR, MMI_NO_SYSTEM_KEY}},
    {380, {"KEY_VCR2", 380, 2675, HOS_KEY_VCR2, MMI_NO_SYSTEM_KEY}},
    {381, {"KEY_SAT", 381, 2676, HOS_KEY_SAT, MMI_NO_SYSTEM_KEY}},
    {383, {"KEY_CD", 383, 2677, HOS_KEY_CD, MMI_NO_SYSTEM_KEY}},
    {384, {"KEY_TAPE", 384, 2678, HOS_KEY_TAPE, MMI_NO_SYSTEM_KEY}},
    {386, {"KEY_TUNER", 386, 2679, HOS_KEY_TUNER, MMI_NO_SYSTEM_KEY}},
    {387, {"KEY_PLAYER", 387, 2680, HOS_KEY_PLAYER, MMI_NO_SYSTEM_KEY}},
    {389, {"KEY_DVD", 389, 2681, HOS_KEY_DVD, MMI_NO_SYSTEM_KEY}},
    {392, {"KEY_AUDIO", 392, 2682, HOS_KEY_AUDIO, MMI_NO_SYSTEM_KEY}},
    {393, {"KEY_VIDEO", 393, 2683, HOS_KEY_VIDEO, MMI_NO_SYSTEM_KEY}},
    {396, {"KEY_MEMO", 396, 2684, HOS_KEY_MEMO, MMI_NO_SYSTEM_KEY}},
    {397, {"KEY_CALENDAR", 397, 2685, HOS_KEY_CALENDAR, MMI_NO_SYSTEM_KEY}},
    {398, {"KEY_RED", 398, 2686, HOS_KEY_RED, MMI_NO_SYSTEM_KEY}},
    {399, {"KEY_GREEN", 399, 2687, HOS_KEY_GREEN, MMI_NO_SYSTEM_KEY}},
    {400, {"KEY_YELLOW", 400, 2688, HOS_KEY_YELLOW, MMI_NO_SYSTEM_KEY}},
    {401, {"KEY_BLUE", 401, 2689, HOS_KEY_BLUE, MMI_NO_SYSTEM_KEY}},
    {402, {"KEY_CHANNELUP", 402, 2690, HOS_KEY_CHANNELUP, MMI_NO_SYSTEM_KEY}},
    {403, {"KEY_CHANNELDOWN", 403, 2691, HOS_KEY_CHANNELDOWN, MMI_NO_SYSTEM_KEY}},
    {405, {"KEY_LAST", 405, 2692, HOS_KEY_LAST, MMI_NO_SYSTEM_KEY}},
    {408, {"KEY_RESTART", 408, 2693, HOS_KEY_RESTART, MMI_NO_SYSTEM_KEY}},
    {409, {"KEY_SLOW", 409, 2694, HOS_KEY_SLOW, MMI_NO_SYSTEM_KEY}},
    {410, {"KEY_SHUFFLE", 410, 2695, HOS_KEY_SHUFFLE, MMI_NO_SYSTEM_KEY}},
    {416, {"KEY_VIDEOPHONE", 416, 2696, HOS_KEY_VIDEOPHONE, MMI_NO_SYSTEM_KEY}},
    {417, {"KEY_GAMES", 417, 2697, HOS_KEY_GAMES, MMI_NO_SYSTEM_KEY}},
    {418, {"KEY_ZOOMIN", 418, 2698, HOS_KEY_ZOOMIN, MMI_NO_SYSTEM_KEY}},
    {419, {"KEY_ZOOMOUT", 419, 2699, HOS_KEY_ZOOMOUT, MMI_NO_SYSTEM_KEY}},
    {420, {"KEY_ZOOMRESET", 420, 2700, HOS_KEY_ZOOMRESET, MMI_NO_SYSTEM_KEY}},
    {421, {"KEY_WORDPROCESSOR", 421, 2701, HOS_KEY_WORDPROCESSOR, MMI_NO_SYSTEM_KEY}},
    {422, {"KEY_EDITOR", 422, 2702, HOS_KEY_EDITOR, MMI_NO_SYSTEM_KEY}},
    {423, {"KEY_SPREADSHEET", 423, 2703, HOS_KEY_SPREADSHEET, MMI_NO_SYSTEM_KEY}},
    {424, {"KEY_GRAPHICSEDITOR", 424, 2704, HOS_KEY_GRAPHICSEDITOR, MMI_NO_SYSTEM_KEY}},
    {425, {"KEY_PRESENTATION", 425, 2705, HOS_KEY_PRESENTATION, MMI_NO_SYSTEM_KEY}},
    {426, {"KEY_DATABASE", 426, 2706, HOS_KEY_DATABASE, MMI_NO_SYSTEM_KEY}},
    {427, {"KEY_NEWS", 427, 2707, HOS_KEY_NEWS, MMI_NO_SYSTEM_KEY}},
    {428, {"KEY_VOICEMAIL", 428, 2708, HOS_KEY_VOICEMAIL, MMI_NO_SYSTEM_KEY}},
    {429, {"KEY_ADDRESSBOOK", 429, 2709, HOS_KEY_ADDRESSBOOK, MMI_NO_SYSTEM_KEY}},
    {430, {"KEY_MESSENGER", 430, 2710, HOS_KEY_MESSENGER, MMI_NO_SYSTEM_KEY}},
    {431, {"KEY_BRIGHTNESS_TOGGLE", 431, 2711, HOS_KEY_BRIGHTNESS_TOGGLE, MMI_NO_SYSTEM_KEY}},
    {432, {"KEY_SPELLCHECK", 432, 2712, HOS_KEY_SPELLCHECK, MMI_NO_SYSTEM_KEY}},
    {433, {"KEY_COFFEE", 433, 2713, HOS_KEY_COFFEE, MMI_NO_SYSTEM_KEY}},
    {439, {"KEY_MEDIA_REPEAT", 439, 2714, HOS_KEY_MEDIA_REPEAT, MMI_NO_SYSTEM_KEY}},
    {442, {"KEY_IMAGES", 442, 2715, HOS_KEY_IMAGES, MMI_NO_SYSTEM_KEY}},
    {576, {"KEY_BUTTONCONFIG", 576, 2716, HOS_KEY_BUTTONCONFIG, MMI_NO_SYSTEM_KEY}},
    {577, {"KEY_TASKMANAGER", 577, 2717, HOS_KEY_TASKMANAGER, MMI_NO_SYSTEM_KEY}},
    {578, {"KEY_JOURNAL", 578, 2718, HOS_KEY_JOURNAL, MMI_NO_SYSTEM_KEY}},
    {579, {"KEY_CONTROLPANEL", 579, 2719, HOS_KEY_CONTROLPANEL, MMI_NO_SYSTEM_KEY}},
    {580, {"KEY_APPSELECT", 580, 2720, HOS_KEY_APPSELECT, MMI_NO_SYSTEM_KEY}},
    {581, {"KEY_SCREENSAVER", 581, 2721, HOS_KEY_SCREENSAVER, MMI_NO_SYSTEM_KEY}},
    {583, {"KEY_ASSISTANT", 583, 2722, HOS_KEY_ASSISTANT, MMI_NO_SYSTEM_KEY}},
    {584, {"KEY_KBD_LAYOUT_NEXT", 584, 2723, HOS_KEY_KBD_LAYOUT_NEXT, MMI_NO_SYSTEM_KEY}},
    {592, {"KEY_BRIGHTNESS_MIN", 592, 2724, HOS_KEY_BRIGHTNESS_MIN, MMI_NO_SYSTEM_KEY}},
    {593, {"KEY_BRIGHTNESS_MAX", 593, 2725, HOS_KEY_BRIGHTNESS_MAX, MMI_NO_SYSTEM_KEY}},
    {608, {"KEY_KBDINPUTASSIST_PREV", 608, 2726, HOS_KEY_KBDINPUTASSIST_PREV, MMI_NO_SYSTEM_KEY}},
    {609, {"KEY_KBDINPUTASSIST_NEXT", 609, 2727, HOS_KEY_KBDINPUTASSIST_NEXT, MMI_NO_SYSTEM_KEY}},
    {610, {"KEY_KBDINPUTASSIST_PREVGROUP", 610, 2728, HOS_KEY_KBDINPUTASSIST_PREVGROUP, MMI_NO_SYSTEM_KEY}},
    {611, {"KEY_KBDINPUTASSIST_NEXTGROUP", 611, 2729, HOS_KEY_KBDINPUTASSIST_NEXTGROUP, MMI_NO_SYSTEM_KEY}},
    {612, {"KEY_KBDINPUTASSIST_ACCEPT", 612, 2730, HOS_KEY_KBDINPUTASSIST_ACCEPT, MMI_NO_SYSTEM_KEY}},
    {613, {"KEY_KBDINPUTASSIST_CANCEL", 613, 2731, HOS_KEY_KBDINPUTASSIST_CANCEL, MMI_NO_SYSTEM_KEY}},

    {132, {"KEY_FRONT", 132, 2800, HOS_KEY_FRONT, MMI_NO_SYSTEM_KEY}},
    {141, {"KEY_SETUP", 141, 2801, HOS_KEY_SETUP, MMI_NO_SYSTEM_KEY}},
    {143, {"KEY_WAKEUP", 143, 2802, HOS_KEY_WAKEUP, MMI_NO_SYSTEM_KEY}},
    {145, {"KEY_SENDFILE", 145, 2803, HOS_KEY_SENDFILE, MMI_NO_SYSTEM_KEY}},
    {146, {"KEY_DELETEFILE", 146, 2804, HOS_KEY_DELETEFILE, MMI_NO_SYSTEM_KEY}},
    {147, {"KEY_XFER", 147, 2805, HOS_KEY_XFER, MMI_NO_SYSTEM_KEY}},
    {148, {"KEY_PROG1", 148, 2806, HOS_KEY_PROG1, MMI_NO_SYSTEM_KEY}},
    {149, {"KEY_PROG2", 149, 2807, HOS_KEY_PROG2, MMI_NO_SYSTEM_KEY}},
    {151, {"KEY_MSDOS", 151, 2808, HOS_KEY_MSDOS, MMI_NO_SYSTEM_KEY}},
    {152, {"KEY_SCREENLOCK", 152, 2809, HOS_KEY_SCREENLOCK, MMI_NO_SYSTEM_KEY}},
    {153, {"KEY_DIRECTION_ROTATE_DISPLAY", 153, 2810, HOS_KEY_DIRECTION_ROTATE_DISPLAY, MMI_NO_SYSTEM_KEY}},
    {154, {"KEY_CYCLEWINDOWS", 154, 2811, HOS_KEY_CYCLEWINDOWS, MMI_NO_SYSTEM_KEY}},
    {157, {"KEY_COMPUTER", 157, 2812, HOS_KEY_COMPUTER, MMI_NO_SYSTEM_KEY}},
    {162, {"KEY_EJECTCLOSECD", 162, 2813, HOS_KEY_EJECTCLOSECD, MMI_NO_SYSTEM_KEY}},
    {170, {"KEY_ISO", 170, 2814, HOS_KEY_ISO, MMI_NO_SYSTEM_KEY}},
    {175, {"KEY_MOVE", 175, 2815, HOS_KEY_MOVE, MMI_NO_SYSTEM_KEY}},
    {183, {"KEY_F13", 183, 2816, HOS_KEY_F13, MMI_NO_SYSTEM_KEY}},
    {184, {"KEY_F14", 184, 2817, HOS_KEY_F14, MMI_NO_SYSTEM_KEY}},
    {185, {"KEY_F15", 185, 2818, HOS_KEY_F15, MMI_NO_SYSTEM_KEY}},
    {186, {"KEY_F16", 186, 2819, HOS_KEY_F16, MMI_NO_SYSTEM_KEY}},
    {187, {"KEY_F17", 187, 2820, HOS_KEY_F17, MMI_NO_SYSTEM_KEY}},
    {188, {"KEY_F18", 188, 2821, HOS_KEY_F18, MMI_NO_SYSTEM_KEY}},
    {189, {"KEY_F19", 189, 2822, HOS_KEY_F19, MMI_NO_SYSTEM_KEY}},
    {190, {"KEY_F20", 190, 2823, HOS_KEY_F20, MMI_NO_SYSTEM_KEY}},
    {191, {"KEY_F21", 191, 2824, HOS_KEY_F21, MMI_NO_SYSTEM_KEY}},
    {192, {"KEY_F22", 192, 2825, HOS_KEY_F22, MMI_NO_SYSTEM_KEY}},
    {193, {"KEY_F23", 193, 2826, HOS_KEY_F23, MMI_NO_SYSTEM_KEY}},
    {194, {"KEY_F24", 194, 2827, HOS_KEY_F24, MMI_NO_SYSTEM_KEY}},
    {202, {"KEY_PROG3", 202, 2828, HOS_KEY_PROG3, MMI_NO_SYSTEM_KEY}},
    {203, {"KEY_PROG4", 203, 2829, HOS_KEY_PROG4, MMI_NO_SYSTEM_KEY}},
    {204, {"KEY_DASHBOARD", 204, 2830, HOS_KEY_DASHBOARD, MMI_NO_SYSTEM_KEY}},
    {205, {"KEY_SUSPEND", 205, 2831, HOS_KEY_SUSPEND, MMI_NO_SYSTEM_KEY}},
    {211, {"KEY_HP", 211, 2832, HOS_KEY_HP, MMI_NO_SYSTEM_KEY}},
    {213, {"KEY_SOUND", 213, 2833, HOS_KEY_SOUND, MMI_NO_SYSTEM_KEY}},
    {214, {"KEY_QUESTION", 214, 2834, HOS_KEY_QUESTION, MMI_NO_SYSTEM_KEY}},
    {215, {"KEY_AT", 215, 2065, HOS_KEY_AT, MMI_NO_SYSTEM_KEY}},
    {218, {"KEY_CONNECT", 218, 2836, HOS_KEY_CONNECT, MMI_NO_SYSTEM_KEY}},
    {220, {"KEY_SPORT", 220, 2837, HOS_KEY_SPORT, MMI_NO_SYSTEM_KEY}},
    {221, {"KEY_SHOP", 221, 2838, HOS_KEY_SHOP, MMI_NO_SYSTEM_KEY}},
    {222, {"KEY_ALTERASE", 222, 2839, HOS_KEY_ALTERASE, MMI_NO_SYSTEM_KEY}},
    {226, {"KEY_HEADSETHOOK", 226, 6, HOS_KEY_HEADSETHOOK, MMI_NO_SYSTEM_KEY}},
    {227, {"KEY_SWITCHVIDEOMODE", 227, 2841, HOS_KEY_SWITCHVIDEOMODE, MMI_NO_SYSTEM_KEY}},
    {236, {"KEY_BATTERY", 236, 2842, HOS_KEY_BATTERY, MMI_NO_SYSTEM_KEY}},
    {237, {"KEY_BLUETOOTH", 237, 2843, HOS_KEY_BLUETOOTH, MMI_NO_SYSTEM_KEY}},
    {238, {"KEY_WLAN", 238, 2844, HOS_KEY_WLAN, MMI_NO_SYSTEM_KEY}},
    {239, {"KEY_UWB", 239, 2845, HOS_KEY_UWB, MMI_NO_SYSTEM_KEY}},
    {246, {"KEY_WWAN_WIMAX", 246, 2846, HOS_KEY_WWAN_WIMAX, MMI_NO_SYSTEM_KEY}},
    {247, {"KEY_RFKILL", 247, 2847, HOS_KEY_RFKILL, MMI_NO_SYSTEM_KEY}},
    {248, {"KEY_MUTE", 248, 23, HOS_KEY_MUTE, MMI_SYSTEM_KEY}},

    {363, {"KEY_CHANNEL", 363, 3001, HOS_KEY_CHANNEL, MMI_NO_SYSTEM_KEY}},
    {256, {"KEY_BTN_0", 256, 3100, HOS_KEY_BTN_0, MMI_NO_SYSTEM_KEY}},
    {257, {"KEY_BTN_1", 257, 3101, HOS_KEY_BTN_1, MMI_NO_SYSTEM_KEY}},
    {258, {"KEY_BTN_2", 258, 3102, HOS_KEY_BTN_2, MMI_NO_SYSTEM_KEY}},
    {259, {"KEY_BTN_3", 259, 3103, HOS_KEY_BTN_3, MMI_NO_SYSTEM_KEY}},
    {260, {"KEY_BTN_4", 260, 3104, HOS_KEY_BTN_4, MMI_NO_SYSTEM_KEY}},
    {261, {"KEY_BTN_5", 261, 3105, HOS_KEY_BTN_5, MMI_NO_SYSTEM_KEY}},
    {262, {"KEY_BTN_6", 262, 3106, HOS_KEY_BTN_6, MMI_NO_SYSTEM_KEY}},
    {263, {"KEY_BTN_7", 263, 3107, HOS_KEY_BTN_7, MMI_NO_SYSTEM_KEY}},
    {264, {"KEY_BTN_8", 264, 3108, HOS_KEY_BTN_8, MMI_NO_SYSTEM_KEY}},
    {265, {"KEY_BTN_9", 265, 3109, HOS_KEY_BTN_9, MMI_NO_SYSTEM_KEY}},

    {497, {"KEY_BRL_DOT1", 497, 3201, HOS_KEY_BRL_DOT1, MMI_NO_SYSTEM_KEY}},
    {498, {"KEY_BRL_DOT2", 498, 3202, HOS_KEY_BRL_DOT2, MMI_NO_SYSTEM_KEY}},
    {499, {"KEY_BRL_DOT3", 499, 3203, HOS_KEY_BRL_DOT3, MMI_NO_SYSTEM_KEY}},
    {500, {"KEY_BRL_DOT4", 500, 3204, HOS_KEY_BRL_DOT4, MMI_NO_SYSTEM_KEY}},
    {501, {"KEY_BRL_DOT5", 501, 3205, HOS_KEY_BRL_DOT5, MMI_NO_SYSTEM_KEY}},
    {502, {"KEY_BRL_DOT6", 502, 3206, HOS_KEY_BRL_DOT6, MMI_NO_SYSTEM_KEY}},
    {503, {"KEY_BRL_DOT7", 503, 3207, HOS_KEY_BRL_DOT7, MMI_NO_SYSTEM_KEY}},
    {504, {"KEY_BRL_DOT8", 504, 3208, HOS_KEY_BRL_DOT8, MMI_NO_SYSTEM_KEY}},
    {505, {"KEY_BRL_DOT9", 505, 3209, HOS_KEY_BRL_DOT9, MMI_NO_SYSTEM_KEY}},
    {506, {"KEY_BRL_DOT10", 506, 3210, HOS_KEY_BRL_DOT10, MMI_NO_SYSTEM_KEY}},
    {744, {"KEY_ENDCALL", 744, 4, HOS_KEY_ENDCALL, MMI_SYSTEM_KEY}},
    {407, {"KEY_NEXT", 407, 2629, HOS_KEY_NEXT, MMI_NO_SYSTEM_KEY}},
    {412, {"KEY_PREVIOUS", 412, 2631, HOS_KEY_PREVIOUS, MMI_NO_SYSTEM_KEY}},
};

KeyEventValueTransformations KeyValueTransformationByInput(int16_t keyValueOfInput)
{
    auto it = MAP_KEY_EVENT_VALUE_TRANSFORMATION.find(keyValueOfInput);
    if (it != MAP_KEY_EVENT_VALUE_TRANSFORMATION.end()) {
        return it->second;
    } else {
        KeyEventValueTransformations unknownEvent = {
            "KEY_UNKNOWN", keyValueOfInput, keyValueOfInput, HOS_KEY_UNKNOWN, 0
        };
        return unknownEvent;
    }
}

// class XkbKeyboardHandlerKey begin
KeyEventValueTransformation::KeyEventValueTransformation()
{
}

KeyEventValueTransformation::~KeyEventValueTransformation()
{
    xkb_state_unref(state_);
}

bool KeyEventValueTransformation::Init()
{
    struct xkb_context* context = nullptr;
    struct xkb_keymap* keyMap = nullptr;
    int32_t ctxFlags = XKB_CONTEXT_NO_DEFAULT_INCLUDES;

    ctxFlags = ctxFlags | XKB_CONTEXT_NO_ENVIRONMENT_NAMES;
    context = xkb_context_new(static_cast<xkb_context_flags>(ctxFlags));
    if (context == nullptr) {
        MMI_LOGE("XkbKeyboardHandlerKey::Init: Failed to allocate context! errCode:%{public}d \n",
                 XKB_ALLOC_CONTEXT_FAIL);
        return false;
    }

    auto strPath = GetEnv("top_srcdir");
    if (strPath.empty()) {
        strPath = DEF_XKB_CONFIG;
    }

    if (!xkb_context_include_path_append(context, strPath.c_str())) {
        xkb_context_unref(context);
        MMI_LOGE("XkbKeyboardHandlerKey::Init: Include path failed! errCode:%{public}d \n", XKB_INCL_PATH_FAIL);
        return false;
    }

    keyMap = xkb_keymap_new_from_names(context, nullptr, XKB_KEYMAP_COMPILE_NO_FLAGS);
    if (keyMap == nullptr) {
        xkb_context_unref(context);
        MMI_LOGE("XkbKeyboardHandlerKey::Init: Failed to compile RMLVO! errCode:%{public}d \n",
                 XKB_COMPILE_KEYMAP_FAIL);
        return false;
    }
    state_ = xkb_state_new(keyMap);
    if (!state_) {
        xkb_context_unref(context);
        xkb_keymap_unref(keyMap);
        MMI_LOGE("XkbKeyboardHandlerKey::Init: Failed to allocate state! errCode:%{public}d \n", XKB_ALLOC_STATE_FAIL);
        return false;
    }

    xkb_context_unref(context);
    xkb_keymap_unref(keyMap);

    return true;
}

uint32_t KeyEventValueTransformation::KeyboardHandleKeySym(uint32_t keyboardKey)
{
    const uint32_t XKB_EVDEV_OFFSET = 8;
    uint32_t code = keyboardKey + XKB_EVDEV_OFFSET;
    xkb_keysym_t syms = XKB_KEY_NoSymbol;
    xkb_keysym_t* pSyms = &syms;
    xkb_keysym_t sym = XKB_KEY_NoSymbol;

    auto numSyms = xkb_state_key_get_syms(state_, code, (const xkb_keysym_t **)&pSyms);
    if (numSyms == 1) {
        sym = pSyms[0];
    }
    return sym;
}
}
}