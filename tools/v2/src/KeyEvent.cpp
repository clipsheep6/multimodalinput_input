#include <linux/input.h>

#include "KeyEvent.h"
#include "TimeUtils.h"
#include "StreamUtils.h"
#include "IInputDefine.h"
#include "Log.h"

namespace Input {

    const std::shared_ptr<KeyEvent> KeyEvent::NULL_VALUE;

    const int32_t KeyEvent::KEYCODE_RESERVED = KEY_RESERVED;
    const int32_t KeyEvent::KEYCODE_ESC = KEY_ESC;
    const int32_t KeyEvent::KEYCODE_1 = KEY_1;
    const int32_t KeyEvent::KEYCODE_2 = KEY_2;
    const int32_t KeyEvent::KEYCODE_3 = KEY_3;
    const int32_t KeyEvent::KEYCODE_4 = KEY_4;
    const int32_t KeyEvent::KEYCODE_5 = KEY_5;
    const int32_t KeyEvent::KEYCODE_6 = KEY_6;
    const int32_t KeyEvent::KEYCODE_7 = KEY_7;
    const int32_t KeyEvent::KEYCODE_8 = KEY_8;
    const int32_t KeyEvent::KEYCODE_9 = KEY_9;
    const int32_t KeyEvent::KEYCODE_0 = KEY_0;
    const int32_t KeyEvent::KEYCODE_MINUS = KEY_MINUS;
    const int32_t KeyEvent::KEYCODE_EQUAL = KEY_EQUAL;
    const int32_t KeyEvent::KEYCODE_BACKSPACE = KEY_BACKSPACE;
    const int32_t KeyEvent::KEYCODE_TAB = KEY_TAB;
    const int32_t KeyEvent::KEYCODE_Q = KEY_Q;
    const int32_t KeyEvent::KEYCODE_W = KEY_W;
    const int32_t KeyEvent::KEYCODE_E = KEY_E;
    const int32_t KeyEvent::KEYCODE_R = KEY_R;
    const int32_t KeyEvent::KEYCODE_T = KEY_T;
    const int32_t KeyEvent::KEYCODE_Y = KEY_Y;
    const int32_t KeyEvent::KEYCODE_U = KEY_U;
    const int32_t KeyEvent::KEYCODE_I = KEY_I;
    const int32_t KeyEvent::KEYCODE_O = KEY_O;
    const int32_t KeyEvent::KEYCODE_P = KEY_P;
    const int32_t KeyEvent::KEYCODE_LEFTBRACE = KEY_LEFTBRACE;
    const int32_t KeyEvent::KEYCODE_RIGHTBRACE = KEY_RIGHTBRACE;
    const int32_t KeyEvent::KEYCODE_ENTER = KEY_ENTER;
    const int32_t KeyEvent::KEYCODE_LEFTCTRL = KEY_LEFTCTRL;
    const int32_t KeyEvent::KEYCODE_A = KEY_A;
    const int32_t KeyEvent::KEYCODE_S = KEY_S;
    const int32_t KeyEvent::KEYCODE_D = KEY_D;
    const int32_t KeyEvent::KEYCODE_F = KEY_F;
    const int32_t KeyEvent::KEYCODE_G = KEY_G;
    const int32_t KeyEvent::KEYCODE_H = KEY_H;
    const int32_t KeyEvent::KEYCODE_J = KEY_J;
    const int32_t KeyEvent::KEYCODE_K = KEY_K;
    const int32_t KeyEvent::KEYCODE_L = KEY_L;
    const int32_t KeyEvent::KEYCODE_SEMICOLON = KEY_SEMICOLON;
    const int32_t KeyEvent::KEYCODE_APOSTROPHE = KEY_APOSTROPHE;
    const int32_t KeyEvent::KEYCODE_GRAVE = KEY_GRAVE;
    const int32_t KeyEvent::KEYCODE_LEFTSHIFT = KEY_LEFTSHIFT;
    const int32_t KeyEvent::KEYCODE_BACKSLASH = KEY_BACKSLASH;
    const int32_t KeyEvent::KEYCODE_Z = KEY_Z;
    const int32_t KeyEvent::KEYCODE_X = KEY_X;
    const int32_t KeyEvent::KEYCODE_C = KEY_C;
    const int32_t KeyEvent::KEYCODE_V = KEY_V;
    const int32_t KeyEvent::KEYCODE_B = KEY_B;
    const int32_t KeyEvent::KEYCODE_N = KEY_N;
    const int32_t KeyEvent::KEYCODE_M = KEY_M;
    const int32_t KeyEvent::KEYCODE_COMMA = KEY_COMMA;
    const int32_t KeyEvent::KEYCODE_DOT = KEY_DOT;
    const int32_t KeyEvent::KEYCODE_SLASH = KEY_SLASH;
    const int32_t KeyEvent::KEYCODE_RIGHTSHIFT = KEY_RIGHTSHIFT;
    const int32_t KeyEvent::KEYCODE_KPASTERISK = KEY_KPASTERISK;
    const int32_t KeyEvent::KEYCODE_LEFTALT = KEY_LEFTALT;
    const int32_t KeyEvent::KEYCODE_SPACE = KEY_SPACE;
    const int32_t KeyEvent::KEYCODE_CAPSLOCK = KEY_CAPSLOCK;
    const int32_t KeyEvent::KEYCODE_F1 = KEY_F1;
    const int32_t KeyEvent::KEYCODE_F2 = KEY_F2;
    const int32_t KeyEvent::KEYCODE_F3 = KEY_F3;
    const int32_t KeyEvent::KEYCODE_F4 = KEY_F4;
    const int32_t KeyEvent::KEYCODE_F5 = KEY_F5;
    const int32_t KeyEvent::KEYCODE_F6 = KEY_F6;
    const int32_t KeyEvent::KEYCODE_F7 = KEY_F7;
    const int32_t KeyEvent::KEYCODE_F8 = KEY_F8;
    const int32_t KeyEvent::KEYCODE_F9 = KEY_F9;
    const int32_t KeyEvent::KEYCODE_F10 = KEY_F10;
    const int32_t KeyEvent::KEYCODE_NUMLOCK = KEY_NUMLOCK;
    const int32_t KeyEvent::KEYCODE_SCROLLLOCK = KEY_SCROLLLOCK;
    const int32_t KeyEvent::KEYCODE_KP7 = KEY_KP7;
    const int32_t KeyEvent::KEYCODE_KP8 = KEY_KP8;
    const int32_t KeyEvent::KEYCODE_KP9 = KEY_KP9;
    const int32_t KeyEvent::KEYCODE_KPMINUS = KEY_KPMINUS;
    const int32_t KeyEvent::KEYCODE_KP4 = KEY_KP4;
    const int32_t KeyEvent::KEYCODE_KP5 = KEY_KP5;
    const int32_t KeyEvent::KEYCODE_KP6 = KEY_KP6;
    const int32_t KeyEvent::KEYCODE_KPPLUS = KEY_KPPLUS;
    const int32_t KeyEvent::KEYCODE_KP1 = KEY_KP1;
    const int32_t KeyEvent::KEYCODE_KP2 = KEY_KP2;
    const int32_t KeyEvent::KEYCODE_KP3 = KEY_KP3;
    const int32_t KeyEvent::KEYCODE_KP0 = KEY_KP0;
    const int32_t KeyEvent::KEYCODE_KPDOT = KEY_KPDOT;
    const int32_t KeyEvent::KEYCODE_102ND = KEY_102ND;
    const int32_t KeyEvent::KEYCODE_F11 = KEY_F11;
    const int32_t KeyEvent::KEYCODE_F12 = KEY_F12;
    const int32_t KeyEvent::KEYCODE_RO = KEY_RO;
    const int32_t KeyEvent::KEYCODE_KATAKANA = KEY_KATAKANA;
    const int32_t KeyEvent::KEYCODE_HIRAGANA = KEY_HIRAGANA;
    const int32_t KeyEvent::KEYCODE_HENKAN = KEY_HENKAN;
    const int32_t KeyEvent::KEYCODE_KATAKANAHIRAGANA = KEY_KATAKANAHIRAGANA;
    const int32_t KeyEvent::KEYCODE_MUHENKAN = KEY_MUHENKAN;
    const int32_t KeyEvent::KEYCODE_KPJPCOMMA = KEY_KPJPCOMMA;
    const int32_t KeyEvent::KEYCODE_KPENTER = KEY_KPENTER;
    const int32_t KeyEvent::KEYCODE_RIGHTCTRL = KEY_RIGHTCTRL;
    const int32_t KeyEvent::KEYCODE_KPSLASH = KEY_KPSLASH;
    const int32_t KeyEvent::KEYCODE_SYSRQ = KEY_SYSRQ;
    const int32_t KeyEvent::KEYCODE_RIGHTALT = KEY_RIGHTALT;
    const int32_t KeyEvent::KEYCODE_LINEFEED = KEY_LINEFEED;
    const int32_t KeyEvent::KEYCODE_HOME = KEY_HOME;
    const int32_t KeyEvent::KEYCODE_UP = KEY_UP;
    const int32_t KeyEvent::KEYCODE_PAGEUP = KEY_PAGEUP;
    const int32_t KeyEvent::KEYCODE_LEFT = KEY_LEFT;
    const int32_t KeyEvent::KEYCODE_RIGHT = KEY_RIGHT;
    const int32_t KeyEvent::KEYCODE_END = KEY_END;
    const int32_t KeyEvent::KEYCODE_DOWN = KEY_DOWN;
    const int32_t KeyEvent::KEYCODE_PAGEDOWN = KEY_PAGEDOWN;
    const int32_t KeyEvent::KEYCODE_INSERT = KEY_INSERT;
    const int32_t KeyEvent::KEYCODE_DELETE = KEY_DELETE;
    const int32_t KeyEvent::KEYCODE_MACRO = KEY_MACRO;
    //const int32_t KeyEvent::KEYCODE_MUTE = KEY_MUTE;
    const int32_t KeyEvent::KEYCODE_VOLUMEDOWN = KEY_VOLUMEDOWN;
    const int32_t KeyEvent::KEYCODE_VOLUMEUP = KEY_VOLUMEUP;
    const int32_t KeyEvent::KEYCODE_POWER = KEY_POWER;
    const int32_t KeyEvent::KEYCODE_KPEQUAL = KEY_KPEQUAL;
    const int32_t KeyEvent::KEYCODE_KPPLUSMINUS = KEY_KPPLUSMINUS;
    const int32_t KeyEvent::KEYCODE_PAUSE = KEY_PAUSE;
    const int32_t KeyEvent::KEYCODE_SCALE = KEY_SCALE;

