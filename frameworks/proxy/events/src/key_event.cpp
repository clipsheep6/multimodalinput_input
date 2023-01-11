/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "key_event.h"

#include "mmi_log.h"


using namespace OHOS::HiviewDFX;
namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KeyEvent" };
const std::map <int32_t, std::string> KEYCODE_TO_STRING = {
    { KeyEvent::KEYCODE_FN, "KEYCODE_FN" },
    { KeyEvent::KEYCODE_UNKNOWN, "KEYCODE_UNKNOWN" },
    { KeyEvent::KEYCODE_HOME, "KEYCODE_HOME" },
    { KeyEvent::KEYCODE_BACK, "KEYCODE_BACK" },
    { KeyEvent::KEYCODE_CALL, "KEYCODE_CALL" },
    { KeyEvent::KEYCODE_ENDCALL, "KEYCODE_ENDCALL" },
    { KeyEvent::KEYCODE_CLEAR, "KEYCODE_CLEAR" },
    { KeyEvent::KEYCODE_HEADSETHOOK, "KEYCODE_HEADSETHOOK" },
    { KeyEvent::KEYCODE_FOCUS, "KEYCODE_FOCUS" },
    { KeyEvent::KEYCODE_NOTIFICATION, "KEYCODE_NOTIFICATION" },
    { KeyEvent::KEYCODE_SEARCH, "KEYCODE_SEARCH" },
    { KeyEvent::KEYCODE_MEDIA_PLAY_PAUSE, "KEYCODE_MEDIA_PLAY_PAUSE" },
    { KeyEvent::KEYCODE_MEDIA_STOP, "KEYCODE_MEDIA_STOP" },
    { KeyEvent::KEYCODE_MEDIA_NEXT, "KEYCODE_MEDIA_NEXT" },
    { KeyEvent::KEYCODE_MEDIA_PREVIOUS, "KEYCODE_MEDIA_PREVIOUS" },
    { KeyEvent::KEYCODE_MEDIA_REWIND, "KEYCODE_MEDIA_REWIND" },
    { KeyEvent::KEYCODE_MEDIA_FAST_FORWARD, "KEYCODE_MEDIA_FAST_FORWARD" },
    { KeyEvent::KEYCODE_VOLUME_UP, "KEYCODE_VOLUME_UP" },
    { KeyEvent::KEYCODE_VOLUME_DOWN, "KEYCODE_VOLUME_DOWN" },
    { KeyEvent::KEYCODE_POWER, "KEYCODE_POWER" },
    { KeyEvent::KEYCODE_CAMERA, "KEYCODE_CAMERA" },
    { KeyEvent::KEYCODE_VOICE_ASSISTANT, "KEYCODE_VOICE_ASSISTANT" },
    { KeyEvent::KEYCODE_CUSTOM1, "KEYCODE_CUSTOM1" },
    { KeyEvent::KEYCODE_VOLUME_MUTE, "KEYCODE_VOLUME_MUTE" },
    { KeyEvent::KEYCODE_MUTE, "KEYCODE_MUTE" },
    { KeyEvent::KEYCODE_BRIGHTNESS_UP, "KEYCODE_BRIGHTNESS_UP" },
    { KeyEvent::KEYCODE_BRIGHTNESS_DOWN, "KEYCODE_BRIGHTNESS_DOWN" },
    { KeyEvent::KEYCODE_WEAR_1, "KEYCODE_WEAR_1" },
    { KeyEvent::KEYCODE_0, "KEYCODE_0" },
    { KeyEvent::KEYCODE_1, "KEYCODE_1" },
    { KeyEvent::KEYCODE_2, "KEYCODE_2" },
    { KeyEvent::KEYCODE_3, "KEYCODE_3" },
    { KeyEvent::KEYCODE_4, "KEYCODE_4" },
    { KeyEvent::KEYCODE_5, "KEYCODE_5" },
    { KeyEvent::KEYCODE_6, "KEYCODE_6" },
    { KeyEvent::KEYCODE_7, "KEYCODE_7" },
    { KeyEvent::KEYCODE_8, "KEYCODE_8" },
    { KeyEvent::KEYCODE_9, "KEYCODE_9" },
    { KeyEvent::KEYCODE_STAR, "KEYCODE_STAR" },
    { KeyEvent::KEYCODE_POUND, "KEYCODE_POUND" },
    { KeyEvent::KEYCODE_DPAD_UP, "KEYCODE_DPAD_UP" },
    { KeyEvent::KEYCODE_DPAD_DOWN, "KEYCODE_DPAD_DOWN" },
    { KeyEvent::KEYCODE_DPAD_LEFT, "KEYCODE_DPAD_LEFT" },
    { KeyEvent::KEYCODE_DPAD_RIGHT, "KEYCODE_DPAD_RIGHT" },
    { KeyEvent::KEYCODE_DPAD_CENTER, "KEYCODE_DPAD_CENTER" },
    { KeyEvent::KEYCODE_A, "KEYCODE_A" },
    { KeyEvent::KEYCODE_B, "KEYCODE_B" },
    { KeyEvent::KEYCODE_C, "KEYCODE_C" },
    { KeyEvent::KEYCODE_D, "KEYCODE_D" },
    { KeyEvent::KEYCODE_E, "KEYCODE_E" },
    { KeyEvent::KEYCODE_F, "KEYCODE_F" },
    { KeyEvent::KEYCODE_G, "KEYCODE_G" },
    { KeyEvent::KEYCODE_H, "KEYCODE_H" },
    { KeyEvent::KEYCODE_I, "KEYCODE_I" },
    { KeyEvent::KEYCODE_J, "KEYCODE_J" },
    { KeyEvent::KEYCODE_K, "KEYCODE_K" },
    { KeyEvent::KEYCODE_L, "KEYCODE_L" },
    { KeyEvent::KEYCODE_M, "KEYCODE_M" },
    { KeyEvent::KEYCODE_N, "KEYCODE_N" },
    { KeyEvent::KEYCODE_O, "KEYCODE_O" },
    { KeyEvent::KEYCODE_P, "KEYCODE_P" },
    { KeyEvent::KEYCODE_Q, "KEYCODE_Q" },
    { KeyEvent::KEYCODE_R, "KEYCODE_R" },
    { KeyEvent::KEYCODE_S, "KEYCODE_S" },
    { KeyEvent::KEYCODE_T, "KEYCODE_T" },
    { KeyEvent::KEYCODE_U, "KEYCODE_U" },
    { KeyEvent::KEYCODE_V, "KEYCODE_V" },
    { KeyEvent::KEYCODE_W, "KEYCODE_W" },
    { KeyEvent::KEYCODE_X, "KEYCODE_X" },
    { KeyEvent::KEYCODE_Y, "KEYCODE_Y" },
    { KeyEvent::KEYCODE_Z, "KEYCODE_Z" },
    { KeyEvent::KEYCODE_COMMA, "KEYCODE_COMMA" },
    { KeyEvent::KEYCODE_PERIOD, "KEYCODE_PERIOD" },
    { KeyEvent::KEYCODE_ALT_LEFT, "KEYCODE_ALT_LEFT" },
    { KeyEvent::KEYCODE_ALT_RIGHT, "KEYCODE_ALT_RIGHT" },
    { KeyEvent::KEYCODE_SHIFT_LEFT, "KEYCODE_SHIFT_LEFT" },
    { KeyEvent::KEYCODE_SHIFT_RIGHT, "KEYCODE_SHIFT_RIGHT" },
    { KeyEvent::KEYCODE_TAB, "KEYCODE_TAB" },
    { KeyEvent::KEYCODE_SPACE, "KEYCODE_SPACE" },
    { KeyEvent::KEYCODE_SYM, "KEYCODE_SYM" },
    { KeyEvent::KEYCODE_EXPLORER, "KEYCODE_EXPLORER" },
    { KeyEvent::KEYCODE_ENVELOPE, "KEYCODE_ENVELOPE" },
    { KeyEvent::KEYCODE_ENTER, "KEYCODE_ENTER" },
    { KeyEvent::KEYCODE_DEL, "KEYCODE_DEL" },
    { KeyEvent::KEYCODE_GRAVE, "KEYCODE_GRAVE" },
    { KeyEvent::KEYCODE_MINUS, "KEYCODE_MINUS" },
    { KeyEvent::KEYCODE_EQUALS, "KEYCODE_EQUALS" },
    { KeyEvent::KEYCODE_LEFT_BRACKET, "KEYCODE_LEFT_BRACKET" },
    { KeyEvent::KEYCODE_RIGHT_BRACKET, "KEYCODE_RIGHT_BRACKET" },
    { KeyEvent::KEYCODE_BACKSLASH, "KEYCODE_BACKSLASH" },
    { KeyEvent::KEYCODE_SEMICOLON, "KEYCODE_SEMICOLON" },
    { KeyEvent::KEYCODE_APOSTROPHE, "KEYCODE_APOSTROPHE" },
    { KeyEvent::KEYCODE_SLASH, "KEYCODE_SLASH" },
    { KeyEvent::KEYCODE_AT, "KEYCODE_AT" },
    { KeyEvent::KEYCODE_PLUS, "KEYCODE_PLUS" },
    { KeyEvent::KEYCODE_MENU, "KEYCODE_MENU" },
    { KeyEvent::KEYCODE_PAGE_UP, "KEYCODE_PAGE_UP" },
    { KeyEvent::KEYCODE_PAGE_DOWN, "KEYCODE_PAGE_DOWN" },
    { KeyEvent::KEYCODE_ESCAPE, "KEYCODE_ESCAPE" },
    { KeyEvent::KEYCODE_FORWARD_DEL, "KEYCODE_FORWARD_DEL" },
    { KeyEvent::KEYCODE_CTRL_LEFT, "KEYCODE_CTRL_LEFT" },
    { KeyEvent::KEYCODE_CTRL_RIGHT, "KEYCODE_CTRL_RIGHT" },
    { KeyEvent::KEYCODE_CAPS_LOCK, "KEYCODE_CAPS_LOCK" },
    { KeyEvent::KEYCODE_SCROLL_LOCK, "KEYCODE_SCROLL_LOCK" },
    { KeyEvent::KEYCODE_META_LEFT, "KEYCODE_META_LEFT" },
    { KeyEvent::KEYCODE_META_RIGHT, "KEYCODE_META_RIGHT" },
    { KeyEvent::KEYCODE_FUNCTION, "KEYCODE_FUNCTION" },
    { KeyEvent::KEYCODE_SYSRQ, "KEYCODE_SYSRQ" },
    { KeyEvent::KEYCODE_BREAK, "KEYCODE_BREAK" },
    { KeyEvent::KEYCODE_MOVE_HOME, "KEYCODE_MOVE_HOME" },
    { KeyEvent::KEYCODE_MOVE_END, "KEYCODE_MOVE_END" },
    { KeyEvent::KEYCODE_INSERT, "KEYCODE_INSERT" },
    { KeyEvent::KEYCODE_FORWARD, "KEYCODE_FORWARD" },
    { KeyEvent::KEYCODE_MEDIA_PLAY, "KEYCODE_MEDIA_PLAY" },
    { KeyEvent::KEYCODE_MEDIA_PAUSE, "KEYCODE_MEDIA_PAUSE" },
    { KeyEvent::KEYCODE_MEDIA_CLOSE, "KEYCODE_MEDIA_CLOSE" },
    { KeyEvent::KEYCODE_MEDIA_EJECT, "KEYCODE_MEDIA_EJECT" },
    { KeyEvent::KEYCODE_MEDIA_RECORD, "KEYCODE_MEDIA_RECORD" },
    { KeyEvent::KEYCODE_F1, "KEYCODE_F1" },
    { KeyEvent::KEYCODE_F2, "KEYCODE_F2" },
    { KeyEvent::KEYCODE_F3, "KEYCODE_F3" },
    { KeyEvent::KEYCODE_F4, "KEYCODE_F4" },
    { KeyEvent::KEYCODE_F5, "KEYCODE_F5" },
    { KeyEvent::KEYCODE_F6, "KEYCODE_F6" },
    { KeyEvent::KEYCODE_F7, "KEYCODE_F7" },
    { KeyEvent::KEYCODE_F8, "KEYCODE_F8" },
    { KeyEvent::KEYCODE_F9, "KEYCODE_F9" },
    { KeyEvent::KEYCODE_F10, "KEYCODE_F10" },
    { KeyEvent::KEYCODE_F11, "KEYCODE_F11" },
    { KeyEvent::KEYCODE_F12, "KEYCODE_F12" },
    { KeyEvent::KEYCODE_NUM_LOCK, "KEYCODE_NUM_LOCK" },
    { KeyEvent::KEYCODE_NUMPAD_0, "KEYCODE_NUMPAD_0" },
    { KeyEvent::KEYCODE_NUMPAD_1, "KEYCODE_NUMPAD_1" },
    { KeyEvent::KEYCODE_NUMPAD_2, "KEYCODE_NUMPAD_2" },
    { KeyEvent::KEYCODE_NUMPAD_3, "KEYCODE_NUMPAD_3" },
    { KeyEvent::KEYCODE_NUMPAD_4, "KEYCODE_NUMPAD_4" },
    { KeyEvent::KEYCODE_NUMPAD_5, "KEYCODE_NUMPAD_5" },
    { KeyEvent::KEYCODE_NUMPAD_6, "KEYCODE_NUMPAD_6" },
    { KeyEvent::KEYCODE_NUMPAD_7, "KEYCODE_NUMPAD_7" },
    { KeyEvent::KEYCODE_NUMPAD_8, "KEYCODE_NUMPAD_8" },
    { KeyEvent::KEYCODE_NUMPAD_9, "KEYCODE_NUMPAD_9" },
    { KeyEvent::KEYCODE_NUMPAD_DIVIDE, "KEYCODE_NUMPAD_DIVIDE" },
    { KeyEvent::KEYCODE_NUMPAD_MULTIPLY, "KEYCODE_NUMPAD_MULTIPLY" },
    { KeyEvent::KEYCODE_NUMPAD_SUBTRACT, "KEYCODE_NUMPAD_SUBTRACT" },
    { KeyEvent::KEYCODE_NUMPAD_ADD, "KEYCODE_NUMPAD_ADD" },
    { KeyEvent::KEYCODE_NUMPAD_DOT, "KEYCODE_NUMPAD_DOT" },
    { KeyEvent::KEYCODE_NUMPAD_COMMA, "KEYCODE_NUMPAD_COMMA" },
    { KeyEvent::KEYCODE_NUMPAD_ENTER, "KEYCODE_NUMPAD_ENTER" },
    { KeyEvent::KEYCODE_NUMPAD_EQUALS, "KEYCODE_NUMPAD_EQUALS" },
    { KeyEvent::KEYCODE_NUMPAD_LEFT_PAREN, "KEYCODE_NUMPAD_LEFT_PAREN" },
    { KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN, "KEYCODE_NUMPAD_RIGHT_PAREN" },
    { KeyEvent::KEYCODE_VIRTUAL_MULTITASK, "KEYCODE_VIRTUAL_MULTITASK" },
    { KeyEvent::KEYCODE_BUTTON_A, "KEYCODE_BUTTON_A" },
    { KeyEvent::KEYCODE_BUTTON_B, "KEYCODE_BUTTON_B" },
    { KeyEvent::KEYCODE_BUTTON_C, "KEYCODE_BUTTON_C" },
    { KeyEvent::KEYCODE_BUTTON_X, "KEYCODE_BUTTON_X" },
    { KeyEvent::KEYCODE_BUTTON_Y, "KEYCODE_BUTTON_Y" },
    { KeyEvent::KEYCODE_BUTTON_Z, "KEYCODE_BUTTON_Z" },
    { KeyEvent::KEYCODE_BUTTON_L1, "KEYCODE_BUTTON_L1" },
    { KeyEvent::KEYCODE_BUTTON_R1, "KEYCODE_BUTTON_R1" },
    { KeyEvent::KEYCODE_BUTTON_L2, "KEYCODE_BUTTON_L2" },
    { KeyEvent::KEYCODE_BUTTON_R2, "KEYCODE_BUTTON_R2" },
    { KeyEvent::KEYCODE_BUTTON_SELECT, "KEYCODE_BUTTON_SELECT" },
    { KeyEvent::KEYCODE_BUTTON_START, "KEYCODE_BUTTON_START" },
    { KeyEvent::KEYCODE_BUTTON_MODE, "KEYCODE_BUTTON_MODE" },
    { KeyEvent::KEYCODE_BUTTON_THUMBL, "KEYCODE_BUTTON_THUMBL" },
    { KeyEvent::KEYCODE_BUTTON_THUMBR, "KEYCODE_BUTTON_THUMBR" },
    { KeyEvent::KEYCODE_BUTTON_TRIGGER, "KEYCODE_BUTTON_TRIGGER" },
    { KeyEvent::KEYCODE_BUTTON_THUMB, "KEYCODE_BUTTON_THUMB" },
    { KeyEvent::KEYCODE_BUTTON_THUMB2, "KEYCODE_BUTTON_THUMB2" },
    { KeyEvent::KEYCODE_BUTTON_TOP, "KEYCODE_BUTTON_TOP" },
    { KeyEvent::KEYCODE_BUTTON_TOP2, "KEYCODE_BUTTON_TOP2" },
    { KeyEvent::KEYCODE_BUTTON_PINKIE, "KEYCODE_BUTTON_PINKIE" },
    { KeyEvent::KEYCODE_BUTTON_BASE1, "KEYCODE_BUTTON_BASE1" },
    { KeyEvent::KEYCODE_BUTTON_BASE2, "KEYCODE_BUTTON_BASE2" },
    { KeyEvent::KEYCODE_BUTTON_BASE3, "KEYCODE_BUTTON_BASE3" },
    { KeyEvent::KEYCODE_BUTTON_BASE4, "KEYCODE_BUTTON_BASE4" },
    { KeyEvent::KEYCODE_BUTTON_BASE5, "KEYCODE_BUTTON_BASE5" },
    { KeyEvent::KEYCODE_BUTTON_BASE6, "KEYCODE_BUTTON_BASE6" },
    { KeyEvent::KEYCODE_BUTTON_BASE7, "KEYCODE_BUTTON_BASE7" },
    { KeyEvent::KEYCODE_BUTTON_BASE8, "KEYCODE_BUTTON_BASE8" },
    { KeyEvent::KEYCODE_BUTTON_BASE9, "KEYCODE_BUTTON_BASE9" },
    { KeyEvent::KEYCODE_BUTTON_DEAD, "KEYCODE_BUTTON_DEAD" },
    { KeyEvent::KEYCODE_SLEEP, "KEYCODE_SLEEP" },
    { KeyEvent::KEYCODE_ZENKAKU_HANKAKU, "KEYCODE_ZENKAKU_HANKAKU" },
    { KeyEvent::KEYCODE_102ND, "KEYCODE_102ND" },
    { KeyEvent::KEYCODE_RO, "KEYCODE_RO" },
    { KeyEvent::KEYCODE_KATAKANA, "KEYCODE_KATAKANA" },
    { KeyEvent::KEYCODE_HIRAGANA, "KEYCODE_HIRAGANA" },
    { KeyEvent::KEYCODE_HENKAN, "KEYCODE_HENKAN" },
    { KeyEvent::KEYCODE_KATAKANA_HIRAGANA, "KEYCODE_KATAKANA_HIRAGANA" },
    { KeyEvent::KEYCODE_MUHENKAN, "KEYCODE_MUHENKAN" },
    { KeyEvent::KEYCODE_LINEFEED, "KEYCODE_LINEFEED" },
    { KeyEvent::KEYCODE_MACRO, "KEYCODE_MACRO" },
    { KeyEvent::KEYCODE_NUMPAD_PLUSMINUS, "KEYCODE_NUMPAD_PLUSMINUS" },
    { KeyEvent::KEYCODE_SCALE, "KEYCODE_SCALE" },
    { KeyEvent::KEYCODE_HANGUEL, "KEYCODE_HANGUEL" },
    { KeyEvent::KEYCODE_HANJA, "KEYCODE_HANJA" },
    { KeyEvent::KEYCODE_YEN, "KEYCODE_YEN" },
    { KeyEvent::KEYCODE_STOP, "KEYCODE_STOP" },
    { KeyEvent::KEYCODE_AGAIN, "KEYCODE_AGAIN" },
    { KeyEvent::KEYCODE_PROPS, "KEYCODE_PROPS" },
    { KeyEvent::KEYCODE_UNDO, "KEYCODE_UNDO" },
    { KeyEvent::KEYCODE_COPY, "KEYCODE_COPY" },
    { KeyEvent::KEYCODE_OPEN, "KEYCODE_OPEN" },
    { KeyEvent::KEYCODE_PASTE, "KEYCODE_PASTE" },
    { KeyEvent::KEYCODE_FIND, "KEYCODE_FIND" },
    { KeyEvent::KEYCODE_CUT, "KEYCODE_CUT" },
    { KeyEvent::KEYCODE_HELP, "KEYCODE_HELP" },
    { KeyEvent::KEYCODE_CALC, "KEYCODE_CALC" },
    { KeyEvent::KEYCODE_FILE, "KEYCODE_FILE" },
    { KeyEvent::KEYCODE_BOOKMARKS, "KEYCODE_BOOKMARKS" },
    { KeyEvent::KEYCODE_NEXT, "KEYCODE_NEXT" },
    { KeyEvent::KEYCODE_PLAYPAUSE, "KEYCODE_PLAYPAUSE" },
    { KeyEvent::KEYCODE_PREVIOUS, "KEYCODE_PREVIOUS" },
    { KeyEvent::KEYCODE_STOPCD, "KEYCODE_STOPCD" },
    { KeyEvent::KEYCODE_CONFIG, "KEYCODE_CONFIG" },
    { KeyEvent::KEYCODE_REFRESH, "KEYCODE_REFRESH" },
    { KeyEvent::KEYCODE_EXIT, "KEYCODE_EXIT" },
    { KeyEvent::KEYCODE_EDIT, "KEYCODE_EDIT" },
    { KeyEvent::KEYCODE_SCROLLUP, "KEYCODE_SCROLLUP" },
    { KeyEvent::KEYCODE_SCROLLDOWN, "KEYCODE_SCROLLDOWN" },
    { KeyEvent::KEYCODE_NEW, "KEYCODE_NEW" },
    { KeyEvent::KEYCODE_REDO, "KEYCODE_REDO" },
    { KeyEvent::KEYCODE_CLOSE, "KEYCODE_CLOSE" },
    { KeyEvent::KEYCODE_PLAY, "KEYCODE_PLAY" },
    { KeyEvent::KEYCODE_BASSBOOST, "KEYCODE_BASSBOOST" },
    { KeyEvent::KEYCODE_PRINT, "KEYCODE_PRINT" },
    { KeyEvent::KEYCODE_CHAT, "KEYCODE_CHAT" },
    { KeyEvent::KEYCODE_FINANCE, "KEYCODE_FINANCE" },
    { KeyEvent::KEYCODE_CANCEL, "KEYCODE_CANCEL" },
    { KeyEvent::KEYCODE_KBDILLUM_TOGGLE, "KEYCODE_KBDILLUM_TOGGLE" },
    { KeyEvent::KEYCODE_KBDILLUM_DOWN, "KEYCODE_KBDILLUM_DOWN" },
    { KeyEvent::KEYCODE_KBDILLUM_UP, "KEYCODE_KBDILLUM_UP" },
    { KeyEvent::KEYCODE_SEND, "KEYCODE_SEND" },
    { KeyEvent::KEYCODE_REPLY, "KEYCODE_REPLY" },
    { KeyEvent::KEYCODE_FORWARDMAIL, "KEYCODE_FORWARDMAIL" },
    { KeyEvent::KEYCODE_SAVE, "KEYCODE_SAVE" },
    { KeyEvent::KEYCODE_DOCUMENTS, "KEYCODE_DOCUMENTS" },
    { KeyEvent::KEYCODE_VIDEO_NEXT, "KEYCODE_VIDEO_NEXT" },
    { KeyEvent::KEYCODE_VIDEO_PREV, "KEYCODE_VIDEO_PREV" },
    { KeyEvent::KEYCODE_BRIGHTNESS_CYCLE, "KEYCODE_BRIGHTNESS_CYCLE" },
    { KeyEvent::KEYCODE_BRIGHTNESS_ZERO, "KEYCODE_BRIGHTNESS_ZERO" },
    { KeyEvent::KEYCODE_DISPLAY_OFF, "KEYCODE_DISPLAY_OFF" },
    { KeyEvent::KEYCODE_BTN_MISC, "KEYCODE_BTN_MISC" },
    { KeyEvent::KEYCODE_GOTO, "KEYCODE_GOTO" },
    { KeyEvent::KEYCODE_INFO, "KEYCODE_INFO" },
    { KeyEvent::KEYCODE_PROGRAM, "KEYCODE_PROGRAM" },
    { KeyEvent::KEYCODE_PVR, "KEYCODE_PVR" },
    { KeyEvent::KEYCODE_SUBTITLE, "KEYCODE_SUBTITLE" },
    { KeyEvent::KEYCODE_FULL_SCREEN, "KEYCODE_FULL_SCREEN" },
    { KeyEvent::KEYCODE_KEYBOARD, "KEYCODE_KEYBOARD" },
    { KeyEvent::KEYCODE_ASPECT_RATIO, "KEYCODE_ASPECT_RATIO" },
    { KeyEvent::KEYCODE_PC, "KEYCODE_PC" },
    { KeyEvent::KEYCODE_TV, "KEYCODE_TV" },
    { KeyEvent::KEYCODE_TV2, "KEYCODE_TV2" },
    { KeyEvent::KEYCODE_VCR, "KEYCODE_VCR" },
    { KeyEvent::KEYCODE_VCR2, "KEYCODE_VCR2" },
    { KeyEvent::KEYCODE_SAT, "KEYCODE_SAT" },
    { KeyEvent::KEYCODE_CD, "KEYCODE_CD" },
    { KeyEvent::KEYCODE_TAPE, "KEYCODE_TAPE" },
    { KeyEvent::KEYCODE_TUNER, "KEYCODE_TUNER" },
    { KeyEvent::KEYCODE_PLAYER, "KEYCODE_PLAYER" },
    { KeyEvent::KEYCODE_DVD, "KEYCODE_DVD" },
    { KeyEvent::KEYCODE_AUDIO, "KEYCODE_AUDIO" },
    { KeyEvent::KEYCODE_VIDEO, "KEYCODE_VIDEO" },
    { KeyEvent::KEYCODE_MEMO, "KEYCODE_MEMO" },
    { KeyEvent::KEYCODE_CALENDAR, "KEYCODE_CALENDAR" },
    { KeyEvent::KEYCODE_RED, "KEYCODE_RED" },
    { KeyEvent::KEYCODE_GREEN, "KEYCODE_GREEN" },
    { KeyEvent::KEYCODE_YELLOW, "KEYCODE_YELLOW" },
    { KeyEvent::KEYCODE_BLUE, "KEYCODE_BLUE" },
    { KeyEvent::KEYCODE_CHANNELUP, "KEYCODE_CHANNELUP" },
    { KeyEvent::KEYCODE_CHANNELDOWN, "KEYCODE_CHANNELDOWN" },
    { KeyEvent::KEYCODE_LAST, "KEYCODE_LAST" },
    { KeyEvent::KEYCODE_RESTART, "KEYCODE_RESTART" },
    { KeyEvent::KEYCODE_SLOW, "KEYCODE_SLOW" },
    { KeyEvent::KEYCODE_SHUFFLE, "KEYCODE_SHUFFLE" },
    { KeyEvent::KEYCODE_VIDEOPHONE, "KEYCODE_VIDEOPHONE" },
    { KeyEvent::KEYCODE_GAMES, "KEYCODE_GAMES" },
    { KeyEvent::KEYCODE_ZOOMIN, "KEYCODE_ZOOMIN" },
    { KeyEvent::KEYCODE_ZOOMOUT, "KEYCODE_ZOOMOUT" },
    { KeyEvent::KEYCODE_ZOOMRESET, "KEYCODE_ZOOMRESET" },
    { KeyEvent::KEYCODE_WORDPROCESSOR, "KEYCODE_WORDPROCESSOR" },
    { KeyEvent::KEYCODE_EDITOR, "KEYCODE_EDITOR" },
    { KeyEvent::KEYCODE_SPREADSHEET, "KEYCODE_SPREADSHEET" },
    { KeyEvent::KEYCODE_GRAPHICSEDITOR, "KEYCODE_GRAPHICSEDITOR" },
    { KeyEvent::KEYCODE_PRESENTATION, "KEYCODE_PRESENTATION" },
    { KeyEvent::KEYCODE_DATABASE, "KEYCODE_DATABASE" },
    { KeyEvent::KEYCODE_NEWS, "KEYCODE_NEWS" },
    { KeyEvent::KEYCODE_VOICEMAIL, "KEYCODE_VOICEMAIL" },
    { KeyEvent::KEYCODE_ADDRESSBOOK, "KEYCODE_ADDRESSBOOK" },
    { KeyEvent::KEYCODE_MESSENGER, "KEYCODE_MESSENGER" },
    { KeyEvent::KEYCODE_BRIGHTNESS_TOGGLE, "KEYCODE_BRIGHTNESS_TOGGLE" },
    { KeyEvent::KEYCODE_SPELLCHECK, "KEYCODE_SPELLCHECK" },
    { KeyEvent::KEYCODE_COFFEE, "KEYCODE_COFFEE" },
    { KeyEvent::KEYCODE_MEDIA_REPEAT, "KEYCODE_MEDIA_REPEAT" },
    { KeyEvent::KEYCODE_IMAGES, "KEYCODE_IMAGES" },
    { KeyEvent::KEYCODE_BUTTONCONFIG, "KEYCODE_BUTTONCONFIG" },
    { KeyEvent::KEYCODE_TASKMANAGER, "KEYCODE_TASKMANAGER" },
    { KeyEvent::KEYCODE_JOURNAL, "KEYCODE_JOURNAL" },
    { KeyEvent::KEYCODE_CONTROLPANEL, "KEYCODE_CONTROLPANEL" },
    { KeyEvent::KEYCODE_APPSELECT, "KEYCODE_APPSELECT" },
    { KeyEvent::KEYCODE_SCREENSAVER, "KEYCODE_SCREENSAVER" },
    { KeyEvent::KEYCODE_ASSISTANT, "KEYCODE_ASSISTANT" },
    { KeyEvent::KEYCODE_KBD_LAYOUT_NEXT, "KEYCODE_KBD_LAYOUT_NEXT" },
    { KeyEvent::KEYCODE_BRIGHTNESS_MIN, "KEYCODE_BRIGHTNESS_MIN" },
    { KeyEvent::KEYCODE_BRIGHTNESS_MAX, "KEYCODE_BRIGHTNESS_MAX" },
    { KeyEvent::KEYCODE_KBDINPUTASSIST_PREV, "KEYCODE_KBDINPUTASSIST_PREV" },
    { KeyEvent::KEYCODE_KBDINPUTASSIST_NEXT, "KEYCODE_KBDINPUTASSIST_NEXT" },
    { KeyEvent::KEYCODE_KBDINPUTASSIST_PREVGROUP, "KEYCODE_KBDINPUTASSIST_PREVGROUP" },
    { KeyEvent::KEYCODE_KBDINPUTASSIST_NEXTGROUP, "KEYCODE_KBDINPUTASSIST_NEXTGROUP" },
    { KeyEvent::KEYCODE_KBDINPUTASSIST_ACCEPT, "KEYCODE_KBDINPUTASSIST_ACCEPT" },
    { KeyEvent::KEYCODE_KBDINPUTASSIST_CANCEL, "KEYCODE_KBDINPUTASSIST_CANCEL" },
    { KeyEvent::KEYCODE_FRONT, "KEYCODE_FRONT" },
    { KeyEvent::KEYCODE_SETUP, "KEYCODE_SETUP" },
    { KeyEvent::KEYCODE_WAKEUP, "KEYCODE_WAKEUP" },
    { KeyEvent::KEYCODE_SENDFILE, "KEYCODE_SENDFILE" },
    { KeyEvent::KEYCODE_DELETEFILE, "KEYCODE_DELETEFILE" },
    { KeyEvent::KEYCODE_XFER, "KEYCODE_XFER" },
    { KeyEvent::KEYCODE_PROG1, "KEYCODE_PROG1" },
    { KeyEvent::KEYCODE_PROG2, "KEYCODE_PROG2" },
    { KeyEvent::KEYCODE_MSDOS, "KEYCODE_MSDOS" },
    { KeyEvent::KEYCODE_SCREENLOCK, "KEYCODE_SCREENLOCK" },
    { KeyEvent::KEYCODE_DIRECTION_ROTATE_DISPLAY, "KEYCODE_DIRECTION_ROTATE_DISPLAY" },
    { KeyEvent::KEYCODE_CYCLEWINDOWS, "KEYCODE_CYCLEWINDOWS" },
    { KeyEvent::KEYCODE_COMPUTER, "KEYCODE_COMPUTER" },
    { KeyEvent::KEYCODE_EJECTCLOSECD, "KEYCODE_EJECTCLOSECD" },
    { KeyEvent::KEYCODE_ISO, "KEYCODE_ISO" },
    { KeyEvent::KEYCODE_MOVE, "KEYCODE_MOVE" },
    { KeyEvent::KEYCODE_F13, "KEYCODE_F13" },
    { KeyEvent::KEYCODE_F14, "KEYCODE_F14" },
    { KeyEvent::KEYCODE_F15, "KEYCODE_F15" },
    { KeyEvent::KEYCODE_F16, "KEYCODE_F16" },
    { KeyEvent::KEYCODE_F17, "KEYCODE_F17" },
    { KeyEvent::KEYCODE_F18, "KEYCODE_F18" },
    { KeyEvent::KEYCODE_F19, "KEYCODE_F19" },
    { KeyEvent::KEYCODE_F20, "KEYCODE_F20" },
    { KeyEvent::KEYCODE_F21, "KEYCODE_F21" },
    { KeyEvent::KEYCODE_F22, "KEYCODE_F22" },
    { KeyEvent::KEYCODE_F23, "KEYCODE_F23" },
    { KeyEvent::KEYCODE_F24, "KEYCODE_F24" },
    { KeyEvent::KEYCODE_PROG3, "KEYCODE_PROG3" },
    { KeyEvent::KEYCODE_PROG4, "KEYCODE_PROG4" },
    { KeyEvent::KEYCODE_DASHBOARD, "KEYCODE_DASHBOARD" },
    { KeyEvent::KEYCODE_SUSPEND, "KEYCODE_SUSPEND" },
    { KeyEvent::KEYCODE_HP, "KEYCODE_HP" },
    { KeyEvent::KEYCODE_SOUND, "KEYCODE_SOUND" },
    { KeyEvent::KEYCODE_QUESTION, "KEYCODE_QUESTION" },
    { KeyEvent::KEYCODE_CONNECT, "KEYCODE_CONNECT" },
    { KeyEvent::KEYCODE_SPORT, "KEYCODE_SPORT" },
    { KeyEvent::KEYCODE_SHOP, "KEYCODE_SHOP" },
    { KeyEvent::KEYCODE_ALTERASE, "KEYCODE_ALTERASE" },
    { KeyEvent::KEYCODE_SWITCHVIDEOMODE, "KEYCODE_SWITCHVIDEOMODE" },
    { KeyEvent::KEYCODE_BATTERY, "KEYCODE_BATTERY" },
    { KeyEvent::KEYCODE_BLUETOOTH, "KEYCODE_BLUETOOTH" },
    { KeyEvent::KEYCODE_WLAN, "KEYCODE_WLAN" },
    { KeyEvent::KEYCODE_UWB, "KEYCODE_UWB" },
    { KeyEvent::KEYCODE_WWAN_WIMAX, "KEYCODE_WWAN_WIMAX" },
    { KeyEvent::KEYCODE_RFKILL, "KEYCODE_RFKILL" },
    { KeyEvent::KEYCODE_CHANNEL, "KEYCODE_CHANNEL" },
    { KeyEvent::KEYCODE_BTN_0, "KEYCODE_BTN_0" },
    { KeyEvent::KEYCODE_BTN_1, "KEYCODE_BTN_1" },
    { KeyEvent::KEYCODE_BTN_2, "KEYCODE_BTN_2" },
    { KeyEvent::KEYCODE_BTN_3, "KEYCODE_BTN_3" },
    { KeyEvent::KEYCODE_BTN_4, "KEYCODE_BTN_4" },
    { KeyEvent::KEYCODE_BTN_5, "KEYCODE_BTN_5" },
    { KeyEvent::KEYCODE_BTN_6, "KEYCODE_BTN_6" },
    { KeyEvent::KEYCODE_BTN_7, "KEYCODE_BTN_7" },
    { KeyEvent::KEYCODE_BTN_8, "KEYCODE_BTN_8" },
    { KeyEvent::KEYCODE_BTN_9, "KEYCODE_BTN_9" },
    { KeyEvent::KEYCODE_BRL_DOT1, "KEYCODE_BRL_DOT1" },
    { KeyEvent::KEYCODE_BRL_DOT2, "KEYCODE_BRL_DOT2" },
    { KeyEvent::KEYCODE_BRL_DOT3, "KEYCODE_BRL_DOT3" },
    { KeyEvent::KEYCODE_BRL_DOT4, "KEYCODE_BRL_DOT4" },
    { KeyEvent::KEYCODE_BRL_DOT5, "KEYCODE_BRL_DOT5" },
    { KeyEvent::KEYCODE_BRL_DOT6, "KEYCODE_BRL_DOT6" },
    { KeyEvent::KEYCODE_BRL_DOT7, "KEYCODE_BRL_DOT7" },
    { KeyEvent::KEYCODE_BRL_DOT8, "KEYCODE_BRL_DOT8" },
    { KeyEvent::KEYCODE_BRL_DOT9, "KEYCODE_BRL_DOT9" },
    { KeyEvent::KEYCODE_BRL_DOT10, "KEYCODE_BRL_DOT10" },
    { KeyEvent::KEYCODE_LEFT_KNOB_ROLL_UP, "KEYCODE_LEFT_KNOB_ROLL_UP" },
    { KeyEvent::KEYCODE_LEFT_KNOB_ROLL_DOWN, "KEYCODE_LEFT_KNOB_ROLL_DOWN" },
    { KeyEvent::KEYCODE_LEFT_KNOB, "KEYCODE_LEFT_KNOB" },
    { KeyEvent::KEYCODE_RIGHT_KNOB_ROLL_UP, "KEYCODE_RIGHT_KNOB_ROLL_UP" },
    { KeyEvent::KEYCODE_RIGHT_KNOB_ROLL_DOWN, "KEYCODE_RIGHT_KNOB_ROLL_DOWN" },
    { KeyEvent::KEYCODE_RIGHT_KNOB, "KEYCODE_RIGHT_KNOB" },
    { KeyEvent::KEYCODE_VOICE_SOURCE_SWITCH, "KEYCODE_VOICE_SOURCE_SWITCH" },
    { KeyEvent::KEYCODE_LAUNCHER_MENU, "KEYCODE_LAUNCHER_MENU" },
};
} // namespace
const int32_t KeyEvent::UNKNOWN_FUNCTION_KEY = -1;
const int32_t KeyEvent::NUM_LOCK_FUNCTION_KEY = 0;
const int32_t KeyEvent::CAPS_LOCK_FUNCTION_KEY = 1;
const int32_t KeyEvent::SCROLL_LOCK_FUNCTION_KEY = 2;
const int32_t KeyEvent::KEYCODE_FN = 0;
const int32_t KeyEvent::KEYCODE_UNKNOWN = -1;
const int32_t KeyEvent::KEYCODE_HOME = 1;
const int32_t KeyEvent::KEYCODE_BACK = 2;
const int32_t KeyEvent::KEYCODE_CALL = 3;
const int32_t KeyEvent::KEYCODE_ENDCALL = 4;
const int32_t KeyEvent::KEYCODE_CLEAR = 5;
const int32_t KeyEvent::KEYCODE_HEADSETHOOK = 6;
const int32_t KeyEvent::KEYCODE_FOCUS = 7;
const int32_t KeyEvent::KEYCODE_NOTIFICATION = 8;
const int32_t KeyEvent::KEYCODE_SEARCH = 9;
const int32_t KeyEvent::KEYCODE_MEDIA_PLAY_PAUSE = 10;
const int32_t KeyEvent::KEYCODE_MEDIA_STOP = 11;
const int32_t KeyEvent::KEYCODE_MEDIA_NEXT = 12;
const int32_t KeyEvent::KEYCODE_MEDIA_PREVIOUS = 13;
const int32_t KeyEvent::KEYCODE_MEDIA_REWIND = 14;
const int32_t KeyEvent::KEYCODE_MEDIA_FAST_FORWARD = 15;
const int32_t KeyEvent::KEYCODE_VOLUME_UP = 16;
const int32_t KeyEvent::KEYCODE_VOLUME_DOWN = 17;
const int32_t KeyEvent::KEYCODE_POWER = 18;
const int32_t KeyEvent::KEYCODE_CAMERA = 19;
const int32_t KeyEvent::KEYCODE_VOICE_ASSISTANT = 20;
const int32_t KeyEvent::KEYCODE_CUSTOM1 = 21;
const int32_t KeyEvent::KEYCODE_VOLUME_MUTE = 22;
const int32_t KeyEvent::KEYCODE_MUTE = 23;
const int32_t KeyEvent::KEYCODE_BRIGHTNESS_UP = 40;
const int32_t KeyEvent::KEYCODE_BRIGHTNESS_DOWN = 41;
const int32_t KeyEvent::KEYCODE_WEAR_1 = 1001;
const int32_t KeyEvent::KEYCODE_0 = 2000;
const int32_t KeyEvent::KEYCODE_1 = 2001;
const int32_t KeyEvent::KEYCODE_2 = 2002;
const int32_t KeyEvent::KEYCODE_3 = 2003;
const int32_t KeyEvent::KEYCODE_4 = 2004;
const int32_t KeyEvent::KEYCODE_5 = 2005;
const int32_t KeyEvent::KEYCODE_6 = 2006;
const int32_t KeyEvent::KEYCODE_7 = 2007;
const int32_t KeyEvent::KEYCODE_8 = 2008;
const int32_t KeyEvent::KEYCODE_9 = 2009;
const int32_t KeyEvent::KEYCODE_STAR = 2010;
const int32_t KeyEvent::KEYCODE_POUND = 2011;
const int32_t KeyEvent::KEYCODE_DPAD_UP = 2012;
const int32_t KeyEvent::KEYCODE_DPAD_DOWN = 2013;
const int32_t KeyEvent::KEYCODE_DPAD_LEFT = 2014;
const int32_t KeyEvent::KEYCODE_DPAD_RIGHT = 2015;
const int32_t KeyEvent::KEYCODE_DPAD_CENTER = 2016;
const int32_t KeyEvent::KEYCODE_A = 2017;
const int32_t KeyEvent::KEYCODE_B = 2018;
const int32_t KeyEvent::KEYCODE_C = 2019;
const int32_t KeyEvent::KEYCODE_D = 2020;
const int32_t KeyEvent::KEYCODE_E = 2021;
const int32_t KeyEvent::KEYCODE_F = 2022;
const int32_t KeyEvent::KEYCODE_G = 2023;
const int32_t KeyEvent::KEYCODE_H = 2024;
const int32_t KeyEvent::KEYCODE_I = 2025;
const int32_t KeyEvent::KEYCODE_J = 2026;
const int32_t KeyEvent::KEYCODE_K = 2027;
const int32_t KeyEvent::KEYCODE_L = 2028;
const int32_t KeyEvent::KEYCODE_M = 2029;
const int32_t KeyEvent::KEYCODE_N = 2030;
const int32_t KeyEvent::KEYCODE_O = 2031;
const int32_t KeyEvent::KEYCODE_P = 2032;
const int32_t KeyEvent::KEYCODE_Q = 2033;
const int32_t KeyEvent::KEYCODE_R = 2034;
const int32_t KeyEvent::KEYCODE_S = 2035;
const int32_t KeyEvent::KEYCODE_T = 2036;
const int32_t KeyEvent::KEYCODE_U = 2037;
const int32_t KeyEvent::KEYCODE_V = 2038;
const int32_t KeyEvent::KEYCODE_W = 2039;
const int32_t KeyEvent::KEYCODE_X = 2040;
const int32_t KeyEvent::KEYCODE_Y = 2041;
const int32_t KeyEvent::KEYCODE_Z = 2042;
const int32_t KeyEvent::KEYCODE_COMMA = 2043;
const int32_t KeyEvent::KEYCODE_PERIOD = 2044;
const int32_t KeyEvent::KEYCODE_ALT_LEFT = 2045;
const int32_t KeyEvent::KEYCODE_ALT_RIGHT = 2046;
const int32_t KeyEvent::KEYCODE_SHIFT_LEFT = 2047;
const int32_t KeyEvent::KEYCODE_SHIFT_RIGHT = 2048;
const int32_t KeyEvent::KEYCODE_TAB = 2049;
const int32_t KeyEvent::KEYCODE_SPACE = 2050;
const int32_t KeyEvent::KEYCODE_SYM = 2051;
const int32_t KeyEvent::KEYCODE_EXPLORER = 2052;
const int32_t KeyEvent::KEYCODE_ENVELOPE = 2053;
const int32_t KeyEvent::KEYCODE_ENTER = 2054;
const int32_t KeyEvent::KEYCODE_DEL = 2055;
const int32_t KeyEvent::KEYCODE_GRAVE = 2056;
const int32_t KeyEvent::KEYCODE_MINUS = 2057;
const int32_t KeyEvent::KEYCODE_EQUALS = 2058;
const int32_t KeyEvent::KEYCODE_LEFT_BRACKET = 2059;
const int32_t KeyEvent::KEYCODE_RIGHT_BRACKET = 2060;
const int32_t KeyEvent::KEYCODE_BACKSLASH = 2061;
const int32_t KeyEvent::KEYCODE_SEMICOLON = 2062;
const int32_t KeyEvent::KEYCODE_APOSTROPHE = 2063;
const int32_t KeyEvent::KEYCODE_SLASH = 2064;
const int32_t KeyEvent::KEYCODE_AT = 2065;
const int32_t KeyEvent::KEYCODE_PLUS = 2066;
const int32_t KeyEvent::KEYCODE_MENU = 2067;
const int32_t KeyEvent::KEYCODE_PAGE_UP = 2068;
const int32_t KeyEvent::KEYCODE_PAGE_DOWN = 2069;
const int32_t KeyEvent::KEYCODE_ESCAPE = 2070;
const int32_t KeyEvent::KEYCODE_FORWARD_DEL = 2071;
const int32_t KeyEvent::KEYCODE_CTRL_LEFT = 2072;
const int32_t KeyEvent::KEYCODE_CTRL_RIGHT = 2073;
const int32_t KeyEvent::KEYCODE_CAPS_LOCK = 2074;
const int32_t KeyEvent::KEYCODE_SCROLL_LOCK = 2075;
const int32_t KeyEvent::KEYCODE_META_LEFT = 2076;
const int32_t KeyEvent::KEYCODE_META_RIGHT = 2077;
const int32_t KeyEvent::KEYCODE_FUNCTION = 2078;
const int32_t KeyEvent::KEYCODE_SYSRQ = 2079;
const int32_t KeyEvent::KEYCODE_BREAK = 2080;
const int32_t KeyEvent::KEYCODE_MOVE_HOME = 2081;
const int32_t KeyEvent::KEYCODE_MOVE_END = 2082;
const int32_t KeyEvent::KEYCODE_INSERT = 2083;
const int32_t KeyEvent::KEYCODE_FORWARD = 2084;
const int32_t KeyEvent::KEYCODE_MEDIA_PLAY = 2085;
const int32_t KeyEvent::KEYCODE_MEDIA_PAUSE = 2086;
const int32_t KeyEvent::KEYCODE_MEDIA_CLOSE = 2087;
const int32_t KeyEvent::KEYCODE_MEDIA_EJECT = 2088;
const int32_t KeyEvent::KEYCODE_MEDIA_RECORD = 2089;
const int32_t KeyEvent::KEYCODE_F1 = 2090;
const int32_t KeyEvent::KEYCODE_F2 = 2091;
const int32_t KeyEvent::KEYCODE_F3 = 2092;
const int32_t KeyEvent::KEYCODE_F4 = 2093;
const int32_t KeyEvent::KEYCODE_F5 = 2094;
const int32_t KeyEvent::KEYCODE_F6 = 2095;
const int32_t KeyEvent::KEYCODE_F7 = 2096;
const int32_t KeyEvent::KEYCODE_F8 = 2097;
const int32_t KeyEvent::KEYCODE_F9 = 2098;
const int32_t KeyEvent::KEYCODE_F10 = 2099;
const int32_t KeyEvent::KEYCODE_F11 = 2100;
const int32_t KeyEvent::KEYCODE_F12 = 2101;
const int32_t KeyEvent::KEYCODE_NUM_LOCK = 2102;
const int32_t KeyEvent::KEYCODE_NUMPAD_0 = 2103;
const int32_t KeyEvent::KEYCODE_NUMPAD_1 = 2104;
const int32_t KeyEvent::KEYCODE_NUMPAD_2 = 2105;
const int32_t KeyEvent::KEYCODE_NUMPAD_3 = 2106;
const int32_t KeyEvent::KEYCODE_NUMPAD_4 = 2107;
const int32_t KeyEvent::KEYCODE_NUMPAD_5 = 2108;
const int32_t KeyEvent::KEYCODE_NUMPAD_6 = 2109;
const int32_t KeyEvent::KEYCODE_NUMPAD_7 = 2110;
const int32_t KeyEvent::KEYCODE_NUMPAD_8 = 2111;
const int32_t KeyEvent::KEYCODE_NUMPAD_9 = 2112;
const int32_t KeyEvent::KEYCODE_NUMPAD_DIVIDE = 2113;
const int32_t KeyEvent::KEYCODE_NUMPAD_MULTIPLY = 2114;
const int32_t KeyEvent::KEYCODE_NUMPAD_SUBTRACT = 2115;
const int32_t KeyEvent::KEYCODE_NUMPAD_ADD = 2116;
const int32_t KeyEvent::KEYCODE_NUMPAD_DOT = 2117;
const int32_t KeyEvent::KEYCODE_NUMPAD_COMMA = 2118;
const int32_t KeyEvent::KEYCODE_NUMPAD_ENTER = 2119;
const int32_t KeyEvent::KEYCODE_NUMPAD_EQUALS = 2120;
const int32_t KeyEvent::KEYCODE_NUMPAD_LEFT_PAREN = 2121;
const int32_t KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN = 2122;
const int32_t KeyEvent::KEYCODE_VIRTUAL_MULTITASK = 2210;
const int32_t KeyEvent::KEYCODE_BUTTON_A = 2301;
const int32_t KeyEvent::KEYCODE_BUTTON_B = 2302;
const int32_t KeyEvent::KEYCODE_BUTTON_C = 2303;
const int32_t KeyEvent::KEYCODE_BUTTON_X = 2304;
const int32_t KeyEvent::KEYCODE_BUTTON_Y = 2305;
const int32_t KeyEvent::KEYCODE_BUTTON_Z = 2306;
const int32_t KeyEvent::KEYCODE_BUTTON_L1 = 2307;
const int32_t KeyEvent::KEYCODE_BUTTON_R1 = 2308;
const int32_t KeyEvent::KEYCODE_BUTTON_L2 = 2309;
const int32_t KeyEvent::KEYCODE_BUTTON_R2 = 2310;
const int32_t KeyEvent::KEYCODE_BUTTON_SELECT = 2311;
const int32_t KeyEvent::KEYCODE_BUTTON_START = 2312;
const int32_t KeyEvent::KEYCODE_BUTTON_MODE = 2313;
const int32_t KeyEvent::KEYCODE_BUTTON_THUMBL = 2314;
const int32_t KeyEvent::KEYCODE_BUTTON_THUMBR = 2315;
const int32_t KeyEvent::KEYCODE_BUTTON_TRIGGER = 2401;
const int32_t KeyEvent::KEYCODE_BUTTON_THUMB = 2402;
const int32_t KeyEvent::KEYCODE_BUTTON_THUMB2 = 2403;
const int32_t KeyEvent::KEYCODE_BUTTON_TOP = 2404;
const int32_t KeyEvent::KEYCODE_BUTTON_TOP2 = 2405;
const int32_t KeyEvent::KEYCODE_BUTTON_PINKIE = 2406;
const int32_t KeyEvent::KEYCODE_BUTTON_BASE1 = 2407;
const int32_t KeyEvent::KEYCODE_BUTTON_BASE2 = 2408;
const int32_t KeyEvent::KEYCODE_BUTTON_BASE3 = 2409;
const int32_t KeyEvent::KEYCODE_BUTTON_BASE4 = 2410;
const int32_t KeyEvent::KEYCODE_BUTTON_BASE5 = 2411;
const int32_t KeyEvent::KEYCODE_BUTTON_BASE6 = 2412;
const int32_t KeyEvent::KEYCODE_BUTTON_BASE7 = 2413;
const int32_t KeyEvent::KEYCODE_BUTTON_BASE8 = 2414;
const int32_t KeyEvent::KEYCODE_BUTTON_BASE9 = 2415;
const int32_t KeyEvent::KEYCODE_BUTTON_DEAD = 2416;
const int32_t KeyEvent::KEYCODE_COMPOSE = 2466;
const int32_t KeyEvent::KEYCODE_SLEEP = 2600;
const int32_t KeyEvent::KEYCODE_ZENKAKU_HANKAKU = 2601;
const int32_t KeyEvent::KEYCODE_102ND = 2602;
const int32_t KeyEvent::KEYCODE_RO = 2603;
const int32_t KeyEvent::KEYCODE_KATAKANA = 2604;
const int32_t KeyEvent::KEYCODE_HIRAGANA = 2605;
const int32_t KeyEvent::KEYCODE_HENKAN = 2606;
const int32_t KeyEvent::KEYCODE_KATAKANA_HIRAGANA = 2607;
const int32_t KeyEvent::KEYCODE_MUHENKAN = 2608;
const int32_t KeyEvent::KEYCODE_LINEFEED = 2609;
const int32_t KeyEvent::KEYCODE_MACRO = 2610;
const int32_t KeyEvent::KEYCODE_NUMPAD_PLUSMINUS = 2611;
const int32_t KeyEvent::KEYCODE_SCALE = 2612;
const int32_t KeyEvent::KEYCODE_HANGUEL = 2613;
const int32_t KeyEvent::KEYCODE_HANJA = 2614;
const int32_t KeyEvent::KEYCODE_YEN = 2615;
const int32_t KeyEvent::KEYCODE_STOP = 2616;
const int32_t KeyEvent::KEYCODE_AGAIN = 2617;
const int32_t KeyEvent::KEYCODE_PROPS = 2618;
const int32_t KeyEvent::KEYCODE_UNDO = 2619;
const int32_t KeyEvent::KEYCODE_COPY = 2620;
const int32_t KeyEvent::KEYCODE_OPEN = 2621;
const int32_t KeyEvent::KEYCODE_PASTE = 2622;
const int32_t KeyEvent::KEYCODE_FIND = 2623;
const int32_t KeyEvent::KEYCODE_CUT = 2624;
const int32_t KeyEvent::KEYCODE_HELP = 2625;
const int32_t KeyEvent::KEYCODE_CALC = 2626;
const int32_t KeyEvent::KEYCODE_FILE = 2627;
const int32_t KeyEvent::KEYCODE_BOOKMARKS = 2628;
const int32_t KeyEvent::KEYCODE_NEXT = 2629;
const int32_t KeyEvent::KEYCODE_PLAYPAUSE = 2630;
const int32_t KeyEvent::KEYCODE_PREVIOUS = 2631;
const int32_t KeyEvent::KEYCODE_STOPCD = 2632;
const int32_t KeyEvent::KEYCODE_CONFIG = 2634;
const int32_t KeyEvent::KEYCODE_REFRESH = 2635;
const int32_t KeyEvent::KEYCODE_EXIT = 2636;
const int32_t KeyEvent::KEYCODE_EDIT = 2637;
const int32_t KeyEvent::KEYCODE_SCROLLUP = 2638;
const int32_t KeyEvent::KEYCODE_SCROLLDOWN = 2639;
const int32_t KeyEvent::KEYCODE_NEW = 2640;
const int32_t KeyEvent::KEYCODE_REDO = 2641;
const int32_t KeyEvent::KEYCODE_CLOSE = 2642;
const int32_t KeyEvent::KEYCODE_PLAY = 2643;
const int32_t KeyEvent::KEYCODE_BASSBOOST = 2644;
const int32_t KeyEvent::KEYCODE_PRINT = 2645;
const int32_t KeyEvent::KEYCODE_CHAT = 2646;
const int32_t KeyEvent::KEYCODE_FINANCE = 2647;
const int32_t KeyEvent::KEYCODE_CANCEL = 2648;
const int32_t KeyEvent::KEYCODE_KBDILLUM_TOGGLE = 2649;
const int32_t KeyEvent::KEYCODE_KBDILLUM_DOWN = 2650;
const int32_t KeyEvent::KEYCODE_KBDILLUM_UP = 2651;
const int32_t KeyEvent::KEYCODE_SEND = 2652;
const int32_t KeyEvent::KEYCODE_REPLY = 2653;
const int32_t KeyEvent::KEYCODE_FORWARDMAIL = 2654;
const int32_t KeyEvent::KEYCODE_SAVE = 2655;
const int32_t KeyEvent::KEYCODE_DOCUMENTS = 2656;
const int32_t KeyEvent::KEYCODE_VIDEO_NEXT = 2657;
const int32_t KeyEvent::KEYCODE_VIDEO_PREV = 2658;
const int32_t KeyEvent::KEYCODE_BRIGHTNESS_CYCLE = 2659;
const int32_t KeyEvent::KEYCODE_BRIGHTNESS_ZERO = 2660;
const int32_t KeyEvent::KEYCODE_DISPLAY_OFF = 2661;
const int32_t KeyEvent::KEYCODE_BTN_MISC = 2662;
const int32_t KeyEvent::KEYCODE_GOTO = 2663;
const int32_t KeyEvent::KEYCODE_INFO = 2664;
const int32_t KeyEvent::KEYCODE_PROGRAM = 2665;
const int32_t KeyEvent::KEYCODE_PVR = 2666;
const int32_t KeyEvent::KEYCODE_SUBTITLE = 2667;
const int32_t KeyEvent::KEYCODE_FULL_SCREEN = 2668;
const int32_t KeyEvent::KEYCODE_KEYBOARD = 2669;
const int32_t KeyEvent::KEYCODE_ASPECT_RATIO = 2670;
const int32_t KeyEvent::KEYCODE_PC = 2671;
const int32_t KeyEvent::KEYCODE_TV = 2672;
const int32_t KeyEvent::KEYCODE_TV2 = 2673;
const int32_t KeyEvent::KEYCODE_VCR = 2674;
const int32_t KeyEvent::KEYCODE_VCR2 = 2675;
const int32_t KeyEvent::KEYCODE_SAT = 2676;
const int32_t KeyEvent::KEYCODE_CD = 2677;
const int32_t KeyEvent::KEYCODE_TAPE = 2678;
const int32_t KeyEvent::KEYCODE_TUNER = 2679;
const int32_t KeyEvent::KEYCODE_PLAYER = 2680;
const int32_t KeyEvent::KEYCODE_DVD = 2681;
const int32_t KeyEvent::KEYCODE_AUDIO = 2682;
const int32_t KeyEvent::KEYCODE_VIDEO = 2683;
const int32_t KeyEvent::KEYCODE_MEMO = 2684;
const int32_t KeyEvent::KEYCODE_CALENDAR = 2685;
const int32_t KeyEvent::KEYCODE_RED = 2686;
const int32_t KeyEvent::KEYCODE_GREEN = 2687;
const int32_t KeyEvent::KEYCODE_YELLOW = 2688;
const int32_t KeyEvent::KEYCODE_BLUE = 2689;
const int32_t KeyEvent::KEYCODE_CHANNELUP = 2690;
const int32_t KeyEvent::KEYCODE_CHANNELDOWN = 2691;
const int32_t KeyEvent::KEYCODE_LAST = 2692;
const int32_t KeyEvent::KEYCODE_RESTART = 2693;
const int32_t KeyEvent::KEYCODE_SLOW = 2694;
const int32_t KeyEvent::KEYCODE_SHUFFLE = 2695;
const int32_t KeyEvent::KEYCODE_VIDEOPHONE = 2696;
const int32_t KeyEvent::KEYCODE_GAMES = 2697;
const int32_t KeyEvent::KEYCODE_ZOOMIN = 2698;
const int32_t KeyEvent::KEYCODE_ZOOMOUT = 2699;
const int32_t KeyEvent::KEYCODE_ZOOMRESET = 2700;
const int32_t KeyEvent::KEYCODE_WORDPROCESSOR = 2701;
const int32_t KeyEvent::KEYCODE_EDITOR = 2702;
const int32_t KeyEvent::KEYCODE_SPREADSHEET = 2703;
const int32_t KeyEvent::KEYCODE_GRAPHICSEDITOR = 2704;
const int32_t KeyEvent::KEYCODE_PRESENTATION = 2705;
const int32_t KeyEvent::KEYCODE_DATABASE = 2706;
const int32_t KeyEvent::KEYCODE_NEWS = 2707;
const int32_t KeyEvent::KEYCODE_VOICEMAIL = 2708;
const int32_t KeyEvent::KEYCODE_ADDRESSBOOK = 2709;
const int32_t KeyEvent::KEYCODE_MESSENGER = 2710;
const int32_t KeyEvent::KEYCODE_BRIGHTNESS_TOGGLE = 2711;
const int32_t KeyEvent::KEYCODE_SPELLCHECK = 2712;
const int32_t KeyEvent::KEYCODE_COFFEE = 2713;
const int32_t KeyEvent::KEYCODE_MEDIA_REPEAT = 2714;
const int32_t KeyEvent::KEYCODE_IMAGES = 2715;
const int32_t KeyEvent::KEYCODE_BUTTONCONFIG = 2716;
const int32_t KeyEvent::KEYCODE_TASKMANAGER = 2717;
const int32_t KeyEvent::KEYCODE_JOURNAL = 2718;
const int32_t KeyEvent::KEYCODE_CONTROLPANEL = 2719;
const int32_t KeyEvent::KEYCODE_APPSELECT = 2720;
const int32_t KeyEvent::KEYCODE_SCREENSAVER = 2721;
const int32_t KeyEvent::KEYCODE_ASSISTANT = 2722;
const int32_t KeyEvent::KEYCODE_KBD_LAYOUT_NEXT = 2723;
const int32_t KeyEvent::KEYCODE_BRIGHTNESS_MIN = 2724;
const int32_t KeyEvent::KEYCODE_BRIGHTNESS_MAX = 2725;
const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_PREV = 2726;
const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_NEXT = 2727;
const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_PREVGROUP = 2728;
const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_NEXTGROUP = 2729;
const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_ACCEPT = 2730;
const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_CANCEL = 2731;
const int32_t KeyEvent::KEYCODE_FRONT = 2800;
const int32_t KeyEvent::KEYCODE_SETUP = 2801;
const int32_t KeyEvent::KEYCODE_WAKEUP = 2802;
const int32_t KeyEvent::KEYCODE_SENDFILE = 2803;
const int32_t KeyEvent::KEYCODE_DELETEFILE = 2804;
const int32_t KeyEvent::KEYCODE_XFER = 2805;
const int32_t KeyEvent::KEYCODE_PROG1 = 2806;
const int32_t KeyEvent::KEYCODE_PROG2 = 2807;
const int32_t KeyEvent::KEYCODE_MSDOS = 2808;
const int32_t KeyEvent::KEYCODE_SCREENLOCK = 2809;
const int32_t KeyEvent::KEYCODE_DIRECTION_ROTATE_DISPLAY = 2810;
const int32_t KeyEvent::KEYCODE_CYCLEWINDOWS = 2811;
const int32_t KeyEvent::KEYCODE_COMPUTER = 2812;
const int32_t KeyEvent::KEYCODE_EJECTCLOSECD = 2813;
const int32_t KeyEvent::KEYCODE_ISO = 2814;
const int32_t KeyEvent::KEYCODE_MOVE = 2815;
const int32_t KeyEvent::KEYCODE_F13 = 2816;
const int32_t KeyEvent::KEYCODE_F14 = 2817;
const int32_t KeyEvent::KEYCODE_F15 = 2818;
const int32_t KeyEvent::KEYCODE_F16 = 2819;
const int32_t KeyEvent::KEYCODE_F17 = 2820;
const int32_t KeyEvent::KEYCODE_F18 = 2821;
const int32_t KeyEvent::KEYCODE_F19 = 2822;
const int32_t KeyEvent::KEYCODE_F20 = 2823;
const int32_t KeyEvent::KEYCODE_F21 = 2824;
const int32_t KeyEvent::KEYCODE_F22 = 2825;
const int32_t KeyEvent::KEYCODE_F23 = 2826;
const int32_t KeyEvent::KEYCODE_F24 = 2827;
const int32_t KeyEvent::KEYCODE_PROG3 = 2828;
const int32_t KeyEvent::KEYCODE_PROG4 = 2829;
const int32_t KeyEvent::KEYCODE_DASHBOARD = 2830;
const int32_t KeyEvent::KEYCODE_SUSPEND = 2831;
const int32_t KeyEvent::KEYCODE_HP = 2832;
const int32_t KeyEvent::KEYCODE_SOUND = 2833;
const int32_t KeyEvent::KEYCODE_QUESTION = 2834;
const int32_t KeyEvent::KEYCODE_CONNECT = 2836;
const int32_t KeyEvent::KEYCODE_SPORT = 2837;
const int32_t KeyEvent::KEYCODE_SHOP = 2838;
const int32_t KeyEvent::KEYCODE_ALTERASE = 2839;
const int32_t KeyEvent::KEYCODE_SWITCHVIDEOMODE = 2841;
const int32_t KeyEvent::KEYCODE_BATTERY = 2842;
const int32_t KeyEvent::KEYCODE_BLUETOOTH = 2843;
const int32_t KeyEvent::KEYCODE_WLAN = 2844;
const int32_t KeyEvent::KEYCODE_UWB = 2845;
const int32_t KeyEvent::KEYCODE_WWAN_WIMAX = 2846;
const int32_t KeyEvent::KEYCODE_RFKILL = 2847;
const int32_t KeyEvent::KEYCODE_CHANNEL = 3001;
const int32_t KeyEvent::KEYCODE_BTN_0 = 3100;
const int32_t KeyEvent::KEYCODE_BTN_1 = 3101;
const int32_t KeyEvent::KEYCODE_BTN_2 = 3102;
const int32_t KeyEvent::KEYCODE_BTN_3 = 3103;
const int32_t KeyEvent::KEYCODE_BTN_4 = 3104;
const int32_t KeyEvent::KEYCODE_BTN_5 = 3105;
const int32_t KeyEvent::KEYCODE_BTN_6 = 3106;
const int32_t KeyEvent::KEYCODE_BTN_7 = 3107;
const int32_t KeyEvent::KEYCODE_BTN_8 = 3108;
const int32_t KeyEvent::KEYCODE_BTN_9 = 3109;
const int32_t KeyEvent::KEYCODE_BRL_DOT1 = 3201;
const int32_t KeyEvent::KEYCODE_BRL_DOT2 = 3202;
const int32_t KeyEvent::KEYCODE_BRL_DOT3 = 3203;
const int32_t KeyEvent::KEYCODE_BRL_DOT4 = 3204;
const int32_t KeyEvent::KEYCODE_BRL_DOT5 = 3205;
const int32_t KeyEvent::KEYCODE_BRL_DOT6 = 3206;
const int32_t KeyEvent::KEYCODE_BRL_DOT7 = 3207;
const int32_t KeyEvent::KEYCODE_BRL_DOT8 = 3208;
const int32_t KeyEvent::KEYCODE_BRL_DOT9 = 3209;
const int32_t KeyEvent::KEYCODE_BRL_DOT10 = 3210;
const int32_t KeyEvent::KEYCODE_LEFT_KNOB_ROLL_UP = 10001;
const int32_t KeyEvent::KEYCODE_LEFT_KNOB_ROLL_DOWN = 10002;
const int32_t KeyEvent::KEYCODE_LEFT_KNOB = 10003;
const int32_t KeyEvent::KEYCODE_RIGHT_KNOB_ROLL_UP = 10004;
const int32_t KeyEvent::KEYCODE_RIGHT_KNOB_ROLL_DOWN = 10005;
const int32_t KeyEvent::KEYCODE_RIGHT_KNOB = 10006;
const int32_t KeyEvent::KEYCODE_VOICE_SOURCE_SWITCH = 10007;
const int32_t KeyEvent::KEYCODE_LAUNCHER_MENU = 10008;

