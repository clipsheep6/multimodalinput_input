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

#include "oh_input_manager.h"

#include "input_manager.h"
#include "input_manager_impl.h"
#include "key_event.h"
#include "mmi_log.h"
#include "oh_key_code.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "OHInputManager"

static std::mutex g_CallBacksMutex;
constexpr size_t PRE_KEYS_SIZE { 4 };
int32_t MICROSECONDS = 1000;
int32_t *g_keys;
struct Input_KeyState {
    int32_t keyCode;
    int32_t keyState;
    int32_t keySwitch;
};

struct Input_KeyEvent {
    int32_t action;
    int32_t keyCode;
    int64_t actionTime { -1 };
};

struct Input_MouseEvent {
    int32_t action;
    int32_t displayX;
    int32_t displayY;
    int32_t button { -1 };
    int32_t axisType { -1 };
    float axisValue { 0.0f };
    int64_t actionTime { -1 };
};

struct Input_TouchEvent {
    int32_t action;
    int32_t id;
    int32_t displayX;
    int32_t displayY;
    int64_t actionTime { -1 };
};

struct Input_KeyEventMonitorInfo {
    int32_t subscribeId;
    std::string eventType;
    Input_ShortcutKeyCallback callback { nullptr };
    std::shared_ptr<OHOS::MMI::KeyOption> keyOption { nullptr };
};

struct Input_ShortcutKey {
    int32_t finalKey { -1 };
    std::set<int32_t> pressedKeys {};
    bool isFinalKeyDown { false };
    int32_t finalKeyDownDuration { 0 };
    bool isRepeat { false };
};

typedef std::map<std::string, std::list<Input_KeyEventMonitorInfo *>> Callbacks;
static Callbacks g_callbacks = {};

static std::shared_ptr<OHOS::MMI::KeyEvent> g_keyEvent = OHOS::MMI::KeyEvent::Create();
static std::shared_ptr<OHOS::MMI::PointerEvent> g_mouseEvent = OHOS::MMI::PointerEvent::Create();
static std::shared_ptr<OHOS::MMI::PointerEvent> g_touchEvent = OHOS::MMI::PointerEvent::Create();

Input_Result OH_Input_GetKeyState(struct Input_KeyState* keyState)
{
    CALL_DEBUG_ENTER;
    CHKPR(keyState, INPUT_PARAMETER_ERROR);
    if (keyState->keyCode < 0 || keyState->keyCode > KEYCODE_NUMPAD_RIGHT_PAREN) {
        MMI_HILOGE("keyCode is invalid, keyCode:%{public}d", keyState->keyCode);
        return INPUT_PARAMETER_ERROR;
    }
    std::vector<int32_t> pressedKeys;
    std::map<int32_t, int32_t> specialKeysState;
    OHOS::MMI::InputManager::GetInstance()->GetKeyState(pressedKeys, specialKeysState);
    auto iter = std::find(pressedKeys.begin(), pressedKeys.end(), keyState->keyCode);
    if (iter != pressedKeys.end()) {
        keyState->keyState = KEY_PRESSED;
    } else {
        keyState->keyState = KEY_RELEASED;
    }
    auto itr = specialKeysState.find(keyState->keyCode);
    if (itr != specialKeysState.end()) {
        if (itr->second == 0) {
            keyState->keySwitch = KEY_SWITCH_OFF;
        } else {
            keyState->keySwitch = KEY_SWITCH_ON;
        }
    } else {
        keyState->keySwitch = KEY_DEFAULT;
    }
    return INPUT_SUCCESS;
}

struct Input_KeyState* OH_Input_CreateKeyState()
{
    Input_KeyState* keyState = new (std::nothrow) Input_KeyState();
    CHKPL(keyState);
    return keyState;
}

void OH_Input_DestroyKeyState(struct Input_KeyState** keyState)
{
    CALL_DEBUG_ENTER;
    CHKPV(keyState);
    CHKPV(*keyState);
    delete *keyState;
    *keyState = nullptr;
}

void OH_Input_SetKeyCode(struct Input_KeyState* keyState, int32_t keyCode)
{
    CHKPV(keyState);
    if (keyCode < 0 || keyState->keyCode > KEYCODE_NUMPAD_RIGHT_PAREN) {
        MMI_HILOGE("keyCode is invalid, keyCode:%{public}d", keyCode);
        return;
    }
    keyState->keyCode = keyCode;
}

int32_t OH_Input_GetKeyCode(const struct Input_KeyState* keyState)
{
    CHKPR(keyState, KEYCODE_UNKNOWN);
    return keyState->keyCode;
}

void OH_Input_SetKeyPressed(struct Input_KeyState* keyState, int32_t keyAction)
{
    CHKPV(keyState);
    keyState->keyState = keyAction;
}

int32_t OH_Input_GetKeyPressed(const struct Input_KeyState* keyState)
{
    CHKPR(keyState, KEY_DEFAULT);
    return keyState->keyState;
}

void OH_Input_SetKeySwitch(struct Input_KeyState* keyState, int32_t keySwitch)
{
    CHKPV(keyState);
    keyState->keySwitch = keySwitch;
}

int32_t OH_Input_GetKeySwitch(const struct Input_KeyState* keyState)
{
    CHKPR(keyState, KEY_DEFAULT);
    return keyState->keySwitch;
}