    const int32_t KeyEvent::KEYCODE_KPCOMMA = KEY_KPCOMMA;
    //const int32_t KeyEvent::KEYCODE_HANGEUL = KEY_HANGEUL;
    //const int32_t KeyEvent::KEYCODE_HANGUEL = KEY_HANGUEL;
    const int32_t KeyEvent::KEYCODE_HANJA = KEY_HANJA;
    const int32_t KeyEvent::KEYCODE_YEN = KEY_YEN;
    const int32_t KeyEvent::KEYCODE_LEFTMETA = KEY_LEFTMETA;
    const int32_t KeyEvent::KEYCODE_RIGHTMETA = KEY_RIGHTMETA;
    const int32_t KeyEvent::KEYCODE_COMPOSE = KEY_COMPOSE;
    const int32_t KeyEvent::KEYCODE_AGAIN = KEY_AGAIN;
    const int32_t KeyEvent::KEYCODE_PROPS = KEY_PROPS;
    const int32_t KeyEvent::KEYCODE_UNDO = KEY_UNDO;
    const int32_t KeyEvent::KEYCODE_FRONT = KEY_FRONT;
    const int32_t KeyEvent::KEYCODE_COPY = KEY_COPY;
    const int32_t KeyEvent::KEYCODE_OPEN = KEY_OPEN;
    const int32_t KeyEvent::KEYCODE_PASTE = KEY_PASTE;
    const int32_t KeyEvent::KEYCODE_FIND = KEY_FIND;
    const int32_t KeyEvent::KEYCODE_CUT = KEY_CUT;
    const int32_t KeyEvent::KEYCODE_HELP = KEY_HELP;
    const int32_t KeyEvent::KEYCODE_MENU = KEY_MENU;
    const int32_t KeyEvent::KEYCODE_CALC = KEY_CALC;
    const int32_t KeyEvent::KEYCODE_SETUP = KEY_SETUP;
    const int32_t KeyEvent::KEYCODE_SLEEP = KEY_SLEEP;
    const int32_t KeyEvent::KEYCODE_WAKEUP = KEY_WAKEUP;
    const int32_t KeyEvent::KEYCODE_FILE = KEY_FILE;
    const int32_t KeyEvent::KEYCODE_SENDFILE = KEY_SENDFILE;
    const int32_t KeyEvent::KEYCODE_DELETEFILE = KEY_DELETEFILE;
    const int32_t KeyEvent::KEYCODE_XFER = KEY_XFER;
    const int32_t KeyEvent::KEYCODE_PROG1 = KEY_PROG1;
    const int32_t KeyEvent::KEYCODE_PROG2 = KEY_PROG2;
    const int32_t KeyEvent::KEYCODE_WWW = KEY_WWW;
    const int32_t KeyEvent::KEYCODE_MSDOS = KEY_MSDOS;
    const int32_t KeyEvent::KEYCODE_COFFEE = KEY_COFFEE;
    //const int32_t KeyEvent::KEYCODE_SCREENLOCK = KEY_SCREENLOCK;
    //const int32_t KeyEvent::KEYCODE_ROTATE_DISPLAY = KEY_ROTATE_DISPLAY;
    //const int32_t KeyEvent::KEYCODE_DIRECTION = KEY_DIRECTION;
    //const int32_t KeyEvent::KEYCODE_CYCLEWINDOWS = KEY_CYCLEWINDOWS;
    const int32_t KeyEvent::KEYCODE_MAIL = KEY_MAIL;
    const int32_t KeyEvent::KEYCODE_BOOKMARKS = KEY_BOOKMARKS;
    const int32_t KeyEvent::KEYCODE_COMPUTER = KEY_COMPUTER;
    const int32_t KeyEvent::KEYCODE_BACK = KEY_BACK;
    const int32_t KeyEvent::KEYCODE_FORWARD = KEY_FORWARD;
    const int32_t KeyEvent::KEYCODE_CLOSECD = KEY_CLOSECD;
    const int32_t KeyEvent::KEYCODE_EJECTCD = KEY_EJECTCD;
    const int32_t KeyEvent::KEYCODE_EJECTCLOSECD = KEY_EJECTCLOSECD;
    const int32_t KeyEvent::KEYCODE_NEXTSONG = KEY_NEXTSONG;
    const int32_t KeyEvent::KEYCODE_PLAYPAUSE = KEY_PLAYPAUSE;
    const int32_t KeyEvent::KEYCODE_PREVIOUSSONG = KEY_PREVIOUSSONG;
    const int32_t KeyEvent::KEYCODE_STOPCD = KEY_STOPCD;
    const int32_t KeyEvent::KEYCODE_RECORD = KEY_RECORD;
    const int32_t KeyEvent::KEYCODE_REWIND = KEY_REWIND;
    const int32_t KeyEvent::KEYCODE_PHONE = KEY_PHONE;
    const int32_t KeyEvent::KEYCODE_ISO = KEY_ISO;
    const int32_t KeyEvent::KEYCODE_CONFIG = KEY_CONFIG;
    const int32_t KeyEvent::KEYCODE_HOMEPAGE = KEY_HOMEPAGE;
    const int32_t KeyEvent::KEYCODE_REFRESH = KEY_REFRESH;
    const int32_t KeyEvent::KEYCODE_EXIT = KEY_EXIT;
    const int32_t KeyEvent::KEYCODE_MOVE = KEY_MOVE;
    const int32_t KeyEvent::KEYCODE_EDIT = KEY_EDIT;
    const int32_t KeyEvent::KEYCODE_SCROLLUP = KEY_SCROLLUP;
    const int32_t KeyEvent::KEYCODE_SCROLLDOWN = KEY_SCROLLDOWN;
    const int32_t KeyEvent::KEYCODE_KPLEFTPAREN = KEY_KPLEFTPAREN;
    const int32_t KeyEvent::KEYCODE_KPRIGHTPAREN = KEY_KPRIGHTPAREN;
    const int32_t KeyEvent::KEYCODE_NEW = KEY_NEW;
    const int32_t KeyEvent::KEYCODE_REDO = KEY_REDO;

    const int32_t KeyEvent::KEYCODE_F13 = KEY_F13;
    const int32_t KeyEvent::KEYCODE_F14 = KEY_F14;
    const int32_t KeyEvent::KEYCODE_F15 = KEY_F15;
    const int32_t KeyEvent::KEYCODE_F16 = KEY_F16;
    const int32_t KeyEvent::KEYCODE_F17 = KEY_F17;
    const int32_t KeyEvent::KEYCODE_F18 = KEY_F18;
    const int32_t KeyEvent::KEYCODE_F19 = KEY_F19;
    const int32_t KeyEvent::KEYCODE_F20 = KEY_F20;
    const int32_t KeyEvent::KEYCODE_F21 = KEY_F21;
    const int32_t KeyEvent::KEYCODE_F22 = KEY_F22;
    const int32_t KeyEvent::KEYCODE_F23 = KEY_F23;
    const int32_t KeyEvent::KEYCODE_F24 = KEY_F24;

    const int32_t KeyEvent::KEYCODE_PLAYCD = KEY_PLAYCD;
    const int32_t KeyEvent::KEYCODE_PAUSECD = KEY_PAUSECD;
    const int32_t KeyEvent::KEYCODE_PROG3 = KEY_PROG3;
    const int32_t KeyEvent::KEYCODE_PROG4 = KEY_PROG4;
    const int32_t KeyEvent::KEYCODE_DASHBOARD = KEY_DASHBOARD;
    const int32_t KeyEvent::KEYCODE_SUSPEND = KEY_SUSPEND;
    const int32_t KeyEvent::KEYCODE_CLOSE = KEY_CLOSE;
    const int32_t KeyEvent::KEYCODE_PLAY = KEY_PLAY;
    const int32_t KeyEvent::KEYCODE_FASTFORWARD = KEY_FASTFORWARD;
    const int32_t KeyEvent::KEYCODE_BASSBOOST = KEY_BASSBOOST;
    const int32_t KeyEvent::KEYCODE_PRINT = KEY_PRINT;
    const int32_t KeyEvent::KEYCODE_HP = KEY_HP;
    const int32_t KeyEvent::KEYCODE_CAMERA = KEY_CAMERA;
    const int32_t KeyEvent::KEYCODE_SOUND = KEY_SOUND;
    const int32_t KeyEvent::KEYCODE_QUESTION = KEY_QUESTION;
    const int32_t KeyEvent::KEYCODE_EMAIL = KEY_EMAIL;
    const int32_t KeyEvent::KEYCODE_CHAT = KEY_CHAT;
    const int32_t KeyEvent::KEYCODE_SEARCH = KEY_SEARCH;
    const int32_t KeyEvent::KEYCODE_CONNECT = KEY_CONNECT;
    const int32_t KeyEvent::KEYCODE_FINANCE = KEY_FINANCE;
    const int32_t KeyEvent::KEYCODE_SPORT = KEY_SPORT;
    const int32_t KeyEvent::KEYCODE_SHOP = KEY_SHOP;
    const int32_t KeyEvent::KEYCODE_ALTERASE = KEY_ALTERASE;
    const int32_t KeyEvent::KEYCODE_CANCEL = KEY_CANCEL;
    const int32_t KeyEvent::KEYCODE_BRIGHTNESSDOWN = KEY_BRIGHTNESSDOWN;
    const int32_t KeyEvent::KEYCODE_BRIGHTNESSUP = KEY_BRIGHTNESSUP;
    const int32_t KeyEvent::KEYCODE_MEDIA = KEY_MEDIA;

    const int32_t KeyEvent::KEYCODE_SWITCHVIDEOMODE = KEY_SWITCHVIDEOMODE;
    const int32_t KeyEvent::KEYCODE_KBDILLUMTOGGLE = KEY_KBDILLUMTOGGLE;
    const int32_t KeyEvent::KEYCODE_KBDILLUMDOWN = KEY_KBDILLUMDOWN;
    const int32_t KeyEvent::KEYCODE_KBDILLUMUP = KEY_KBDILLUMUP;

    const int32_t KeyEvent::KEYCODE_SEND = KEY_SEND;
    const int32_t KeyEvent::KEYCODE_REPLY = KEY_REPLY;
    const int32_t KeyEvent::KEYCODE_FORWARDMAIL = KEY_FORWARDMAIL;
    const int32_t KeyEvent::KEYCODE_SAVE = KEY_SAVE;
    const int32_t KeyEvent::KEYCODE_DOCUMENTS = KEY_DOCUMENTS;

    const int32_t KeyEvent::KEYCODE_BATTERY = KEY_BATTERY;

    const int32_t KeyEvent::KEYCODE_BLUETOOTH = KEY_BLUETOOTH;
    const int32_t KeyEvent::KEYCODE_WLAN = KEY_WLAN;
    const int32_t KeyEvent::KEYCODE_UWB = KEY_UWB;

    const int32_t KeyEvent::KEYCODE_UNKNOWN = KEY_UNKNOWN;

    const int32_t KeyEvent::KEYCODE_VIDEO_NEXT = KEY_VIDEO_NEXT;
    const int32_t KeyEvent::KEYCODE_VIDEO_PREV = KEY_VIDEO_PREV;
    const int32_t KeyEvent::KEYCODE_BRIGHTNESS_CYCLE = KEY_BRIGHTNESS_CYCLE;
    //const int32_t KeyEvent::KEYCODE_BRIGHTNESS_AUTO = KEY_BRIGHTNESS_AUTO;

    //const int32_t KeyEvent::KEYCODE_BRIGHTNESS_ZERO = KEY_BRIGHTNESS_ZERO;
    const int32_t KeyEvent::KEYCODE_DISPLAY_OFF = KEY_DISPLAY_OFF;

    //const int32_t KeyEvent::KEYCODE_WWAN = KEY_WWAN;
    //const int32_t KeyEvent::KEYCODE_WIMAX = KEY_WIMAX;
    const int32_t KeyEvent::KEYCODE_RFKILL = KEY_RFKILL;

    const int32_t KeyEvent::KEYCODE_MICMUTE = KEY_MICMUTE;

    //const int32_t KeyEvent::BTNCODE_MISC = BTN_MISC;
    const int32_t KeyEvent::BTNCODE_0 = BTN_0;
    const int32_t KeyEvent::BTNCODE_1 = BTN_1;
    const int32_t KeyEvent::BTNCODE_2 = BTN_2;
    const int32_t KeyEvent::BTNCODE_3 = BTN_3;
    const int32_t KeyEvent::BTNCODE_4 = BTN_4;
    const int32_t KeyEvent::BTNCODE_5 = BTN_5;
    const int32_t KeyEvent::BTNCODE_6 = BTN_6;
    const int32_t KeyEvent::BTNCODE_7 = BTN_7;
    const int32_t KeyEvent::BTNCODE_8 = BTN_8;
    const int32_t KeyEvent::BTNCODE_9 = BTN_9;

    //const int32_t KeyEvent::BTNCODE_MOUSE = BTN_MOUSE;
    const int32_t KeyEvent::BTNCODE_LEFT = BTN_LEFT;
    const int32_t KeyEvent::BTNCODE_RIGHT = BTN_RIGHT;
    const int32_t KeyEvent::BTNCODE_MIDDLE = BTN_MIDDLE;
    const int32_t KeyEvent::BTNCODE_SIDE = BTN_SIDE;
    const int32_t KeyEvent::BTNCODE_EXTRA = BTN_EXTRA;
    const int32_t KeyEvent::BTNCODE_FORWARD = BTN_FORWARD;
    const int32_t KeyEvent::BTNCODE_BACK = BTN_BACK;
    const int32_t KeyEvent::BTNCODE_TASK = BTN_TASK;

    //const int32_t KeyEvent::BTNCODE_JOYSTICK = BTN_JOYSTICK;
    const int32_t KeyEvent::BTNCODE_TRIGGER = BTN_TRIGGER;
    const int32_t KeyEvent::BTNCODE_THUMB = BTN_THUMB;
    const int32_t KeyEvent::BTNCODE_THUMB2 = BTN_THUMB2;
    const int32_t KeyEvent::BTNCODE_TOP = BTN_TOP;
    const int32_t KeyEvent::BTNCODE_TOP2 = BTN_TOP2;
    const int32_t KeyEvent::BTNCODE_PINKIE = BTN_PINKIE;
    const int32_t KeyEvent::BTNCODE_BASE = BTN_BASE;
    const int32_t KeyEvent::BTNCODE_BASE2 = BTN_BASE2;
    const int32_t KeyEvent::BTNCODE_BASE3 = BTN_BASE3;
    const int32_t KeyEvent::BTNCODE_BASE4 = BTN_BASE4;
    const int32_t KeyEvent::BTNCODE_BASE5 = BTN_BASE5;
    const int32_t KeyEvent::BTNCODE_BASE6 = BTN_BASE6;
    const int32_t KeyEvent::BTNCODE_DEAD = BTN_DEAD;

