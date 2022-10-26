#pragma once
#include <memory>
#include <vector>
#include <map>

#include "InputEvent.h"

namespace Input {

    class KeyEvent : public InputEvent {
        public:
            static const std::shared_ptr<KeyEvent> NULL_VALUE;
            static std::shared_ptr<KeyEvent> CreateInstance();
            static std::shared_ptr<KeyEvent> CreateInstance(const std::shared_ptr<KeyEvent>& keyEvent);

        public:
            static constexpr int32_t KEY_ACTION_NONE = 0;
            static constexpr int32_t KEY_ACTION_CANCEL = 1;
            static constexpr int32_t KEY_ACTION_DOWN = 2;
            static constexpr int32_t KEY_ACTION_UP = 3;
            static constexpr int32_t KEY_ACTION_MAX = KEY_ACTION_UP + 1;

            static const int32_t KEYCODE_RESERVED;
            static const int32_t KEYCODE_ESC;
            static const int32_t KEYCODE_1;
            static const int32_t KEYCODE_2;
            static const int32_t KEYCODE_3;
            static const int32_t KEYCODE_4;
            static const int32_t KEYCODE_5;
            static const int32_t KEYCODE_6;
            static const int32_t KEYCODE_7;
            static const int32_t KEYCODE_8;
            static const int32_t KEYCODE_9;
            static const int32_t KEYCODE_0;
            static const int32_t KEYCODE_MINUS;
            static const int32_t KEYCODE_EQUAL;
            static const int32_t KEYCODE_BACKSPACE;
            static const int32_t KEYCODE_TAB;
            static const int32_t KEYCODE_Q;
            static const int32_t KEYCODE_W;
            static const int32_t KEYCODE_E;
            static const int32_t KEYCODE_R;
            static const int32_t KEYCODE_T;
            static const int32_t KEYCODE_Y;
            static const int32_t KEYCODE_U;
            static const int32_t KEYCODE_I;
            static const int32_t KEYCODE_O;
            static const int32_t KEYCODE_P;
            static const int32_t KEYCODE_LEFTBRACE;
            static const int32_t KEYCODE_RIGHTBRACE;
            static const int32_t KEYCODE_ENTER;
            static const int32_t KEYCODE_LEFTCTRL;
            static const int32_t KEYCODE_A;
            static const int32_t KEYCODE_S;
            static const int32_t KEYCODE_D;
            static const int32_t KEYCODE_F;
            static const int32_t KEYCODE_G;
            static const int32_t KEYCODE_H;
            static const int32_t KEYCODE_J;
            static const int32_t KEYCODE_K;
            static const int32_t KEYCODE_L;
            static const int32_t KEYCODE_SEMICOLON;
            static const int32_t KEYCODE_APOSTROPHE;
            static const int32_t KEYCODE_GRAVE;
            static const int32_t KEYCODE_LEFTSHIFT;
            static const int32_t KEYCODE_BACKSLASH;
            static const int32_t KEYCODE_Z;
            static const int32_t KEYCODE_X;
            static const int32_t KEYCODE_C;
            static const int32_t KEYCODE_V;
            static const int32_t KEYCODE_B;
            static const int32_t KEYCODE_N;
            static const int32_t KEYCODE_M;
            static const int32_t KEYCODE_COMMA;
            static const int32_t KEYCODE_DOT;
            static const int32_t KEYCODE_SLASH;
            static const int32_t KEYCODE_RIGHTSHIFT;
            static const int32_t KEYCODE_KPASTERISK;
            static const int32_t KEYCODE_LEFTALT;
            static const int32_t KEYCODE_SPACE;
            static const int32_t KEYCODE_CAPSLOCK;
            static const int32_t KEYCODE_F1;
            static const int32_t KEYCODE_F2;
            static const int32_t KEYCODE_F3;
            static const int32_t KEYCODE_F4;
            static const int32_t KEYCODE_F5;
            static const int32_t KEYCODE_F6;
            static const int32_t KEYCODE_F7;
            static const int32_t KEYCODE_F8;
            static const int32_t KEYCODE_F9;
            static const int32_t KEYCODE_F10;
            static const int32_t KEYCODE_NUMLOCK;
            static const int32_t KEYCODE_SCROLLLOCK;
            static const int32_t KEYCODE_KP7;
            static const int32_t KEYCODE_KP8;
            static const int32_t KEYCODE_KP9;
            static const int32_t KEYCODE_KPMINUS;
            static const int32_t KEYCODE_KP4;
            static const int32_t KEYCODE_KP5;
            static const int32_t KEYCODE_KP6;
            static const int32_t KEYCODE_KPPLUS;
            static const int32_t KEYCODE_KP1;
            static const int32_t KEYCODE_KP2;
            static const int32_t KEYCODE_KP3;
            static const int32_t KEYCODE_KP0;
            static const int32_t KEYCODE_KPDOT;
            static const int32_t KEYCODE_102ND;
            static const int32_t KEYCODE_F11;
            static const int32_t KEYCODE_F12;
            static const int32_t KEYCODE_RO;
            static const int32_t KEYCODE_KATAKANA;
            static const int32_t KEYCODE_HIRAGANA;
            static const int32_t KEYCODE_HENKAN;
            static const int32_t KEYCODE_KATAKANAHIRAGANA;
            static const int32_t KEYCODE_MUHENKAN;
            static const int32_t KEYCODE_KPJPCOMMA;
            static const int32_t KEYCODE_KPENTER;
            static const int32_t KEYCODE_RIGHTCTRL;
            static const int32_t KEYCODE_KPSLASH;
            static const int32_t KEYCODE_SYSRQ;
            static const int32_t KEYCODE_RIGHTALT;
            static const int32_t KEYCODE_LINEFEED;
            static const int32_t KEYCODE_HOME;
            static const int32_t KEYCODE_UP;
            static const int32_t KEYCODE_PAGEUP;
            static const int32_t KEYCODE_LEFT;
            static const int32_t KEYCODE_RIGHT;
            static const int32_t KEYCODE_END;
            static const int32_t KEYCODE_DOWN;
            static const int32_t KEYCODE_PAGEDOWN;
            static const int32_t KEYCODE_INSERT;
            static const int32_t KEYCODE_DELETE;
            static const int32_t KEYCODE_MACRO;
            //static const int32_t KEYCODE_MUTE;
            static const int32_t KEYCODE_VOLUMEDOWN;
            static const int32_t KEYCODE_VOLUMEUP;
            static const int32_t KEYCODE_POWER;
            static const int32_t KEYCODE_KPEQUAL;
            static const int32_t KEYCODE_KPPLUSMINUS;
            static const int32_t KEYCODE_PAUSE;
            static const int32_t KEYCODE_SCALE;

