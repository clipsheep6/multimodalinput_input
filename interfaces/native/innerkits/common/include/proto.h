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
#ifndef OHOS_PROTO_H
#define OHOS_PROTO_H

#include <sys/types.h>

enum class MmiMessageId : int32_t {
    INVALID = 0,

    // libinput message id
    LIBINPUT_EVENT_NONE = 0,
    LIBINPUT_EVENT_DEVICE_ADDED,
    LIBINPUT_EVENT_DEVICE_REMOVED,

    LIBINPUT_EVENT_KEYBOARD_KEY = 300,

    LIBINPUT_EVENT_POINTER_MOTION = 400,
    LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE,
    LIBINPUT_EVENT_POINTER_BUTTON,
    LIBINPUT_EVENT_POINTER_AXIS,

    LIBINPUT_EVENT_TOUCH_DOWN = 500,
    LIBINPUT_EVENT_TOUCH_UP,
    LIBINPUT_EVENT_TOUCH_MOTION,
    LIBINPUT_EVENT_TOUCH_CANCEL,
    LIBINPUT_EVENT_TOUCH_FRAME,
    LIBINPUT_EVENT_TOUCHPAD_DOWN = 550,
    LIBINPUT_EVENT_TOUCHPAD_UP,
    LIBINPUT_EVENT_TOUCHPAD_MOTION,
    LIBINPUT_EVENT_TABLET_TOOL_AXIS = 600,
    LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY,
    LIBINPUT_EVENT_TABLET_TOOL_TIP,
    LIBINPUT_EVENT_TABLET_TOOL_BUTTON,
    LIBINPUT_EVENT_TABLET_PAD_BUTTON = 700,
    LIBINPUT_EVENT_TABLET_PAD_RING,
    LIBINPUT_EVENT_TABLET_PAD_STRIP,
    LIBINPUT_EVENT_TABLET_PAD_KEY,

    LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN = 800,
    LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE,
    LIBINPUT_EVENT_GESTURE_SWIPE_END,
    LIBINPUT_EVENT_GESTURE_PINCH_BEGIN,
    LIBINPUT_EVENT_GESTURE_PINCH_UPDATE,
    LIBINPUT_EVENT_GESTURE_PINCH_END,
    LIBINPUT_EVENT_SWITCH_TOGGLE = 900,
    INPUT_DEVICE_INFO,
    INPUT_DEVICE_ID_LIST,

    // mmi message id
    BEGIN = 1001,
    REGISTER_APP_INFO,
    REGISTER_MSG_HANDLER,
    UNREGISTER_MSG_HANDLER,
    CHECK_REPLY_MESSAGE,
    ADD_KEY_EVENT_INTERCEPTOR,
    KEY_EVENT_INTERCEPTOR,
    REMOVE_KEY_EVENT_INTERCEPTOR,
    ADD_TOUCH_EVENT_INTERCEPTOR,
    TOUCH_EVENT_INTERCEPTOR,
    REMOVE_TOUCH_EVENT_INTERCEPTOR,
    ADD_TOUCHPAD_EVENT_INTERCEPTOR,
	TOUCHPAD_EVENT_INTERCEPTOR,
	REMOVE_TOUCHPAD_EVENT_INTERCEPTOR,
    DISPLAY_INFO,
    ADD_POINTER_INTERCEPTOR,
    REMOVE_POINTER_INTERCEPTOR,
    POINTER_EVENT_INTERCEPTOR,
    ADD_INPUT_EVENT_MONITOR,
    REMOVE_INPUT_EVENT_MONITOR,
    ADD_INPUT_HANDLER,
    REMOVE_INPUT_HANDLER,
    MARK_CONSUMED,
    SUBSCRIBE_KEY_EVENT,
    UNSUBSCRIBE_KEY_EVENT,
	ADD_INPUT_EVENT_TOUCHPAD_MONITOR,
    REMOVE_INPUT_EVENT_TOUCHPAD_MONITOR,

    COMMON_EVENT_BEGIN = 1100,
    ON_SHOW_MENU,
    ON_SEND,
    ON_COPY,
    ON_PASTE,
    ON_CUT,
    ON_UNDO,
    ON_REFRESH,
    ON_START_DRAG,
    ON_CANCEL,
    ON_ENTER,
    ON_PREVIOUS,
    ON_NEXT,
    ON_BACK,
    ON_PRINT,
    COMMON_EVENT_END,

    KEY_EVENT_BEGIN = 2000,
    ON_KEY,
    ON_SUBSCRIBE_KEY,
    KEY_EVENT_END,

    ON_KEYEVENT = 2100,
    ON_POINTER_EVENT,
    ON_KEYMONITOR = 2200,
    REPORT_KEY_EVENT,
    REPORT_POINTER_EVENT,
    ON_TOUCHPAD_MONITOR,

    MEDIA_EVENT_BEGIN = 3000,
    ON_PLAY,
    ON_PAUSE,
    ON_MEDIA_CONTROL,
    MEDIA_EVENT_END,

    SYSTEM_EVENT_BEGIN = 4000,
    ON_SCREEN_SHOT,
    ON_SCREEN_SPLIT,
    ON_START_SCREEN_RECORD,
    ON_STOP_SCREEN_RECORD,
    ON_GOTO_DESKTOP,
    ON_RECENT,
    ON_SHOW_NOTIFICATION,
    ON_LOCK_SCREEN,
    ON_SEARCH,
    ON_CLOSE_PAGE,
    ON_LAUNCH_VOICE_ASSISTANT,
    ON_MUTE,
    SYSTEM_EVENT_END,

    TELEPHONE_EVENT_BEGIN = 5000,
    ON_ANSWER,
    ON_REFUSE,
    ON_HANG_UP,
    ON_TELEPHONE_CONTROL,
    TELEPHONE_EVENT_END,