    const int32_t KeyEvent::BTNCODE_GAMEPAD = BTN_GAMEPAD;
    //const int32_t KeyEvent::BTNCODE_SOUTH = BTN_SOUTH;
    //const int32_t KeyEvent::BTNCODE_A = BTN_A;
    const int32_t KeyEvent::BTNCODE_EAST = BTN_EAST;
    //const int32_t KeyEvent::BTNCODE_B = BTN_B;
    const int32_t KeyEvent::BTNCODE_C = BTN_C;
    //const int32_t KeyEvent::BTNCODE_NORTH = BTN_NORTH;
    //const int32_t KeyEvent::BTNCODE_X = BTN_X;
    //const int32_t KeyEvent::BTNCODE_WEST = BTN_WEST;
    //const int32_t KeyEvent::BTNCODE_Y = BTN_Y;
    const int32_t KeyEvent::BTNCODE_Z = BTN_Z;
    const int32_t KeyEvent::BTNCODE_TL = BTN_TL;
    const int32_t KeyEvent::BTNCODE_TR = BTN_TR;
    const int32_t KeyEvent::BTNCODE_TL2 = BTN_TL2;
    const int32_t KeyEvent::BTNCODE_TR2 = BTN_TR2;
    const int32_t KeyEvent::BTNCODE_SELECT = BTN_SELECT;
    const int32_t KeyEvent::BTNCODE_START = BTN_START;
    const int32_t KeyEvent::BTNCODE_MODE = BTN_MODE;
    const int32_t KeyEvent::BTNCODE_THUMBL = BTN_THUMBL;
    const int32_t KeyEvent::BTNCODE_THUMBR = BTN_THUMBR;

    const int32_t KeyEvent::BTNCODE_DIGI = BTN_DIGI;
    //const int32_t KeyEvent::BTNCODE_TOOL_PEN = BTN_TOOL_PEN;
    const int32_t KeyEvent::BTNCODE_TOOL_RUBBER = BTN_TOOL_RUBBER;
    const int32_t KeyEvent::BTNCODE_TOOL_BRUSH = BTN_TOOL_BRUSH;
    const int32_t KeyEvent::BTNCODE_TOOL_PENCIL = BTN_TOOL_PENCIL;
    const int32_t KeyEvent::BTNCODE_TOOL_AIRBRUSH = BTN_TOOL_AIRBRUSH;
    const int32_t KeyEvent::BTNCODE_TOOL_FINGER = BTN_TOOL_FINGER;
    const int32_t KeyEvent::BTNCODE_TOOL_MOUSE = BTN_TOOL_MOUSE;
    const int32_t KeyEvent::BTNCODE_TOOL_LENS = BTN_TOOL_LENS;
    const int32_t KeyEvent::BTNCODE_TOOL_QUINTTAP = BTN_TOOL_QUINTTAP;
    const int32_t KeyEvent::BTNCODE_STYLUS3 = BTN_STYLUS3;
    const int32_t KeyEvent::BTNCODE_TOUCH = BTN_TOUCH;
    const int32_t KeyEvent::BTNCODE_STYLUS = BTN_STYLUS;
    const int32_t KeyEvent::BTNCODE_STYLUS2 = BTN_STYLUS2;
    const int32_t KeyEvent::BTNCODE_TOOL_DOUBLETAP = BTN_TOOL_DOUBLETAP;
    const int32_t KeyEvent::BTNCODE_TOOL_TRIPLETAP = BTN_TOOL_TRIPLETAP;
    const int32_t KeyEvent::BTNCODE_TOOL_QUADTAP = BTN_TOOL_QUADTAP;

    //const int32_t KeyEvent::BTNCODE_WHEEL = BTN_WHEEL;
    const int32_t KeyEvent::BTNCODE_GEAR_DOWN = BTN_GEAR_DOWN;
    const int32_t KeyEvent::BTNCODE_GEAR_UP = BTN_GEAR_UP;

    const int32_t KeyEvent::KEYCODE_OK = KEY_OK;
    const int32_t KeyEvent::KEYCODE_SELECT = KEY_SELECT;
    const int32_t KeyEvent::KEYCODE_GOTO = KEY_GOTO;
    const int32_t KeyEvent::KEYCODE_CLEAR = KEY_CLEAR;
    const int32_t KeyEvent::KEYCODE_POWER2 = KEY_POWER2;
    const int32_t KeyEvent::KEYCODE_OPTION = KEY_OPTION;
    const int32_t KeyEvent::KEYCODE_INFO = KEY_INFO;
    const int32_t KeyEvent::KEYCODE_TIME = KEY_TIME;
    const int32_t KeyEvent::KEYCODE_VENDOR = KEY_VENDOR;
    const int32_t KeyEvent::KEYCODE_ARCHIVE = KEY_ARCHIVE;
    const int32_t KeyEvent::KEYCODE_PROGRAM = KEY_PROGRAM;
    const int32_t KeyEvent::KEYCODE_CHANNEL = KEY_CHANNEL;
    const int32_t KeyEvent::KEYCODE_FAVORITES = KEY_FAVORITES;
    const int32_t KeyEvent::KEYCODE_EPG = KEY_EPG;
    const int32_t KeyEvent::KEYCODE_PVR = KEY_PVR;
    const int32_t KeyEvent::KEYCODE_MHP = KEY_MHP;
    const int32_t KeyEvent::KEYCODE_LANGUAGE = KEY_LANGUAGE;
    const int32_t KeyEvent::KEYCODE_TITLE = KEY_TITLE;
    const int32_t KeyEvent::KEYCODE_SUBTITLE = KEY_SUBTITLE;
    const int32_t KeyEvent::KEYCODE_ANGLE = KEY_ANGLE;
    const int32_t KeyEvent::KEYCODE_FULL_SCREEN = KEY_FULL_SCREEN;
    //const int32_t KeyEvent::KEYCODE_ZOOM = KEY_ZOOM;
    const int32_t KeyEvent::KEYCODE_MODE = KEY_MODE;
    const int32_t KeyEvent::KEYCODE_KEYBOARD = KEY_KEYBOARD;
    const int32_t KeyEvent::KEYCODE_ASPECT_RATIO = KEY_ASPECT_RATIO;
    //const int32_t KeyEvent::KEYCODE_SCREEN = KEY_SCREEN;
    const int32_t KeyEvent::KEYCODE_PC = KEY_PC;
    const int32_t KeyEvent::KEYCODE_TV = KEY_TV;
    const int32_t KeyEvent::KEYCODE_TV2 = KEY_TV2;
    const int32_t KeyEvent::KEYCODE_VCR = KEY_VCR;
    const int32_t KeyEvent::KEYCODE_VCR2 = KEY_VCR2;
    const int32_t KeyEvent::KEYCODE_SAT = KEY_SAT;
    const int32_t KeyEvent::KEYCODE_SAT2 = KEY_SAT2;
    const int32_t KeyEvent::KEYCODE_CD = KEY_CD;
    const int32_t KeyEvent::KEYCODE_TAPE = KEY_TAPE;
    const int32_t KeyEvent::KEYCODE_RADIO = KEY_RADIO;
    const int32_t KeyEvent::KEYCODE_TUNER = KEY_TUNER;
    const int32_t KeyEvent::KEYCODE_PLAYER = KEY_PLAYER;
    const int32_t KeyEvent::KEYCODE_TEXT = KEY_TEXT;
    const int32_t KeyEvent::KEYCODE_DVD = KEY_DVD;
    const int32_t KeyEvent::KEYCODE_AUX = KEY_AUX;
    const int32_t KeyEvent::KEYCODE_MP3 = KEY_MP3;
    const int32_t KeyEvent::KEYCODE_AUDIO = KEY_AUDIO;
    const int32_t KeyEvent::KEYCODE_VIDEO = KEY_VIDEO;
    const int32_t KeyEvent::KEYCODE_DIRECTORY = KEY_DIRECTORY;
    const int32_t KeyEvent::KEYCODE_LIST = KEY_LIST;
    const int32_t KeyEvent::KEYCODE_MEMO = KEY_MEMO;
    const int32_t KeyEvent::KEYCODE_CALENDAR = KEY_CALENDAR;
    const int32_t KeyEvent::KEYCODE_RED = KEY_RED;
    const int32_t KeyEvent::KEYCODE_GREEN = KEY_GREEN;
    const int32_t KeyEvent::KEYCODE_YELLOW = KEY_YELLOW;
    const int32_t KeyEvent::KEYCODE_BLUE = KEY_BLUE;
    const int32_t KeyEvent::KEYCODE_CHANNELUP = KEY_CHANNELUP;
    const int32_t KeyEvent::KEYCODE_CHANNELDOWN = KEY_CHANNELDOWN;
    const int32_t KeyEvent::KEYCODE_FIRST = KEY_FIRST;
    const int32_t KeyEvent::KEYCODE_LAST = KEY_LAST;
    const int32_t KeyEvent::KEYCODE_AB = KEY_AB;
    const int32_t KeyEvent::KEYCODE_NEXT = KEY_NEXT;
    const int32_t KeyEvent::KEYCODE_RESTART = KEY_RESTART;
    const int32_t KeyEvent::KEYCODE_SLOW = KEY_SLOW;
    const int32_t KeyEvent::KEYCODE_SHUFFLE = KEY_SHUFFLE;
    const int32_t KeyEvent::KEYCODE_BREAK = KEY_BREAK;
    const int32_t KeyEvent::KEYCODE_PREVIOUS = KEY_PREVIOUS;
    const int32_t KeyEvent::KEYCODE_DIGITS = KEY_DIGITS;
    const int32_t KeyEvent::KEYCODE_TEEN = KEY_TEEN;
    const int32_t KeyEvent::KEYCODE_TWEN = KEY_TWEN;
    const int32_t KeyEvent::KEYCODE_VIDEOPHONE = KEY_VIDEOPHONE;
    const int32_t KeyEvent::KEYCODE_GAMES = KEY_GAMES;
    const int32_t KeyEvent::KEYCODE_ZOOMIN = KEY_ZOOMIN;
    const int32_t KeyEvent::KEYCODE_ZOOMOUT = KEY_ZOOMOUT;
    const int32_t KeyEvent::KEYCODE_ZOOMRESET = KEY_ZOOMRESET;
    const int32_t KeyEvent::KEYCODE_WORDPROCESSOR = KEY_WORDPROCESSOR;
    const int32_t KeyEvent::KEYCODE_EDITOR = KEY_EDITOR;
    const int32_t KeyEvent::KEYCODE_SPREADSHEET = KEY_SPREADSHEET;
    const int32_t KeyEvent::KEYCODE_GRAPHICSEDITOR = KEY_GRAPHICSEDITOR;
    const int32_t KeyEvent::KEYCODE_PRESENTATION = KEY_PRESENTATION;
    const int32_t KeyEvent::KEYCODE_DATABASE = KEY_DATABASE;
    const int32_t KeyEvent::KEYCODE_NEWS = KEY_NEWS;
    const int32_t KeyEvent::KEYCODE_VOICEMAIL = KEY_VOICEMAIL;
    const int32_t KeyEvent::KEYCODE_ADDRESSBOOK = KEY_ADDRESSBOOK;
    const int32_t KeyEvent::KEYCODE_MESSENGER = KEY_MESSENGER;
    const int32_t KeyEvent::KEYCODE_DISPLAYTOGGLE = KEY_DISPLAYTOGGLE;
    //const int32_t KeyEvent::KEYCODE_BRIGHTNESS_TOGGLE = KEY_BRIGHTNESS_TOGGLE;
    const int32_t KeyEvent::KEYCODE_SPELLCHECK = KEY_SPELLCHECK;
    const int32_t KeyEvent::KEYCODE_LOGOFF = KEY_LOGOFF;

    const int32_t KeyEvent::KEYCODE_DOLLAR = KEY_DOLLAR;
    const int32_t KeyEvent::KEYCODE_EURO = KEY_EURO;

