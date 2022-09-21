/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "enum_utils.h"

#include <string>

#include <linux/input.h>
#include <linux/input-event-codes.h>

#include "i_input_define.h"

namespace OHOS {
namespace MMI {

    static const char* IntValueToCStr(int32_t eventCode) {
        static std::string codeStr;
        codeStr = std::to_string(eventCode);
        return codeStr.c_str();
    }

    const char* EnumUtils::InputEventTypeToString(int32_t eventType) {
        switch(eventType) {
            CASE_STR(EV_SYN);
            CASE_STR(EV_KEY);
            CASE_STR(EV_REL);
            CASE_STR(EV_ABS);
            CASE_STR(EV_MSC);
            CASE_STR(EV_SW);
            CASE_STR(EV_LED);
            CASE_STR(EV_SND);
            CASE_STR(EV_REP);
            CASE_STR(EV_FF);
            CASE_STR(EV_PWR);
            CASE_STR(EV_FF_STATUS);
            default:
                return "EV_UNKNOWN";
        }
    }

    const char* EnumUtils::InputEventCodeToString(int32_t eventType, int32_t eventCode) {
        switch(eventType) {
            case EV_SYN:
                return InputEventSynCodeToString(eventCode);
            case EV_KEY:
                return InputEventKeyCodeToString(eventCode);
            case EV_REL:
                return InputEventRelCodeToString(eventCode);
            case EV_ABS:
                return InputEventAbsCodeToString(eventCode);
            case EV_MSC:
                return InputEventMscCodeToString(eventCode);
            case EV_SW:
                return InputEventSwCodeToString(eventCode);
            case EV_LED:
                return InputEventLedCodeToString(eventCode);
            case EV_SND:
                return InputEventSndCodeToString(eventCode);
            case EV_REP:
                return InputEventRepCodeToString(eventCode);
            case EV_FF:
                return InputEventFfCodeToString(eventCode);
            case EV_PWR:
                return InputEventPwrCodeToString(eventCode);
            case EV_FF_STATUS:
                return InputEventFfstatusCodeToString(eventCode);
            default:
                return IntValueToCStr(eventCode);
        }
    }

    const char* EnumUtils::InputEventSynCodeToString(int32_t eventCode) {
        switch(eventCode) {
            CASE_STR(SYN_REPORT);
            CASE_STR(SYN_CONFIG);
            CASE_STR(SYN_MT_REPORT);
            CASE_STR(SYN_DROPPED);
            default:
            return "SYNC_UNKNOWN";
        }
    }