const int32_t KeyEvent::KEY_ACTION_UNKNOWN = 0X00000000;
const int32_t KeyEvent::KEY_ACTION_CANCEL = 0X00000001;

const int32_t KeyEvent::KEY_ACTION_DOWN = 0x00000002;
const int32_t KeyEvent::KEY_ACTION_UP = 0X00000003;

const int32_t KeyEvent::INTENTION_UNKNOWN = -1;
const int32_t KeyEvent::INTENTION_UP = 1;
const int32_t KeyEvent::INTENTION_DOWN = 2;
const int32_t KeyEvent::INTENTION_LEFT = 3;
const int32_t KeyEvent::INTENTION_RIGHT = 4;
const int32_t KeyEvent::INTENTION_SELECT = 5;
const int32_t KeyEvent::INTENTION_ESCAPE = 6;
const int32_t KeyEvent::INTENTION_BACK = 7;
const int32_t KeyEvent::INTENTION_FORWARD = 8;
const int32_t KeyEvent::INTENTION_MENU = 9;
const int32_t KeyEvent::INTENTION_HOME = 10;
const int32_t KeyEvent::INTENTION_PAGE_UP = 11;
const int32_t KeyEvent::INTENTION_PAGE_DOWN = 12;
const int32_t KeyEvent::INTENTION_ZOOM_OUT = 13;
const int32_t KeyEvent::INTENTION_ZOOM_IN = 14;