    const int32_t KeyEvent::KEYCODE_FRAMEBACK = KEY_FRAMEBACK;
    const int32_t KeyEvent::KEYCODE_FRAMEFORWARD = KEY_FRAMEFORWARD;
    const int32_t KeyEvent::KEYCODE_CONTEXT_MENU = KEY_CONTEXT_MENU;
    const int32_t KeyEvent::KEYCODE_MEDIA_REPEAT = KEY_MEDIA_REPEAT;
    const int32_t KeyEvent::KEYCODE_10CHANNELSUP = KEY_10CHANNELSUP;
    const int32_t KeyEvent::KEYCODE_10CHANNELSDOWN = KEY_10CHANNELSDOWN;
    const int32_t KeyEvent::KEYCODE_IMAGES = KEY_IMAGES;
    //const int32_t KeyEvent::KEYCODE_NOTIFICATION_CENTER = KEY_NOTIFICATION_CENTER;
    //const int32_t KeyEvent::KEYCODE_PICKUP_PHONE = KEY_PICKUP_PHONE;
    //const int32_t KeyEvent::KEYCODE_HANGUP_PHONE = KEY_HANGUP_PHONE;

    const int32_t KeyEvent::KEYCODE_DEL_EOL = KEY_DEL_EOL;
    const int32_t KeyEvent::KEYCODE_DEL_EOS = KEY_DEL_EOS;
    const int32_t KeyEvent::KEYCODE_INS_LINE = KEY_INS_LINE;
    const int32_t KeyEvent::KEYCODE_DEL_LINE = KEY_DEL_LINE;

    const int32_t KeyEvent::KEYCODE_FN = KEY_FN;
    const int32_t KeyEvent::KEYCODE_FN_ESC = KEY_FN_ESC;
    const int32_t KeyEvent::KEYCODE_FN_F1 = KEY_FN_F1;
    const int32_t KeyEvent::KEYCODE_FN_F2 = KEY_FN_F2;
    const int32_t KeyEvent::KEYCODE_FN_F3 = KEY_FN_F3;
    const int32_t KeyEvent::KEYCODE_FN_F4 = KEY_FN_F4;
    const int32_t KeyEvent::KEYCODE_FN_F5 = KEY_FN_F5;
    const int32_t KeyEvent::KEYCODE_FN_F6 = KEY_FN_F6;
    const int32_t KeyEvent::KEYCODE_FN_F7 = KEY_FN_F7;
    const int32_t KeyEvent::KEYCODE_FN_F8 = KEY_FN_F8;
    const int32_t KeyEvent::KEYCODE_FN_F9 = KEY_FN_F9;
    const int32_t KeyEvent::KEYCODE_FN_F10 = KEY_FN_F10;
    const int32_t KeyEvent::KEYCODE_FN_F11 = KEY_FN_F11;
    const int32_t KeyEvent::KEYCODE_FN_F12 = KEY_FN_F12;
    const int32_t KeyEvent::KEYCODE_FN_1 = KEY_FN_1;
    const int32_t KeyEvent::KEYCODE_FN_2 = KEY_FN_2;
    const int32_t KeyEvent::KEYCODE_FN_D = KEY_FN_D;
    const int32_t KeyEvent::KEYCODE_FN_E = KEY_FN_E;
    const int32_t KeyEvent::KEYCODE_FN_F = KEY_FN_F;
    const int32_t KeyEvent::KEYCODE_FN_S = KEY_FN_S;
    const int32_t KeyEvent::KEYCODE_FN_B = KEY_FN_B;
    //const int32_t KeyEvent::KEYCODE_FN_RIGHT_SHIFT = KEY_FN_RIGHT_SHIFT;

    const int32_t KeyEvent::KEYCODE_BRL_DOT1 = KEY_BRL_DOT1;
    const int32_t KeyEvent::KEYCODE_BRL_DOT2 = KEY_BRL_DOT2;
    const int32_t KeyEvent::KEYCODE_BRL_DOT3 = KEY_BRL_DOT3;
    const int32_t KeyEvent::KEYCODE_BRL_DOT4 = KEY_BRL_DOT4;
    const int32_t KeyEvent::KEYCODE_BRL_DOT5 = KEY_BRL_DOT5;
    const int32_t KeyEvent::KEYCODE_BRL_DOT6 = KEY_BRL_DOT6;
    const int32_t KeyEvent::KEYCODE_BRL_DOT7 = KEY_BRL_DOT7;
    const int32_t KeyEvent::KEYCODE_BRL_DOT8 = KEY_BRL_DOT8;
    const int32_t KeyEvent::KEYCODE_BRL_DOT9 = KEY_BRL_DOT9;
    const int32_t KeyEvent::KEYCODE_BRL_DOT10 = KEY_BRL_DOT10;

    const int32_t KeyEvent::KEYCODE_NUMERIC_0 = KEY_NUMERIC_0;
    const int32_t KeyEvent::KEYCODE_NUMERIC_1 = KEY_NUMERIC_1;
    const int32_t KeyEvent::KEYCODE_NUMERIC_2 = KEY_NUMERIC_2;
    const int32_t KeyEvent::KEYCODE_NUMERIC_3 = KEY_NUMERIC_3;
    const int32_t KeyEvent::KEYCODE_NUMERIC_4 = KEY_NUMERIC_4;
    const int32_t KeyEvent::KEYCODE_NUMERIC_5 = KEY_NUMERIC_5;
    const int32_t KeyEvent::KEYCODE_NUMERIC_6 = KEY_NUMERIC_6;
    const int32_t KeyEvent::KEYCODE_NUMERIC_7 = KEY_NUMERIC_7;
    const int32_t KeyEvent::KEYCODE_NUMERIC_8 = KEY_NUMERIC_8;
    const int32_t KeyEvent::KEYCODE_NUMERIC_9 = KEY_NUMERIC_9;
    const int32_t KeyEvent::KEYCODE_NUMERIC_STAR = KEY_NUMERIC_STAR;
    const int32_t KeyEvent::KEYCODE_NUMERIC_POUND = KEY_NUMERIC_POUND;
    const int32_t KeyEvent::KEYCODE_NUMERIC_A = KEY_NUMERIC_A;
    const int32_t KeyEvent::KEYCODE_NUMERIC_B = KEY_NUMERIC_B;
    const int32_t KeyEvent::KEYCODE_NUMERIC_C = KEY_NUMERIC_C;
    const int32_t KeyEvent::KEYCODE_NUMERIC_D = KEY_NUMERIC_D;

    const int32_t KeyEvent::KEYCODE_CAMERA_FOCUS = KEY_CAMERA_FOCUS;
    const int32_t KeyEvent::KEYCODE_WPS_BUTTON = KEY_WPS_BUTTON;

    const int32_t KeyEvent::KEYCODE_TOUCHPAD_TOGGLE = KEY_TOUCHPAD_TOGGLE;
    const int32_t KeyEvent::KEYCODE_TOUCHPAD_ON = KEY_TOUCHPAD_ON;
    const int32_t KeyEvent::KEYCODE_TOUCHPAD_OFF = KEY_TOUCHPAD_OFF;

    const int32_t KeyEvent::KEYCODE_CAMERA_ZOOMIN = KEY_CAMERA_ZOOMIN;
    const int32_t KeyEvent::KEYCODE_CAMERA_ZOOMOUT = KEY_CAMERA_ZOOMOUT;
    const int32_t KeyEvent::KEYCODE_CAMERA_UP = KEY_CAMERA_UP;
    const int32_t KeyEvent::KEYCODE_CAMERA_DOWN = KEY_CAMERA_DOWN;
    const int32_t KeyEvent::KEYCODE_CAMERA_LEFT = KEY_CAMERA_LEFT;
    const int32_t KeyEvent::KEYCODE_CAMERA_RIGHT = KEY_CAMERA_RIGHT;

    const int32_t KeyEvent::KEYCODE_ATTENDANT_ON = KEY_ATTENDANT_ON;
    const int32_t KeyEvent::KEYCODE_ATTENDANT_OFF = KEY_ATTENDANT_OFF;
    const int32_t KeyEvent::KEYCODE_ATTENDANT_TOGGLE = KEY_ATTENDANT_TOGGLE;
    const int32_t KeyEvent::KEYCODE_LIGHTS_TOGGLE = KEY_LIGHTS_TOGGLE;

    const int32_t KeyEvent::BTNCODE_DPAD_UP = BTN_DPAD_UP;
    const int32_t KeyEvent::BTNCODE_DPAD_DOWN = BTN_DPAD_DOWN;
    const int32_t KeyEvent::BTNCODE_DPAD_LEFT = BTN_DPAD_LEFT;
    const int32_t KeyEvent::BTNCODE_DPAD_RIGHT = BTN_DPAD_RIGHT;

    const int32_t KeyEvent::KEYCODE_ALS_TOGGLE = KEY_ALS_TOGGLE;
    const int32_t KeyEvent::KEYCODE_ROTATE_LOCK_TOGGLE = KEY_ROTATE_LOCK_TOGGLE;

    const int32_t KeyEvent::KEYCODE_BUTTONCONFIG = KEY_BUTTONCONFIG;
    const int32_t KeyEvent::KEYCODE_TASKMANAGER = KEY_TASKMANAGER;
    const int32_t KeyEvent::KEYCODE_JOURNAL = KEY_JOURNAL;
    const int32_t KeyEvent::KEYCODE_CONTROLPANEL = KEY_CONTROLPANEL;
    const int32_t KeyEvent::KEYCODE_APPSELECT = KEY_APPSELECT;
    const int32_t KeyEvent::KEYCODE_SCREENSAVER = KEY_SCREENSAVER;
    const int32_t KeyEvent::KEYCODE_VOICECOMMAND = KEY_VOICECOMMAND;
    const int32_t KeyEvent::KEYCODE_ASSISTANT = KEY_ASSISTANT;
    const int32_t KeyEvent::KEYCODE_KBD_LAYOUT_NEXT = KEY_KBD_LAYOUT_NEXT;
    // const int32_t KeyEvent::KEYCODE_EMOJI_PICKER = KEY_EMOJI_PICKER;

    const int32_t KeyEvent::KEYCODE_BRIGHTNESS_MIN = KEY_BRIGHTNESS_MIN;
    const int32_t KeyEvent::KEYCODE_BRIGHTNESS_MAX = KEY_BRIGHTNESS_MAX;

    const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_PREV = KEY_KBDINPUTASSIST_PREV;
    const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_NEXT = KEY_KBDINPUTASSIST_NEXT;
    const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_PREVGROUP = KEY_KBDINPUTASSIST_PREVGROUP;
    const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_NEXTGROUP = KEY_KBDINPUTASSIST_NEXTGROUP;
    const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_ACCEPT = KEY_KBDINPUTASSIST_ACCEPT;
    const int32_t KeyEvent::KEYCODE_KBDINPUTASSIST_CANCEL = KEY_KBDINPUTASSIST_CANCEL;

    const int32_t KeyEvent::KEYCODE_RIGHT_UP = KEY_RIGHT_UP;
    const int32_t KeyEvent::KEYCODE_RIGHT_DOWN = KEY_RIGHT_DOWN;
    const int32_t KeyEvent::KEYCODE_LEFT_UP = KEY_LEFT_UP;
    const int32_t KeyEvent::KEYCODE_LEFT_DOWN = KEY_LEFT_DOWN;