static void HandleKeyAction(const struct Input_KeyEvent* keyEvent, OHOS::MMI::KeyEvent::KeyItem &item)
{
    if (keyEvent->action == KEY_ACTION_DOWN) {
        g_keyEvent->AddPressedKeyItems(item);
    }
    if (keyEvent->action == KEY_ACTION_UP) {
        std::optional<OHOS::MMI::KeyEvent::KeyItem> pressedKeyItem = g_keyEvent->GetKeyItem(keyEvent->keyCode);
        if (pressedKeyItem) {
            item.SetDownTime(pressedKeyItem->GetDownTime());
        } else {
            MMI_HILOGW("Find pressed key failed, keyCode:%{public}d", keyEvent->keyCode);
        }
        g_keyEvent->RemoveReleasedKeyItems(item);
        g_keyEvent->AddPressedKeyItems(item);
    }
}

int32_t OH_Input_InjectKeyEvent(const struct Input_KeyEvent* keyEvent)
{
    MMI_HILOGI("Input_KeyEvent injectEvent");
    CHKPR(keyEvent, INPUT_PARAMETER_ERROR);
    if (keyEvent->keyCode < 0) {
        MMI_HILOGE("keyCode:%{public}d is less 0, can not process", keyEvent->keyCode);
        return INPUT_PARAMETER_ERROR;
    }
    CHKPR(g_keyEvent, INPUT_PARAMETER_ERROR);
    g_keyEvent->ClearFlag();
    if (g_keyEvent->GetAction() == OHOS::MMI::KeyEvent::KEY_ACTION_UP) {
        std::optional<OHOS::MMI::KeyEvent::KeyItem> preUpKeyItem = g_keyEvent->GetKeyItem();
        if (preUpKeyItem) {
            g_keyEvent->RemoveReleasedKeyItems(*preUpKeyItem);
        } else {
            MMI_HILOGE("The preUpKeyItem is nullopt");
        }
    }
    int64_t time = keyEvent->actionTime;
    if (time < 0) {
        time = OHOS::MMI::GetSysClockTime();
    }
    g_keyEvent->SetActionTime(time);
    g_keyEvent->SetRepeat(true);
    g_keyEvent->SetKeyCode(keyEvent->keyCode);
    bool isKeyPressed = false;
    if (keyEvent->action == KEY_ACTION_DOWN) {
        g_keyEvent->SetAction(OHOS::MMI::KeyEvent::KEY_ACTION_DOWN);
        g_keyEvent->SetKeyAction(OHOS::MMI::KeyEvent::KEY_ACTION_DOWN);
        isKeyPressed = true;
    } else if (keyEvent->action == KEY_ACTION_UP) {
        g_keyEvent->SetAction(OHOS::MMI::KeyEvent::KEY_ACTION_UP);
        g_keyEvent->SetKeyAction(OHOS::MMI::KeyEvent::KEY_ACTION_UP);
        isKeyPressed = false;
    }
    OHOS::MMI::KeyEvent::KeyItem item;
    item.SetDownTime(time);
    item.SetKeyCode(keyEvent->keyCode);
    item.SetPressed(isKeyPressed);
    HandleKeyAction(keyEvent, item);
    g_keyEvent->AddFlag(OHOS::MMI::InputEvent::EVENT_FLAG_SIMULATE);
    OHOS::Singleton<OHOS::MMI::InputManagerImpl>::GetInstance().SimulateInputEvent(g_keyEvent, true);
    return INPUT_SUCCESS;
}

struct Input_KeyEvent* OH_Input_CreateKeyEvent()
{
    Input_KeyEvent* keyEvent = new (std::nothrow) Input_KeyEvent();
    CHKPL(keyEvent);
    return keyEvent;
}

void OH_Input_DestroyKeyEvent(struct Input_KeyEvent** keyEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(keyEvent);
    CHKPV(*keyEvent);
    delete *keyEvent;
    *keyEvent = nullptr;
}

void OH_Input_SetKeyEventAction(struct Input_KeyEvent* keyEvent, int32_t action)
{
    CHKPV(keyEvent);
    keyEvent->action = action;
}

int32_t OH_Input_GetKeyEventAction(const struct Input_KeyEvent* keyEvent)
{
    CHKPR(keyEvent, RET_ERR);
    return keyEvent->action;
}

void OH_Input_SetKeyEventKeyCode(struct Input_KeyEvent* keyEvent, int32_t keyCode)
{
    CHKPV(keyEvent);
    keyEvent->keyCode = keyCode;
}

int32_t OH_Input_GetKeyEventKeyCode(const struct Input_KeyEvent* keyEvent)
{
    CHKPR(keyEvent, KEYCODE_UNKNOWN);
    return keyEvent->keyCode;
}

void OH_Input_SetKeyEventActionTime(struct Input_KeyEvent* keyEvent, int64_t actionTime)
{
    CHKPV(keyEvent);
    keyEvent->actionTime = actionTime;
}

int64_t OH_Input_GetKeyEventActionTime(const struct Input_KeyEvent* keyEvent)
{
    CHKPR(keyEvent, RET_ERR);
    return keyEvent->actionTime;
}