const int32_t KeyEvent::INTENTION_MEDIA_PLAY_PAUSE = 100;
const int32_t KeyEvent::INTENTION_MEDIA_FAST_FORWARD = 101;
const int32_t KeyEvent::INTENTION_MEDIA_FAST_REWIND = 102;
const int32_t KeyEvent::INTENTION_MEDIA_FAST_PLAYBACK = 103;
const int32_t KeyEvent::INTENTION_MEDIA_NEXT = 104;
const int32_t KeyEvent::INTENTION_MEDIA_PREVIOUS = 105;
const int32_t KeyEvent::INTENTION_MEDIA_MUTE = 106;
const int32_t KeyEvent::INTENTION_VOLUTE_UP = 107;
const int32_t KeyEvent::INTENTION_VOLUTE_DOWN = 108;

const int32_t KeyEvent::INTENTION_CALL = 200;
const int32_t KeyEvent::INTENTION_ENDCALL = 201;
const int32_t KeyEvent::INTENTION_REJECTCALL = 202;

const int32_t KeyEvent::INTENTION_CAMERA = 300;

KeyEvent::KeyItem::KeyItem() {}

KeyEvent::KeyItem::~KeyItem() {}

int32_t KeyEvent::KeyItem::GetKeyCode() const
{
    return keyCode_;
}

