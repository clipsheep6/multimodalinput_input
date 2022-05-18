/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "input_event_normalize_handler.h"

#include "bytrace_adapter.h"
#include "define_multimodal.h"
#include "error_multimodal.h"
#include "input_device_manager.h"
#include "input_event_handler.h"
#include "key_event_value_transformation.h"
#include "mmi_log.h"
#include "timer_manager.h"
#include "touch_transform_point_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputEventNormalizeHandler" };
}

void InputEventNormalizeHandler::HandleLibinputEvent(libinput_event* event)
{
    CALL_LOG_ENTER;
    CHKPV(event);
    auto type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_TOUCH_CANCEL || type == LIBINPUT_EVENT_TOUCH_FRAME) {
        MMI_HILOGD("This touch event is canceled type:%{public}d", type);
        return;
    }
    switch (type) {
        case LIBINPUT_EVENT_KEYBOARD_KEY: {
            HandleKeyboardEvent(event);
            break;
        }
        case LIBINPUT_EVENT_TOUCHPAD_DOWN:
        case LIBINPUT_EVENT_TOUCHPAD_UP:
        case LIBINPUT_EVENT_TOUCHPAD_MOTION: {
            HandleTouchPadEvent(event);
            break;
        }
        case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
        case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
        case LIBINPUT_EVENT_GESTURE_SWIPE_END:
        case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
        case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
        case LIBINPUT_EVENT_GESTURE_PINCH_END: {
            HandleGestureEvent(event);
            break;
        }
        case LIBINPUT_EVENT_POINTER_MOTION:
        case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
        case LIBINPUT_EVENT_POINTER_BUTTON:
        case LIBINPUT_EVENT_POINTER_AXIS: {
            HandleMouseEvent(event);
            break;
        }
        case LIBINPUT_EVENT_TOUCH_DOWN:
        case LIBINPUT_EVENT_TOUCH_UP:
        case LIBINPUT_EVENT_TOUCH_MOTION: {
            HandleTouchEvent(event);
            break;
        }
        case LIBINPUT_EVENT_TABLET_TOOL_AXIS:
        case LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY:
        case LIBINPUT_EVENT_TABLET_TOOL_TIP: {
            HandleTableToolEvent(event);
            break;
        }
        default: {
            MMI_HILOGW("This device does not support");
            break;
        }
    }
}

void InputEventNormalizeHandler::HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Keyboard device does not support");
        return;
    }
    CHKPV(keyEvent);
    nextHandler_->HandleKeyEvent(keyEvent);
}

void InputEventNormalizeHandler::HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Pointer device does not support");
        return;
    }
    CHKPV(pointerEvent);
    if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_AXIS_END) {
        MMI_HILOGI("MouseEvent Normalization Results, PointerAction:%{public}d,PointerId:%{public}d,"
            "SourceType:%{public}d,ButtonId:%{public}d,"
            "VerticalAxisValue:%{public}lf,HorizontalAxisValue:%{public}lf",
            pointerEvent->GetPointerAction(), pointerEvent->GetPointerId(), pointerEvent->GetSourceType(),
            pointerEvent->GetButtonId(), pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
            pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL));
        PointerEvent::PointerItem item;
        if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item)) {
            MMI_HILOGE("Get pointer item failed. pointer:%{public}d", pointerEvent->GetPointerId());
            return;
        }
        MMI_HILOGI("MouseEvent Item Normalization Results, DownTime:%{public}" PRId64 ",IsPressed:%{public}d,"
            "GlobalX:%{public}d,GlobalY:%{public}d,LocalX:%{public}d,LocalY:%{public}d,"
            "Width:%{public}d,Height:%{public}d,Pressure:%{public}f,Device:%{public}d",
            item.GetDownTime(), static_cast<int32_t>(item.IsPressed()), item.GetGlobalX(), item.GetGlobalY(),
            item.GetLocalX(), item.GetLocalY(), item.GetWidth(), item.GetHeight(), item.GetPressure(),
            item.GetDeviceId());
    }
    nextHandler_->HandlePointerEvent(pointerEvent);
}

void InputEventNormalizeHandler::HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Touch device does not support");
        return;
    }
    CHKPV(pointerEvent);
    nextHandler_->HandleTouchEvent(pointerEvent);
}

int32_t InputEventNormalizeHandler::HandleKeyboardEvent(libinput_event* event)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Touch device does not support");
        return ERROR_UNSUPPORT;
    }
    auto keyEvent = InputHandler->GetKeyEvent();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    CHKPR(event, ERROR_NULL_POINTER);
    auto packageResult = eventPackage_.PackageKeyEvent(event, keyEvent);
    if (packageResult == MULTIDEVICE_SAME_EVENT_MARK) {
        MMI_HILOGD("The same event reported by multi_device should be discarded");
        return RET_OK;
    }
    if (packageResult != RET_OK) {
        MMI_HILOGE("KeyEvent package failed. ret:%{public}d,errCode:%{public}d", packageResult, KEY_EVENT_PKG_FAIL);
        return KEY_EVENT_PKG_FAIL;
    }
    BytraceAdapter::StartBytrace(keyEvent);
    nextHandler_->HandleKeyEvent(keyEvent);
    Repeat(keyEvent);
    MMI_HILOGD("keyCode:%{public}d,action:%{public}d", keyEvent->GetKeyCode(), keyEvent->GetKeyAction());
    return RET_OK;
}