static int32_t HandleMouseButton(const struct Input_MouseEvent* mouseEvent)
{
    int32_t button = mouseEvent->button;
    switch (button) {
        case MOUSE_BUTTON_NONE: {
            button = OHOS::MMI::PointerEvent::BUTTON_NONE;
            break;
        }
        case MOUSE_BUTTON_LEFT: {
            button = OHOS::MMI::PointerEvent::MOUSE_BUTTON_LEFT;
            break;
        }
        case MOUSE_BUTTON_MIDDLE: {
            button = OHOS::MMI::PointerEvent::MOUSE_BUTTON_MIDDLE;
            break;
        }
        case MOUSE_BUTTON_RIGHT: {
            button = OHOS::MMI::PointerEvent::MOUSE_BUTTON_RIGHT;
            break;
        }
        case MOUSE_BUTTON_FORWARD: {
            button = OHOS::MMI::PointerEvent::MOUSE_BUTTON_FORWARD;
            break;
        }
        case MOUSE_BUTTON_BACK: {
            button = OHOS::MMI::PointerEvent::MOUSE_BUTTON_BACK;
            break;
        }
        default: {
            MMI_HILOGE("button:%{public}d is invalid", button);
            return INPUT_PARAMETER_ERROR;
        }
    }
    if (mouseEvent->action == MOUSE_ACTION_BUTTON_DOWN) {
        g_mouseEvent->SetButtonPressed(button);
    } else if (mouseEvent->action == MOUSE_ACTION_BUTTON_UP) {
        g_mouseEvent->DeleteReleaseButton(button);
    }
    g_mouseEvent->SetButtonId(button);
    return INPUT_SUCCESS;
}

static int32_t HandleMouseAction(const struct Input_MouseEvent* mouseEvent, OHOS::MMI::PointerEvent::PointerItem &item)
{
    switch (mouseEvent->action) {
        case MOUSE_ACTION_CANCEL:
            g_mouseEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_CANCEL);
            break;
        case MOUSE_ACTION_MOVE:
            g_mouseEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_MOVE);
            break;
        case MOUSE_ACTION_BUTTON_DOWN:
            g_mouseEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
            item.SetPressed(true);
            break;
        case MOUSE_ACTION_BUTTON_UP:
            g_mouseEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
            item.SetPressed(false);
            break;
        case MOUSE_ACTION_AXIS_BEGIN:
            g_mouseEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_BEGIN);
            break;
        case MOUSE_ACTION_AXIS_UPDATE:
            g_mouseEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_UPDATE);
            break;
        case MOUSE_ACTION_AXIS_END:
            g_mouseEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_END);
            break;
        default:
            MMI_HILOGE("action:%{public}d is invalid", mouseEvent->action);
            return INPUT_PARAMETER_ERROR;
    }
    if (mouseEvent->axisType == MOUSE_AXIS_SCROLL_VERTICAL) {
        g_mouseEvent->SetAxisValue(OHOS::MMI::PointerEvent::AXIS_TYPE_SCROLL_VERTICAL, mouseEvent->axisValue);
    }
    if (mouseEvent->axisType == MOUSE_AXIS_SCROLL_HORIZONTAL) {
        g_mouseEvent->SetAxisValue(OHOS::MMI::PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL, mouseEvent->axisValue);
    }
    return HandleMouseButton(mouseEvent);
}