void KeyEvent::KeyItem::SetKeyCode(int32_t keyCode)
{
    keyCode_ = keyCode;
}

int64_t KeyEvent::KeyItem::GetDownTime() const
{
    return downTime_;
}

void KeyEvent::KeyItem::SetDownTime(int64_t downTime)
{
    downTime_ = downTime;
}

int32_t KeyEvent::KeyItem::GetDeviceId() const
{
    return deviceId_;
}

void KeyEvent::KeyItem::SetDeviceId(int32_t deviceId)
{
    deviceId_ = deviceId;
}

bool KeyEvent::KeyItem::IsPressed() const
{
    return pressed_;
}

void KeyEvent::KeyItem::SetPressed(bool pressed)
{
    pressed_ = pressed;
}

void KeyEvent::KeyItem::SetUnicode(uint32_t unicode)
{
    unicode_ = unicode;
}

uint32_t KeyEvent::KeyItem::GetUnicode() const
{
    return unicode_;
}

bool KeyEvent::KeyItem::WriteToParcel(Parcel &out) const
{
    WRITEBOOL(out, pressed_);
    WRITEINT64(out, downTime_);
    WRITEINT32(out, deviceId_);
    WRITEINT32(out, keyCode_);

    return true;
}

bool KeyEvent::KeyItem::ReadFromParcel(Parcel &in)
{
    READBOOL(in, pressed_);
    READINT64(in, downTime_);
    READINT32(in, deviceId_);
    READINT32(in, keyCode_);

    return true;
}