            static const int32_t KEYCODE_KPCOMMA;
            //static const int32_t KEYCODE_HANGEUL;
            //static const int32_t KEYCODE_HANGUEL;
            static const int32_t KEYCODE_HANJA;
            static const int32_t KEYCODE_YEN;
            static const int32_t KEYCODE_LEFTMETA;
            static const int32_t KEYCODE_RIGHTMETA;
            static const int32_t KEYCODE_COMPOSE;
            static const int32_t KEYCODE_AGAIN;
            static const int32_t KEYCODE_PROPS;
            static const int32_t KEYCODE_UNDO;
            static const int32_t KEYCODE_FRONT;
            static const int32_t KEYCODE_COPY;
            static const int32_t KEYCODE_OPEN;
            static const int32_t KEYCODE_PASTE;
            static const int32_t KEYCODE_FIND;
            static const int32_t KEYCODE_CUT;
            static const int32_t KEYCODE_HELP;
            static const int32_t KEYCODE_MENU;
            static const int32_t KEYCODE_CALC;
            static const int32_t KEYCODE_SETUP;
            static const int32_t KEYCODE_SLEEP;
            static const int32_t KEYCODE_WAKEUP;
            static const int32_t KEYCODE_FILE;
            static const int32_t KEYCODE_SENDFILE;
            static const int32_t KEYCODE_DELETEFILE;
            static const int32_t KEYCODE_XFER;
            static const int32_t KEYCODE_PROG1;
            static const int32_t KEYCODE_PROG2;
            static const int32_t KEYCODE_WWW;
            static const int32_t KEYCODE_MSDOS;
            static const int32_t KEYCODE_COFFEE;
            //static const int32_t KEYCODE_SCREENLOCK;
            //static const int32_t KEYCODE_ROTATE_DISPLAY;
            //static const int32_t KEYCODE_DIRECTION;
            //static const int32_t KEYCODE_CYCLEWINDOWS;
            static const int32_t KEYCODE_MAIL;
            static const int32_t KEYCODE_BOOKMARKS;
            static const int32_t KEYCODE_COMPUTER;
            static const int32_t KEYCODE_BACK;
            static const int32_t KEYCODE_FORWARD;
            static const int32_t KEYCODE_CLOSECD;
            static const int32_t KEYCODE_EJECTCD;
            static const int32_t KEYCODE_EJECTCLOSECD;
            static const int32_t KEYCODE_NEXTSONG;
            static const int32_t KEYCODE_PLAYPAUSE;
            static const int32_t KEYCODE_PREVIOUSSONG;
            static const int32_t KEYCODE_STOPCD;
            static const int32_t KEYCODE_RECORD;
            static const int32_t KEYCODE_REWIND;
            static const int32_t KEYCODE_PHONE;
            static const int32_t KEYCODE_ISO;
            static const int32_t KEYCODE_CONFIG;
            static const int32_t KEYCODE_HOMEPAGE;
            static const int32_t KEYCODE_REFRESH;
            static const int32_t KEYCODE_EXIT;
            static const int32_t KEYCODE_MOVE;
            static const int32_t KEYCODE_EDIT;
            static const int32_t KEYCODE_SCROLLUP;
            static const int32_t KEYCODE_SCROLLDOWN;
            static const int32_t KEYCODE_KPLEFTPAREN;
            static const int32_t KEYCODE_KPRIGHTPAREN;
            static const int32_t KEYCODE_NEW;
            static const int32_t KEYCODE_REDO;