    const char* EnumUtils::InputEventKeyCodeToString(int32_t eventCode) {
        switch(eventCode) {
            CASE_STR(KEY_RESERVED);
            CASE_STR(KEY_ESC);
            CASE_STR(KEY_1);
            CASE_STR(KEY_2);
            CASE_STR(KEY_3);
            CASE_STR(KEY_4);
            CASE_STR(KEY_5);
            CASE_STR(KEY_6);
            CASE_STR(KEY_7);
            CASE_STR(KEY_8);
            CASE_STR(KEY_9);
            CASE_STR(KEY_0);
            CASE_STR(KEY_MINUS);
            CASE_STR(KEY_EQUAL);
            CASE_STR(KEY_BACKSPACE);
            CASE_STR(KEY_TAB);
            CASE_STR(KEY_Q);
            CASE_STR(KEY_W);
            CASE_STR(KEY_E);
            CASE_STR(KEY_R);
            CASE_STR(KEY_T);
            CASE_STR(KEY_Y);
            CASE_STR(KEY_U);
            CASE_STR(KEY_I);
            CASE_STR(KEY_O);
            CASE_STR(KEY_P);
            CASE_STR(KEY_LEFTBRACE);
            CASE_STR(KEY_RIGHTBRACE);
            CASE_STR(KEY_ENTER);
            CASE_STR(KEY_LEFTCTRL);
            CASE_STR(KEY_A);
            CASE_STR(KEY_S);
            CASE_STR(KEY_D);
            CASE_STR(KEY_F);
            CASE_STR(KEY_G);
            CASE_STR(KEY_H);
            CASE_STR(KEY_J);
            CASE_STR(KEY_K);
            CASE_STR(KEY_L);
            CASE_STR(KEY_SEMICOLON);
            CASE_STR(KEY_APOSTROPHE);
            CASE_STR(KEY_GRAVE);
            CASE_STR(KEY_LEFTSHIFT);
            CASE_STR(KEY_BACKSLASH);
            CASE_STR(KEY_Z);
            CASE_STR(KEY_X);
            CASE_STR(KEY_C);
            CASE_STR(KEY_V);
            CASE_STR(KEY_B);
            CASE_STR(KEY_N);
            CASE_STR(KEY_M);
            CASE_STR(KEY_COMMA);
            CASE_STR(KEY_DOT);
            CASE_STR(KEY_SLASH);
            CASE_STR(KEY_RIGHTSHIFT);
            CASE_STR(KEY_KPASTERISK);
            CASE_STR(KEY_LEFTALT);
            CASE_STR(KEY_SPACE);
            CASE_STR(KEY_CAPSLOCK);
            CASE_STR(KEY_F1);
            CASE_STR(KEY_F2);
            CASE_STR(KEY_F3);
            CASE_STR(KEY_F4);
            CASE_STR(KEY_F5);
            CASE_STR(KEY_F6);
            CASE_STR(KEY_F7);
            CASE_STR(KEY_F8);
            CASE_STR(KEY_F9);
            CASE_STR(KEY_F10);
            CASE_STR(KEY_NUMLOCK);
            CASE_STR(KEY_SCROLLLOCK);
            CASE_STR(KEY_KP7);
            CASE_STR(KEY_KP8);
            CASE_STR(KEY_KP9);
            CASE_STR(KEY_KPMINUS);
            CASE_STR(KEY_KP4);
            CASE_STR(KEY_KP5);
            CASE_STR(KEY_KP6);
            CASE_STR(KEY_KPPLUS);
            CASE_STR(KEY_KP1);
            CASE_STR(KEY_KP2);
            CASE_STR(KEY_KP3);
            CASE_STR(KEY_KP0);
            CASE_STR(KEY_KPDOT);
            CASE_STR(KEY_102ND);
            CASE_STR(KEY_F11);
            CASE_STR(KEY_F12);
            CASE_STR(KEY_RO);
            CASE_STR(KEY_KATAKANA);
            CASE_STR(KEY_HIRAGANA);
            CASE_STR(KEY_HENKAN);
            CASE_STR(KEY_KATAKANAHIRAGANA);
            CASE_STR(KEY_MUHENKAN);
            CASE_STR(KEY_KPJPCOMMA);
            CASE_STR(KEY_KPENTER);
            CASE_STR(KEY_RIGHTCTRL);
            CASE_STR(KEY_KPSLASH);
            CASE_STR(KEY_SYSRQ);
            CASE_STR(KEY_RIGHTALT);
            CASE_STR(KEY_LINEFEED);
            CASE_STR(KEY_HOME);
            CASE_STR(KEY_UP);
            CASE_STR(KEY_PAGEUP);
            CASE_STR(KEY_LEFT);
            CASE_STR(KEY_RIGHT);
            CASE_STR(KEY_END);
            CASE_STR(KEY_DOWN);
            CASE_STR(KEY_PAGEDOWN);
            CASE_STR(KEY_INSERT);
            CASE_STR(KEY_DELETE);
            CASE_STR(KEY_MACRO);
            //CASE_STR(KEY_MUTE);
            CASE_STR(KEY_VOLUMEDOWN);
            CASE_STR(KEY_VOLUMEUP);
            CASE_STR(KEY_POWER);
            CASE_STR(KEY_KPEQUAL);
            CASE_STR(KEY_KPPLUSMINUS);
            CASE_STR(KEY_PAUSE);
            CASE_STR(KEY_SCALE);

            CASE_STR(KEY_KPCOMMA);
            //CASE_STR(KEY_HANGEUL);
            //CASE_STR(KEY_HANGUEL);
            CASE_STR(KEY_HANJA);
            CASE_STR(KEY_YEN);
            CASE_STR(KEY_LEFTMETA);
            CASE_STR(KEY_RIGHTMETA);
            CASE_STR(KEY_COMPOSE);
            CASE_STR(KEY_AGAIN);
            CASE_STR(KEY_PROPS);
            CASE_STR(KEY_UNDO);
            CASE_STR(KEY_FRONT);
            CASE_STR(KEY_COPY);
            CASE_STR(KEY_OPEN);
            CASE_STR(KEY_PASTE);
            CASE_STR(KEY_FIND);
            CASE_STR(KEY_CUT);
            CASE_STR(KEY_HELP);
            CASE_STR(KEY_MENU);
            CASE_STR(KEY_CALC);
            CASE_STR(KEY_SETUP);
            CASE_STR(KEY_SLEEP);
            CASE_STR(KEY_WAKEUP);
            CASE_STR(KEY_FILE);
            CASE_STR(KEY_SENDFILE);
            CASE_STR(KEY_DELETEFILE);
            CASE_STR(KEY_XFER);
            CASE_STR(KEY_PROG1);
            CASE_STR(KEY_PROG2);
            CASE_STR(KEY_WWW);
            CASE_STR(KEY_MSDOS);
            CASE_STR(KEY_COFFEE);
            //CASE_STR(KEY_SCREENLOCK);
            //CASE_STR(KEY_ROTATE_DISPLAY);
            //CASE_STR(KEY_DIRECTION);
            //CASE_STR(KEY_CYCLEWINDOWS);
            CASE_STR(KEY_MAIL);
            CASE_STR(KEY_BOOKMARKS);
            CASE_STR(KEY_COMPUTER);
            CASE_STR(KEY_BACK);
            CASE_STR(KEY_FORWARD);
            CASE_STR(KEY_CLOSECD);
            CASE_STR(KEY_EJECTCD);
            CASE_STR(KEY_EJECTCLOSECD);
            CASE_STR(KEY_NEXTSONG);
            CASE_STR(KEY_PLAYPAUSE);
            CASE_STR(KEY_PREVIOUSSONG);
            CASE_STR(KEY_STOPCD);
            CASE_STR(KEY_RECORD);
            CASE_STR(KEY_REWIND);
            CASE_STR(KEY_PHONE);
            CASE_STR(KEY_ISO);
            CASE_STR(KEY_CONFIG);
            CASE_STR(KEY_HOMEPAGE);
            CASE_STR(KEY_REFRESH);
            CASE_STR(KEY_EXIT);
            CASE_STR(KEY_MOVE);
            CASE_STR(KEY_EDIT);
            CASE_STR(KEY_SCROLLUP);
            CASE_STR(KEY_SCROLLDOWN);
            CASE_STR(KEY_KPLEFTPAREN);
            CASE_STR(KEY_KPRIGHTPAREN);
            CASE_STR(KEY_NEW);
            CASE_STR(KEY_REDO);

            CASE_STR(KEY_F13);
            CASE_STR(KEY_F14);
            CASE_STR(KEY_F15);
            CASE_STR(KEY_F16);
            CASE_STR(KEY_F17);
            CASE_STR(KEY_F18);
            CASE_STR(KEY_F19);
            CASE_STR(KEY_F20);
            CASE_STR(KEY_F21);
            CASE_STR(KEY_F22);
            CASE_STR(KEY_F23);
            CASE_STR(KEY_F24);

            CASE_STR(KEY_PLAYCD);
            CASE_STR(KEY_PAUSECD);
            CASE_STR(KEY_PROG3);
            CASE_STR(KEY_PROG4);
            CASE_STR(KEY_DASHBOARD);
            CASE_STR(KEY_SUSPEND);
            CASE_STR(KEY_CLOSE);
            CASE_STR(KEY_PLAY);
            CASE_STR(KEY_FASTFORWARD);
            CASE_STR(KEY_BASSBOOST);
            CASE_STR(KEY_PRINT);
            CASE_STR(KEY_HP);
            CASE_STR(KEY_CAMERA);
            CASE_STR(KEY_SOUND);
            CASE_STR(KEY_QUESTION);
            CASE_STR(KEY_EMAIL);
            CASE_STR(KEY_CHAT);
            CASE_STR(KEY_SEARCH);
            CASE_STR(KEY_CONNECT);
            CASE_STR(KEY_FINANCE);
            CASE_STR(KEY_SPORT);
            CASE_STR(KEY_SHOP);
            CASE_STR(KEY_ALTERASE);
            CASE_STR(KEY_CANCEL);
            CASE_STR(KEY_BRIGHTNESSDOWN);
            CASE_STR(KEY_BRIGHTNESSUP);
            CASE_STR(KEY_MEDIA);

            CASE_STR(KEY_SWITCHVIDEOMODE);
            CASE_STR(KEY_KBDILLUMTOGGLE);
            CASE_STR(KEY_KBDILLUMDOWN);
            CASE_STR(KEY_KBDILLUMUP);

            CASE_STR(KEY_SEND);
            CASE_STR(KEY_REPLY);
            CASE_STR(KEY_FORWARDMAIL);
            CASE_STR(KEY_SAVE);
            CASE_STR(KEY_DOCUMENTS);

            CASE_STR(KEY_BATTERY);

            CASE_STR(KEY_BLUETOOTH);
            CASE_STR(KEY_WLAN);
            CASE_STR(KEY_UWB);

            CASE_STR(KEY_UNKNOWN);

            CASE_STR(KEY_VIDEO_NEXT);
            CASE_STR(KEY_VIDEO_PREV);
            CASE_STR(KEY_BRIGHTNESS_CYCLE);
            //CASE_STR(KEY_BRIGHTNESS_AUTO);

            //CASE_STR(KEY_BRIGHTNESS_ZERO);
            CASE_STR(KEY_DISPLAY_OFF);

            //CASE_STR(KEY_WWAN);
            //CASE_STR(KEY_WIMAX);
            CASE_STR(KEY_RFKILL);

            CASE_STR(KEY_MICMUTE);

            //CASE_STR(BTN_MISC);
            CASE_STR(BTN_0);
            CASE_STR(BTN_1);
            CASE_STR(BTN_2);
            CASE_STR(BTN_3);
            CASE_STR(BTN_4);
            CASE_STR(BTN_5);
            CASE_STR(BTN_6);
            CASE_STR(BTN_7);
            CASE_STR(BTN_8);
            CASE_STR(BTN_9);

            //CASE_STR(BTN_MOUSE);
            CASE_STR(BTN_LEFT);
            CASE_STR(BTN_RIGHT);
            CASE_STR(BTN_MIDDLE);
            CASE_STR(BTN_SIDE);
            CASE_STR(BTN_EXTRA);
            CASE_STR(BTN_FORWARD);
            CASE_STR(BTN_BACK);
            CASE_STR(BTN_TASK);

            //CASE_STR(BTN_JOYSTICK);
            CASE_STR(BTN_TRIGGER);
            CASE_STR(BTN_THUMB);
            CASE_STR(BTN_THUMB2);
            CASE_STR(BTN_TOP);
            CASE_STR(BTN_TOP2);
            CASE_STR(BTN_PINKIE);
            CASE_STR(BTN_BASE);
            CASE_STR(BTN_BASE2);
            CASE_STR(BTN_BASE3);
            CASE_STR(BTN_BASE4);
            CASE_STR(BTN_BASE5);
            CASE_STR(BTN_BASE6);
            CASE_STR(BTN_DEAD);

            CASE_STR(BTN_GAMEPAD);
            //CASE_STR(BTN_SOUTH);
            //CASE_STR(BTN_A);
            CASE_STR(BTN_EAST);
            //CASE_STR(BTN_B);
            CASE_STR(BTN_C);
            //CASE_STR(BTN_NORTH);
            //CASE_STR(BTN_X);
            //CASE_STR(BTN_WEST);
            //CASE_STR(BTN_Y);
            CASE_STR(BTN_Z);
            CASE_STR(BTN_TL);
            CASE_STR(BTN_TR);
            CASE_STR(BTN_TL2);
            CASE_STR(BTN_TR2);
            CASE_STR(BTN_SELECT);
            CASE_STR(BTN_START);
            CASE_STR(BTN_MODE);
            CASE_STR(BTN_THUMBL);
            CASE_STR(BTN_THUMBR);

            CASE_STR(BTN_DIGI);
            //CASE_STR(BTN_TOOL_PEN);
            CASE_STR(BTN_TOOL_RUBBER);
            CASE_STR(BTN_TOOL_BRUSH);
            CASE_STR(BTN_TOOL_PENCIL);
            CASE_STR(BTN_TOOL_AIRBRUSH);
            CASE_STR(BTN_TOOL_FINGER);
            CASE_STR(BTN_TOOL_MOUSE);
            CASE_STR(BTN_TOOL_LENS);
            CASE_STR(BTN_TOOL_QUINTTAP);
            CASE_STR(BTN_STYLUS3);
            CASE_STR(BTN_TOUCH);
            CASE_STR(BTN_STYLUS);
            CASE_STR(BTN_STYLUS2);
            CASE_STR(BTN_TOOL_DOUBLETAP);
            CASE_STR(BTN_TOOL_TRIPLETAP);
            CASE_STR(BTN_TOOL_QUADTAP);

            //CASE_STR(BTN_WHEEL);
            CASE_STR(BTN_GEAR_DOWN);
            CASE_STR(BTN_GEAR_UP);

            CASE_STR(KEY_OK);
            CASE_STR(KEY_SELECT);
            CASE_STR(KEY_GOTO);
            CASE_STR(KEY_CLEAR);
            CASE_STR(KEY_POWER2);
            CASE_STR(KEY_OPTION);
            CASE_STR(KEY_INFO);
            CASE_STR(KEY_TIME);
            CASE_STR(KEY_VENDOR);
            CASE_STR(KEY_ARCHIVE);
            CASE_STR(KEY_PROGRAM);
            CASE_STR(KEY_CHANNEL);
            CASE_STR(KEY_FAVORITES);
            CASE_STR(KEY_EPG);
            CASE_STR(KEY_PVR);
            CASE_STR(KEY_MHP);
            CASE_STR(KEY_LANGUAGE);
            CASE_STR(KEY_TITLE);
            CASE_STR(KEY_SUBTITLE);
            CASE_STR(KEY_ANGLE);
            CASE_STR(KEY_FULL_SCREEN);
            //CASE_STR(KEY_ZOOM);
            CASE_STR(KEY_MODE);
            CASE_STR(KEY_KEYBOARD);
            CASE_STR(KEY_ASPECT_RATIO);
            //CASE_STR(KEY_SCREEN);
            CASE_STR(KEY_PC);
            CASE_STR(KEY_TV);
            CASE_STR(KEY_TV2);
            CASE_STR(KEY_VCR);
            CASE_STR(KEY_VCR2);
            CASE_STR(KEY_SAT);
            CASE_STR(KEY_SAT2);
            CASE_STR(KEY_CD);
            CASE_STR(KEY_TAPE);
            CASE_STR(KEY_RADIO);
            CASE_STR(KEY_TUNER);
            CASE_STR(KEY_PLAYER);
            CASE_STR(KEY_TEXT);
            CASE_STR(KEY_DVD);
            CASE_STR(KEY_AUX);
            CASE_STR(KEY_MP3);
            CASE_STR(KEY_AUDIO);
            CASE_STR(KEY_VIDEO);
            CASE_STR(KEY_DIRECTORY);
            CASE_STR(KEY_LIST);
            CASE_STR(KEY_MEMO);
            CASE_STR(KEY_CALENDAR);
            CASE_STR(KEY_RED);
            CASE_STR(KEY_GREEN);
            CASE_STR(KEY_YELLOW);
            CASE_STR(KEY_BLUE);
            CASE_STR(KEY_CHANNELUP);
            CASE_STR(KEY_CHANNELDOWN);
            CASE_STR(KEY_FIRST);
            CASE_STR(KEY_LAST);
            CASE_STR(KEY_AB);
            CASE_STR(KEY_NEXT);
            CASE_STR(KEY_RESTART);
            CASE_STR(KEY_SLOW);
            CASE_STR(KEY_SHUFFLE);
            CASE_STR(KEY_BREAK);
            CASE_STR(KEY_PREVIOUS);
            CASE_STR(KEY_DIGITS);
            CASE_STR(KEY_TEEN);
            CASE_STR(KEY_TWEN);
            CASE_STR(KEY_VIDEOPHONE);
            CASE_STR(KEY_GAMES);
            CASE_STR(KEY_ZOOMIN);
            CASE_STR(KEY_ZOOMOUT);
            CASE_STR(KEY_ZOOMRESET);
            CASE_STR(KEY_WORDPROCESSOR);
            CASE_STR(KEY_EDITOR);
            CASE_STR(KEY_SPREADSHEET);
            CASE_STR(KEY_GRAPHICSEDITOR);
            CASE_STR(KEY_PRESENTATION);
            CASE_STR(KEY_DATABASE);
            CASE_STR(KEY_NEWS);
            CASE_STR(KEY_VOICEMAIL);
            CASE_STR(KEY_ADDRESSBOOK);
            CASE_STR(KEY_MESSENGER);
            CASE_STR(KEY_DISPLAYTOGGLE);
            //CASE_STR(KEY_BRIGHTNESS_TOGGLE);
            CASE_STR(KEY_SPELLCHECK);
            CASE_STR(KEY_LOGOFF);

            CASE_STR(KEY_DOLLAR);
            CASE_STR(KEY_EURO);

            CASE_STR(KEY_FRAMEBACK);
            CASE_STR(KEY_FRAMEFORWARD);
            CASE_STR(KEY_CONTEXT_MENU);
            CASE_STR(KEY_MEDIA_REPEAT);
            CASE_STR(KEY_10CHANNELSUP);
            CASE_STR(KEY_10CHANNELSDOWN);
            CASE_STR(KEY_IMAGES);
            //CASE_STR(KEY_NOTIFICATION_CENTER);
            //CASE_STR(KEY_PICKUP_PHONE);
            //CASE_STR(KEY_HANGUP_PHONE);

            CASE_STR(KEY_DEL_EOL);
            CASE_STR(KEY_DEL_EOS);
            CASE_STR(KEY_INS_LINE);
            CASE_STR(KEY_DEL_LINE);

            CASE_STR(KEY_FN);
            CASE_STR(KEY_FN_ESC);
            CASE_STR(KEY_FN_F1);
            CASE_STR(KEY_FN_F2);
            CASE_STR(KEY_FN_F3);
            CASE_STR(KEY_FN_F4);
            CASE_STR(KEY_FN_F5);
            CASE_STR(KEY_FN_F6);
            CASE_STR(KEY_FN_F7);
            CASE_STR(KEY_FN_F8);
            CASE_STR(KEY_FN_F9);
            CASE_STR(KEY_FN_F10);
            CASE_STR(KEY_FN_F11);
            CASE_STR(KEY_FN_F12);
            CASE_STR(KEY_FN_1);
            CASE_STR(KEY_FN_2);
            CASE_STR(KEY_FN_D);
            CASE_STR(KEY_FN_E);
            CASE_STR(KEY_FN_F);
            CASE_STR(KEY_FN_S);
            CASE_STR(KEY_FN_B);
            //CASE_STR(KEY_FN_RIGHT_SHIFT);

            CASE_STR(KEY_BRL_DOT1);
            CASE_STR(KEY_BRL_DOT2);
            CASE_STR(KEY_BRL_DOT3);
            CASE_STR(KEY_BRL_DOT4);
            CASE_STR(KEY_BRL_DOT5);
            CASE_STR(KEY_BRL_DOT6);
            CASE_STR(KEY_BRL_DOT7);
            CASE_STR(KEY_BRL_DOT8);
            CASE_STR(KEY_BRL_DOT9);
            CASE_STR(KEY_BRL_DOT10);

            CASE_STR(KEY_NUMERIC_0);
            CASE_STR(KEY_NUMERIC_1);
            CASE_STR(KEY_NUMERIC_2);
            CASE_STR(KEY_NUMERIC_3);
            CASE_STR(KEY_NUMERIC_4);
            CASE_STR(KEY_NUMERIC_5);
            CASE_STR(KEY_NUMERIC_6);
            CASE_STR(KEY_NUMERIC_7);
            CASE_STR(KEY_NUMERIC_8);
            CASE_STR(KEY_NUMERIC_9);
            CASE_STR(KEY_NUMERIC_STAR);
            CASE_STR(KEY_NUMERIC_POUND);
            CASE_STR(KEY_NUMERIC_A);
            CASE_STR(KEY_NUMERIC_B);
            CASE_STR(KEY_NUMERIC_C);
            CASE_STR(KEY_NUMERIC_D);

            CASE_STR(KEY_CAMERA_FOCUS);
            CASE_STR(KEY_WPS_BUTTON);

            CASE_STR(KEY_TOUCHPAD_TOGGLE);
            CASE_STR(KEY_TOUCHPAD_ON);
            CASE_STR(KEY_TOUCHPAD_OFF);

            CASE_STR(KEY_CAMERA_ZOOMIN);
            CASE_STR(KEY_CAMERA_ZOOMOUT);
            CASE_STR(KEY_CAMERA_UP);
            CASE_STR(KEY_CAMERA_DOWN);
            CASE_STR(KEY_CAMERA_LEFT);
            CASE_STR(KEY_CAMERA_RIGHT);

            CASE_STR(KEY_ATTENDANT_ON);
            CASE_STR(KEY_ATTENDANT_OFF);
            CASE_STR(KEY_ATTENDANT_TOGGLE);
            CASE_STR(KEY_LIGHTS_TOGGLE);

            CASE_STR(BTN_DPAD_UP);
            CASE_STR(BTN_DPAD_DOWN);
            CASE_STR(BTN_DPAD_LEFT);
            CASE_STR(BTN_DPAD_RIGHT);

            CASE_STR(KEY_ALS_TOGGLE);
            CASE_STR(KEY_ROTATE_LOCK_TOGGLE);

            CASE_STR(KEY_BUTTONCONFIG);
            CASE_STR(KEY_TASKMANAGER);
            CASE_STR(KEY_JOURNAL);
            CASE_STR(KEY_CONTROLPANEL);
            CASE_STR(KEY_APPSELECT);
            CASE_STR(KEY_SCREENSAVER);
            CASE_STR(KEY_VOICECOMMAND);
            CASE_STR(KEY_ASSISTANT);
            CASE_STR(KEY_KBD_LAYOUT_NEXT);
            // CASE_STR(KEY_EMOJI_PICKER);

            CASE_STR(KEY_BRIGHTNESS_MIN);
            CASE_STR(KEY_BRIGHTNESS_MAX);

            CASE_STR(KEY_KBDINPUTASSIST_PREV);
            CASE_STR(KEY_KBDINPUTASSIST_NEXT);
            CASE_STR(KEY_KBDINPUTASSIST_PREVGROUP);
            CASE_STR(KEY_KBDINPUTASSIST_NEXTGROUP);
            CASE_STR(KEY_KBDINPUTASSIST_ACCEPT);
            CASE_STR(KEY_KBDINPUTASSIST_CANCEL);

            CASE_STR(KEY_RIGHT_UP);
            CASE_STR(KEY_RIGHT_DOWN);
            CASE_STR(KEY_LEFT_UP);
            CASE_STR(KEY_LEFT_DOWN);

            CASE_STR(KEY_ROOT_MENU);
            CASE_STR(KEY_MEDIA_TOP_MENU);
            CASE_STR(KEY_NUMERIC_11);
            CASE_STR(KEY_NUMERIC_12);
            CASE_STR(KEY_AUDIO_DESC);
            CASE_STR(KEY_3D_MODE);
            CASE_STR(KEY_NEXT_FAVORITE);
            CASE_STR(KEY_STOP_RECORD);
            CASE_STR(KEY_PAUSE_RECORD);
            CASE_STR(KEY_VOD);
            CASE_STR(KEY_UNMUTE);
            CASE_STR(KEY_FASTREVERSE);
            CASE_STR(KEY_SLOWREVERSE);
            CASE_STR(KEY_DATA);
            CASE_STR(KEY_ONSCREEN_KEYBOARD);
            //CASE_STR(KEY_PRIVACY_SCREEN_TOGGLE);
            //CASE_STR(KEY_SELECTIVE_SCREENSHOT);
            //CASE_STR(KEY_MACRO1);
            //CASE_STR(KEY_MACRO2);
            //CASE_STR(KEY_MACRO3);
            //CASE_STR(KEY_MACRO4);
            //CASE_STR(KEY_MACRO5);
            //CASE_STR(KEY_MACRO6);
            //CASE_STR(KEY_MACRO7);
            //CASE_STR(KEY_MACRO8);
            //CASE_STR(KEY_MACRO9);
            //CASE_STR(KEY_MACRO10);
            //CASE_STR(KEY_MACRO11);
            //CASE_STR(KEY_MACRO12);
            //CASE_STR(KEY_MACRO13);
            //CASE_STR(KEY_MACRO14);
            //CASE_STR(KEY_MACRO15);
            //CASE_STR(KEY_MACRO16);
            //CASE_STR(KEY_MACRO17);
            //CASE_STR(KEY_MACRO18);
            //CASE_STR(KEY_MACRO19);
            //CASE_STR(KEY_MACRO20);
            //CASE_STR(KEY_MACRO21);
            //CASE_STR(KEY_MACRO22);
            //CASE_STR(KEY_MACRO23);
            //CASE_STR(KEY_MACRO24);
            //CASE_STR(KEY_MACRO25);
            //CASE_STR(KEY_MACRO26);
            //CASE_STR(KEY_MACRO27);
            //CASE_STR(KEY_MACRO28);
            //CASE_STR(KEY_MACRO29);
            //CASE_STR(KEY_MACRO30);
            //CASE_STR(KEY_MACRO_RECORD_START);
            //CASE_STR(KEY_MACRO_RECORD_STOP);
            //CASE_STR(KEY_MACRO_PRESET_CYCLE);
            //CASE_STR(KEY_MACRO_PRESET1);
            //CASE_STR(KEY_MACRO_PRESET2);
            //CASE_STR(KEY_MACRO_PRESET3);
            //CASE_STR(KEY_KBD_LCD_MENU1);
            //CASE_STR(KEY_KBD_LCD_MENU2);
            //CASE_STR(KEY_KBD_LCD_MENU3);
            //CASE_STR(KEY_KBD_LCD_MENU4);
            //CASE_STR(KEY_KBD_LCD_MENU5);
            //CASE_STR(BTN_TRIGGER_HAPPY);
            //CASE_STR(BTN_TRIGGER_HAPPY1);
            CASE_STR(BTN_TRIGGER_HAPPY2);
            CASE_STR(BTN_TRIGGER_HAPPY3);
            CASE_STR(BTN_TRIGGER_HAPPY4);
            CASE_STR(BTN_TRIGGER_HAPPY5);
            CASE_STR(BTN_TRIGGER_HAPPY6);
            CASE_STR(BTN_TRIGGER_HAPPY7);
            CASE_STR(BTN_TRIGGER_HAPPY8);
            CASE_STR(BTN_TRIGGER_HAPPY9);
            CASE_STR(BTN_TRIGGER_HAPPY10);
            CASE_STR(BTN_TRIGGER_HAPPY11);
            CASE_STR(BTN_TRIGGER_HAPPY12);
            CASE_STR(BTN_TRIGGER_HAPPY13);
            CASE_STR(BTN_TRIGGER_HAPPY14);
            CASE_STR(BTN_TRIGGER_HAPPY15);
            CASE_STR(BTN_TRIGGER_HAPPY16);
            CASE_STR(BTN_TRIGGER_HAPPY17);
            CASE_STR(BTN_TRIGGER_HAPPY18);
            CASE_STR(BTN_TRIGGER_HAPPY19);
            CASE_STR(BTN_TRIGGER_HAPPY20);
            CASE_STR(BTN_TRIGGER_HAPPY21);
            CASE_STR(BTN_TRIGGER_HAPPY22);
            CASE_STR(BTN_TRIGGER_HAPPY23);
            CASE_STR(BTN_TRIGGER_HAPPY24);
            CASE_STR(BTN_TRIGGER_HAPPY25);
            CASE_STR(BTN_TRIGGER_HAPPY26);
            CASE_STR(BTN_TRIGGER_HAPPY27);
            CASE_STR(BTN_TRIGGER_HAPPY28);
            CASE_STR(BTN_TRIGGER_HAPPY29);
            CASE_STR(BTN_TRIGGER_HAPPY30);
            CASE_STR(BTN_TRIGGER_HAPPY31);
            CASE_STR(BTN_TRIGGER_HAPPY32);
            CASE_STR(BTN_TRIGGER_HAPPY33);
            CASE_STR(BTN_TRIGGER_HAPPY34);
            CASE_STR(BTN_TRIGGER_HAPPY35);
            CASE_STR(BTN_TRIGGER_HAPPY36);
            CASE_STR(BTN_TRIGGER_HAPPY37);
            CASE_STR(BTN_TRIGGER_HAPPY38);
            CASE_STR(BTN_TRIGGER_HAPPY39);
            CASE_STR(BTN_TRIGGER_HAPPY40);
            CASE_STR(KEY_MIN_INTERESTING);
            CASE_STR(KEY_MAX);
            CASE_STR(KEY_CNT);
            default:
            return IntValueToCStr(eventCode);
        }
    }