std::shared_ptr<KeyEvent> KeyEvent::from(std::shared_ptr<InputEvent> inputEvent)
{
    return nullptr;
}

KeyEvent::KeyEvent(int32_t eventType) : InputEvent(eventType) {}

KeyEvent::KeyEvent(const KeyEvent& other)
    : InputEvent(other),
      keyCode_(other.keyCode_),
      keys_(other.keys_),
      keyAction_(other.keyAction_),
      keyIntention_(other.keyIntention_) {}

KeyEvent::~KeyEvent() {}

std::shared_ptr<KeyEvent> KeyEvent::Create()
{
    auto event = std::shared_ptr<KeyEvent>(new (std::nothrow) KeyEvent(InputEvent::EVENT_TYPE_KEY));
    CHKPP(event);
    return event;
}

int32_t KeyEvent::GetKeyCode() const
{
    return keyCode_;
}

void KeyEvent::SetKeyCode(int32_t keyCode)
{
    keyCode_ = keyCode;
}

int32_t KeyEvent::GetKeyAction() const
{
    return keyAction_;
}

void KeyEvent::SetKeyAction(int32_t keyAction)
{
    keyAction_ = keyAction;
}

void KeyEvent::AddKeyItem(const KeyItem& keyItem)
{
    keys_.push_back(keyItem);
}