            static const int32_t KEYCODE_F13;
            static const int32_t KEYCODE_F14;
            static const int32_t KEYCODE_F15;
            static const int32_t KEYCODE_F16;
            static const int32_t KEYCODE_F17;
            static const int32_t KEYCODE_F18;
            static const int32_t KEYCODE_F19;
            static const int32_t KEYCODE_F20;
            static const int32_t KEYCODE_F21;
            static const int32_t KEYCODE_F22;
            static const int32_t KEYCODE_F23;
            static const int32_t KEYCODE_F24;

            static const int32_t KEYCODE_PLAYCD;
            static const int32_t KEYCODE_PAUSECD;
            static const int32_t KEYCODE_PROG3;
            static const int32_t KEYCODE_PROG4;
            static const int32_t KEYCODE_DASHBOARD;
            static const int32_t KEYCODE_SUSPEND;
            static const int32_t KEYCODE_CLOSE;
            static const int32_t KEYCODE_PLAY;
            static const int32_t KEYCODE_FASTFORWARD;
            static const int32_t KEYCODE_BASSBOOST;
            static const int32_t KEYCODE_PRINT;
            static const int32_t KEYCODE_HP;
            static const int32_t KEYCODE_CAMERA;
            static const int32_t KEYCODE_SOUND;
            static const int32_t KEYCODE_QUESTION;
            static const int32_t KEYCODE_EMAIL;
            static const int32_t KEYCODE_CHAT;
            static const int32_t KEYCODE_SEARCH;
            static const int32_t KEYCODE_CONNECT;
            static const int32_t KEYCODE_FINANCE;
            static const int32_t KEYCODE_SPORT;
            static const int32_t KEYCODE_SHOP;
            static const int32_t KEYCODE_ALTERASE;
            static const int32_t KEYCODE_CANCEL;
            static const int32_t KEYCODE_BRIGHTNESSDOWN;
            static const int32_t KEYCODE_BRIGHTNESSUP;
            static const int32_t KEYCODE_MEDIA;

            static const int32_t KEYCODE_SWITCHVIDEOMODE;
            static const int32_t KEYCODE_KBDILLUMTOGGLE;
            static const int32_t KEYCODE_KBDILLUMDOWN;
            static const int32_t KEYCODE_KBDILLUMUP;

            static const int32_t KEYCODE_SEND;
            static const int32_t KEYCODE_REPLY;
            static const int32_t KEYCODE_FORWARDMAIL;
            static const int32_t KEYCODE_SAVE;
            static const int32_t KEYCODE_DOCUMENTS;

            static const int32_t KEYCODE_BATTERY;

            static const int32_t KEYCODE_BLUETOOTH;
            static const int32_t KEYCODE_WLAN;
            static const int32_t KEYCODE_UWB;

            static const int32_t KEYCODE_UNKNOWN;

            static const int32_t KEYCODE_VIDEO_NEXT;
            static const int32_t KEYCODE_VIDEO_PREV;
            static const int32_t KEYCODE_BRIGHTNESS_CYCLE;
            //static const int32_t KEYCODE_BRIGHTNESS_AUTO;

            //static const int32_t KEYCODE_BRIGHTNESS_ZERO;
            static const int32_t KEYCODE_DISPLAY_OFF;

            //static const int32_t KEYCODE_WWAN;
            //static const int32_t KEYCODE_WIMAX;
            static const int32_t KEYCODE_RFKILL;

            static const int32_t KEYCODE_MICMUTE;

            //static const int32_t BTNCODE_MISC;
            static const int32_t BTNCODE_0;
            static const int32_t BTNCODE_1;
            static const int32_t BTNCODE_2;
            static const int32_t BTNCODE_3;
            static const int32_t BTNCODE_4;
            static const int32_t BTNCODE_5;
            static const int32_t BTNCODE_6;
            static const int32_t BTNCODE_7;
            static const int32_t BTNCODE_8;
            static const int32_t BTNCODE_9;

