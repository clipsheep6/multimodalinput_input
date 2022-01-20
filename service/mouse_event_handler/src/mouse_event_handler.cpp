/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "mouse_event_handler.h"
#include "libmmi_util.h"
#include "util.h"
#include "input_windows_manager.h"
#include "timer_manager.h"
#include "mouse_state_gesture.h"

namespace OHOS {
namespace MMI {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "MouseEventHandler"};
}
MouseEventHandler::MouseEventHandler()
{
    pointerEvent_ = PointerEvent::Create();
}

MouseEventHandler::~MouseEventHandler()
{
}

void MouseEventHandler::CalcMovedCoordinate(struct libinput_event_pointer& pointEventData)
{
    absolutionX_ += libinput_event_pointer_get_dx(&pointEventData);
    absolutionY_ += libinput_event_pointer_get_dy(&pointEventData);

    WinMgr->AdjustCoordinate(absolutionX_, absolutionY_);
    MMI_LOGI("absolutionX_ is : %{public}lf, absolutionY_ is : %{public}lf", absolutionX_, absolutionY_);
}

void MouseEventHandler::SetMouseMotion(PointerEvent::PointerItem& pointerItem)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
}

void MouseEventHandler::DoHandleMouseButon(PointerEvent::PointerItem& pointerItem, struct libinput_event_pointer& pointEventData)
{
    MMI_LOGT("enter, current action: %{public}d", pointerEvent_->GetPointerAction());

    auto button = libinput_event_pointer_get_button(&pointEventData);
    if (button == BTN_LEFT) {
        pointerEvent_->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    } else if (button == BTN_RIGHT) {
        pointerEvent_->SetButtonId(PointerEvent::MOUSE_BUTTON_RIGHT);
    } else if (button == BTN_MIDDLE) {
        pointerEvent_->SetButtonId(PointerEvent::MOUSE_BUTTON_MIDDLE);
    } else {
        MMI_LOGW("unknown btn, btn: %{public}u", button);
    }

    auto state = libinput_event_pointer_get_button_state(&pointEventData);
    if (state == LIBINPUT_BUTTON_STATE_RELEASED) {
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);        
        pointerEvent_->SetButtonId(PointerEvent::BUTTON_NONE);
        pointerItem.SetPressed(false);
    } else if (state == LIBINPUT_BUTTON_STATE_PRESSED) {
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
        pointerEvent_->SetButtonPressed(state);
        pointerItem.SetPressed(true);
    } else {
        MMI_LOGW("unknown state, state: %{public}u", state);
    }

    MouseState->CountState(button, state);
}

void MouseEventHandler::DoHandleMouseAxis(struct libinput_event_pointer& pointEventData)
{
    if (TimerMgr->IsExist(timerId_)) {
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_AXIS_UPDATE);
        TimerMgr->ResetTimer(timerId_);
        MMI_LOGI("pointer axis event update");
    } else {
        const int32_t mouseTimeout = 100;
        timerId_ = TimerMgr->AddTimer(mouseTimeout, 1, []() {
            const int32_t defaultTimerId = -1;
            MouseEvent->SetTimerId(defaultTimerId);
            MMI_LOGI("pointer axis event end TimerCallback run");
            MouseEvent->SetMouseAction(PointerEvent::POINTER_ACTION_AXIS_END);
            auto pointerEvent = MouseEvent->GetPointerEventPtr();
            if (pointerEvent == nullptr) {
                MMI_LOGE("the pointerEvent is nullptr");
                return;
            }
            InputHandler->OnMouseEventTimerHanler(pointerEvent);
            MMI_LOGI("pointer axis event end");
        });
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_AXIS_BEGIN);
        MMI_LOGI("pointer axis event begin");
    }

    double axisValue = 0;
    if (libinput_event_pointer_has_axis(&pointEventData, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL)) {
        axisValue = libinput_event_pointer_get_axis_value(&pointEventData,
                                                          LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
        pointerEvent_->SetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL, axisValue);
    }
    if (libinput_event_pointer_has_axis(&pointEventData, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL)) {
        axisValue = libinput_event_pointer_get_axis_value(&pointEventData,
                                                          LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);
        pointerEvent_->SetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL, axisValue);
    }
}

