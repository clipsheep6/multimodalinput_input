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
#include "input-event-codes.h"
#include "util.h"
#include "input_windows_manager.h"
#include "input_event_handler.h"
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
    if (pointerEvent_ == nullptr) {
        MMI_LOGF("pointerEvent_ create fail");
    }
}

std::shared_ptr<PointerEvent> MouseEventHandler::GetPointerEvent()
{
    return pointerEvent_;
}

void MouseEventHandler::HandleMotionInner(libinput_event_pointer* data)
{
    MMI_LOGT("enter");
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);

    absolutionX_ += libinput_event_pointer_get_dx(data);
    absolutionY_ += libinput_event_pointer_get_dy(data);

    WinMgr->UpdateAndAdjustMouseLoction(absolutionX_, absolutionY_);

    MMI_LOGD("Change Coordinate : x:%{public}lf, y:%{public}lf",  absolutionX_, absolutionY_);
}

void MouseEventHandler::HandleButonInner(libinput_event_pointer* data, PointerEvent::PointerItem& pointerItem)
{
    MMI_LOGT("enter, current action: %{public}d", pointerEvent_->GetPointerAction());

    auto button = libinput_event_pointer_get_button(data);
    if (button == BTN_LEFT) {
        pointerEvent_->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    } else if (button == BTN_RIGHT) {
        pointerEvent_->SetButtonId(PointerEvent::MOUSE_BUTTON_RIGHT);
    } else if (button == BTN_MIDDLE) {
        pointerEvent_->SetButtonId(PointerEvent::MOUSE_BUTTON_MIDDLE);
    } else {
        MMI_LOGW("unknown btn, btn: %{public}u", button);
    }

    auto state = libinput_event_pointer_get_button_state(data);
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

void MouseEventHandler::HandleAxisInner(libinput_event_pointer* data)
{
    MMI_LOGT("enter");

    if (TimerMgr->IsExist(timerId_)) {
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_AXIS_UPDATE);
        TimerMgr->ResetTimer(timerId_);
        MMI_LOGD("pointer axis update");
    } else {
        const int32_t timeout = 100; // 100 ms
        timerId_ = TimerMgr->AddTimer(timeout, 1, [this]() {
            MMI_LOGD("enter, timer: %{public}d", timerId_);

            timerId_ = -1;

            auto pointerEvent = GetPointerEvent();
            if (pointerEvent == nullptr) {
                MMI_LOGE("the pointerEvent is nullptr");
                return;
            }
            pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_AXIS_END);

            InputHandler->OnMouseEventEndTimerHandler(pointerEvent);
            MMI_LOGD("leave, pointer axis end");
        });

        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_AXIS_BEGIN);
        MMI_LOGD("pointer axis begin");
    }

    if (libinput_event_pointer_has_axis(data, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL)) {
        auto axisValue = libinput_event_pointer_get_axis_value(data, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
        pointerEvent_->SetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL, axisValue);
    }
    if (libinput_event_pointer_has_axis(data, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL)) {
        auto axisValue = libinput_event_pointer_get_axis_value(data, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);
        pointerEvent_->SetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL, axisValue);
    }
}

void MouseEventHandler::HandlePostInner(libinput_event_pointer* data, PointerEvent::PointerItem& pointerItem)
{
    MMI_LOGT("enter");

    auto mouseInfo = WinMgr->GetMouseInfo();
    MMI_LOGD("mouseInfo: globleX=%{public}d globleY=%{public}d localX=%{public}d localY=%{public}d", 
                        mouseInfo.globleX, mouseInfo.globleY, mouseInfo.localX, mouseInfo.localY);
    MouseState->SetMouseCoords(mouseInfo.globleX, mouseInfo.globleY);
    pointerItem.SetGlobalX(mouseInfo.globleX);
    pointerItem.SetGlobalY(mouseInfo.globleY);
    pointerItem.SetLocalX(mouseInfo.localX);
    pointerItem.SetLocalY(mouseInfo.localY);
    pointerItem.SetPointerId(0);

    uint64_t time = libinput_event_pointer_get_time_usec(data);
    int32_t deviceId = 0;
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

void MouseEventHandler::Normalize(libinput_event *event)
{
    CHK(event, PARAM_INPUT_INVALID);

    MMI_LOGD("Mouse Process Start");
    auto data = libinput_event_get_pointer_event(event);
    CHKP(data, ERROR_NULL_POINTER);    

    PointerEvent::PointerItem pointerItem;
    const int32_t type = libinput_event_get_type(event);
    switch (type) {
        case LIBINPUT_EVENT_POINTER_MOTION:
        case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE: {
            HandleMotionInner(data);
            break;
        }
        case LIBINPUT_EVENT_POINTER_BUTTON: {
            HandleButonInner(data, pointerItem);
            break;
        }
        case LIBINPUT_EVENT_POINTER_AXIS: {
            HandleAxisInner(data);
            break;
        }
        default: {
            MMI_LOGW("unknow type: %{public}d", type);
            break;
        }
    }

    HandlePostInner(data, pointerItem); 

    // ���� ��Ϣ���
    DumpInner();
}

void MouseEventHandler::DumpInner()
{
    MMI_LOGI("PointerAction: %{public}d, PointerId: %{public}d, SourceType: %{public}d,"
        "ButtonId: %{public}d, VerticalAxisValue: %{public}lf, HorizontalAxisValue: %{public}lf",
        pointerEvent_->GetPointerAction(), pointerEvent_->GetPointerId(), pointerEvent_->GetSourceType(),
        pointerEvent_->GetButtonId(), pointerEvent_->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
        pointerEvent_->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL));

    PointerEvent::PointerItem item;
    pointerEvent_->GetPointerItem(pointerEvent_->GetPointerId(), item);
    MMI_LOGI("item: DownTime: %{public}d, IsPressed: %{public}d,"
        "GlobalX: %{public}d, GlobalY: %{public}d, LocalX: %{public}d, LocalY: %{public}d, Width: %{public}d,"
        "Height: %{public}d, Pressure: %{public}d, DeviceId: %{public}d",
        item.GetDownTime(), static_cast<int32_t>(item.IsPressed()), item.GetGlobalX(), item.GetGlobalY(),
        item.GetLocalX(), item.GetLocalY(), item.GetWidth(), item.GetHeight(), item.GetPressure(),
        item.GetDeviceId());
}
} // namespace MMI
} // namespace OHOS

