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

#include "native_input_event_manager.h"
#include "input_manager.h"
#include "mmi_log.h"
#include "key_event.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, OHOS::MMI::MMI_LOG_DOMAIN, "NativeInputEventManager" };
} // namespace

int32_t OH_InputEvent_InjectEvent(struct CAPI_KeyEvent* keyEvent)
{
    MMI_HILOGI("InputEvent injectEvent");
    if (keyEvent->keyDownDuration < 0) {
        MMI_HILOGE("keyDownDuration:%{public}d is less 0, can not process", keyEvent->keyDownDuration);
        return COMMON_PARAMETER_ERROR;
    }
    if (keyEvent->keyCode < 0) {
        MMI_HILOGE("keyCode:%{public}d is less 0, can not process", keyEvent->keyCode);
        return COMMON_PARAMETER_ERROR;
    }
    auto mmiKeyEvent = OHOS::MMI::KeyEvent::Create();
    CHKPR(mmiKeyEvent, OTHER_ERROR);
    mmiKeyEvent->SetRepeat(true);
    if (keyEvent->isIntercepted) {
        mmiKeyEvent->AddFlag(OHOS::MMI::InputEvent::EVENT_FLAG_NONE);
    } else {
        mmiKeyEvent->AddFlag(OHOS::MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
    }
    
    mmiKeyEvent->SetKeyCode(keyEvent->keyCode);
    if (keyEvent->isPressed) {
        mmiKeyEvent->SetKeyAction(OHOS::MMI::KeyEvent::KEY_ACTION_DOWN);
    } else {
        mmiKeyEvent->SetKeyAction(OHOS::MMI::KeyEvent::KEY_ACTION_UP);
    }

    OHOS::MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(keyEvent->keyCode);
    item.SetPressed(keyEvent->isPressed);
    item.SetDownTime(static_cast<int64_t>(keyEvent->keyDownDuration));
    mmiKeyEvent->AddKeyItem(item);

    OHOS::MMI::InputManager::GetInstance()->SimulateInputEvent(mmiKeyEvent);
    std::this_thread::sleep_for(std::chrono::milliseconds(keyEvent->keyDownDuration));
    return 0;
}

static int32_t HandleMouseButton(struct CAPI_MouseEvent* mouseEvent, std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent)
{
    int32_t button = mouseEvent->button;
    if (button < 0) {
        MMI_HILOGE("button:%{public}d is less 0, can not process", button);
        return COMMON_PARAMETER_ERROR;
    }

    switch (button) {
        case MOUSE_BUTTON_MIDDLE:
            button = OHOS::MMI::PointerEvent::MOUSE_BUTTON_MIDDLE;
            break;
        case MOUSE_BUTTON_RIGHT:
            button = OHOS::MMI::PointerEvent::MOUSE_BUTTON_RIGHT;
            break;
        default:
            break;
    }

    pointerEvent->SetButtonId(button);
    pointerEvent->SetButtonPressed(button);
    return 0;
}

static int32_t HandleMouseAction(struct CAPI_MouseEvent* mouseEvent,
    std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent, OHOS::MMI::PointerEvent::PointerItem &item)
{
    switch (mouseEvent->action) {
        case MOUSE_ACTION_MOVE:
            pointerEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_MOVE);
            break;
        case MOUSE_ACTION_BUTTON_DOWN:
            pointerEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
            item.SetPressed(true);
            break;
        case MOUSE_ACTION_BUTTON_UP:
            pointerEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
            item.SetPressed(false);
            break;
        case MOUSE_ACTION_ACTION_DOWN:
            pointerEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_DOWN);
            item.SetPressed(true);
            break;
        case MOUSE_ACTION_ACTION_UP:
            pointerEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_UP);
            item.SetPressed(false);
            break;
        default:
            MMI_HILOGE("action is unknown");
            break;
    }
    if (mouseEvent->action == MOUSE_ACTION_ACTION_DOWN || mouseEvent->action == MOUSE_ACTION_ACTION_UP) {
        return HandleMouseButton(mouseEvent, pointerEvent);
    }
    return 0;
}