std::vector<KeyEvent::KeyItem> KeyEvent::GetKeyItems() const
{
    return keys_;
}

std::vector<int32_t> KeyEvent::GetPressedKeys() const
{
    std::vector<int32_t> result;
    for (const auto &item : keys_) {
        if (item.IsPressed()) {
            result.push_back(item.GetKeyCode());
        }
    }
    return result;
}

void KeyEvent::AddPressedKeyItems(const KeyItem& keyItem)
{
    std::vector<int32_t> pressedkeys = GetPressedKeys();
    std::vector<int32_t>::iterator result = std::find(pressedkeys.begin(),
        pressedkeys.end(), keyItem.GetKeyCode());
    if (result == pressedkeys.end()) {
        keys_.push_back(keyItem);
    }
}

void KeyEvent::RemoveReleasedKeyItems(const KeyItem& keyItem)
{
    if (keyItem.IsPressed()) {
        return;
    }
    int32_t keyCode = keyItem.GetKeyCode();
    for (auto it = keys_.begin(); it != keys_.end(); ++it) {
        if (it->GetKeyCode() == keyCode) {
            keys_.erase(it);
            return;
        }
    }
}

std::optional<KeyEvent::KeyItem> KeyEvent::GetKeyItem() const
{
    return GetKeyItem(keyCode_);
}