    const int32_t KeyEvent::KEYCODE_ROOT_MENU = KEY_ROOT_MENU;
    const int32_t KeyEvent::KEYCODE_MEDIA_TOP_MENU = KEY_MEDIA_TOP_MENU;
    const int32_t KeyEvent::KEYCODE_NUMERIC_11 = KEY_NUMERIC_11;
    const int32_t KeyEvent::KEYCODE_NUMERIC_12 = KEY_NUMERIC_12;
    const int32_t KeyEvent::KEYCODE_AUDIO_DESC = KEY_AUDIO_DESC;
    const int32_t KeyEvent::KEYCODE_3D_MODE = KEY_3D_MODE;
    const int32_t KeyEvent::KEYCODE_NEXT_FAVORITE = KEY_NEXT_FAVORITE;
    const int32_t KeyEvent::KEYCODE_STOP_RECORD = KEY_STOP_RECORD;
    const int32_t KeyEvent::KEYCODE_PAUSE_RECORD = KEY_PAUSE_RECORD;
    const int32_t KeyEvent::KEYCODE_VOD = KEY_VOD;
    const int32_t KeyEvent::KEYCODE_UNMUTE = KEY_UNMUTE;
    const int32_t KeyEvent::KEYCODE_FASTREVERSE = KEY_FASTREVERSE;
    const int32_t KeyEvent::KEYCODE_SLOWREVERSE = KEY_SLOWREVERSE;
    const int32_t KeyEvent::KEYCODE_DATA = KEY_DATA;
    const int32_t KeyEvent::KEYCODE_ONSCREEN_KEYBOARD = KEY_ONSCREEN_KEYBOARD;
    //const int32_t KeyEvent::KEYCODE_PRIVACY_SCREEN_TOGGLE = KEY_PRIVACY_SCREEN_TOGGLE;
    //const int32_t KeyEvent::KEYCODE_SELECTIVE_SCREENSHOT = KEY_SELECTIVE_SCREENSHOT;
    //const int32_t KeyEvent::KEYCODE_MACRO1 = KEY_MACRO1;
    //const int32_t KeyEvent::KEYCODE_MACRO2 = KEY_MACRO2;
    //const int32_t KeyEvent::KEYCODE_MACRO3 = KEY_MACRO3;
    //const int32_t KeyEvent::KEYCODE_MACRO4 = KEY_MACRO4;
    //const int32_t KeyEvent::KEYCODE_MACRO5 = KEY_MACRO5;
    //const int32_t KeyEvent::KEYCODE_MACRO6 = KEY_MACRO6;
    //const int32_t KeyEvent::KEYCODE_MACRO7 = KEY_MACRO7;
    //const int32_t KeyEvent::KEYCODE_MACRO8 = KEY_MACRO8;
    //const int32_t KeyEvent::KEYCODE_MACRO9 = KEY_MACRO9;
    //const int32_t KeyEvent::KEYCODE_MACRO10 = KEY_MACRO10;
    //const int32_t KeyEvent::KEYCODE_MACRO11 = KEY_MACRO11;
    //const int32_t KeyEvent::KEYCODE_MACRO12 = KEY_MACRO12;
    //const int32_t KeyEvent::KEYCODE_MACRO13 = KEY_MACRO13;
    //const int32_t KeyEvent::KEYCODE_MACRO14 = KEY_MACRO14;
    //const int32_t KeyEvent::KEYCODE_MACRO15 = KEY_MACRO15;
    //const int32_t KeyEvent::KEYCODE_MACRO16 = KEY_MACRO16;
    //const int32_t KeyEvent::KEYCODE_MACRO17 = KEY_MACRO17;
    //const int32_t KeyEvent::KEYCODE_MACRO18 = KEY_MACRO18;
    //const int32_t KeyEvent::KEYCODE_MACRO19 = KEY_MACRO19;
    //const int32_t KeyEvent::KEYCODE_MACRO20 = KEY_MACRO20;
    //const int32_t KeyEvent::KEYCODE_MACRO21 = KEY_MACRO21;
    //const int32_t KeyEvent::KEYCODE_MACRO22 = KEY_MACRO22;
    //const int32_t KeyEvent::KEYCODE_MACRO23 = KEY_MACRO23;
    //const int32_t KeyEvent::KEYCODE_MACRO24 = KEY_MACRO24;
    //const int32_t KeyEvent::KEYCODE_MACRO25 = KEY_MACRO25;
    //const int32_t KeyEvent::KEYCODE_MACRO26 = KEY_MACRO26;
    //const int32_t KeyEvent::KEYCODE_MACRO27 = KEY_MACRO27;
    //const int32_t KeyEvent::KEYCODE_MACRO28 = KEY_MACRO28;
    //const int32_t KeyEvent::KEYCODE_MACRO29 = KEY_MACRO29;
    //const int32_t KeyEvent::KEYCODE_MACRO30 = KEY_MACRO30;
    //const int32_t KeyEvent::KEYCODE_MACRO_RECORD_START = KEY_MACRO_RECORD_START;
    //const int32_t KeyEvent::KEYCODE_MACRO_RECORD_STOP = KEY_MACRO_RECORD_STOP;
    //const int32_t KeyEvent::KEYCODE_MACRO_PRESET_CYCLE = KEY_MACRO_PRESET_CYCLE;
    //const int32_t KeyEvent::KEYCODE_MACRO_PRESET1 = KEY_MACRO_PRESET1;
    //const int32_t KeyEvent::KEYCODE_MACRO_PRESET2 = KEY_MACRO_PRESET2;
    //const int32_t KeyEvent::KEYCODE_MACRO_PRESET3 = KEY_MACRO_PRESET3;
    //const int32_t KeyEvent::KEYCODE_KBD_LCD_MENU1 = KEY_KBD_LCD_MENU1;
    //const int32_t KeyEvent::KEYCODE_KBD_LCD_MENU2 = KEY_KBD_LCD_MENU2;
    //const int32_t KeyEvent::KEYCODE_KBD_LCD_MENU3 = KEY_KBD_LCD_MENU3;
    //const int32_t KeyEvent::KEYCODE_KBD_LCD_MENU4 = KEY_KBD_LCD_MENU4;
    //const int32_t KeyEvent::KEYCODE_KBD_LCD_MENU5 = KEY_KBD_LCD_MENU5;
    //const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY = BTN_TRIGGER_HAPPY;
    //const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY1 = BTN_TRIGGER_HAPPY1;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY2 = BTN_TRIGGER_HAPPY2;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY3 = BTN_TRIGGER_HAPPY3;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY4 = BTN_TRIGGER_HAPPY4;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY5 = BTN_TRIGGER_HAPPY5;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY6 = BTN_TRIGGER_HAPPY6;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY7 = BTN_TRIGGER_HAPPY7;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY8 = BTN_TRIGGER_HAPPY8;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY9 = BTN_TRIGGER_HAPPY9;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY10 = BTN_TRIGGER_HAPPY10;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY11 = BTN_TRIGGER_HAPPY11;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY12 = BTN_TRIGGER_HAPPY12;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY13 = BTN_TRIGGER_HAPPY13;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY14 = BTN_TRIGGER_HAPPY14;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY15 = BTN_TRIGGER_HAPPY15;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY16 = BTN_TRIGGER_HAPPY16;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY17 = BTN_TRIGGER_HAPPY17;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY18 = BTN_TRIGGER_HAPPY18;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY19 = BTN_TRIGGER_HAPPY19;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY20 = BTN_TRIGGER_HAPPY20;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY21 = BTN_TRIGGER_HAPPY21;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY22 = BTN_TRIGGER_HAPPY22;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY23 = BTN_TRIGGER_HAPPY23;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY24 = BTN_TRIGGER_HAPPY24;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY25 = BTN_TRIGGER_HAPPY25;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY26 = BTN_TRIGGER_HAPPY26;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY27 = BTN_TRIGGER_HAPPY27;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY28 = BTN_TRIGGER_HAPPY28;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY29 = BTN_TRIGGER_HAPPY29;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY30 = BTN_TRIGGER_HAPPY30;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY31 = BTN_TRIGGER_HAPPY31;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY32 = BTN_TRIGGER_HAPPY32;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY33 = BTN_TRIGGER_HAPPY33;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY34 = BTN_TRIGGER_HAPPY34;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY35 = BTN_TRIGGER_HAPPY35;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY36 = BTN_TRIGGER_HAPPY36;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY37 = BTN_TRIGGER_HAPPY37;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY38 = BTN_TRIGGER_HAPPY38;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY39 = BTN_TRIGGER_HAPPY39;
    const int32_t KeyEvent::BTNCODE_TRIGGER_HAPPY40 = BTN_TRIGGER_HAPPY40;
    const int32_t KeyEvent::KEYCODE_MIN_INTERESTING = KEY_MIN_INTERESTING;