    const char* EnumUtils::InputEventRelCodeToString(int32_t eventCode) {
        switch(eventCode) {
            CASE_STR(REL_X);
            CASE_STR(REL_Y);
            CASE_STR(REL_Z);
            CASE_STR(REL_RX);
            CASE_STR(REL_RY);
            CASE_STR(REL_RZ);
            CASE_STR(REL_HWHEEL);
            CASE_STR(REL_DIAL);
            CASE_STR(REL_WHEEL);
            CASE_STR(REL_MISC);
            CASE_STR(REL_RESERVED);
            CASE_STR(REL_WHEEL_HI_RES);
            CASE_STR(REL_HWHEEL_HI_RES);
            CASE_STR(REL_MAX);
            CASE_STR(REL_CNT);
            default:
            return IntValueToCStr(eventCode);
        }
    }

    const char* EnumUtils::InputEventAbsCodeToString(int32_t eventCode) {
        switch(eventCode) {
            CASE_STR(ABS_X);
            CASE_STR(ABS_Y);
            CASE_STR(ABS_Z);
            CASE_STR(ABS_RX);
            CASE_STR(ABS_RY);
            CASE_STR(ABS_RZ);
            CASE_STR(ABS_THROTTLE);
            CASE_STR(ABS_RUDDER);
            CASE_STR(ABS_WHEEL);
            CASE_STR(ABS_GAS);
            CASE_STR(ABS_BRAKE);
            CASE_STR(ABS_HAT0X);
            CASE_STR(ABS_HAT0Y);
            CASE_STR(ABS_HAT1X);
            CASE_STR(ABS_HAT1Y);
            CASE_STR(ABS_HAT2X);
            CASE_STR(ABS_HAT2Y);
            CASE_STR(ABS_HAT3X);
            CASE_STR(ABS_HAT3Y);
            CASE_STR(ABS_PRESSURE);
            CASE_STR(ABS_DISTANCE);
            CASE_STR(ABS_TILT_X);
            CASE_STR(ABS_TILT_Y);
            CASE_STR(ABS_TOOL_WIDTH);
            CASE_STR(ABS_VOLUME);
            CASE_STR(ABS_MISC);
            CASE_STR(ABS_RESERVED);
            CASE_STR(ABS_MT_SLOT);
            CASE_STR(ABS_MT_TOUCH_MAJOR);
            CASE_STR(ABS_MT_TOUCH_MINOR);
            CASE_STR(ABS_MT_WIDTH_MAJOR);
            CASE_STR(ABS_MT_WIDTH_MINOR);
            CASE_STR(ABS_MT_ORIENTATION);
            CASE_STR(ABS_MT_POSITION_X);
            CASE_STR(ABS_MT_POSITION_Y);
            CASE_STR(ABS_MT_TOOL_TYPE);
            CASE_STR(ABS_MT_BLOB_ID);
            CASE_STR(ABS_MT_TRACKING_ID);
            CASE_STR(ABS_MT_PRESSURE);
            CASE_STR(ABS_MT_DISTANCE);
            CASE_STR(ABS_MT_TOOL_X);
            CASE_STR(ABS_MT_TOOL_Y);
            CASE_STR(ABS_MAX);
            CASE_STR(ABS_CNT);
            default:
                return IntValueToCStr(eventCode);
        }
    }