static int32_t HandleMouseProperty(const struct Input_MouseEvent* mouseEvent,
    OHOS::MMI::PointerEvent::PointerItem &item)
{
    int32_t screenX = mouseEvent->displayX;
    int32_t screenY = mouseEvent->displayY;
    g_mouseEvent->SetSourceType(OHOS::MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    item.SetPointerId(0);
    item.SetDisplayX(screenX);
    item.SetDisplayY(screenY);
    g_mouseEvent->SetPointerId(0);
    g_mouseEvent->UpdatePointerItem(g_mouseEvent->GetPointerId(), item);
    return INPUT_SUCCESS;
}

int32_t OH_Input_InjectMouseEvent(const struct Input_MouseEvent* mouseEvent)
{
    MMI_HILOGI("Input_MouseEvent injectEvent");
    CHKPR(mouseEvent, INPUT_PARAMETER_ERROR);
    CHKPR(g_mouseEvent, INPUT_PARAMETER_ERROR);
    g_mouseEvent->ClearFlag();
    g_mouseEvent->ClearAxisValue();
    g_mouseEvent->SetTargetDisplayId(0);
    int64_t time = mouseEvent->actionTime;
    if (time < 0) {
        time = OHOS::MMI::GetSysClockTime();
    }
    g_mouseEvent->SetActionTime(time);
    OHOS::MMI::PointerEvent::PointerItem item;
    item.SetDownTime(time);
    int32_t result = HandleMouseAction(mouseEvent, item);
    if (result != 0) {
        return result;
    }
    result = HandleMouseProperty(mouseEvent, item);
    if (result != 0) {
        return result;
    }
    g_mouseEvent->AddFlag(OHOS::MMI::InputEvent::EVENT_FLAG_SIMULATE);
    OHOS::Singleton<OHOS::MMI::InputManagerImpl>::GetInstance().SimulateInputEvent(g_mouseEvent, true);
    return INPUT_SUCCESS;
}

struct Input_MouseEvent* OH_Input_CreateMouseEvent()
{
    CALL_DEBUG_ENTER;
    Input_MouseEvent* mouseEvent = new (std::nothrow) Input_MouseEvent();
    CHKPL(mouseEvent);
    return mouseEvent;
}

void OH_Input_DestroyMouseEvent(struct Input_MouseEvent** mouseEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(mouseEvent);
    CHKPV(*mouseEvent);
    delete *mouseEvent;
    *mouseEvent = nullptr;
}

void OH_Input_SetMouseEventAction(struct Input_MouseEvent* mouseEvent, int32_t action)
{
    CALL_DEBUG_ENTER;
    CHKPV(mouseEvent);
    mouseEvent->action = action;
}

int32_t OH_Input_GetMouseEventAction(const struct Input_MouseEvent* mouseEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(mouseEvent, RET_ERR);
    return mouseEvent->action;
}

void OH_Input_SetMouseEventDisplayX(struct Input_MouseEvent* mouseEvent, int32_t displayX)
{
    CALL_DEBUG_ENTER;
    CHKPV(mouseEvent);
    mouseEvent->displayX = displayX;
}

int32_t OH_Input_GetMouseEventDisplayX(const struct Input_MouseEvent* mouseEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(mouseEvent, RET_ERR);
    return mouseEvent->displayX;
}

void OH_Input_SetMouseEventDisplayY(struct Input_MouseEvent* mouseEvent, int32_t displayY)
{
    CALL_DEBUG_ENTER;
    CHKPV(mouseEvent);
    mouseEvent->displayY = displayY;
}

int32_t OH_Input_GetMouseEventDisplayY(const struct Input_MouseEvent* mouseEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(mouseEvent, RET_ERR);
    return mouseEvent->displayY;
}

void OH_Input_SetMouseEventButton(struct Input_MouseEvent* mouseEvent, int32_t button)
{
    CALL_DEBUG_ENTER;
    CHKPV(mouseEvent);
    mouseEvent->button = button;
}

int32_t OH_Input_GetMouseEventButton(const struct Input_MouseEvent* mouseEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(mouseEvent, RET_ERR);
    return mouseEvent->button;
}

void OH_Input_SetMouseEventAxisType(struct Input_MouseEvent* mouseEvent, int32_t axisType)
{
    CALL_DEBUG_ENTER;
    CHKPV(mouseEvent);
    mouseEvent->axisType = axisType;
}

int32_t OH_Input_GetMouseEventAxisType(const struct Input_MouseEvent* mouseEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(mouseEvent, RET_ERR);
    return mouseEvent->axisType;
}

void OH_Input_SetMouseEventAxisValue(struct Input_MouseEvent* mouseEvent, float axisValue)
{
    CALL_DEBUG_ENTER;
    CHKPV(mouseEvent);
    mouseEvent->axisValue = axisValue;
}

float OH_Input_GetMouseEventAxisValue(const struct Input_MouseEvent* mouseEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(mouseEvent, RET_ERR);
    return mouseEvent->axisValue;
}

void OH_Input_SetMouseEventActionTime(struct Input_MouseEvent* mouseEvent, int64_t actionTime)
{
    CALL_DEBUG_ENTER;
    CHKPV(mouseEvent);
    mouseEvent->actionTime = actionTime;
}

int64_t OH_Input_GetMouseEventActionTime(const struct Input_MouseEvent* mouseEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(mouseEvent, RET_ERR);
    return mouseEvent->actionTime;
}

static void HandleTouchActionDown(OHOS::MMI::PointerEvent::PointerItem &item, int64_t time)
{
    auto pointIds = g_touchEvent->GetPointerIds();
    if (pointIds.empty()) {
        g_touchEvent->SetActionStartTime(time);
        g_touchEvent->SetTargetDisplayId(0);
    }
    g_touchEvent->SetActionTime(time);
    g_touchEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_DOWN);
    item.SetDownTime(time);
    item.SetPressed(true);
}

static int32_t HandleTouchAction(const struct Input_TouchEvent* touchEvent, OHOS::MMI::PointerEvent::PointerItem &item)
{
    CALL_DEBUG_ENTER;
    int64_t time = touchEvent->actionTime;
    if (time < 0) {
        time = OHOS::MMI::GetSysClockTime();
    }
    switch (touchEvent->action) {
        case TOUCH_ACTION_CANCEL:{
            g_touchEvent->SetActionTime(time);
            g_touchEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_CANCEL);
            if (!(g_touchEvent->GetPointerItem(touchEvent->id, item))) {
                MMI_HILOGE("Get pointer parameter failed");
                return INPUT_PARAMETER_ERROR;
            }
            item.SetPressed(false);
            break;
        }
        case TOUCH_ACTION_DOWN: {
            HandleTouchActionDown(item, time);
            break;
        }
        case TOUCH_ACTION_MOVE: {
            g_touchEvent->SetActionTime(time);
            g_touchEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_MOVE);
            if (!(g_touchEvent->GetPointerItem(touchEvent->id, item))) {
                MMI_HILOGE("Get pointer parameter failed");
                return INPUT_PARAMETER_ERROR;
            }
            break;
        }
        case TOUCH_ACTION_UP: {
            g_touchEvent->SetActionTime(time);
            g_touchEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_UP);
            if (!(g_touchEvent->GetPointerItem(touchEvent->id, item))) {
                MMI_HILOGE("Get pointer parameter failed");
                return INPUT_PARAMETER_ERROR;
            }
            item.SetPressed(false);
            break;
        }
        default: {
            MMI_HILOGE("action:%{public}d is invalid", touchEvent->action);
            return INPUT_PARAMETER_ERROR;
        }
    }
    return INPUT_SUCCESS;
}

