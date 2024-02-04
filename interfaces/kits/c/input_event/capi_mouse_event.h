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

#ifndef CAPI_MOUSE_EVENT_H
#define CAPI_MOUSE_EVENT_H

#include <stdint.h>
#include <vector>
#include "capi_key_event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /** Cancel. */
    MOUSE_ACTION_CANCEL = 0,
    /** Moving of the mouse pointer. */
    MOUSE_ACTION_MOVE = 1,
    /** Pressing down of the mouse. */
    MOUSE_ACTION_BUTTON_DOWN = 2,
    /** Lifting of the mouse button. */
    MOUSE_ACTION_BUTTON_UP = 3,
    /** Beginning of the axis event associated with the mouse. */
    MOUSE_ACTION_AXIS_BEGIN = 4,
    /** Updating of the axis event associated with the mouse. */
    MOUSE_ACTION_AXIS_UPDATE = 5,
    /** Ending of the axis event associated with the mouse. */
    MOUSE_ACTION_AXIS_END = 6,
    /** Pressing down of the mouse touch pad. */
    MOUSE_ACTION_ACTION_DOWN = 7,
    /** Lifting of the mouse touch pad. */
    MOUSE_ACTION_ACTION_UP = 8
} InputEvent_MouseAction;

typedef enum {
    /** Left button on the mouse. */
    MOUSE_BUTTON_LEFT = 0,
    /** Middle button on the mouse. */
    MOUSE_BUTTON_MIDDLE = 1,
    /** Right button on the mouse. */
    MOUSE_BUTTON_RIGHT = 2,
    /** Side button on the mouse. */
    MOUSE_BUTTON_SIDE = 3,
    /** Extended button on the mouse. */
    MOUSE_BUTTON_EXTRA = 4,
    /** Forward button on the mouse. */
    MOUSE_BUTTON_FORWARD = 5,
    /** Back button on the mouse. */
    MOUSE_BUTTON_BACK = 6,
    /** Task key on the mouse. */
    MOUSE_BUTTON_TASK = 7
} InputEvent_MouseButton;

typedef enum {
    /** Vertical scroll axis. */
    MOUSE_AXIS_SCROLL_VERTICAL = 0,
    /** Horizontal scroll axis. */
    MOUSE_AXIS_SCROLL_HORIZONTAL = 1,
    /** Pinch axis. */
    MOUSE_AXIS_PINCH = 2
} InputEvent_MouseAxis;

typedef enum {
    /** Unknown type. */
    MOUSE_TOOLTYPE_UNKNOWN = 0,
    /** Mouse. */
    MOUSE_TOOLTYPE_MOUSE = 1,
    /** Joystick. */
    MOUSE_TOOLTYPE_JOYSTICK = 2,
    /** Touch pad. */
    MOUSE_TOOLTYPE_TOUCHPAD = 3
} InputEvent_MouseToolType;


struct InputEvent_AxisValue {
    /** Axis type. */
    InputEvent_MouseAxis axis;
    /** Axis value. */
    int32_t value;
};

struct InputEvent_MouseEvent {
    /** Mouse event action. */
    InputEvent_MouseAction action;
    /** X coordinate of the mouse pointer on the screen. */
    int32_t screenX;
    /** Y coordinate of the mouse pointer on the screen. */
    int32_t screenY;
    /** X coordinate of the mouse pointer in the window. */
    int32_t windowX;
    /** Y coordinate of the mouse pointer in the window. */
    int32_t windowY;
    /**
     * X axis offset relative to the previous reported mouse pointer position. When the mouse pointer is at
     * the edge of the screen, the value may be less than the difference of the X coordinate reported twice.
     */
    int32_t rawDeltaX;
    /** Y axis offset relative to the previous reported mouse pointer position. */
    int32_t rawDeltaY;
    /** Button that is currently pressed or released. */
    InputEvent_MouseButton button;
    /** Button that is being pressed. */
    std::vector<InputEvent_MouseButton> pressedButtons;
    /** All axis data contained in the event. */
    std::vector<InputEvent_AxisValue> axes;
    /** List of pressed keys. */
    std::vector<InputEvent_KeyCode> pressedKeys;
    /** Whether ctrlKey is being pressed. */
    bool ctrlKey;
    /** Whether altKey is being pressed. */
    bool altKey;
    /** Whether shiftKey is being pressed. */
    bool shiftKey;
    /** Whether logoKey is being pressed. */
    bool logoKey;
    /** Whether fnKey is being pressed. */
    bool fnKey;
    /** Whether capsLock is active. */
    bool capsLock;
    /** Whether numLock is active. */
    bool numLock;
    /** Whether scrollLock is active. */
    bool scrollLock;
    /** Tool type */
    InputEvent_MouseToolType toolType;
};

#ifdef __cplusplus
}
#endif

#endif // CAPI_MOUSE_EVENT_H