    const char* EnumUtils::InputEventMscCodeToString(int32_t eventCode) {
        switch(eventCode) {
            CASE_STR(MSC_SERIAL);
            CASE_STR(MSC_PULSELED);
            CASE_STR(MSC_GESTURE);
            CASE_STR(MSC_RAW);
            CASE_STR(MSC_SCAN);
            CASE_STR(MSC_TIMESTAMP);
            CASE_STR(MSC_MAX);
            CASE_STR(MSC_CNT);
            default:
                return IntValueToCStr(eventCode);
        }
    }

    const char* EnumUtils::InputEventSwCodeToString(int32_t eventCode) {
        switch(eventCode) {
            CASE_STR(MSC_SERIAL);
            CASE_STR(MSC_PULSELED);
            CASE_STR(MSC_GESTURE);
            CASE_STR(MSC_RAW);
            CASE_STR(MSC_SCAN);
            CASE_STR(MSC_TIMESTAMP);
            CASE_STR(MSC_MAX);
            CASE_STR(MSC_CNT);
            default:
                return IntValueToCStr(eventCode);
        }
    }

    const char* EnumUtils::InputEventLedCodeToString(int32_t eventCode) {
        switch(eventCode) {
            CASE_STR(LED_NUML);
            CASE_STR(LED_CAPSL);
            CASE_STR(LED_SCROLLL);
            CASE_STR(LED_COMPOSE);
            CASE_STR(LED_KANA);
            CASE_STR(LED_SLEEP);
            CASE_STR(LED_SUSPEND);
            CASE_STR(LED_MUTE);
            CASE_STR(LED_MISC);
            CASE_STR(LED_MAIL);
            CASE_STR(LED_CHARGING);
            CASE_STR(LED_MAX);
            CASE_STR(LED_CNT);
            default:
                return IntValueToCStr(eventCode);
        }

    }