static int32_t HandleTouchProperty(const struct Input_TouchEvent* touchEvent,
    OHOS::MMI::PointerEvent::PointerItem &item)
{
    CALL_DEBUG_ENTER;
    int32_t id = touchEvent->id;
    int32_t screenX = touchEvent->displayX;
    int32_t screenY = touchEvent->displayY;
    if (screenX < 0 || screenY < 0) {
        MMI_HILOGE("touch parameter is less 0, can not process");
        return INPUT_PARAMETER_ERROR;
    }
    item.SetDisplayX(screenX);
    item.SetDisplayY(screenY);
    item.SetPointerId(id);
    g_touchEvent->SetPointerId(id);
    g_touchEvent->SetSourceType(OHOS::MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    if (touchEvent->action == TOUCH_ACTION_DOWN) {
        g_touchEvent->AddPointerItem(item);
    } else if ((touchEvent->action == TOUCH_ACTION_MOVE) || (touchEvent->action == TOUCH_ACTION_UP)) {
        g_touchEvent->UpdatePointerItem(id, item);
    }
    return INPUT_SUCCESS;
}

int32_t OH_Input_InjectTouchEvent(const struct Input_TouchEvent* touchEvent)
{
    MMI_HILOGI("Input_TouchEvent injectTouchEvent");
    CHKPR(touchEvent, INPUT_PARAMETER_ERROR);
    CHKPR(g_touchEvent, INPUT_PARAMETER_ERROR);
    g_touchEvent->ClearFlag();
    OHOS::MMI::PointerEvent::PointerItem item;
    int32_t result = HandleTouchAction(touchEvent, item);
    if (result != 0) {
        return INPUT_PARAMETER_ERROR;
    }
    result = HandleTouchProperty(touchEvent, item);
    if (result != 0) {
        return INPUT_PARAMETER_ERROR;
    }
    g_touchEvent->AddFlag(OHOS::MMI::InputEvent::EVENT_FLAG_SIMULATE);
    OHOS::Singleton<OHOS::MMI::InputManagerImpl>::GetInstance().SimulateInputEvent(g_touchEvent, true);
    if (touchEvent->action == TOUCH_ACTION_UP) {
        g_touchEvent->RemovePointerItem(g_touchEvent->GetPointerId());
        MMI_HILOGD("This touch event is up remove this finger");
        if (g_touchEvent->GetPointerIds().empty()) {
            MMI_HILOGD("This touch event is final finger up remove this finger");
            g_touchEvent->Reset();
        }
    }
    return INPUT_SUCCESS;
}

struct Input_TouchEvent* OH_Input_CreateTouchEvent()
{
    CALL_DEBUG_ENTER;
    Input_TouchEvent* touchEvent = new (std::nothrow) Input_TouchEvent();
    CHKPL(touchEvent);
    return touchEvent;
}

void OH_Input_DestroyTouchEvent(struct Input_TouchEvent** touchEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(touchEvent);
    CHKPV(*touchEvent);
    delete *touchEvent;
    *touchEvent = nullptr;
}

void OH_Input_SetTouchEventAction(struct Input_TouchEvent* touchEvent, int32_t action)
{
    CALL_DEBUG_ENTER;
    CHKPV(touchEvent);
    touchEvent->action = action;
}

int32_t OH_Input_GetTouchEventAction(const struct Input_TouchEvent* touchEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(touchEvent, RET_ERR);
    return touchEvent->action;
}

void OH_Input_SetTouchEventFingerId(struct Input_TouchEvent* touchEvent, int32_t id)
{
    CALL_DEBUG_ENTER;
    CHKPV(touchEvent);
    touchEvent->id = id;
}

int32_t OH_Input_GetTouchEventFingerId(const struct Input_TouchEvent* touchEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(touchEvent, RET_ERR);
    return touchEvent->id;
}

void OH_Input_SetTouchEventDisplayX(struct Input_TouchEvent* touchEvent, int32_t displayX)
{
    CALL_DEBUG_ENTER;
    CHKPV(touchEvent);
    touchEvent->displayX = displayX;
}

int32_t OH_Input_GetTouchEventDisplayX(const struct Input_TouchEvent* touchEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(touchEvent, RET_ERR);
    return touchEvent->displayX;
}

void OH_Input_SetTouchEventDisplayY(struct Input_TouchEvent* touchEvent, int32_t displayY)
{
    CALL_DEBUG_ENTER;
    CHKPV(touchEvent);
    touchEvent->displayY = displayY;
}

int32_t OH_Input_GetTouchEventDisplayY(const struct Input_TouchEvent* touchEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(touchEvent, RET_ERR);
    return touchEvent->displayY;
}

void OH_Input_SetTouchEventActionTime(struct Input_TouchEvent* touchEvent, int64_t actionTime)
{
    CALL_DEBUG_ENTER;
    CHKPV(touchEvent);
    touchEvent->actionTime = actionTime;
}

int64_t OH_Input_GetTouchEventActionTime(const struct Input_TouchEvent* touchEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(touchEvent, RET_ERR);
    return touchEvent->actionTime;
}

void OH_Input_CancelInjection()
{
    CALL_DEBUG_ENTER;
    OHOS::Singleton<OHOS::MMI::InputManagerImpl>::GetInstance().CancelInjection();
}

struct Input_ShortcutKey* OH_Input_CreateShortcutKey()
{
    CALL_DEBUG_ENTER;
    Input_ShortcutKey* shortcutKey = new (std::nothrow) Input_ShortcutKey();
    CHKPL(shortcutKey);
    return shortcutKey;
}

void OH_Input_DestroyShortcutKey(struct Input_ShortcutKey** shortcutKey)
{
    CALL_DEBUG_ENTER;
    CHKPV(shortcutKey);
    CHKPV(*shortcutKey);

    if (g_keys != nullptr) {
        free(g_keys);
        g_keys = nullptr;
    }

    delete *shortcutKey;
    *shortcutKey = nullptr;
}

void OH_Input_SetpressedKeys(struct Input_ShortcutKey* shortcutKey, int32_t* preKeys, int32_t size)
{
    CALL_DEBUG_ENTER;
    CHKPV(shortcutKey);
    CHKPV(preKeys);
    if (size <= 0) {
        MMI_HILOGE("PreKeys does not exist");
        return;
    }

    for (int32_t i = 0; i < size; ++i) {
        shortcutKey->pressedKeys.insert(preKeys[i]);
    }
    return;
}

int32_t* OH_Input_GetpressedKeys(const struct Input_ShortcutKey* shortcutKey)
{
    CALL_DEBUG_ENTER;
    CHKPP(shortcutKey);
    std::set<int32_t> preKeys = shortcutKey->pressedKeys;
    int32_t size = preKeys.size();
    if (size <= 0) {
        MMI_HILOGE("The pressKeys not exit");
        return nullptr;
    }
    g_keys = (int32_t *)malloc(size * sizeof(int32_t));
    if (g_keys == nullptr) {
        MMI_HILOGE("Memory allocation failed");
        return nullptr;
    }
    int32_t index = 0;
    for (auto it = preKeys.begin(); it != preKeys.end(); ++it) {
        g_keys[index++] = *it;
    }
    return g_keys;
}

void OH_Input_SetfinalKey(struct Input_ShortcutKey* shortcutKey, int32_t finalKey)
{
    CALL_DEBUG_ENTER;
    CHKPV(shortcutKey);
    shortcutKey->finalKey = finalKey;
    return;
}

int32_t OH_Input_GetfinalKey(const struct Input_ShortcutKey* shortcutKey, int32_t *finalKeyCode)
{
    CALL_DEBUG_ENTER;
    CHKPR(shortcutKey, INPUT_PARAMETER_ERROR);
    *finalKeyCode = shortcutKey->finalKey;
    return INPUT_SUCCESS;
}

void OH_Input_SetIsFinalKeyDown(struct Input_ShortcutKey* shortcutKey, bool isFinalKeyDown)
{
    CALL_DEBUG_ENTER;
    CHKPV(shortcutKey);
    shortcutKey->isFinalKeyDown = isFinalKeyDown;
    return;
}

bool OH_Input_GetIsFinalKeyDown(const struct Input_ShortcutKey* shortcutKey)
{
    CALL_DEBUG_ENTER;
    CHKPR(shortcutKey, RET_ERR);
    return shortcutKey->isFinalKeyDown;
}

void OH_Input_SetIsRepeat(struct Input_ShortcutKey* shortcutKey, bool isRepeat)
{
    CALL_DEBUG_ENTER;
    CHKPV(shortcutKey);
    shortcutKey->isRepeat  = isRepeat;
    return;
}

bool OH_Input_GetIsRepeat(const struct Input_ShortcutKey* shortcutKey)
{
    CALL_DEBUG_ENTER;
    CHKPR(shortcutKey, RET_ERR);
    return shortcutKey->isRepeat;
}

void OH_Input_SetFinalKeyDownDuration(struct Input_ShortcutKey* shortcutKey, int32_t duration)
{
    CALL_DEBUG_ENTER;
    CHKPV(shortcutKey);
    shortcutKey->finalKeyDownDuration  = duration;
    return;
}

int32_t OH_Input_GetFinalKeyDownDuration(const struct Input_ShortcutKey* shortcutKey)
{
    CALL_DEBUG_ENTER;
    CHKPR(shortcutKey, INPUT_PARAMETER_ERROR);
    return shortcutKey->finalKeyDownDuration;
}

static int32_t GetEventInfo(const struct Input_ShortcutKey* shortcutKey,
    Input_KeyEventMonitorInfo* event, std::shared_ptr<OHOS::MMI::KeyOption> keyOption)
{
    CALL_DEBUG_ENTER;
    CHKPR(shortcutKey, INPUT_PARAMETER_ERROR);
    CHKPR(event, INPUT_PARAMETER_ERROR);
    CHKPR(keyOption, INPUT_PARAMETER_ERROR);
    if (shortcutKey->pressedKeys.empty()) {
        MMI_HILOGE("pressedKeys not found");
        return INPUT_FAILED;
    }

    std::set<int32_t> preKeys;
    preKeys = shortcutKey->pressedKeys;
    if (preKeys.size() > PRE_KEYS_SIZE) {
        MMI_HILOGE("preKeys size invalid");
        return INPUT_FAILED;
    }
    keyOption->SetPreKeys(preKeys);
    std::string subKeyNames = "";
    for (const auto &item : preKeys) {
        subKeyNames += std::to_string(item);
        subKeyNames += ",";
    }

    int32_t finalKey = shortcutKey->finalKey;
    if (finalKey < 0) {
        MMI_HILOGE("finalKey:%{public}d is less 0, can not process", finalKey);
        return INPUT_FAILED;
    }
    subKeyNames += std::to_string(finalKey);
    subKeyNames += ",";
    keyOption->SetFinalKey(finalKey);

    bool isFinalKeyDown = shortcutKey->isFinalKeyDown;
    subKeyNames += std::to_string(isFinalKeyDown);
    subKeyNames += ",";
    keyOption->SetFinalKeyDown(isFinalKeyDown);

    int32_t finalKeyDownDuration = shortcutKey->finalKeyDownDuration;
    if (finalKeyDownDuration < 0) {
        MMI_HILOGE("finalKeyDownDuration:%{public}d is less 0, can not process", finalKeyDownDuration);
        return INPUT_FAILED;
    }
    subKeyNames += std::to_string(finalKeyDownDuration);
    subKeyNames += ",";
    keyOption->SetFinalKeyDownDuration(finalKeyDownDuration);

    bool isRepeat = shortcutKey->isRepeat;
    subKeyNames += std::to_string(isRepeat);
    keyOption->SetIsRepeat(isRepeat);
    event->eventType = subKeyNames;
    MMI_HILOGD("event->eventType:%{public}s", event->eventType.c_str());
    return INPUT_SUCCESS;
}

static int32_t GetPreSubscribeId(Input_KeyEventMonitorInfo* event)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, INPUT_PARAMETER_ERROR);
    auto it = g_callbacks.find(event->eventType);
    if (it == g_callbacks.end() || it->second.empty()) {
        MMI_HILOGE("The callbacks is empty");
        return INPUT_FAILED;
    }
    CHKPR(it->second.front(), INPUT_PARAMETER_ERROR);
    return it->second.front()->subscribeId;
}