void InputEventNormalizeHandler::Repeat(const std::shared_ptr<KeyEvent> keyEvent)
{
        if (keyEvent->GetKeyCode() == KeyEvent::KEYCODE_VOLUME_UP ||
            keyEvent->GetKeyCode() == KeyEvent::KEYCODE_VOLUME_DOWN ||
            keyEvent->GetKeyCode() == KeyEvent::KEYCODE_DEL) {
            if (!TimerMgr->IsExist(timerId_) && keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_DOWN) {
                AddHandleTimer();
                MMI_HILOGD("add a timer");
            }
        if (keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_UP && TimerMgr->IsExist(timerId_)) {
            TimerMgr->RemoveTimer(timerId_);
            timerId_ = -1;
        }
    }
}

void InputEventNormalizeHandler::AddHandleTimer(int32_t timeout)
{
    timerId_ = TimerMgr->AddTimer(timeout, 1, [this]() {
        MMI_HILOGD("enter");
        auto keyEvent = InputHandler->GetKeyEvent();
        CHKPV(keyEvent);
        if (keyEvent->GetKeyAction() == KeyEvent::KEY_ACTION_UP) {
            MMI_HILOGD("key up");
            return;
        }
        CHKPV(nextHandler_);
        nextHandler_->HandleKeyEvent(keyEvent);
        constexpr int32_t triggerTime = 100;
        this->AddHandleTimer(triggerTime);
        MMI_HILOGD("leave");
    });
}

int32_t InputEventNormalizeHandler::HandleTouchPadEvent(libinput_event* event){

    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Pointer device does not support");
        return ERROR_UNSUPPORT;
    }
    CHKPR(event, ERROR_NULL_POINTER);
    auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_TOUCH_PAD);
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    nextHandler_->HandlePointerEvent(pointerEvent);
    auto type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_TOUCHPAD_UP) {
        pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
        MMI_HILOGD("This touch pad event is up remove this finger");
        if (pointerEvent->GetPointersIdList().empty()) {
            MMI_HILOGD("This touch pad event is final finger up remove this finger");
            pointerEvent->Reset();
        }
    }
    return RET_OK;
}

int32_t InputEventNormalizeHandler::HandleGestureEvent(libinput_event* event)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Pointer device does not support");
        return ERROR_UNSUPPORT;
    }
    CHKPR(event, ERROR_NULL_POINTER);
    auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_GESTURE);
    CHKPR(pointerEvent, GESTURE_EVENT_PKG_FAIL);
    MMI_HILOGD("GestrueEvent package, eventType:%{public}d,actionTime:%{public}" PRId64 ","
               "action:%{public}d,actionStartTime:%{public}" PRId64 ","
               "pointerAction:%{public}d,sourceType:%{public}d,"
               "PinchAxisValue:%{public}.2f",
                pointerEvent->GetEventType(), pointerEvent->GetActionTime(),
                pointerEvent->GetAction(), pointerEvent->GetActionStartTime(),
                pointerEvent->GetPointerAction(), pointerEvent->GetSourceType(),
                pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_PINCH));

    PointerEvent::PointerItem item;
    pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item);
    MMI_HILOGD("Item:DownTime:%{public}" PRId64 ",IsPressed:%{public}s,"
               "GlobalX:%{public}d,GlobalY:%{public}d,LocalX:%{public}d,LocalY:%{public}d,"
               "Width:%{public}d,Height:%{public}d",
               item.GetDownTime(), (item.IsPressed() ? "true" : "false"),
               item.GetGlobalX(), item.GetGlobalY(), item.GetLocalX(), item.GetLocalY(),
               item.GetWidth(), item.GetHeight());
    nextHandler_->HandlePointerEvent(pointerEvent);
    return RET_OK;
}

int32_t InputEventNormalizeHandler::HandleMouseEvent(libinput_event* event)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Pointer device does not support");
        return ERROR_UNSUPPORT;
    }
    MouseEventHdr->Normalize(event);
    auto pointerEvent = MouseEventHdr->GetPointerEvent();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    auto keyEvent = InputHandler->GetKeyEvent();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    std::vector<int32_t> pressedKeys = keyEvent->GetPressedKeys();
    for (const int32_t& keyCode : pressedKeys) {
        MMI_HILOGI("Pressed keyCode:%{public}d", keyCode);
    }
    pointerEvent->SetPressedKeys(pressedKeys);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    nextHandler_->HandlePointerEvent(pointerEvent);
    return RET_OK;
}

int32_t InputEventNormalizeHandler::HandleTouchEvent(libinput_event* event)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("TP device does not support");
        return ERROR_UNSUPPORT;
    }
    CHKPR(event, ERROR_NULL_POINTER);
    auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_TOUCH);
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    nextHandler_->HandleTouchEvent(pointerEvent);
    auto type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_TOUCH_UP) {
        pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
        MMI_HILOGD("This touch event is up remove this finger");
        if (pointerEvent->GetPointersIdList().empty()) {
            MMI_HILOGD("This touch event is final finger up remove this finger");
            pointerEvent->Reset();
        }
    }
    return RET_OK;
}

int32_t InputEventNormalizeHandler::HandleTableToolEvent(libinput_event* event)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("TP device does not support");
        return ERROR_UNSUPPORT;
    }
    CHKPR(event, ERROR_NULL_POINTER);
    auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_TABLET_TOOL);
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    nextHandler_->HandleTouchEvent(pointerEvent);
    if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_UP) {
        pointerEvent->Reset();
    }
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS