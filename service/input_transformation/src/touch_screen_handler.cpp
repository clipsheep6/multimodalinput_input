/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "touch_screen_handler.h"

#include "i_input_context.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "TouchScreenHandler" };
};
std::unique_ptr<TouchScreenHandler> TouchScreenHandler::CreateInstance(const IInputContext* context) 
{
    if (context == nullptr) {
         return nullptr;
     }
    return std::unique_ptr<TouchScreenHandler>(new TouchScreenHandler(context));
}

TouchScreenHandler::TouchScreenHandler(const IInputContext* context) 
        : context_(context)
{
}

std::shared_ptr<PointerEvent> TouchScreenHandler::GetPointerEvent()
{
    return pointerEvent_;
}

void TouchScreenHandler::OnInputEvent(const std::shared_ptr<const AbsEvent>& event)
{
    MMI_HILOGD("Enter absEvent:%{public}p", event.get());
    int32_t pointerAction = PointerEvent::POINTER_ACTION_UNKNOWN;
    int64_t actionTime = 0;
    auto ret = ConvertPointer(event, pointerAction, actionTime);
    if (!ret) {
        MMI_HILOGE("Leave ConvertPointer Failed");
        return;
    }

    /*auto retCode = DispatchTo(pointerAction, actionTime,  pointer);
    if (retCode < 0) {
        MMI_HILOGE("Leave, Dispatch Failed");
        return;
    }*/

    MMI_HILOGD("Leave,  pointerAction:%{public}d", pointerAction);
}

int32_t TouchScreenHandler::DispatchTo(int32_t pointerAction, int64_t actionTime, std::shared_ptr<PointerEvent::PointerItem>& pointer)
{

    // auto pointerEvent = targetDisplay->HandleEvent(pointerAction, actionTime, pointer);
    // if (!pointerEvent) {
    //     MMI_HILOGE("Leave, null pointerEvent");
    //     return -1;
    // }

    if (context_ == nullptr) {
        MMI_HILOGE("Leave, null context_");
        return -1;
    }
    // auto inputEventNormalizeHandler = InputHandler->GetEventNormalizeHandler();
    // CHKPR(inputEventNormalizeHandler, ERROR_NULL_POINTER);
    // inputEventNormalizeHandler->HandleTouchEvent(pointerEvent);
    return 0;
}

bool TouchScreenHandler::ConvertPointer(const std::shared_ptr<const AbsEvent>& absEvent,
        int32_t& pointerAction, int64_t& actionTime)
{
    std::shared_ptr<PointerEvent::PointerItem> pointer;
    if (!absEvent) {
        MMI_HILOGE("Leave, null absEvent");
        return false;
    }
    const auto& absEventPointer = absEvent->GetPointer();
    if (!absEventPointer) {
        MMI_HILOGE("Leave, null absEventPointer");
        return false;
    }

    if (context_ == nullptr) {
        MMI_HILOGE("Leave, null context_");
        return false;
    }

    auto action = ConvertAction(absEvent->GetAction());
    if (action == PointerEvent::POINTER_ACTION_UNKNOWN) {
        MMI_HILOGE("Leave, ConvertAction Failed");
        return false;
    }

    switch (action) {
        case PointerEvent::POINTER_ACTION_DOWN: {
           /*if (!OnEventTouchDown(absEvent)) {
                MMI_HILOGE("Get OnEventTouchDown failed");
                return false;
            }*/
            break;
        }
        case PointerEvent::POINTER_ACTION_UP: {
            break;
        }
        case PointerEvent::POINTER_ACTION_MOVE: {
            break;
        }
        default: {
            MMI_HILOGE("Leave, unknown absEvent Action:%{public}s", AbsEvent::ActionToString(action));
            return false;
        }
    }

    MMI_HILOGD("Leave");
    return true;
}

int32_t TouchScreenHandler::ConvertAction(int32_t absEventAction) const
{
    if (absEventAction == AbsEvent::ACTION_DOWN) {
        return PointerEvent::POINTER_ACTION_DOWN;
    }

    if (absEventAction == AbsEvent::ACTION_UP) {
        return PointerEvent::POINTER_ACTION_UP;
    }

    if (absEventAction == AbsEvent::ACTION_MOVE) {
        return PointerEvent::POINTER_ACTION_MOVE;
    }

    return PointerEvent::POINTER_ACTION_UNKNOWN;
}

bool TouchScreenHandler::OnEventTouchDown(std::shared_ptr<const AbsEvent>& absEvent)
{
#if 0
    CALL_DEBUG_ENTER;
    CHKPF(absEvent);
    auto touch = libinput_event_get_touch_event(event);
    CHKPF(touch);
    auto device = libinput_event_get_device(event);
    CHKPF(device);
    EventTouch touchInfo;
    int32_t logicalDisplayId = -1;
    if (!WinMgr->TouchPointToDisplayPoint(deviceId_, touch, touchInfo, logicalDisplayId)) {
        MMI_HILOGE("TouchDownPointToDisplayPoint failed");
        return false;
    }
    auto pointIds = pointerEvent_->GetPointerIds();
    int64_t time = GetSysClockTime();
    if (pointIds.empty()) {
        pointerEvent_->SetActionStartTime(time);
        pointerEvent_->SetTargetDisplayId(logicalDisplayId);
    }
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);

    PointerEvent::PointerItem item;
    double pressure = libinput_event_touch_get_pressure(touch);
    int32_t seatSlot = libinput_event_touch_get_seat_slot(touch);
    int32_t longAxis = libinput_event_get_touch_contact_long_axis(touch);
    int32_t shortAxis = libinput_event_get_touch_contact_short_axis(touch);
    item.SetPressure(pressure);
    item.SetLongAxis(longAxis);
    item.SetShortAxis(shortAxis);
    int32_t toolType = GetTouchToolType(touch, device);
    item.SetToolType(toolType);
    item.SetPointerId(seatSlot);
    item.SetDownTime(time);
    item.SetPressed(true);
    item.SetDisplayX(touchInfo.point.x);
    item.SetDisplayY(touchInfo.point.y);
    item.SetToolDisplayX(touchInfo.toolRect.point.x);
    item.SetToolDisplayY(touchInfo.toolRect.point.y);
    item.SetToolWidth(touchInfo.toolRect.width);
    item.SetToolHeight(touchInfo.toolRect.height);
    item.SetDeviceId(deviceId_);
    pointerEvent_->SetDeviceId(deviceId_);
    pointerEvent_->AddPointerItem(item);
    pointerEvent_->SetPointerId(seatSlot);
    EventLogHelper::PrintEventData(pointerEvent_, pointerEvent_->GetPointerAction(),
        pointerEvent_->GetPointerIds().size());
#endif
    return true;
}
} // namespace MMI
} // namespace OHOS