static int32_t AddEventCallback(Input_KeyEventMonitorInfo* event)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, INPUT_PARAMETER_ERROR);
    if (g_callbacks.find(event->eventType) == g_callbacks.end()) {
        MMI_HILOGD("No callback in %{public}s", event->eventType.c_str());
        g_callbacks[event->eventType] = {};
    }
    auto it = g_callbacks.find(event->eventType);
    if (it != g_callbacks.end()) {
        for (const auto &iter: it->second) {
            if (iter->callback == event->callback) {
                MMI_HILOGI("Callback already exist");
                return INPUT_SUCCESS;
            }
        }
    }
    it->second.push_back(event);
    return INPUT_SUCCESS;
}

static bool IsMatchKeyAction(bool isFinalKeydown, int32_t keyAction)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGD("isFinalKeydown:%{public}d, keyAction:%{public}d", isFinalKeydown, keyAction);
    if (isFinalKeydown && keyAction == OHOS::MMI::KeyEvent::KEY_ACTION_DOWN) {
        return true;
    }
    if (!isFinalKeydown && keyAction == OHOS::MMI::KeyEvent::KEY_ACTION_UP) {
        return true;
    }
    MMI_HILOGE("isFinalKeydown not matched with keyAction");
    return false;
}

static bool MatchCombinationKeys(Input_KeyEventMonitorInfo* monitorInfo, std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent)
{
    CALL_DEBUG_ENTER;
    CHKPF(monitorInfo);
    CHKPF(keyEvent);
    auto keyOption = monitorInfo->keyOption;
    CHKPF(keyOption);
    std::vector<OHOS::MMI::KeyEvent::KeyItem> items = keyEvent->GetKeyItems();
    int32_t infoFinalKey = keyOption->GetFinalKey();
    int32_t keyEventFinalKey = keyEvent->GetKeyCode();
    bool isFinalKeydown = keyOption->IsFinalKeyDown();
    MMI_HILOGD("infoFinalKey:%{public}d,keyEventFinalKey:%{public}d", infoFinalKey, keyEventFinalKey);
    if (infoFinalKey != keyEventFinalKey || items.size() > PRE_KEYS_SIZE ||
        !IsMatchKeyAction(isFinalKeydown, keyEvent->GetKeyAction())) {
        MMI_HILOGD("Param invalid");
        return false;
    }
    std::set<int32_t> infoPreKeys = keyOption->GetPreKeys();
    int32_t infoSize = 0;
    for (auto it = infoPreKeys.begin(); it != infoPreKeys.end(); ++it) {
        if (*it >= 0) {
            infoSize++;
        }
    }
    int32_t count = 0;
    for (const auto &item : items) {
        if (item.GetKeyCode() == keyEventFinalKey) {
            continue;
        }
        auto iter = find(infoPreKeys.begin(), infoPreKeys.end(), item.GetKeyCode());
        if (iter == infoPreKeys.end()) {
            MMI_HILOGW("No keyCode in preKeys");
            return false;
        }
        count++;
    }
    MMI_HILOGD("kevEventSize:%{public}d, infoSize:%{public}d", count, infoSize);
    std::optional<OHOS::MMI::KeyEvent::KeyItem> keyItem = keyEvent->GetKeyItem();
    if (!keyItem) {
        MMI_HILOGE("The keyItem is nullopt");
        return false;
    }
    auto downTime = keyItem->GetDownTime();
    auto upTime = keyEvent->GetActionTime();
    auto curDurationTime = keyOption->GetFinalKeyDownDuration();
    if (curDurationTime > 0 && (upTime - downTime >= (static_cast<int64_t>(curDurationTime) * MICROSECONDS))) {
        MMI_HILOGE("Skip, upTime - downTime >= duration");
        return false;
    }
    return count == infoSize;
}
static void OnNotifyCallbackWorkResult(Input_KeyEventMonitorInfo* reportEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(reportEvent);

    Input_KeyEventMonitorInfo *info = new(std::nothrow) Input_KeyEventMonitorInfo();
    if (info == nullptr) {
        MMI_HILOGE("info is null");
        return;
    }
    info->keyOption = reportEvent->keyOption;
    if (info->keyOption == nullptr) {
        delete info;
        MMI_HILOGE("keyOption is null");
        return;
    }
    info->callback = reportEvent->callback;
    if (info->callback == nullptr) {
        delete info;
        MMI_HILOGE("callback is null");
        return;
    }

    Input_ShortcutKey shortcutKey;
    shortcutKey.pressedKeys = info->keyOption->GetPreKeys();
    shortcutKey.finalKey = info->keyOption->GetFinalKey();
    shortcutKey.finalKeyDownDuration = info->keyOption->GetFinalKeyDownDuration();
    shortcutKey.isFinalKeyDown = info->keyOption->IsFinalKeyDown();
    shortcutKey.isRepeat = info->keyOption->GetIsRepeat();
    info->callback(&shortcutKey);
    delete info;
}