    const char* KeyEvent::KeyActionToString(int32_t keyAction)
    {
        switch(keyAction) {
            CASE_STR(KEY_ACTION_NONE);
            CASE_STR(KEY_ACTION_CANCEL);
            CASE_STR(KEY_ACTION_DOWN);
            CASE_STR(KEY_ACTION_UP);
            CASE_STR(KEY_ACTION_MAX);
            default:
            return "KEY_ACTION_UNKNOWN";
        }
    }
    const char* KeyEvent::KeyCodeToString(int32_t keyCode)
    {
        switch(keyCode) {
            CASE_STR(KEYCODE_RESERVED);
            CASE_STR(KEYCODE_ESC);
            CASE_STR(KEYCODE_1);
            CASE_STR(KEYCODE_2);
            CASE_STR(KEYCODE_3);
            CASE_STR(KEYCODE_4);
            CASE_STR(KEYCODE_5);
            CASE_STR(KEYCODE_6);
            CASE_STR(KEYCODE_7);
            CASE_STR(KEYCODE_8);
            CASE_STR(KEYCODE_9);
            CASE_STR(KEYCODE_0);
            CASE_STR(KEYCODE_MINUS);
            CASE_STR(KEYCODE_EQUAL);
            CASE_STR(KEYCODE_BACKSPACE);
            CASE_STR(KEYCODE_TAB);
            CASE_STR(KEYCODE_Q);
            CASE_STR(KEYCODE_W);
            CASE_STR(KEYCODE_E);
            CASE_STR(KEYCODE_R);
            CASE_STR(KEYCODE_T);
            CASE_STR(KEYCODE_Y);
            CASE_STR(KEYCODE_U);
            CASE_STR(KEYCODE_I);
            CASE_STR(KEYCODE_O);
            CASE_STR(KEYCODE_P);
            CASE_STR(KEYCODE_LEFTBRACE);
            CASE_STR(KEYCODE_RIGHTBRACE);
            CASE_STR(KEYCODE_ENTER);
            CASE_STR(KEYCODE_LEFTCTRL);
            CASE_STR(KEYCODE_A);
            CASE_STR(KEYCODE_S);
            CASE_STR(KEYCODE_D);
            CASE_STR(KEYCODE_F);
            CASE_STR(KEYCODE_G);
            CASE_STR(KEYCODE_H);
            CASE_STR(KEYCODE_J);
            CASE_STR(KEYCODE_K);
            CASE_STR(KEYCODE_L);
            CASE_STR(KEYCODE_SEMICOLON);
            CASE_STR(KEYCODE_APOSTROPHE);
            CASE_STR(KEYCODE_GRAVE);
            CASE_STR(KEYCODE_LEFTSHIFT);
            CASE_STR(KEYCODE_BACKSLASH);
            CASE_STR(KEYCODE_Z);
            CASE_STR(KEYCODE_X);
            CASE_STR(KEYCODE_C);
            CASE_STR(KEYCODE_V);
            CASE_STR(KEYCODE_B);
            CASE_STR(KEYCODE_N);
            CASE_STR(KEYCODE_M);
            CASE_STR(KEYCODE_COMMA);
            CASE_STR(KEYCODE_DOT);
            CASE_STR(KEYCODE_SLASH);
            CASE_STR(KEYCODE_RIGHTSHIFT);
            CASE_STR(KEYCODE_KPASTERISK);
            CASE_STR(KEYCODE_LEFTALT);
            CASE_STR(KEYCODE_SPACE);
            CASE_STR(KEYCODE_CAPSLOCK);
            CASE_STR(KEYCODE_F1);
            CASE_STR(KEYCODE_F2);
            CASE_STR(KEYCODE_F3);
            CASE_STR(KEYCODE_F4);
            CASE_STR(KEYCODE_F5);
            CASE_STR(KEYCODE_F6);
            CASE_STR(KEYCODE_F7);
            CASE_STR(KEYCODE_F8);
            CASE_STR(KEYCODE_F9);
            CASE_STR(KEYCODE_F10);
            CASE_STR(KEYCODE_NUMLOCK);
            CASE_STR(KEYCODE_SCROLLLOCK);
            CASE_STR(KEYCODE_KP7);
            CASE_STR(KEYCODE_KP8);
            CASE_STR(KEYCODE_KP9);
            CASE_STR(KEYCODE_KPMINUS);
            CASE_STR(KEYCODE_KP4);
            CASE_STR(KEYCODE_KP5);
            CASE_STR(KEYCODE_KP6);
            CASE_STR(KEYCODE_KPPLUS);
            CASE_STR(KEYCODE_KP1);
            CASE_STR(KEYCODE_KP2);
            CASE_STR(KEYCODE_KP3);
            CASE_STR(KEYCODE_KP0);
            CASE_STR(KEYCODE_KPDOT);
            CASE_STR(KEYCODE_102ND);
            CASE_STR(KEYCODE_F11);
            CASE_STR(KEYCODE_F12);
            CASE_STR(KEYCODE_RO);
            CASE_STR(KEYCODE_KATAKANA);
            CASE_STR(KEYCODE_HIRAGANA);
            CASE_STR(KEYCODE_HENKAN);
            CASE_STR(KEYCODE_KATAKANAHIRAGANA);
            CASE_STR(KEYCODE_MUHENKAN);
            CASE_STR(KEYCODE_KPJPCOMMA);
            CASE_STR(KEYCODE_KPENTER);
            CASE_STR(KEYCODE_RIGHTCTRL);
            CASE_STR(KEYCODE_KPSLASH);
            CASE_STR(KEYCODE_SYSRQ);
            CASE_STR(KEYCODE_RIGHTALT);
            CASE_STR(KEYCODE_LINEFEED);
            CASE_STR(KEYCODE_HOME);
            CASE_STR(KEYCODE_UP);
            CASE_STR(KEYCODE_PAGEUP);
            CASE_STR(KEYCODE_LEFT);
            CASE_STR(KEYCODE_RIGHT);
            CASE_STR(KEYCODE_END);
            CASE_STR(KEYCODE_DOWN);
            CASE_STR(KEYCODE_PAGEDOWN);
            CASE_STR(KEYCODE_INSERT);
            CASE_STR(KEYCODE_DELETE);
            CASE_STR(KEYCODE_MACRO);
            //CASE_STR(KEYCODE_MUTE);
            CASE_STR(KEYCODE_VOLUMEDOWN);
            CASE_STR(KEYCODE_VOLUMEUP);
            CASE_STR(KEYCODE_POWER);
            CASE_STR(KEYCODE_KPEQUAL);
            CASE_STR(KEYCODE_KPPLUSMINUS);
            CASE_STR(KEYCODE_PAUSE);
            CASE_STR(KEYCODE_SCALE);

            CASE_STR(KEYCODE_KPCOMMA);
            //CASE_STR(KEYCODE_HANGEUL);
            //CASE_STR(KEYCODE_HANGUEL);
            CASE_STR(KEYCODE_HANJA);
            CASE_STR(KEYCODE_YEN);
            CASE_STR(KEYCODE_LEFTMETA);
            CASE_STR(KEYCODE_RIGHTMETA);
            CASE_STR(KEYCODE_COMPOSE);
            CASE_STR(KEYCODE_AGAIN);
            CASE_STR(KEYCODE_PROPS);
            CASE_STR(KEYCODE_UNDO);
            CASE_STR(KEYCODE_FRONT);
            CASE_STR(KEYCODE_COPY);
            CASE_STR(KEYCODE_OPEN);
            CASE_STR(KEYCODE_PASTE);
            CASE_STR(KEYCODE_FIND);
            CASE_STR(KEYCODE_CUT);
            CASE_STR(KEYCODE_HELP);
            CASE_STR(KEYCODE_MENU);
            CASE_STR(KEYCODE_CALC);
            CASE_STR(KEYCODE_SETUP);
            CASE_STR(KEYCODE_SLEEP);
            CASE_STR(KEYCODE_WAKEUP);
            CASE_STR(KEYCODE_FILE);
            CASE_STR(KEYCODE_SENDFILE);
            CASE_STR(KEYCODE_DELETEFILE);
            CASE_STR(KEYCODE_XFER);
            CASE_STR(KEYCODE_PROG1);
            CASE_STR(KEYCODE_PROG2);
            CASE_STR(KEYCODE_WWW);
            CASE_STR(KEYCODE_MSDOS);
            CASE_STR(KEYCODE_COFFEE);
            //CASE_STR(KEYCODE_SCREENLOCK);
            //CASE_STR(KEYCODE_ROTATE_DISPLAY);
            //CASE_STR(KEYCODE_DIRECTION);
            //CASE_STR(KEYCODE_CYCLEWINDOWS);
            CASE_STR(KEYCODE_MAIL);
            CASE_STR(KEYCODE_BOOKMARKS);
            CASE_STR(KEYCODE_COMPUTER);
            CASE_STR(KEYCODE_BACK);
            CASE_STR(KEYCODE_FORWARD);
            CASE_STR(KEYCODE_CLOSECD);
            CASE_STR(KEYCODE_EJECTCD);
            CASE_STR(KEYCODE_EJECTCLOSECD);
            CASE_STR(KEYCODE_NEXTSONG);
            CASE_STR(KEYCODE_PLAYPAUSE);
            CASE_STR(KEYCODE_PREVIOUSSONG);
            CASE_STR(KEYCODE_STOPCD);
            CASE_STR(KEYCODE_RECORD);
            CASE_STR(KEYCODE_REWIND);
            CASE_STR(KEYCODE_PHONE);
            CASE_STR(KEYCODE_ISO);
            CASE_STR(KEYCODE_CONFIG);
            CASE_STR(KEYCODE_HOMEPAGE);
            CASE_STR(KEYCODE_REFRESH);
            CASE_STR(KEYCODE_EXIT);
            CASE_STR(KEYCODE_MOVE);
            CASE_STR(KEYCODE_EDIT);
            CASE_STR(KEYCODE_SCROLLUP);
            CASE_STR(KEYCODE_SCROLLDOWN);
            CASE_STR(KEYCODE_KPLEFTPAREN);
            CASE_STR(KEYCODE_KPRIGHTPAREN);
            CASE_STR(KEYCODE_NEW);
            CASE_STR(KEYCODE_REDO);

            CASE_STR(KEYCODE_F13);
            CASE_STR(KEYCODE_F14);
            CASE_STR(KEYCODE_F15);
            CASE_STR(KEYCODE_F16);
            CASE_STR(KEYCODE_F17);
            CASE_STR(KEYCODE_F18);
            CASE_STR(KEYCODE_F19);
            CASE_STR(KEYCODE_F20);
            CASE_STR(KEYCODE_F21);
            CASE_STR(KEYCODE_F22);
            CASE_STR(KEYCODE_F23);
            CASE_STR(KEYCODE_F24);

            CASE_STR(KEYCODE_PLAYCD);
            CASE_STR(KEYCODE_PAUSECD);
            CASE_STR(KEYCODE_PROG3);
            CASE_STR(KEYCODE_PROG4);
            CASE_STR(KEYCODE_DASHBOARD);
            CASE_STR(KEYCODE_SUSPEND);
            CASE_STR(KEYCODE_CLOSE);
            CASE_STR(KEYCODE_PLAY);
            CASE_STR(KEYCODE_FASTFORWARD);
            CASE_STR(KEYCODE_BASSBOOST);
            CASE_STR(KEYCODE_PRINT);
            CASE_STR(KEYCODE_HP);
            CASE_STR(KEYCODE_CAMERA);
            CASE_STR(KEYCODE_SOUND);
            CASE_STR(KEYCODE_QUESTION);
            CASE_STR(KEYCODE_EMAIL);
            CASE_STR(KEYCODE_CHAT);
            CASE_STR(KEYCODE_SEARCH);
            CASE_STR(KEYCODE_CONNECT);
            CASE_STR(KEYCODE_FINANCE);
            CASE_STR(KEYCODE_SPORT);
            CASE_STR(KEYCODE_SHOP);
            CASE_STR(KEYCODE_ALTERASE);
            CASE_STR(KEYCODE_CANCEL);
            CASE_STR(KEYCODE_BRIGHTNESSDOWN);
            CASE_STR(KEYCODE_BRIGHTNESSUP);
            CASE_STR(KEYCODE_MEDIA);

            CASE_STR(KEYCODE_SWITCHVIDEOMODE);
            CASE_STR(KEYCODE_KBDILLUMTOGGLE);
            CASE_STR(KEYCODE_KBDILLUMDOWN);
            CASE_STR(KEYCODE_KBDILLUMUP);

            CASE_STR(KEYCODE_SEND);
            CASE_STR(KEYCODE_REPLY);
            CASE_STR(KEYCODE_FORWARDMAIL);
            CASE_STR(KEYCODE_SAVE);
            CASE_STR(KEYCODE_DOCUMENTS);

            CASE_STR(KEYCODE_BATTERY);

            CASE_STR(KEYCODE_BLUETOOTH);
            CASE_STR(KEYCODE_WLAN);
            CASE_STR(KEYCODE_UWB);

            CASE_STR(KEYCODE_UNKNOWN);

            CASE_STR(KEYCODE_VIDEO_NEXT);
            CASE_STR(KEYCODE_VIDEO_PREV);
            CASE_STR(KEYCODE_BRIGHTNESS_CYCLE);
            //CASE_STR(KEYCODE_BRIGHTNESS_AUTO);

            //CASE_STR(KEYCODE_BRIGHTNESS_ZERO);
            CASE_STR(KEYCODE_DISPLAY_OFF);

            //CASE_STR(KEYCODE_WWAN);
            //CASE_STR(KEYCODE_WIMAX);
            CASE_STR(KEYCODE_RFKILL);

            CASE_STR(KEYCODE_MICMUTE);

            //CASE_STR(BTNCODE_MISC);
            CASE_STR(BTNCODE_0);
            CASE_STR(BTNCODE_1);
            CASE_STR(BTNCODE_2);
            CASE_STR(BTNCODE_3);
            CASE_STR(BTNCODE_4);
            CASE_STR(BTNCODE_5);
            CASE_STR(BTNCODE_6);
            CASE_STR(BTNCODE_7);
            CASE_STR(BTNCODE_8);
            CASE_STR(BTNCODE_9);

            //CASE_STR(BTNCODE_MOUSE);
            CASE_STR(BTNCODE_LEFT);
            CASE_STR(BTNCODE_RIGHT);
            CASE_STR(BTNCODE_MIDDLE);
            CASE_STR(BTNCODE_SIDE);
            CASE_STR(BTNCODE_EXTRA);
            CASE_STR(BTNCODE_FORWARD);
            CASE_STR(BTNCODE_BACK);
            CASE_STR(BTNCODE_TASK);

            //CASE_STR(BTNCODE_JOYSTICK);
            CASE_STR(BTNCODE_TRIGGER);
            CASE_STR(BTNCODE_THUMB);
            CASE_STR(BTNCODE_THUMB2);
            CASE_STR(BTNCODE_TOP);
            CASE_STR(BTNCODE_TOP2);
            CASE_STR(BTNCODE_PINKIE);
            CASE_STR(BTNCODE_BASE);
            CASE_STR(BTNCODE_BASE2);
            CASE_STR(BTNCODE_BASE3);
            CASE_STR(BTNCODE_BASE4);
            CASE_STR(BTNCODE_BASE5);
            CASE_STR(BTNCODE_BASE6);
            CASE_STR(BTNCODE_DEAD);

            CASE_STR(BTNCODE_GAMEPAD);
            //CASE_STR(BTNCODE_SOUTH);
            //CASE_STR(BTNCODE_A);
            CASE_STR(BTNCODE_EAST);
            //CASE_STR(BTNCODE_B);
            CASE_STR(BTNCODE_C);
            //CASE_STR(BTNCODE_NORTH);
            //CASE_STR(BTNCODE_X);
            //CASE_STR(BTNCODE_WEST);
            //CASE_STR(BTNCODE_Y);
            CASE_STR(BTNCODE_Z);
            CASE_STR(BTNCODE_TL);
            CASE_STR(BTNCODE_TR);
            CASE_STR(BTNCODE_TL2);
            CASE_STR(BTNCODE_TR2);
            CASE_STR(BTNCODE_SELECT);
            CASE_STR(BTNCODE_START);
            CASE_STR(BTNCODE_MODE);
            CASE_STR(BTNCODE_THUMBL);
            CASE_STR(BTNCODE_THUMBR);

            CASE_STR(BTNCODE_DIGI);
            //CASE_STR(BTNCODE_TOOL_PEN);
            CASE_STR(BTNCODE_TOOL_RUBBER);
            CASE_STR(BTNCODE_TOOL_BRUSH);
            CASE_STR(BTNCODE_TOOL_PENCIL);
            CASE_STR(BTNCODE_TOOL_AIRBRUSH);
            CASE_STR(BTNCODE_TOOL_FINGER);
            CASE_STR(BTNCODE_TOOL_MOUSE);
            CASE_STR(BTNCODE_TOOL_LENS);
            CASE_STR(BTNCODE_TOOL_QUINTTAP);
            CASE_STR(BTNCODE_STYLUS3);
            CASE_STR(BTNCODE_TOUCH);
            CASE_STR(BTNCODE_STYLUS);
            CASE_STR(BTNCODE_STYLUS2);
            CASE_STR(BTNCODE_TOOL_DOUBLETAP);
            CASE_STR(BTNCODE_TOOL_TRIPLETAP);
            CASE_STR(BTNCODE_TOOL_QUADTAP);

            //CASE_STR(BTNCODE_WHEEL);
            CASE_STR(BTNCODE_GEAR_DOWN);
            CASE_STR(BTNCODE_GEAR_UP);

            CASE_STR(KEYCODE_OK);
            CASE_STR(KEYCODE_SELECT);
            CASE_STR(KEYCODE_GOTO);
            CASE_STR(KEYCODE_CLEAR);
            CASE_STR(KEYCODE_POWER2);
            CASE_STR(KEYCODE_OPTION);
            CASE_STR(KEYCODE_INFO);
            CASE_STR(KEYCODE_TIME);
            CASE_STR(KEYCODE_VENDOR);
            CASE_STR(KEYCODE_ARCHIVE);
            CASE_STR(KEYCODE_PROGRAM);
            CASE_STR(KEYCODE_CHANNEL);
            CASE_STR(KEYCODE_FAVORITES);
            CASE_STR(KEYCODE_EPG);
            CASE_STR(KEYCODE_PVR);
            CASE_STR(KEYCODE_MHP);
            CASE_STR(KEYCODE_LANGUAGE);
            CASE_STR(KEYCODE_TITLE);
            CASE_STR(KEYCODE_SUBTITLE);
            CASE_STR(KEYCODE_ANGLE);
            CASE_STR(KEYCODE_FULL_SCREEN);
            //CASE_STR(KEYCODE_ZOOM);
            CASE_STR(KEYCODE_MODE);
            CASE_STR(KEYCODE_KEYBOARD);
            CASE_STR(KEYCODE_ASPECT_RATIO);
            //CASE_STR(KEYCODE_SCREEN);
            CASE_STR(KEYCODE_PC);
            CASE_STR(KEYCODE_TV);
            CASE_STR(KEYCODE_TV2);
            CASE_STR(KEYCODE_VCR);
            CASE_STR(KEYCODE_VCR2);
            CASE_STR(KEYCODE_SAT);
            CASE_STR(KEYCODE_SAT2);
            CASE_STR(KEYCODE_CD);
            CASE_STR(KEYCODE_TAPE);
            CASE_STR(KEYCODE_RADIO);
            CASE_STR(KEYCODE_TUNER);
            CASE_STR(KEYCODE_PLAYER);
            CASE_STR(KEYCODE_TEXT);
            CASE_STR(KEYCODE_DVD);
            CASE_STR(KEYCODE_AUX);
            CASE_STR(KEYCODE_MP3);
            CASE_STR(KEYCODE_AUDIO);
            CASE_STR(KEYCODE_VIDEO);
            CASE_STR(KEYCODE_DIRECTORY);
            CASE_STR(KEYCODE_LIST);
            CASE_STR(KEYCODE_MEMO);
            CASE_STR(KEYCODE_CALENDAR);
            CASE_STR(KEYCODE_RED);
            CASE_STR(KEYCODE_GREEN);
            CASE_STR(KEYCODE_YELLOW);
            CASE_STR(KEYCODE_BLUE);
            CASE_STR(KEYCODE_CHANNELUP);
            CASE_STR(KEYCODE_CHANNELDOWN);
            CASE_STR(KEYCODE_FIRST);
            CASE_STR(KEYCODE_LAST);
            CASE_STR(KEYCODE_AB);
            CASE_STR(KEYCODE_NEXT);
            CASE_STR(KEYCODE_RESTART);
            CASE_STR(KEYCODE_SLOW);
            CASE_STR(KEYCODE_SHUFFLE);
            CASE_STR(KEYCODE_BREAK);
            CASE_STR(KEYCODE_PREVIOUS);
            CASE_STR(KEYCODE_DIGITS);
            CASE_STR(KEYCODE_TEEN);
            CASE_STR(KEYCODE_TWEN);
            CASE_STR(KEYCODE_VIDEOPHONE);
            CASE_STR(KEYCODE_GAMES);
            CASE_STR(KEYCODE_ZOOMIN);
            CASE_STR(KEYCODE_ZOOMOUT);
            CASE_STR(KEYCODE_ZOOMRESET);
            CASE_STR(KEYCODE_WORDPROCESSOR);
            CASE_STR(KEYCODE_EDITOR);
            CASE_STR(KEYCODE_SPREADSHEET);
            CASE_STR(KEYCODE_GRAPHICSEDITOR);
            CASE_STR(KEYCODE_PRESENTATION);
            CASE_STR(KEYCODE_DATABASE);
            CASE_STR(KEYCODE_NEWS);
            CASE_STR(KEYCODE_VOICEMAIL);
            CASE_STR(KEYCODE_ADDRESSBOOK);
            CASE_STR(KEYCODE_MESSENGER);
            CASE_STR(KEYCODE_DISPLAYTOGGLE);
            //CASE_STR(KEYCODE_BRIGHTNESS_TOGGLE);
            CASE_STR(KEYCODE_SPELLCHECK);
            CASE_STR(KEYCODE_LOGOFF);

            CASE_STR(KEYCODE_DOLLAR);
            CASE_STR(KEYCODE_EURO);

            CASE_STR(KEYCODE_FRAMEBACK);
            CASE_STR(KEYCODE_FRAMEFORWARD);
            CASE_STR(KEYCODE_CONTEXT_MENU);
            CASE_STR(KEYCODE_MEDIA_REPEAT);
            CASE_STR(KEYCODE_10CHANNELSUP);
            CASE_STR(KEYCODE_10CHANNELSDOWN);
            CASE_STR(KEYCODE_IMAGES);
            //CASE_STR(KEYCODE_NOTIFICATION_CENTER);
            //CASE_STR(KEYCODE_PICKUP_PHONE);
            //CASE_STR(KEYCODE_HANGUP_PHONE);

            CASE_STR(KEYCODE_DEL_EOL);
            CASE_STR(KEYCODE_DEL_EOS);
            CASE_STR(KEYCODE_INS_LINE);
            CASE_STR(KEYCODE_DEL_LINE);

            CASE_STR(KEYCODE_FN);
            CASE_STR(KEYCODE_FN_ESC);
            CASE_STR(KEYCODE_FN_F1);
            CASE_STR(KEYCODE_FN_F2);
            CASE_STR(KEYCODE_FN_F3);
            CASE_STR(KEYCODE_FN_F4);
            CASE_STR(KEYCODE_FN_F5);
            CASE_STR(KEYCODE_FN_F6);
            CASE_STR(KEYCODE_FN_F7);
            CASE_STR(KEYCODE_FN_F8);
            CASE_STR(KEYCODE_FN_F9);
            CASE_STR(KEYCODE_FN_F10);
            CASE_STR(KEYCODE_FN_F11);
            CASE_STR(KEYCODE_FN_F12);
            CASE_STR(KEYCODE_FN_1);
            CASE_STR(KEYCODE_FN_2);
            CASE_STR(KEYCODE_FN_D);
            CASE_STR(KEYCODE_FN_E);
            CASE_STR(KEYCODE_FN_F);
            CASE_STR(KEYCODE_FN_S);
            CASE_STR(KEYCODE_FN_B);
            //CASE_STR(KEYCODE_FN_RIGHT_SHIFT);

            CASE_STR(KEYCODE_BRL_DOT1);
            CASE_STR(KEYCODE_BRL_DOT2);
            CASE_STR(KEYCODE_BRL_DOT3);
            CASE_STR(KEYCODE_BRL_DOT4);
            CASE_STR(KEYCODE_BRL_DOT5);
            CASE_STR(KEYCODE_BRL_DOT6);
            CASE_STR(KEYCODE_BRL_DOT7);
            CASE_STR(KEYCODE_BRL_DOT8);
            CASE_STR(KEYCODE_BRL_DOT9);
            CASE_STR(KEYCODE_BRL_DOT10);

            CASE_STR(KEYCODE_NUMERIC_0);
            CASE_STR(KEYCODE_NUMERIC_1);
            CASE_STR(KEYCODE_NUMERIC_2);
            CASE_STR(KEYCODE_NUMERIC_3);
            CASE_STR(KEYCODE_NUMERIC_4);
            CASE_STR(KEYCODE_NUMERIC_5);
            CASE_STR(KEYCODE_NUMERIC_6);
            CASE_STR(KEYCODE_NUMERIC_7);
            CASE_STR(KEYCODE_NUMERIC_8);
            CASE_STR(KEYCODE_NUMERIC_9);
            CASE_STR(KEYCODE_NUMERIC_STAR);
            CASE_STR(KEYCODE_NUMERIC_POUND);
            CASE_STR(KEYCODE_NUMERIC_A);
            CASE_STR(KEYCODE_NUMERIC_B);
            CASE_STR(KEYCODE_NUMERIC_C);
            CASE_STR(KEYCODE_NUMERIC_D);

            CASE_STR(KEYCODE_CAMERA_FOCUS);
            CASE_STR(KEYCODE_WPS_BUTTON);

            CASE_STR(KEYCODE_TOUCHPAD_TOGGLE);
            CASE_STR(KEYCODE_TOUCHPAD_ON);
            CASE_STR(KEYCODE_TOUCHPAD_OFF);

            CASE_STR(KEYCODE_CAMERA_ZOOMIN);
            CASE_STR(KEYCODE_CAMERA_ZOOMOUT);
            CASE_STR(KEYCODE_CAMERA_UP);
            CASE_STR(KEYCODE_CAMERA_DOWN);
            CASE_STR(KEYCODE_CAMERA_LEFT);
            CASE_STR(KEYCODE_CAMERA_RIGHT);

            CASE_STR(KEYCODE_ATTENDANT_ON);
            CASE_STR(KEYCODE_ATTENDANT_OFF);
            CASE_STR(KEYCODE_ATTENDANT_TOGGLE);
            CASE_STR(KEYCODE_LIGHTS_TOGGLE);

            CASE_STR(BTNCODE_DPAD_UP);
            CASE_STR(BTNCODE_DPAD_DOWN);
            CASE_STR(BTNCODE_DPAD_LEFT);
            CASE_STR(BTNCODE_DPAD_RIGHT);

            CASE_STR(KEYCODE_ALS_TOGGLE);
            CASE_STR(KEYCODE_ROTATE_LOCK_TOGGLE);

            CASE_STR(KEYCODE_BUTTONCONFIG);
            CASE_STR(KEYCODE_TASKMANAGER);
            CASE_STR(KEYCODE_JOURNAL);
            CASE_STR(KEYCODE_CONTROLPANEL);
            CASE_STR(KEYCODE_APPSELECT);
            CASE_STR(KEYCODE_SCREENSAVER);
            CASE_STR(KEYCODE_VOICECOMMAND);
            CASE_STR(KEYCODE_ASSISTANT);
            CASE_STR(KEYCODE_KBD_LAYOUT_NEXT);
            // CASE_STR(KEYCODE_EMOJI_PICKER);

            CASE_STR(KEYCODE_BRIGHTNESS_MIN);
            CASE_STR(KEYCODE_BRIGHTNESS_MAX);

            CASE_STR(KEYCODE_KBDINPUTASSIST_PREV);
            CASE_STR(KEYCODE_KBDINPUTASSIST_NEXT);
            CASE_STR(KEYCODE_KBDINPUTASSIST_PREVGROUP);
            CASE_STR(KEYCODE_KBDINPUTASSIST_NEXTGROUP);
            CASE_STR(KEYCODE_KBDINPUTASSIST_ACCEPT);
            CASE_STR(KEYCODE_KBDINPUTASSIST_CANCEL);

            CASE_STR(KEYCODE_RIGHT_UP);
            CASE_STR(KEYCODE_RIGHT_DOWN);
            CASE_STR(KEYCODE_LEFT_UP);
            CASE_STR(KEYCODE_LEFT_DOWN);

            CASE_STR(KEYCODE_ROOT_MENU);
            CASE_STR(KEYCODE_MEDIA_TOP_MENU);
            CASE_STR(KEYCODE_NUMERIC_11);
            CASE_STR(KEYCODE_NUMERIC_12);
            CASE_STR(KEYCODE_AUDIO_DESC);
            CASE_STR(KEYCODE_3D_MODE);
            CASE_STR(KEYCODE_NEXT_FAVORITE);
            CASE_STR(KEYCODE_STOP_RECORD);
            CASE_STR(KEYCODE_PAUSE_RECORD);
            CASE_STR(KEYCODE_VOD);
            CASE_STR(KEYCODE_UNMUTE);
            CASE_STR(KEYCODE_FASTREVERSE);
            CASE_STR(KEYCODE_SLOWREVERSE);
            CASE_STR(KEYCODE_DATA);
            CASE_STR(KEYCODE_ONSCREEN_KEYBOARD);
            //CASE_STR(KEYCODE_PRIVACY_SCREEN_TOGGLE);
            //CASE_STR(KEYCODE_SELECTIVE_SCREENSHOT);
            //CASE_STR(KEYCODE_MACRO1);
            //CASE_STR(KEYCODE_MACRO2);
            //CASE_STR(KEYCODE_MACRO3);
            //CASE_STR(KEYCODE_MACRO4);
            //CASE_STR(KEYCODE_MACRO5);
            //CASE_STR(KEYCODE_MACRO6);
            //CASE_STR(KEYCODE_MACRO7);
            //CASE_STR(KEYCODE_MACRO8);
            //CASE_STR(KEYCODE_MACRO9);
            //CASE_STR(KEYCODE_MACRO10);
            //CASE_STR(KEYCODE_MACRO11);
            //CASE_STR(KEYCODE_MACRO12);
            //CASE_STR(KEYCODE_MACRO13);
            //CASE_STR(KEYCODE_MACRO14);
            //CASE_STR(KEYCODE_MACRO15);
            //CASE_STR(KEYCODE_MACRO16);
            //CASE_STR(KEYCODE_MACRO17);
            //CASE_STR(KEYCODE_MACRO18);
            //CASE_STR(KEYCODE_MACRO19);
            //CASE_STR(KEYCODE_MACRO20);
            //CASE_STR(KEYCODE_MACRO21);
            //CASE_STR(KEYCODE_MACRO22);
            //CASE_STR(KEYCODE_MACRO23);
            //CASE_STR(KEYCODE_MACRO24);
            //CASE_STR(KEYCODE_MACRO25);
            //CASE_STR(KEYCODE_MACRO26);
            //CASE_STR(KEYCODE_MACRO27);
            //CASE_STR(KEYCODE_MACRO28);
            //CASE_STR(KEYCODE_MACRO29);
            //CASE_STR(KEYCODE_MACRO30);
            //CASE_STR(KEYCODE_MACRO_RECORD_START);
            //CASE_STR(KEYCODE_MACRO_RECORD_STOP);
            //CASE_STR(KEYCODE_MACRO_PRESET_CYCLE);
            //CASE_STR(KEYCODE_MACRO_PRESET1);
            //CASE_STR(KEYCODE_MACRO_PRESET2);
            //CASE_STR(KEYCODE_MACRO_PRESET3);
            //CASE_STR(KEYCODE_KBD_LCD_MENU1);
            //CASE_STR(KEYCODE_KBD_LCD_MENU2);
            //CASE_STR(KEYCODE_KBD_LCD_MENU3);
            //CASE_STR(KEYCODE_KBD_LCD_MENU4);
            //CASE_STR(KEYCODE_KBD_LCD_MENU5);
            //CASE_STR(BTNCODE_TRIGGER_HAPPY);
            //CASE_STR(BTNCODE_TRIGGER_HAPPY1);
            CASE_STR(BTNCODE_TRIGGER_HAPPY2);
            CASE_STR(BTNCODE_TRIGGER_HAPPY3);
            CASE_STR(BTNCODE_TRIGGER_HAPPY4);
            CASE_STR(BTNCODE_TRIGGER_HAPPY5);
            CASE_STR(BTNCODE_TRIGGER_HAPPY6);
            CASE_STR(BTNCODE_TRIGGER_HAPPY7);
            CASE_STR(BTNCODE_TRIGGER_HAPPY8);
            CASE_STR(BTNCODE_TRIGGER_HAPPY9);
            CASE_STR(BTNCODE_TRIGGER_HAPPY10);
            CASE_STR(BTNCODE_TRIGGER_HAPPY11);
            CASE_STR(BTNCODE_TRIGGER_HAPPY12);
            CASE_STR(BTNCODE_TRIGGER_HAPPY13);
            CASE_STR(BTNCODE_TRIGGER_HAPPY14);
            CASE_STR(BTNCODE_TRIGGER_HAPPY15);
            CASE_STR(BTNCODE_TRIGGER_HAPPY16);
            CASE_STR(BTNCODE_TRIGGER_HAPPY17);
            CASE_STR(BTNCODE_TRIGGER_HAPPY18);
            CASE_STR(BTNCODE_TRIGGER_HAPPY19);
            CASE_STR(BTNCODE_TRIGGER_HAPPY20);
            CASE_STR(BTNCODE_TRIGGER_HAPPY21);
            CASE_STR(BTNCODE_TRIGGER_HAPPY22);
            CASE_STR(BTNCODE_TRIGGER_HAPPY23);
            CASE_STR(BTNCODE_TRIGGER_HAPPY24);
            CASE_STR(BTNCODE_TRIGGER_HAPPY25);
            CASE_STR(BTNCODE_TRIGGER_HAPPY26);
            CASE_STR(BTNCODE_TRIGGER_HAPPY27);
            CASE_STR(BTNCODE_TRIGGER_HAPPY28);
            CASE_STR(BTNCODE_TRIGGER_HAPPY29);
            CASE_STR(BTNCODE_TRIGGER_HAPPY30);
            CASE_STR(BTNCODE_TRIGGER_HAPPY31);
            CASE_STR(BTNCODE_TRIGGER_HAPPY32);
            CASE_STR(BTNCODE_TRIGGER_HAPPY33);
            CASE_STR(BTNCODE_TRIGGER_HAPPY34);
            CASE_STR(BTNCODE_TRIGGER_HAPPY35);
            CASE_STR(BTNCODE_TRIGGER_HAPPY36);
            CASE_STR(BTNCODE_TRIGGER_HAPPY37);
            CASE_STR(BTNCODE_TRIGGER_HAPPY38);
            CASE_STR(BTNCODE_TRIGGER_HAPPY39);
            CASE_STR(BTNCODE_TRIGGER_HAPPY40);
            CASE_STR(KEYCODE_MIN_INTERESTING);
            default:
            return "KEYCODE_UNKNOWN";
        }
    }