    TOUCH_EVENT_BEGIN = 6000,
    ON_TOUCH,
    TOUCH_EVENT_END,

    DEVICE_BEGIN = 7000,
    ON_DEVICE_ADDED,
    ON_DEVICE_REMOVED,
    DEVICE_END,

    ON_STANDARD = 9000,
    SENIOR_INPUT_FUNC = 9050,
    HDI_INJECT = 9051,
    ON_JOYSTICK = 10000,
    ON_WINDOW = 11000,

    ON_VIRTUAL_KEY = 12000,
    ON_VIRTUAL_HOME,
    ON_VIRTUAL_BACK,
    ON_VIRTUAL_MULTI_XERA,
    GET_MMI_INFO_REQ = 12100,
    GET_MMI_INFO_ACK,

    ON_DUMP = 13000,
    ON_EXCEPTION = 13050,
    ON_LIST,
    INJECT_KEY_EVENT = 14000,
    INJECT_POINTER_EVENT,

    // STClient message id
    ST_MESSAGE_BEGIN = 30000,
    ST_MESSAGE_ACK,
    ST_MESSAGE_LIBPKT,
    ST_MESSAGE_MAPPKT,
    ST_MESSAGE_STDPKT,
    ST_MESSAGE_MAGPKT,
    ST_MESSAGE_DPCPKT,
    ST_MESSAGE_CLTPKT,
    ST_MESSAGE_CLISTPKT,
    ST_MESSAGE_REPLYPKT,
    ST_MESSAGE_KEYTYPEPKT,
    ST_MESSAGE_END,

    END,
};

enum INPUT_DEVICE_TYPE {
    INPUT_DEVICE_CAP_KEYBOARD = 0,
    INPUT_DEVICE_CAP_POINTER = 1,
    INPUT_DEVICE_CAP_TOUCH = 2,
    INPUT_DEVICE_CAP_TABLET_TOOL = 3,
    INPUT_DEVICE_CAP_TABLET_PAD = 4,
    INPUT_DEVICE_CAP_GESTURE = 5,
    INPUT_DEVICE_CAP_SWITCH = 6,
    INPUT_DEVICE_CAP_JOYSTICK = 7,
    INPUT_DEVICE_CAP_AISENSOR = 8,
    INPUT_DEVICE_CAP_TOUCH_PAD = 9,
    INPUT_DEVICE_CAP_REMOTE_CONTROL = 10,
    INPUT_DEVICE_CAP_TRACK_BALL = 11,
    INPUT_DEVICE_CAP_KNUCKLE = 12,
    INPUT_DEVICE_CAP_TRACKPAD5 = 13,
    INPUT_DEVICE_CAP_GAMEPAD = 14,
};

enum INPUT_DEVICE_INDEX {
    INPUT_DEVICE_POINTER_INDEX = 32,
    INPUT_DEVICE_KEYBOARD_INDEX = 33,
    INPUT_DEVICE_TOUCH_INDEX = 34,
    INPUT_DEVICE_TABLET_TOOL_INDEX = 35,
    INPUT_DEVICE_TABLET_PAD_INDEX = 36,
    INPUT_DEVICE_GESTURE_INDEX = 37,
    INPUT_DEVICE_FINGER_INDEX = 38,
    INPUT_DEVICE_JOYSTICK_INDEX = 39,
    INPUT_DEVICE_AISENSOR_INDEX = 40,
    INPUT_DEVICE_TOUCH_PAD = 41,
    INPUT_DEVICE_REMOTE_CONTROL = 42,
    INPUT_DEVICE_SWITCH_INDEX = 43,
    INPUT_DEVICE_TRACKPAD5_INDEX = 44,
    INPUT_DEVICE_GAMEPAD_INDEX = 45,
};

enum MSG_TYPE {
    MSG_TYPE_DEVICE_INIT,
    MSG_TYPE_DEVICE_INFO,
};
enum JOYSTICK_AXIS_SOURCE {
    JOYSTICK_AXIS_SOURCE_UNKNOW = 0,
    JOYSTICK_AXIS_SOURCE_ABS_X = 1UL,
    JOYSTICK_AXIS_SOURCE_ABS_Y = 1UL << 1,
    JOYSTICK_AXIS_SOURCE_ABS_Z = 1UL << 2,
    JOYSTICK_AXIS_SOURCE_ABS_RX = 1UL << 3,
    JOYSTICK_AXIS_SOURCE_ABS_RY = 1UL << 4,
    JOYSTICK_AXIS_SOURCE_ABS_RZ = 1UL << 5,
    JOYSTICK_AXIS_SOURCE_ABS_THROTTLE = 1UL << 6,
    JOYSTICK_AXIS_SOURCE_ABS_HAT0X = 1UL << 11,
    JOYSTICK_AXIS_SOURCE_ABS_HAT0Y = 1UL << 12,
};

enum TestMutInputAnrErr {
    MULTIMODE_INPUT_ANR_NORMAL = 200,
    MULTIMODE_INPUT_ANR_NOWINDOW,
    MULTIMODE_INPUT_ANR_NOFD,
    MULTIMODE_INPUT_ANR_CONNECTDEAD,
    MULTIMODE_INPUT_ANR_BUFFERFULL,
    MULTIMODE_INPUT_ANR_QUEUEBLOCK,
    MULTIMODE_INPUT_TOUCH_NORMAL = 300,
    MULTIMODE_INPUT_TOUCH_THREEFINGERS = 303,
};

enum Authority {
    NO_AUTHORITY = 0,
    LOW_AUTHORITY,
    MIDDLE_AUTHORITY,
    HIGH_AUTHORITY,
};
#endif