static void OnSubKeyEventCallback(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(keyEvent);
    std::lock_guard guard(g_CallBacksMutex);
    auto iter = g_callbacks.begin();
    while (iter != g_callbacks.end()) {
        auto &list = iter->second;
        ++iter;
        MMI_HILOGD("list size:%{public}zu", list.size());
        auto infoIter = list.begin();
        while (infoIter != list.end()) {
            auto monitorInfo = *infoIter;
            if (MatchCombinationKeys(monitorInfo, keyEvent)) {
                OnNotifyCallbackWorkResult(monitorInfo);
            }
            ++infoIter;
        }
    }
}

int32_t OH_Input_AddShortcutKeyMonitor(const struct Input_ShortcutKey* shortcutKey, Input_ShortcutKeyCallback callback)
{
    CALL_DEBUG_ENTER;
    CHKPR(shortcutKey, INPUT_PARAMETER_ERROR);
    CHKPR(callback, INPUT_PARAMETER_ERROR);
    std::lock_guard guard(g_CallBacksMutex);

    Input_KeyEventMonitorInfo *event = new (std::nothrow) Input_KeyEventMonitorInfo();
    auto keyOption = std::make_shared<OHOS::MMI::KeyOption>();
    if (GetEventInfo(shortcutKey, event, keyOption) != INPUT_SUCCESS) {
        delete event;
        MMI_HILOGE("GetEventInfo failed");
        return INPUT_FAILED;
    }
    event->keyOption = keyOption;
    event->callback = callback;
    int32_t preSubscribeId = GetPreSubscribeId(event);
    if (preSubscribeId < 0) {
        MMI_HILOGD("eventType:%{public}s", event->eventType.c_str());
        int32_t subscribeId = -1;
        subscribeId = OHOS::MMI::InputManager::GetInstance()->SubscribeKeyEvent(keyOption, OnSubKeyEventCallback);
        if (subscribeId < 0) {
            delete event;
            MMI_HILOGE("SubscribeId invalid:%{public}d", subscribeId);
            return INPUT_FAILED;
        }
        MMI_HILOGD("SubscribeId:%{public}d", subscribeId);
        event->subscribeId = subscribeId;
    } else {
        event->subscribeId = preSubscribeId;
    }
    if (AddEventCallback(event) < 0) {
        delete event;
        MMI_HILOGE("Add event callback failed");
        return INPUT_FAILED;
    }
    return INPUT_SUCCESS;
}

int32_t DelEventCallbackRef(std::list<Input_KeyEventMonitorInfo *> &infos,
    Input_ShortcutKeyCallback callback, int32_t &subscribeId)
{
    CALL_DEBUG_ENTER;
    CHKPR(&infos, INPUT_PARAMETER_ERROR);
    CHKPR(callback, INPUT_PARAMETER_ERROR);

    auto iter = infos.begin();
    while (iter != infos.end()) {
        if (*iter == nullptr) {
            iter = infos.erase(iter);
            continue;
        }
        if (callback == nullptr) {
            Input_KeyEventMonitorInfo *monitorInfo = *iter;
            infos.erase(iter++);
            if (infos.empty()) {
                subscribeId = monitorInfo->subscribeId;
            }
            delete monitorInfo;
            MMI_HILOGD("Callback has been deleted, size:%{public}zu", infos.size());
            continue;
        }
        if ((*iter)->callback == callback) {
            Input_KeyEventMonitorInfo *monitorInfo = *iter;
            iter = infos.erase(iter);
            if (infos.empty()) {
                subscribeId = monitorInfo->subscribeId;
            }
            delete monitorInfo;
            MMI_HILOGD("Callback has been deleted, size:%{public}zu", infos.size());
            return INPUT_SUCCESS;
        }
        ++iter;
    }
    return INPUT_SUCCESS;
}

int32_t DelEventCallback(Input_KeyEventMonitorInfo* event, int32_t &subscribeId)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, INPUT_PARAMETER_ERROR);
    if (g_callbacks.count(event->eventType) <= 0) {
        MMI_HILOGE("Callback doesn't exists");
        return INPUT_PARAMETER_ERROR;
    }
    auto &info = g_callbacks[event->eventType];
    MMI_HILOGD("EventType:%{public}s, Input_KeyEventMonitorInfo:%{public}zu", event->eventType.c_str(), info.size());
    return DelEventCallbackRef(info, event->callback, subscribeId);
}

void OH_Input_RemoveShortcutKeyMonitor(const struct Input_ShortcutKey *shortcutKey, Input_ShortcutKeyCallback callback)
{
    CALL_DEBUG_ENTER;
    CHKPV(shortcutKey);
    CHKPV(callback);
    std::lock_guard guard(g_CallBacksMutex);

    Input_KeyEventMonitorInfo *event = new (std::nothrow) Input_KeyEventMonitorInfo();
    auto keyOption = std::make_shared<OHOS::MMI::KeyOption>();
    if (GetEventInfo(shortcutKey, event, keyOption) != INPUT_SUCCESS) {
        delete event;
        MMI_HILOGE("Get eventInfo failed");
        return;
    }
    event->callback = callback;
    int32_t subscribeId = -1;
    if (DelEventCallback(event, subscribeId) == INPUT_PARAMETER_ERROR) {
        delete event;
        MMI_HILOGE("DelEventCallback failed");
        return;
    }
    MMI_HILOGD("SubscribeId:%{public}d", subscribeId);
    if (subscribeId >= 0) {
        OHOS::MMI::InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId);
    }
    delete event;
    return;
}