            //static const int32_t BTNCODE_MOUSE;
            static const int32_t BTNCODE_LEFT;
            static const int32_t BTNCODE_RIGHT;
            static const int32_t BTNCODE_MIDDLE;
            static const int32_t BTNCODE_SIDE;
            static const int32_t BTNCODE_EXTRA;
            static const int32_t BTNCODE_FORWARD;
            static const int32_t BTNCODE_BACK;
            static const int32_t BTNCODE_TASK;

            //static const int32_t BTNCODE_JOYSTICK;
            static const int32_t BTNCODE_TRIGGER;
            static const int32_t BTNCODE_THUMB;
            static const int32_t BTNCODE_THUMB2;
            static const int32_t BTNCODE_TOP;
            static const int32_t BTNCODE_TOP2;
            static const int32_t BTNCODE_PINKIE;
            static const int32_t BTNCODE_BASE;
            static const int32_t BTNCODE_BASE2;
            static const int32_t BTNCODE_BASE3;
            static const int32_t BTNCODE_BASE4;
            static const int32_t BTNCODE_BASE5;
            static const int32_t BTNCODE_BASE6;
            static const int32_t BTNCODE_DEAD;

            static const int32_t BTNCODE_GAMEPAD;
            //static const int32_t BTNCODE_SOUTH;
            //static const int32_t BTNCODE_A;
            static const int32_t BTNCODE_EAST;
            //static const int32_t BTNCODE_B;
            static const int32_t BTNCODE_C;
            //static const int32_t BTNCODE_NORTH;
            //static const int32_t BTNCODE_X;
            //static const int32_t BTNCODE_WEST;
            //static const int32_t BTNCODE_Y;
            static const int32_t BTNCODE_Z;
            static const int32_t BTNCODE_TL;
            static const int32_t BTNCODE_TR;
            static const int32_t BTNCODE_TL2;
            static const int32_t BTNCODE_TR2;
            static const int32_t BTNCODE_SELECT;
            static const int32_t BTNCODE_START;
            static const int32_t BTNCODE_MODE;
            static const int32_t BTNCODE_THUMBL;
            static const int32_t BTNCODE_THUMBR;

            static const int32_t BTNCODE_DIGI;
            //static const int32_t BTNCODE_TOOL_PEN;
            static const int32_t BTNCODE_TOOL_RUBBER;
            static const int32_t BTNCODE_TOOL_BRUSH;
            static const int32_t BTNCODE_TOOL_PENCIL;
            static const int32_t BTNCODE_TOOL_AIRBRUSH;
            static const int32_t BTNCODE_TOOL_FINGER;
            static const int32_t BTNCODE_TOOL_MOUSE;
            static const int32_t BTNCODE_TOOL_LENS;
            static const int32_t BTNCODE_TOOL_QUINTTAP;
            static const int32_t BTNCODE_STYLUS3;
            static const int32_t BTNCODE_TOUCH;
            static const int32_t BTNCODE_STYLUS;
            static const int32_t BTNCODE_STYLUS2;
            static const int32_t BTNCODE_TOOL_DOUBLETAP;
            static const int32_t BTNCODE_TOOL_TRIPLETAP;
            static const int32_t BTNCODE_TOOL_QUADTAP;

            //static const int32_t BTNCODE_WHEEL;
            static const int32_t BTNCODE_GEAR_DOWN;
            static const int32_t BTNCODE_GEAR_UP;