    KeyEvent::KeyItem::KeyItem()
        : KeyEvent::KeyItem::KeyItem(KEYCODE_RESERVED, -1) 
    {
    }

    KeyEvent::KeyItem::KeyItem(int32_t keyCode, int64_t downTime) 
        : KeyEvent::KeyItem::KeyItem(keyCode, downTime, -1)
    {
    }

    KeyEvent::KeyItem::KeyItem(int32_t keyCode, int64_t downTime, int64_t upTime) 
        : keyCode_(keyCode), downTime_(downTime), upTime_(upTime)
    {
    }

    int32_t KeyEvent::KeyItem::GetKeyCode() const {
        return keyCode_;
    }

    void KeyEvent::KeyItem::SetKeyCode(int32_t keyCode) {
        keyCode_ = keyCode;
    }

    int64_t KeyEvent::KeyItem::GetDownTime() const {
        return downTime_;
    }

    void KeyEvent::KeyItem::SetDownTime(int64_t downTime) {
        downTime_ = downTime;
    }

    int64_t KeyEvent::KeyItem::GetUpTime() const {
        return upTime_;
    }

    void KeyEvent::KeyItem::SetUpTime(int64_t upTime) {
        upTime_ = upTime;
    }

    std::vector<int32_t> KeyEvent::KeyItem::GetDevices() const {
        return devices_;
    }