    const char* EnumUtils::InputEventSndCodeToString(int32_t eventCode) {
        switch(eventCode) {
            CASE_STR(SND_CLICK);
            CASE_STR(SND_BELL);
            CASE_STR(SND_TONE);
            CASE_STR(SND_MAX);
            CASE_STR(SND_CNT);
            default:
            return IntValueToCStr(eventCode);
        }
    }

    const char* EnumUtils::InputEventRepCodeToString(int32_t eventCode) {
        switch(eventCode) {
            CASE_STR(REP_DELAY);
            //CASE_STR(REP_PERIOD);
            CASE_STR(REP_MAX);
            CASE_STR(REP_CNT);
            default:
            return IntValueToCStr(eventCode);
        }
    }

    const char* EnumUtils::InputEventFfCodeToString(int32_t eventCode) {
        return IntValueToCStr(eventCode);
    }

    const char* EnumUtils::InputEventPwrCodeToString(int32_t eventCode) {
        return IntValueToCStr(eventCode);
    }
    const char* EnumUtils::InputEventFfstatusCodeToString(int32_t eventCode) {
        return IntValueToCStr(eventCode);
    }

    const char* EnumUtils::InputPropertyToString(int32_t property) {
        switch(property) {
            CASE_STR(INPUT_PROP_POINTER);
            CASE_STR(INPUT_PROP_DIRECT);
            CASE_STR(INPUT_PROP_BUTTONPAD);
            CASE_STR(INPUT_PROP_SEMI_MT);
            CASE_STR(INPUT_PROP_TOPBUTTONPAD);
            CASE_STR(INPUT_PROP_POINTING_STICK);
            CASE_STR(INPUT_PROP_ACCELEROMETER);
            CASE_STR(INPUT_PROP_MAX);
            CASE_STR(INPUT_PROP_CNT);
            default:
                return IntValueToCStr(property);
        }
    }
} // namespace MMI
} // namespace OHOS