            static const int32_t KEYCODE_OK;
            static const int32_t KEYCODE_SELECT;
            static const int32_t KEYCODE_GOTO;
            static const int32_t KEYCODE_CLEAR;
            static const int32_t KEYCODE_POWER2;
            static const int32_t KEYCODE_OPTION;
            static const int32_t KEYCODE_INFO;
            static const int32_t KEYCODE_TIME;
            static const int32_t KEYCODE_VENDOR;
            static const int32_t KEYCODE_ARCHIVE;
            static const int32_t KEYCODE_PROGRAM;
            static const int32_t KEYCODE_CHANNEL;
            static const int32_t KEYCODE_FAVORITES;
            static const int32_t KEYCODE_EPG;
            static const int32_t KEYCODE_PVR;
            static const int32_t KEYCODE_MHP;
            static const int32_t KEYCODE_LANGUAGE;
            static const int32_t KEYCODE_TITLE;
            static const int32_t KEYCODE_SUBTITLE;
            static const int32_t KEYCODE_ANGLE;
            static const int32_t KEYCODE_FULL_SCREEN;
            //static const int32_t KEYCODE_ZOOM;
            static const int32_t KEYCODE_MODE;
            static const int32_t KEYCODE_KEYBOARD;
            static const int32_t KEYCODE_ASPECT_RATIO;
            //static const int32_t KEYCODE_SCREEN;
            static const int32_t KEYCODE_PC;
            static const int32_t KEYCODE_TV;
            static const int32_t KEYCODE_TV2;
            static const int32_t KEYCODE_VCR;
            static const int32_t KEYCODE_VCR2;
            static const int32_t KEYCODE_SAT;
            static const int32_t KEYCODE_SAT2;
            static const int32_t KEYCODE_CD;
            static const int32_t KEYCODE_TAPE;
            static const int32_t KEYCODE_RADIO;
            static const int32_t KEYCODE_TUNER;
            static const int32_t KEYCODE_PLAYER;
            static const int32_t KEYCODE_TEXT;
            static const int32_t KEYCODE_DVD;
            static const int32_t KEYCODE_AUX;
            static const int32_t KEYCODE_MP3;
            static const int32_t KEYCODE_AUDIO;
            static const int32_t KEYCODE_VIDEO;
            static const int32_t KEYCODE_DIRECTORY;
            static const int32_t KEYCODE_LIST;
            static const int32_t KEYCODE_MEMO;
            static const int32_t KEYCODE_CALENDAR;
            static const int32_t KEYCODE_RED;
            static const int32_t KEYCODE_GREEN;
            static const int32_t KEYCODE_YELLOW;
            static const int32_t KEYCODE_BLUE;
            static const int32_t KEYCODE_CHANNELUP;
            static const int32_t KEYCODE_CHANNELDOWN;
            static const int32_t KEYCODE_FIRST;
            static const int32_t KEYCODE_LAST;
            static const int32_t KEYCODE_AB;
            static const int32_t KEYCODE_NEXT;
            static const int32_t KEYCODE_RESTART;
            static const int32_t KEYCODE_SLOW;
            static const int32_t KEYCODE_SHUFFLE;
            static const int32_t KEYCODE_BREAK;
            static const int32_t KEYCODE_PREVIOUS;
            static const int32_t KEYCODE_DIGITS;
            static const int32_t KEYCODE_TEEN;
            static const int32_t KEYCODE_TWEN;
            static const int32_t KEYCODE_VIDEOPHONE;
            static const int32_t KEYCODE_GAMES;
            static const int32_t KEYCODE_ZOOMIN;
            static const int32_t KEYCODE_ZOOMOUT;
            static const int32_t KEYCODE_ZOOMRESET;
            static const int32_t KEYCODE_WORDPROCESSOR;
            static const int32_t KEYCODE_EDITOR;
            static const int32_t KEYCODE_SPREADSHEET;
            static const int32_t KEYCODE_GRAPHICSEDITOR;
            static const int32_t KEYCODE_PRESENTATION;
            static const int32_t KEYCODE_DATABASE;
            static const int32_t KEYCODE_NEWS;
            static const int32_t KEYCODE_VOICEMAIL;
            static const int32_t KEYCODE_ADDRESSBOOK;
            static const int32_t KEYCODE_MESSENGER;
            static const int32_t KEYCODE_DISPLAYTOGGLE;
            //static const int32_t KEYCODE_BRIGHTNESS_TOGGLE;
            static const int32_t KEYCODE_SPELLCHECK;
            static const int32_t KEYCODE_LOGOFF;

            static const int32_t KEYCODE_DOLLAR;
            static const int32_t KEYCODE_EURO;

            static const int32_t KEYCODE_FRAMEBACK;
            static const int32_t KEYCODE_FRAMEFORWARD;
            static const int32_t KEYCODE_CONTEXT_MENU;
            static const int32_t KEYCODE_MEDIA_REPEAT;
            static const int32_t KEYCODE_10CHANNELSUP;
            static const int32_t KEYCODE_10CHANNELSDOWN;
            static const int32_t KEYCODE_IMAGES;
            //static const int32_t KEYCODE_NOTIFICATION_CENTER;
            //static const int32_t KEYCODE_PICKUP_PHONE;
            //static const int32_t KEYCODE_HANGUP_PHONE;

            static const int32_t KEYCODE_DEL_EOL;
            static const int32_t KEYCODE_DEL_EOS;
            static const int32_t KEYCODE_INS_LINE;
            static const int32_t KEYCODE_DEL_LINE;

