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

#ifndef OHOS_MSG_HEAD_H
#define OHOS_MSG_HEAD_H

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <functional>
#include <algorithm>
#include <fstream>
#include <vector>
#include "nlohmann/json.hpp"
#include "libmmi_util.h"

using Json = nlohmann::json;

#define FILE_NUM 70
#define FILE_BUFF_SIZE 64
#define STR_NUM 8
#define NS_CONVERT_MS 1000
#define PERMISSION_RWX  777
#define OBFUSCATED 30
#define TOUCH_DEVICE_INDEX 1
#define MUT_SCREEN_WIDTH 480
#define MUT_SCREEN_HEIGHT 960
#define EVENT_REPROT_RATE  50

/*
 * All driver file structures are
 * used to store the processing
 * objects for operating driver files
 */
struct DriverFileHander {
    int32_t fp;
    char path[FILE_BUFF_SIZE];
    DriverFileHander()
    {
        fp = -1;
        memset_s(path, sizeof(path), 0, sizeof(path));
    }
};

struct EventArray {
    DriverFileHander drivers[FILE_NUM];
};

/* All supported event handling types */
enum devType {
    KEY_EVENT = 1,          /* keyboard event: press or release */
    KEY_LONG_EVENT = 10,    /* keyboard long press event */
    KEY_PRESS_EVENT = 11,   /* keyboard press event */
    KEY_RELEASE_EVENT = 12, /* keyboard release event */
    DELAY_KEY_EVENT = 14,   /* keyboard long press event */
    MOUSE_MOVE_EVENT = 2,
    MOUSE_CLICK_EVENT = 20,      /* mouse click event */
    MOUSE_MOVE_WHEEL_EVENT = 21, /* mouse wheel event */
    MOUSE_MOVE_HWHEEL_EVENT = 22,
    ROCKER_EVENT = 3,

    TOUCH_EVENT = 4,
    TOUCH_PRESS_EVENT = 41,
    TOUCH_RELEASE_EVENT = 42,
    TOUCH_MOVE_EVENT = 43,

    XBOX_RIGHT_KEY = 50,         /* Button on the right side of the handle */
    XBOX_ABS_KEY = 51,           /* Handle direction button ABS */
    XBOX_LOWERRIGHT_ABSRXY = 52, /* Lower right rocker of handle */
    XBOX_TOPLEFT_ABSRXY = 53,    /* Handle upper left rocker */
    XBOX_ABSRZ = 54,             /* Button behind handle */

    ROCKER_KEY = 60,   /* Keys on both sides of the rocker */
    ROCKER_HAT0Y = 61, /* Push rod under rocker */
    ROCKER_ABSRZ = 62, /* Direction controller above rocker */
    ROCKER_ABS = 63,   /* Rocker master */
    TOUCH_PAD_KEY_EVENT = 70,
    TOUCH_PAD_RING_PRESS_EVENT = 71,
    TOUCH_PAD_RING_MOVE_EVENT = 72,
    TOUCH_PAD_RING_RELEASE_EVENT = 73,
    TOUCH_FINGER_PRESS_EVENT = 74,
    TOUCH_FINGER_MOVE_EVENT = 75,
    TOUCH_FINGER_RELEASE_EVENT = 76,
    TOUCH_PEN_EVENT = 77,
    DEV_TYPE_MAX
};

/*
 * Structure for docking libinput
 */
struct InputEvent {
    uint16_t code;
    int32_t target;
    int32_t type;
    int32_t value;
    int32_t x;
    int32_t y;
    int32_t fp;
    int32_t devType;
    int32_t track;
    int32_t blockTime;
    int32_t multiReprot;
};

/*
 * Click events on devices
 * such as mouse and keyboard
 */
enum PressEvent {
    EVENT_RELEASE = 0,
    EVENT_PRESS = 1,
    LONG_PRESS = 2,
    TOUCH_PAD_PRESS = 15
};

enum HdiInfoType :uint16_t {
    GET_STATUS_INFO = 1001,
    SET_HOT_PLUGS = 1002,
    SET_EVENT_INJECT = 1003,
    GET_DEVICE_INFO = 1004,
    SHOW_DEVICE_INFO = 1005,
    REPLY_STATUS_INFO = 2001,
};

struct DeviceInformation {
    bool status;
    int32_t devIndex;
    int32_t devType;
    int16_t fd;
    char chipName[32];
};

struct InjectEvent {
    input_event event;
    int32_t blockTime;
};

struct InputEventArray {
    std::string deviceName;
    std::string target;
    std::vector<InjectEvent> events;
};

typedef std::function<void(void)> HandleInjectCommandItemsFuncType;
typedef std::function<int32_t (const InputEvent& inputEvent)> WriteDeviceFun;
#endif