    int32_t KeyEvent::KeyItem::AddDevice(int32_t deviceId) {
        for (auto it = devices_.begin(); it != devices_.end(); ++it) {
            if ((*it) == deviceId) {
                devices_.erase(it);
                break;
            }
        }

        devices_.push_back(deviceId);
        return (int32_t)(devices_.size());
    }

    int32_t KeyEvent::KeyItem::RemoveDevice(int32_t deviceId) {
        for (auto it = devices_.begin(); it != devices_.end(); ++it) {
            if ((*it) == deviceId) {
                devices_.erase(it);
                return (int32_t)(devices_.size());
            }
        }

        return -1;
    }

    std::ostream& KeyEvent::KeyItem::operator<<(std::ostream& outStream) const {
        return outStream << '{'
            << "keyCode:" << KeyCodeToString(keyCode_) << ','
            << "downTime:" << downTime_ << ','
            << "upTime:" << upTime_<< ','
            << "devices:" << devices_
            << '}';
    }

    std::shared_ptr<KeyEvent> KeyEvent::CreateInstance() {
        return std::shared_ptr<KeyEvent>(new KeyEvent(InputEvent::EVENT_TYPE_KEY));
    }

    std::shared_ptr<KeyEvent> KeyEvent::CreateInstance(const std::shared_ptr<KeyEvent>& keyEvent) {
        if (!keyEvent) {
            return nullptr;
        }

        KeyEvent* newInstance = keyEvent->Clone();
        if (newInstance == nullptr) {
            return nullptr;
        }

        return std::shared_ptr<KeyEvent>(newInstance);
    }

    KeyEvent::KeyEvent() : KeyEvent(InputEvent::EVENT_TYPE_KEY) {
    }

    KeyEvent::KeyEvent(int32_t eventType)
        : InputEvent(eventType), keyAction_(KEY_ACTION_NONE), keyCode_(KEYCODE_RESERVED) {
        }

    KeyEvent* KeyEvent::Clone() const {
        if (GetEventType() != EVENT_TYPE_KEY) {
            return nullptr;
        }

        return new KeyEvent(*this);
    }

    int32_t KeyEvent::GetKeyAction() const {
        return keyAction_;
    }
    
    int32_t KeyEvent::GetKeyCode() const {
        return keyCode_;
    }

    std::vector<int32_t> KeyEvent::GetPressedKeys() const {
        std::vector<int32_t> result;
        for (auto& item : keyItems_) {
            result.push_back(item.first);
        }
        return result;
    }

    KeyEvent::KeyItem* KeyEvent::GetKeyItem(int32_t keyCode) {
        auto it = keyItems_.find(keyCode);
        if (it == keyItems_.end()) {
            return nullptr;
        }

        return &(it->second);
    }

    KeyEvent::KeyItem* KeyEvent::SetKeyDown(int32_t keyCode, int32_t deviceId, int64_t downTime) {
        LOG_D("Enter keyCode:$s deviceId:$s downTime:$s", keyCode, deviceId, downTime);
        RemoveReleasedKeys();

        auto& keyItem = keyItems_[keyCode];
        keyItem.SetKeyCode(keyCode);
        keyItem.AddDevice(deviceId);
        keyItem.SetUpTime(-1);
        keyItem.SetDownTime(downTime);

        this->keyCode_ = keyCode;
        this->keyAction_ = KEY_ACTION_DOWN;
        this->SetDeviceId(deviceId);
        this->SetActionTime(downTime);
        if (keyItems_.size() == 1) {
            SetActionStartTime(downTime);
        }
        this->AssignNewId();

        LOG_D("Leave keyCode:$s deviceId:$s downTime:$s", keyCode, deviceId, downTime);
        return &keyItem;
    }

    KeyEvent::KeyItem* KeyEvent::SetKeyUp(int32_t keyCode, int32_t deviceId, int64_t upTime) {
        LOG_D("Enter keyCode:$s deviceId:$s upTime:$s", keyCode, deviceId, upTime);
        RemoveReleasedKeys();

        auto it = keyItems_.find(keyCode);
        if (it == keyItems_.end()) {
            LOG_E("Leave keyCode:$s deviceId:$s upTime:$s, NonPresssed", keyCode, deviceId, upTime);
            return nullptr;
        }

        auto& keyItem = it->second;
        auto retCode = keyItem.RemoveDevice(deviceId);
        if (retCode == -1) {
            LOG_E("Leave keyCode:$s deviceId:$s upTime:$s, NonPresssed by device", keyCode, deviceId, upTime);
            return nullptr;
        }

        if (retCode > 0) {
            return &keyItem;
        }

        keyItem.SetUpTime(upTime);
        this->keyCode_ = keyCode;
        this->keyAction_ = KEY_ACTION_UP;

        this->SetDeviceId(deviceId);
        this->SetActionTime(upTime);
        this->AssignNewId();
        LOG_D("Leave keyCode:$s deviceId:$s upTime:$s", keyCode, deviceId, upTime);
        return &keyItem;
    }

    std::ostream& KeyEvent::operator<<(std::ostream& outStream) const
    {
        return PrintInternal(outStream);
    }

    std::ostream& KeyEvent::PrintInternal(std::ostream& outStream) const {
        outStream 
            << '{'
            << "InputEvent:";

        InputEvent::PrintInternal(outStream);

        return outStream
            << ','
            << "keyCode:" << KeyCodeToString(keyCode_) << ','
            << "keyAction:" << KeyActionToString(keyAction_) << ','
            << "keyItems:" << keyItems_ 
            << '}' ;
    }

    void KeyEvent::RemoveReleasedKeys() {
        for (auto it = keyItems_.begin(); it != keyItems_.end();) {
            if (it->second.devices_.empty()) {
                it = keyItems_.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::ostream& operator<<(std::ostream& outStream, const KeyEvent& keyEvent) {
        return keyEvent.operator<<(outStream);
    }

    std::ostream& operator<<(std::ostream& outStream, const KeyEvent::KeyItem& keyItem) {
        return keyItem.operator<<(outStream);
    }

}