std::optional<KeyEvent::KeyItem> KeyEvent::GetKeyItem(int32_t keyCode) const
{
    for (const auto &item : keys_) {
        if (item.GetKeyCode() == keyCode) {
            return std::make_optional(item);
        }
    }
    return std::nullopt;
}

const char* KeyEvent::ActionToString(int32_t action)
{
    switch (action) {
        case KEY_ACTION_UNKNOWN: {
            return "KEY_ACTION_UNKNOWN";
        }
        case KEY_ACTION_CANCEL: {
            return "KEY_ACTION_CANCEL";
        }
        case KEY_ACTION_DOWN: {
            return "KEY_ACTION_DOWN";
        }
        case KEY_ACTION_UP: {
            return "KEY_ACTION_UP";
        }
        default: {
            return "KEY_ACTION_INVALID";
        }
    }
}

const char* KeyEvent::KeyCodeToString(int32_t keyCode)
{
    CALL_DEBUG_ENTER;
    auto iter = KEYCODE_TO_STRING.find(keyCode);
    if (iter == KEYCODE_TO_STRING.end()) {
        MMI_HILOGD("Failed to find the keycode");
        return "KEYCODE_INVALID";
    }
    return iter->second.c_str();
}

std::shared_ptr<KeyEvent> KeyEvent::Clone(std::shared_ptr<KeyEvent> keyEvent)
{
    if (!keyEvent) {
        return nullptr;
    }
    auto event = std::shared_ptr<KeyEvent>(new (std::nothrow) KeyEvent(*keyEvent.get()));
    CHKPP(event);
    return event;
}

