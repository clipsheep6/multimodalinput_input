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
    coordinateX_ += libinput_event_pointer_get_dx(&pointEventData);
    coordinateY_ += libinput_event_pointer_get_dy(&pointEventData);

    WinMgr->AdjustCoordinate(coordinateX_, coordinateY_);
    MMI_LOGI("coordinateX_ is : %{public}lf, coordinateY_ is : %{public}lf", coordinateX_, coordinateY_);
}

void OHOS::MMI::MouseEventHandler::SetMouseMotion(PointerEvent::PointerItem& pointerItem)
{
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
}

void MouseEventHandler::SetMouseButon(PointerEvent::PointerItem& pointerItem,
                                      struct libinput_event_pointer& pointEventData)
{
    auto button = libinput_event_pointer_get_button(&pointEventData);
    if (button == BTN_LEFT) {
        pointerEvent_->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    } else if (button == BTN_RIGHT) {
        pointerEvent_->SetButtonId(PointerEvent::MOUSE_BUTTON_RIGHT);
    } else if (button == BTN_MIDDLE) {
        pointerEvent_->SetButtonId(PointerEvent::MOUSE_BUTTON_MIDDLE);
    } else {
        MMI_LOGW("PointerAction : %{public}d, unProces Button code : %{public}u",
                 pointerEvent_->GetPointerAction(), button);
    }
    bool isPressed = false;
    auto state = libinput_event_pointer_get_button_state(&pointEventData);
    if (state == LIBINPUT_BUTTON_STATE_RELEASED) {
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_UP);
        isPressed = false;
    } else if (state == LIBINPUT_BUTTON_STATE_PRESSED) {
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
        pointerEvent_->SetButtonPressed(state);
        isPressed = true;
    }
    MouseState->CountState(button, state);
    pointerItem.SetPressed(isPressed);
}

void MouseEventHandler::SetMouseAxis(struct libinput_event_pointer& pointEventData)
{
    if (TimerMgr->IsExist(timerId_)) {
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_AXIS_UPDATE);
        TimerMgr->ResetTimer(timerId_);
        MMI_LOGI("pointer axis event update");
    } else {
        const int32_t mouseTimeout = 100;
        timerId_ = TimerMgr->AddTimer(mouseTimeout, 1, []() {
            const int32_t defaultTimerId = -1;
            MouseEvent.SetTimerId(defaultTimerId);
            MMI_LOGI("pointer axis event end TimerCallback run");
            MouseEvent.SetMouseAction(PointerEvent::POINTER_ACTION_AXIS_END);
            auto pointerEvent = MouseEvent.GetPointerEventPtr();
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

void MouseEventHandler::ProcessMouseData(libinput_event *event, int32_t deviceId)
{
    CHK(event, PARAM_INPUT_INVALID);
    auto pointEventData = libinput_event_get_pointer_event(event);
    CHKP(pointEventData, ERROR_NULL_POINTER);
    int32_t type = libinput_event_get_type(event);
    PointerEvent::PointerItem pointerItem;
    if ((type == LIBINPUT_EVENT_POINTER_MOTION) || (type == LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE)) {
        CalcMovedCoordinate(*pointEventData);
        WinMgr->SetMouseInfo(coordinateX_, coordinateY_);
        MMI_LOGI("Change Coordinate : coordinateX_ = %{public}lf, coordinateY_ = %{public}lf",
                 coordinateX_, coordinateY_);
        SetMouseMotion(pointerItem);
    } else if (type == LIBINPUT_EVENT_POINTER_BUTTON) {
        SetMouseButon(pointerItem, *pointEventData);
    } else if (type == LIBINPUT_EVENT_POINTER_AXIS) {
        SetMouseAxis(*pointEventData);
    }

    auto mouseInfo = WinMgr->GetMouseInfo();
    MMI_LOGD("mouseInfo.globleX=%{public}d mouseInfo.globleY=%{public}d mouseInfo.localX=%{public}d mouseInfo.localY=%{public}d", 
                        mouseInfo.globleX, mouseInfo.globleY, mouseInfo.localX, mouseInfo.localY);
    MouseState->SetMouseCoords(mouseInfo.globleX, mouseInfo.globleY);
    pointerItem.SetGlobalX(mouseInfo.globleX);
    pointerItem.SetGlobalY(mouseInfo.globleY);
    pointerItem.SetLocalX(mouseInfo.localX);
    pointerItem.SetLocalY(mouseInfo.localY);
    pointerItem.SetPointerId(0);

    uint64_t time = libinput_event_pointer_get_time_usec(pointEventData);
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