            static const int32_t KEYCODE_FN;
            static const int32_t KEYCODE_FN_ESC;
            static const int32_t KEYCODE_FN_F1;
            static const int32_t KEYCODE_FN_F2;
            static const int32_t KEYCODE_FN_F3;
            static const int32_t KEYCODE_FN_F4;
            static const int32_t KEYCODE_FN_F5;
            static const int32_t KEYCODE_FN_F6;
            static const int32_t KEYCODE_FN_F7;
            static const int32_t KEYCODE_FN_F8;
            static const int32_t KEYCODE_FN_F9;
            static const int32_t KEYCODE_FN_F10;
            static const int32_t KEYCODE_FN_F11;
            static const int32_t KEYCODE_FN_F12;
            static const int32_t KEYCODE_FN_1;
            static const int32_t KEYCODE_FN_2;
            static const int32_t KEYCODE_FN_D;
            static const int32_t KEYCODE_FN_E;
            static const int32_t KEYCODE_FN_F;
            static const int32_t KEYCODE_FN_S;
            static const int32_t KEYCODE_FN_B;
            //static const int32_t KEYCODE_FN_RIGHT_SHIFT;

            static const int32_t KEYCODE_BRL_DOT1;
            static const int32_t KEYCODE_BRL_DOT2;
            static const int32_t KEYCODE_BRL_DOT3;
            static const int32_t KEYCODE_BRL_DOT4;
            static const int32_t KEYCODE_BRL_DOT5;
            static const int32_t KEYCODE_BRL_DOT6;
            static const int32_t KEYCODE_BRL_DOT7;
            static const int32_t KEYCODE_BRL_DOT8;
            static const int32_t KEYCODE_BRL_DOT9;
            static const int32_t KEYCODE_BRL_DOT10;

            static const int32_t KEYCODE_NUMERIC_0;
            static const int32_t KEYCODE_NUMERIC_1;
            static const int32_t KEYCODE_NUMERIC_2;
            static const int32_t KEYCODE_NUMERIC_3;
            static const int32_t KEYCODE_NUMERIC_4;
            static const int32_t KEYCODE_NUMERIC_5;
            static const int32_t KEYCODE_NUMERIC_6;
            static const int32_t KEYCODE_NUMERIC_7;
            static const int32_t KEYCODE_NUMERIC_8;
            static const int32_t KEYCODE_NUMERIC_9;
            static const int32_t KEYCODE_NUMERIC_STAR;
            static const int32_t KEYCODE_NUMERIC_POUND;
            static const int32_t KEYCODE_NUMERIC_A;
            static const int32_t KEYCODE_NUMERIC_B;
            static const int32_t KEYCODE_NUMERIC_C;
            static const int32_t KEYCODE_NUMERIC_D;

            static const int32_t KEYCODE_CAMERA_FOCUS;
            static const int32_t KEYCODE_WPS_BUTTON;

            static const int32_t KEYCODE_TOUCHPAD_TOGGLE;
            static const int32_t KEYCODE_TOUCHPAD_ON;
            static const int32_t KEYCODE_TOUCHPAD_OFF;

            static const int32_t KEYCODE_CAMERA_ZOOMIN;
            static const int32_t KEYCODE_CAMERA_ZOOMOUT;
            static const int32_t KEYCODE_CAMERA_UP;
            static const int32_t KEYCODE_CAMERA_DOWN;
            static const int32_t KEYCODE_CAMERA_LEFT;
            static const int32_t KEYCODE_CAMERA_RIGHT;

            static const int32_t KEYCODE_ATTENDANT_ON;
            static const int32_t KEYCODE_ATTENDANT_OFF;
            static const int32_t KEYCODE_ATTENDANT_TOGGLE;
            static const int32_t KEYCODE_LIGHTS_TOGGLE;

            static const int32_t BTNCODE_DPAD_UP;
            static const int32_t BTNCODE_DPAD_DOWN;
            static const int32_t BTNCODE_DPAD_LEFT;
            static const int32_t BTNCODE_DPAD_RIGHT;

            static const int32_t KEYCODE_ALS_TOGGLE;
            static const int32_t KEYCODE_ROTATE_LOCK_TOGGLE;

            static const int32_t KEYCODE_BUTTONCONFIG;
            static const int32_t KEYCODE_TASKMANAGER;
            static const int32_t KEYCODE_JOURNAL;
            static const int32_t KEYCODE_CONTROLPANEL;
            static const int32_t KEYCODE_APPSELECT;
            static const int32_t KEYCODE_SCREENSAVER;
            static const int32_t KEYCODE_VOICECOMMAND;
            static const int32_t KEYCODE_ASSISTANT;
            static const int32_t KEYCODE_KBD_LAYOUT_NEXT;
            // static const int32_t KEYCODE_EMOJI_PICKER;

            static const int32_t KEYCODE_BRIGHTNESS_MIN;
            static const int32_t KEYCODE_BRIGHTNESS_MAX;

