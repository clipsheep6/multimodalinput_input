/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef CAPI_TOUCH_EVENT_H
#define CAPI_TOUCH_EVENT_H

#include <stdint.h>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /** Touch cancelled. */
    TOUCH_ACTION_CANCEL = 0,
    /** Touch pressed. */
    TOUCH_ACTION_DOWN = 1,
    /** Touch moved. */
    TOUCH_ACTION_MOVE = 2,
    /** Touch lifted. */
    TOUCH_ACTION_UP = 3,
} InputEvent_TouchAction;

typedef enum {
    /** Touchscreen. */
    TOUCH_SOURCE_TYPE_TOUCH_SCREEN = 0,
    /** Stylus. */
    TOUCH_SOURCE_TYPE_PEN = 1,
    /** Touchpad. */
    TOUCH_SOURCE_TYPE_TOUCH_PAD = 2
} InputEvent_TouchSourceType;

typedef enum {
    /** Finger. */
    TOUCH_TOOLTYPE_FINGER = 0,
    /** Stylus. */
    TOUCH_TOOLTYPE_PEN = 1,
    /** Rubber. */
    TOUCH_TOOLTYPE_RUBBER = 2,
    /** Brush. */
    TOUCH_TOOLTYPE_BRUSH = 3,
    /** Pencil. */
    TOUCH_TOOLTYPE_PENCIL = 4,
    /** Air brush. */
    TOUCH_TOOLTYPE_AIRBRUSH = 5,
    /** Mouse. */
    TOUCH_TOOLTYPE_MOUSE = 6,
    /** Lens. */
    TOUCH_TOOLTYPE_LENS = 7,
} InputEvent_TouchToolType;

struct InputEvent_Touch {
    /** Pointer identifier. */
    int32_t id;
    /** Time stamp when touch is pressed. */
    int32_t pressedTime;
    /** X coordinate of the touch position on the screen. */
    int32_t screenX;
    /** Y coordinate of the touch position on the screen. */
    int32_t screenY;
    /** X coordinate of the touch position in the window. */
    int32_t windowX;
    /** Y coordinate of the touch position in the window. */
    int32_t windowY;
    /** Pressure value. The value range is [0.0, 1.0]. The value 0.0 indicates that the pressure is not supported. */
    int32_t pressure;
    /** Width of the contact area when touch is pressed. */
    int32_t width;
    /** Height of the contact area when touch is pressed. */
    int32_t height;
    /** Angle relative to the YZ plane. The value range is [-90, 90]. A positive value indicates a rightward tilt. */
    int32_t tiltX;
    /** Angle relative to the XZ plane. The value range is [-90, 90]. A positive value indicates a downward tilt. */
    int32_t tiltY;
    /** Center point X of the tool area. */
    int32_t toolX;
    /** Center point Y of the tool area. */
    int32_t toolY;
    /** Width of the tool area. */
    int32_t toolWidth;
    /** Height of the tool area. */
    int32_t toolHeight;
    /** X coordinate of the input device. */
    int32_t rawX;
    /** Y coordinate of the input device. */
    int32_t rawY;
    /** Tool type. */
    InputEvent_TouchToolType toolType;
};

struct InputEvent_TouchEvent {
    /** Touch action. */
    InputEvent_TouchAction action;
    /** Current touch point. */
    InputEvent_Touch touch;
    /** All touch points. */
    std::vector<Touch> touches;
    /** Device type of the touch source. */
    InputEvent_TouchSourceType sourceType;
};

#ifdef __cplusplus
}
#endif

#endif // CAPI_TOUCH_EVENT_H