bool KeyEvent::IsValidKeyItem() const
{
    CALL_DEBUG_ENTER;
    int32_t sameKeyCodeNum = 0;
    int32_t keyCode = GetKeyCode();
    int32_t action = GetKeyAction();

    for (auto it = keys_.begin(); it != keys_.end(); ++it) {
        if (it->GetKeyCode() == keyCode) {
            if (++sameKeyCodeNum > 1) {
                MMI_HILOGE("Keyitems keyCode is not unique with keyEvent keyCode");
                return false;
            }
        }
        if (it->GetKeyCode() <= KEYCODE_UNKNOWN) {
            MMI_HILOGE("The keyCode is invalid");
            return false;
        }
        if (it->GetDownTime() <= 0) {
            MMI_HILOGE("The downtime is invalid");
            return false;
        }
        if (action != KEY_ACTION_UP && it->IsPressed() == false) {
            MMI_HILOGE("The isPressed is invalid");
            return false;
        }
        if (action == KEY_ACTION_UP && it->IsPressed() == false) {
            if (it->GetKeyCode() != keyCode) {
                MMI_HILOGE("The keyCode is invalid when isPressed is false");
                return false;
            }
        }

        auto item = it;
        for (++item; item != keys_.end(); item++) {
            if (it->GetKeyCode() == item->GetKeyCode()) {
                MMI_HILOGE("Keyitems keyCode exist same items");
                return false;
            }
        }
    }

    if (sameKeyCodeNum == 0) {
        MMI_HILOGE("Keyitems keyCode is not exist equal item with keyEvent keyCode");
        return false;
    }
    return true;
}

bool KeyEvent::IsValid() const
{
    CALL_DEBUG_ENTER;
    int32_t keyCode = GetKeyCode();
    if (keyCode <= KEYCODE_UNKNOWN) {
        MMI_HILOGE("KeyCode_ is invalid");
        return false;
    }

    if (GetActionTime() <= 0) {
        MMI_HILOGE("Actiontime is invalid");
        return false;
    }

    int32_t action = GetKeyAction();
    if (action != KEY_ACTION_CANCEL && action != KEY_ACTION_UP &&
        action != KEY_ACTION_DOWN) {
        MMI_HILOGE("Action is invalid");
        return false;
    }

    if (!IsValidKeyItem()) {
        MMI_HILOGE("IsValidKeyItem is invalid");
        return false;
    }
    return true;
}


bool KeyEvent::WriteToParcel(Parcel &out) const
{
    if (!InputEvent::WriteToParcel(out)) {
        return false;
    }
    WRITEINT32(out, keyCode_);
    if (keys_.size() > INT_MAX) {
        return false;
    }
    WRITEINT32(out, static_cast<int32_t>(keys_.size()));
    for (const auto &item : keys_) {
        if (!item.WriteToParcel(out)) {
            return false;
        }
    }
    WRITEINT32(out, keyAction_);
    WRITEINT32(out, keyIntention_);
    return true;
}

bool KeyEvent::ReadFromParcel(Parcel &in)
{
    if (!InputEvent::ReadFromParcel(in)) {
        return false;
    }
    READINT32(in, keyCode_);
    const int32_t keysSize = in.ReadInt32();
    if (keysSize < 0) {
        return false;
    }
    for (int32_t i = 0; i < keysSize; i++) {
        KeyItem val = {};
        if (!val.ReadFromParcel(in)) {
            return false;
        }
        keys_.push_back(val);
    }
    READINT32(in, keyAction_);
    READINT32(in, keyIntention_);
    return true;
}

int32_t KeyEvent::TransitionFunctionKey(int32_t keyCode)
{
    switch (keyCode) {
        case KEYCODE_NUM_LOCK: {
            return NUM_LOCK_FUNCTION_KEY;
        }
        case KEYCODE_CAPS_LOCK: {
            return CAPS_LOCK_FUNCTION_KEY;
        }
        case KEYCODE_SCROLL_LOCK: {
            return SCROLL_LOCK_FUNCTION_KEY;
        }
        default: {
            MMI_HILOGW("Unknown key code");
            return UNKNOWN_FUNCTION_KEY;
        }
    }
}

bool KeyEvent::GetFunctionKey(int32_t funcKey) const
{
    switch (funcKey) {
        case NUM_LOCK_FUNCTION_KEY: {
            return numLock_;
        }
        case CAPS_LOCK_FUNCTION_KEY: {
            return capsLock_;
        }
        case SCROLL_LOCK_FUNCTION_KEY: {
            return scrollLock_;
        }
        default: {
            MMI_HILOGW("Unknown function key");
            return false;
        }
    }
}

int32_t KeyEvent::SetFunctionKey(int32_t funcKey, int32_t value)
{
    bool state = static_cast<bool>(value);
    switch (funcKey) {
        case NUM_LOCK_FUNCTION_KEY: {
            numLock_ = state;
            return funcKey;
        }
        case CAPS_LOCK_FUNCTION_KEY: {
            capsLock_ = state;
            return funcKey;
        }
        case SCROLL_LOCK_FUNCTION_KEY: {
            scrollLock_ = state;
            return funcKey;
        }
        default: {
            MMI_HILOGW("Unknown function key");
            return UNKNOWN_FUNCTION_KEY;
        }
    }
}

int32_t KeyEvent::GetKeyIntention() const
{
    return keyIntention_;
}

void KeyEvent::SetKeyIntention(int32_t keyIntention)
{
    keyIntention_ = keyIntention;
}
} // namespace MMI
} // namespace OHOS