            static const int32_t KEYCODE_KBDINPUTASSIST_PREV;
            static const int32_t KEYCODE_KBDINPUTASSIST_NEXT;
            static const int32_t KEYCODE_KBDINPUTASSIST_PREVGROUP;
            static const int32_t KEYCODE_KBDINPUTASSIST_NEXTGROUP;
            static const int32_t KEYCODE_KBDINPUTASSIST_ACCEPT;
            static const int32_t KEYCODE_KBDINPUTASSIST_CANCEL;

            static const int32_t KEYCODE_RIGHT_UP;
            static const int32_t KEYCODE_RIGHT_DOWN;
            static const int32_t KEYCODE_LEFT_UP;
            static const int32_t KEYCODE_LEFT_DOWN;

            static const int32_t KEYCODE_ROOT_MENU;
            static const int32_t KEYCODE_MEDIA_TOP_MENU;
            static const int32_t KEYCODE_NUMERIC_11;
            static const int32_t KEYCODE_NUMERIC_12;
            static const int32_t KEYCODE_AUDIO_DESC;
            static const int32_t KEYCODE_3D_MODE;
            static const int32_t KEYCODE_NEXT_FAVORITE;
            static const int32_t KEYCODE_STOP_RECORD;
            static const int32_t KEYCODE_PAUSE_RECORD;
            static const int32_t KEYCODE_VOD;
            static const int32_t KEYCODE_UNMUTE;
            static const int32_t KEYCODE_FASTREVERSE;
            static const int32_t KEYCODE_SLOWREVERSE;
            static const int32_t KEYCODE_DATA;
            static const int32_t KEYCODE_ONSCREEN_KEYBOARD;
            //static const int32_t KEYCODE_PRIVACY_SCREEN_TOGGLE;
            //static const int32_t KEYCODE_SELECTIVE_SCREENSHOT;
            //static const int32_t KEYCODE_MACRO1;
            //static const int32_t KEYCODE_MACRO2;
            //static const int32_t KEYCODE_MACRO3;
            //static const int32_t KEYCODE_MACRO4;
            //static const int32_t KEYCODE_MACRO5;
            //static const int32_t KEYCODE_MACRO6;
            //static const int32_t KEYCODE_MACRO7;
            //static const int32_t KEYCODE_MACRO8;
            //static const int32_t KEYCODE_MACRO9;
            //static const int32_t KEYCODE_MACRO10;
            //static const int32_t KEYCODE_MACRO11;
            //static const int32_t KEYCODE_MACRO12;
            //static const int32_t KEYCODE_MACRO13;
            //static const int32_t KEYCODE_MACRO14;
            //static const int32_t KEYCODE_MACRO15;
            //static const int32_t KEYCODE_MACRO16;
            //static const int32_t KEYCODE_MACRO17;
            //static const int32_t KEYCODE_MACRO18;
            //static const int32_t KEYCODE_MACRO19;
            //static const int32_t KEYCODE_MACRO20;
            //static const int32_t KEYCODE_MACRO21;
            //static const int32_t KEYCODE_MACRO22;
            //static const int32_t KEYCODE_MACRO23;
            //static const int32_t KEYCODE_MACRO24;
            //static const int32_t KEYCODE_MACRO25;
            //static const int32_t KEYCODE_MACRO26;
            //static const int32_t KEYCODE_MACRO27;
            //static const int32_t KEYCODE_MACRO28;
            //static const int32_t KEYCODE_MACRO29;
            //static const int32_t KEYCODE_MACRO30;
            //static const int32_t KEYCODE_MACRO_RECORD_START;
            //static const int32_t KEYCODE_MACRO_RECORD_STOP;
            //static const int32_t KEYCODE_MACRO_PRESET_CYCLE;
            //static const int32_t KEYCODE_MACRO_PRESET1;
            //static const int32_t KEYCODE_MACRO_PRESET2;
            //static const int32_t KEYCODE_MACRO_PRESET3;
            //static const int32_t KEYCODE_KBD_LCD_MENU1;
            //static const int32_t KEYCODE_KBD_LCD_MENU2;
            //static const int32_t KEYCODE_KBD_LCD_MENU3;
            //static const int32_t KEYCODE_KBD_LCD_MENU4;
            //static const int32_t KEYCODE_KBD_LCD_MENU5;
            //static const int32_t BTNCODE_TRIGGER_HAPPY;
            //static const int32_t BTNCODE_TRIGGER_HAPPY1;
            static const int32_t BTNCODE_TRIGGER_HAPPY2;
            static const int32_t BTNCODE_TRIGGER_HAPPY3;
            static const int32_t BTNCODE_TRIGGER_HAPPY4;
            static const int32_t BTNCODE_TRIGGER_HAPPY5;
            static const int32_t BTNCODE_TRIGGER_HAPPY6;
            static const int32_t BTNCODE_TRIGGER_HAPPY7;
            static const int32_t BTNCODE_TRIGGER_HAPPY8;
            static const int32_t BTNCODE_TRIGGER_HAPPY9;
            static const int32_t BTNCODE_TRIGGER_HAPPY10;
            static const int32_t BTNCODE_TRIGGER_HAPPY11;
            static const int32_t BTNCODE_TRIGGER_HAPPY12;
            static const int32_t BTNCODE_TRIGGER_HAPPY13;
            static const int32_t BTNCODE_TRIGGER_HAPPY14;
            static const int32_t BTNCODE_TRIGGER_HAPPY15;
            static const int32_t BTNCODE_TRIGGER_HAPPY16;
            static const int32_t BTNCODE_TRIGGER_HAPPY17;
            static const int32_t BTNCODE_TRIGGER_HAPPY18;
            static const int32_t BTNCODE_TRIGGER_HAPPY19;
            static const int32_t BTNCODE_TRIGGER_HAPPY20;
            static const int32_t BTNCODE_TRIGGER_HAPPY21;
            static const int32_t BTNCODE_TRIGGER_HAPPY22;
            static const int32_t BTNCODE_TRIGGER_HAPPY23;
            static const int32_t BTNCODE_TRIGGER_HAPPY24;
            static const int32_t BTNCODE_TRIGGER_HAPPY25;
            static const int32_t BTNCODE_TRIGGER_HAPPY26;
            static const int32_t BTNCODE_TRIGGER_HAPPY27;
            static const int32_t BTNCODE_TRIGGER_HAPPY28;
            static const int32_t BTNCODE_TRIGGER_HAPPY29;
            static const int32_t BTNCODE_TRIGGER_HAPPY30;
            static const int32_t BTNCODE_TRIGGER_HAPPY31;
            static const int32_t BTNCODE_TRIGGER_HAPPY32;
            static const int32_t BTNCODE_TRIGGER_HAPPY33;
            static const int32_t BTNCODE_TRIGGER_HAPPY34;
            static const int32_t BTNCODE_TRIGGER_HAPPY35;
            static const int32_t BTNCODE_TRIGGER_HAPPY36;
            static const int32_t BTNCODE_TRIGGER_HAPPY37;
            static const int32_t BTNCODE_TRIGGER_HAPPY38;
            static const int32_t BTNCODE_TRIGGER_HAPPY39;
            static const int32_t BTNCODE_TRIGGER_HAPPY40;
            static const int32_t KEYCODE_MIN_INTERESTING;