void MouseEventHandler::Normalize(libinput_event *event, int32_t deviceId)
{
    CHK(event, PARAM_INPUT_INVALID);
    MMI_LOGD("Mouse Process Start");
    auto libinputPointerEvent = libinput_event_get_pointer_event(event);
    CHKP(libinputPointerEvent, ERROR_NULL_POINTER);    

    PointerEvent::PointerItem pointerItem;
    const int32_t type = libinput_event_get_type(event);
    if ((type == LIBINPUT_EVENT_POINTER_MOTION) || (type == LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE)) { // mouse move
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);

        absolutionX_ += libinput_event_pointer_get_dx(libinputPointerEvent);
        absolutionY_ += libinput_event_pointer_get_dy(libinputPointerEvent);

        WinMgr->UpdateAndAdjustMouseLoction(absolutionX_, absolutionY_);

        MMI_LOGD("Change Coordinate : x:%{public}lf, y:%{public}lf",  absolutionX_, absolutionY_);        
    } else if (type == LIBINPUT_EVENT_POINTER_BUTTON) { // mouse button
        DoHandleMouseButon(pointerItem, *libinputPointerEvent);
    } else if (type == LIBINPUT_EVENT_POINTER_AXIS) { // mouse axis
        DoHandleMouseAxis(*libinputPointerEvent);
    }

    auto mouseInfo = WinMgr->GetMouseInfo();
    MMI_LOGD("mouseInfo: globleX=%{public}d globleY=%{public}d localX=%{public}d localY=%{public}d", 
                        mouseInfo.globleX, mouseInfo.globleY, mouseInfo.localX, mouseInfo.localY);
    MouseState->SetMouseCoords(mouseInfo.globleX, mouseInfo.globleY);
    pointerItem.SetGlobalX(mouseInfo.globleX);
    pointerItem.SetGlobalY(mouseInfo.globleY);
    pointerItem.SetLocalX(mouseInfo.localX);
    pointerItem.SetLocalY(mouseInfo.localY);
    pointerItem.SetPointerId(0);

    uint64_t time = libinput_event_pointer_get_time_usec(libinputPointerEvent);
    pointerItem.SetDownTime(static_cast<int32_t>(time));
    pointerItem.SetWidth(0);
    pointerItem.SetHeight(0);
    pointerItem.SetPressure(0);
    pointerItem.SetDeviceId(deviceId);

    pointerEvent_->AddPointerItem(pointerItem);
    pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent_->SetActionTime(static_cast<int32_t>(GetSysClockTime()));
    pointerEvent_->SetActionStartTime(static_cast<int32_t>(time));
    pointerEvent_->SetDeviceId(deviceId);
    pointerEvent_->SetPointerId(0);
    pointerEvent_->SetTargetDisplayId(-1);
    pointerEvent_->SetTargetWindowId(-1);
    pointerEvent_->SetAgentWindowId(-1);

    // 调试 信息输出

    // MouseEvent Normalization Results
    MMI_LOGI("MouseEvent Normalization Results : PointerAction = %{public}d, PointerId = %{public}d,"
        "SourceType = %{public}d, ButtonId = %{public}d,"
        "VerticalAxisValue = %{public}lf, HorizontalAxisValue = %{public}lf",
        pointerEvent->GetPointerAction(), pointerEvent->GetPointerId(), pointerEvent->GetSourceType(),
        pointerEvent->GetButtonId(), pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
        pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL));
    PointerEvent::PointerItem item;
    pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item);
    MMI_LOGI("MouseEvent Item Normalization Results : DownTime = %{public}d, IsPressed = %{public}d,"
        "GlobalX = %{public}d, GlobalY = %{public}d, LocalX = %{public}d, LocalY = %{public}d, Width = %{public}d,"
        "Height = %{public}d, Pressure = %{public}d, DeviceId = %{public}d",
        item.GetDownTime(), static_cast<int32_t>(item.IsPressed()), item.GetGlobalX(), item.GetGlobalY(),
        item.GetLocalX(), item.GetLocalY(), item.GetWidth(), item.GetHeight(), item.GetPressure(),
        item.GetDeviceId());
}


HanleKey()
{
    
        
        std::vector<int32_t> pressedKeys = keyEvent->GetPressedKeys();
        if (pressedKeys.empty()) {
            MMI_LOGI("Pressed keys is empty");
        } else {
            for (int32_t keyCode : pressedKeys) {
                MMI_LOGI("Pressed keyCode=%{public}d", keyCode);
            }
        }
        pointerEvent->SetPressedKeys(pressedKeys);
}

void MouseEventHandler::SetMouseAction(const int32_t action)
{
    pointerEvent_->SetPointerAction(action);
}

std::shared_ptr<PointerEvent> MouseEventHandler::GetPointerEventPtr()
{
    return pointerEvent_;
}
void MouseEventHandler::SetTimerId(const int32_t id)
{
    timerId_ = id;
}
} // namespace MMI
} // namespace OHOS