static int32_t HandleMousePropertyInt32(struct CAPI_MouseEvent* mouseEvent,
    std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent, OHOS::MMI::PointerEvent::PointerItem &item)
{
    int32_t screenX = mouseEvent->screenX;
    int32_t screenY = mouseEvent->screenY;
    int32_t toolType = mouseEvent->toolType;
    if (toolType < 0) {
        MMI_HILOGE("toolType:%{public}d is less 0, can not process", toolType);
        return COMMON_PARAMETER_ERROR;
    }
    pointerEvent->SetSourceType(toolType);
    item.SetPointerId(0);
    item.SetDisplayX(screenX);
    item.SetDisplayY(screenY);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    return 0;
}


int32_t OH_InputEvent_InjectMouseEvent(struct CAPI_MouseEvent* mouseEvent)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGI("InputEvent injectEvent");
    auto pointerEvent = OHOS::MMI::PointerEvent::Create();
    OHOS::MMI::PointerEvent::PointerItem item;
    CHKPR(pointerEvent, OTHER_ERROR);
    int32_t result = HandleMouseAction(mouseEvent, pointerEvent, item);
    if (result != 0) {
        return result;
    }
    result = HandleMousePropertyInt32(mouseEvent, pointerEvent, item);
    if (result != 0) {
        return result;
    }
    OHOS::MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    return 0;
}

static int32_t HandleTouchAction(struct CAPI_TouchEvent* touchEvent,
    std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent, OHOS::MMI::PointerEvent::PointerItem &item)
{
    int32_t action = touchEvent->action;
    switch (action) {
        case TOUCH_ACTION_DOWN:
            pointerEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_DOWN);
            item.SetPressed(true);
            break;
        case TOUCH_ACTION_MOVE:
            pointerEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_MOVE);
            break;
        case TOUCH_ACTION_UP:
            pointerEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_UP);
            item.SetPressed(false);
            break;
        default:
            MMI_HILOGE("action is unknown");
            break;
    }
    return action;
}

static int32_t HandleTouchPropertyInt32(struct CAPI_TouchEvent* touchEvent,
    std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent, OHOS::MMI::PointerEvent::PointerItem &item, int32_t action)
{
    int32_t sourceType = touchEvent->sourceType;
    if (sourceType < 0) {
        MMI_HILOGE("sourceType:%{public}d is less 0, can not process", sourceType);
        return COMMON_PARAMETER_ERROR;
    }
    if (sourceType == TOUCH_SOURCE_TYPE_TOUCH_SCREEN || sourceType == TOUCH_SOURCE_TYPE_PEN) {
        sourceType = OHOS::MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN;
    }
    int32_t screenX = touchEvent->touch.screenX;
    int32_t screenY = touchEvent->touch.screenY;
    int64_t pressedTime = touchEvent->touch.pressedTime;
    int32_t toolType = touchEvent->touch.toolType;
    double pressure = touchEvent->touch.pressure;
    if (screenX < 0 || screenY < 0 || pressedTime < 0 || toolType < 0 || pressure < 0) {
        MMI_HILOGE("touch parameter is less 0, can not process");
        return COMMON_PARAMETER_ERROR;
    }
    item.SetDisplayX(screenX);
    item.SetDisplayY(screenY);
    item.SetPointerId(0);
    item.SetToolType(toolType);
    item.SetPressure(pressure);
    pointerEvent->SetPointerId(0);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetSourceType(sourceType);
    pointerEvent->SetActionTime(pressedTime);
    if ((action == TOUCH_ACTION_MOVE) || (action == TOUCH_ACTION_UP)) {
        pointerEvent->UpdatePointerItem(0, item);
    }
    return 0;
}

int32_t OH_InputEvent_InjectTouchEvent(struct CAPI_TouchEvent* touchEvent)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGI("InputEvent injectTouchEvent");
    auto pointerEvent = OHOS::MMI::PointerEvent::Create();
    OHOS::MMI::PointerEvent::PointerItem item;
    CHKPR(pointerEvent, OTHER_ERROR);

    int32_t action = HandleTouchAction(touchEvent, pointerEvent, item);
    int32_t result = HandleTouchPropertyInt32(touchEvent, pointerEvent, item, action);
    if (result != 0) {
        return COMMON_PARAMETER_ERROR;
    }
    OHOS::MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    return 0;
}