        public:
            static const char* KeyActionToString(int32_t keyAction);
            static const char* KeyCodeToString(int32_t keyCode);

        public:
            class KeyItem {
                public:
                    friend class KeyEvent;
                public:
                    KeyItem();
                    KeyItem(int32_t keyCode, int64_t downTime);
                    KeyItem(int32_t keyCode, int64_t downTime, int64_t upTime);
                    ~KeyItem() = default;

                    int32_t GetKeyCode() const;
                    void SetKeyCode(int32_t keyCode);

                    int64_t GetDownTime() const;
                    void SetDownTime(int64_t downTime);

                    int64_t GetUpTime() const;
                    void SetUpTime(int64_t upTime);

                    std::vector<int32_t> GetDevices() const;
                    int32_t AddDevice(int32_t deviceId);
                    int32_t RemoveDevice(int32_t deviceId);

                    virtual std::ostream& operator<<(std::ostream& outStream) const;

                private:
                    int32_t keyCode_;
                    int64_t downTime_;
                    int64_t upTime_;
                    std::vector<int32_t> devices_;
            };

        public:
            KeyEvent();
            virtual ~KeyEvent() = default;
            int32_t GetKeyAction() const;
            int32_t GetKeyCode() const;
            std::vector<int32_t> GetPressedKeys() const;
            KeyItem* GetKeyItem(int32_t keyCode);

            KeyItem* SetKeyDown(int32_t keyCode, int32_t deviceId, int64_t downTime);
            KeyItem* SetKeyUp(int32_t keyCode, int32_t deviceId, int64_t upTime);

            virtual std::ostream& operator<<(std::ostream& outStream) const;

        protected:
            KeyEvent(int32_t eventType);

            virtual KeyEvent* Clone() const;
            std::ostream& PrintInternal(std::ostream& outStream) const;

            void RemoveReleasedKeys();

        private:
            int32_t keyAction_;
            int32_t keyCode_;
            std::map<int32_t, KeyItem> keyItems_;

    };

    std::ostream& operator<<(std::ostream& outStream, const KeyEvent& keyEvent);
    std::ostream& operator<<(std::ostream& outStream, const KeyEvent::KeyItem& keyItem);
}
