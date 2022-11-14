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

#include "input_event_handler.h"
#include "input_windows_manager.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "TouchScreenHandler" };
};

std::shared_ptr<TouchScreenHandler> TouchScreenHandler::CreateInstance()
{
    return std::shared_ptr<TouchScreenHandler>(new TouchScreenHandler());
}

TouchScreenHandler::TouchScreenHandler() {}

int32_t TouchScreenHandler::BindInputDevice(const std::shared_ptr<IDevice> inputDevice)
{
    CALL_DEBUG_ENTER;
    CHKPR(inputDevice, ERROR_NULL_POINTER);
    inputDevice_ = inputDevice;
    return RET_OK;
}

int32_t TouchScreenHandler::UnbindInputDevice(const std::shared_ptr<IDevice> inputDevice)
{
    CALL_DEBUG_ENTER;
    if (inputDevice != inputDevice_) {
        MMI_HILOGE("inputDevice != inputDevice_");
        return RET_ERR;
    }
    inputDevice_.reset();
    return RET_OK;
}

void TouchScreenHandler::ResetTouchUpEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPV(pointerEvent);
    pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
    if (pointerEvent->GetPointerIds().empty()) {
        MMI_HILOGD("This touch event is final finger up remove this finger");
        pointerEvent->Reset();
    }
}

void TouchScreenHandler::OnInputEvent(const std::shared_ptr<AbsEvent> event)
{
    CALL_DEBUG_ENTER;
    CHKPV(event);
    if (pointerEvent_ == nullptr) {
        pointerEvent_ = PointerEvent::Create();
        CHKPV(pointerEvent_);
    }
    auto pointerEvent = TransformToPointerEvent(event);
    if (!pointerEvent) {
        MMI_HILOGE("TransformToPointerEvent Failed");
        return;
    }
    auto inputEventNormalizeHandler = InputHandler->GetEventNormalizeHandler();
    CHKPV(inputEventNormalizeHandler);
    inputEventNormalizeHandler->HandleTouchEvent(pointerEvent);
    if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_UP) {
        ResetTouchUpEvent(pointerEvent);
    }
}

const std::shared_ptr<PointerEvent> TouchScreenHandler::TransformToPointerEvent(const std::shared_ptr<AbsEvent> event)
{
    CALL_DEBUG_ENTER;
    CHKPP(event);
    auto action = event->GetAction();
    switch (action) {
        case AbsEvent::ACTION_DOWN: {
            if (!OnEventTouchDown(event)) {
                MMI_HILOGE("Get OnEventTouchDown failed");
                return nullptr;
            }
            break;
        }
        case AbsEvent::ACTION_UP: {
            if (!OnEventTouchUp(event)) {
                MMI_HILOGE("Get OnEventTouchUp failed");
                return nullptr;
            }
            break;
        }
        case AbsEvent::ACTION_MOVE: {
            if (!OnEventTouchMotion(event)) {
                MMI_HILOGE("Get OnEventTouchMotion failed");
                return nullptr;
            }
            break;
        }
        default: {
            MMI_HILOGE("Unknown event Action:%{public}d", action);
            return nullptr;
        }
    }
    pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent_->UpdateId();
    return pointerEvent_;
}

bool TouchScreenHandler::OnEventTouchDown(const std::shared_ptr<AbsEvent> absEvent)
{
    CHKPF(absEvent);
    CHKPF(inputDevice_);
    int32_t deviceId = inputDevice_->GetDeviceId();
    EventTouch touchInfo;
    int32_t logicalDisplayId = -1;
    if (!WinMgr->TouchPointToDisplayPoint(deviceId, absEvent, touchInfo, logicalDisplayId)) {
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
    double pressure = 0;    //TO DO ...
    int32_t seatSlot = absEvent->GetCurSlot();
    int32_t longAxis = 0;   //TO DO ...
    int32_t shortAxis = 0;  //TO DO ...
    item.SetPressure(pressure);
    item.SetLongAxis(longAxis);
    item.SetShortAxis(shortAxis);
    int32_t toolType = 0;   //TO DO ...
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
    item.SetDeviceId(deviceId);
    pointerEvent_->SetDeviceId(deviceId);
    pointerEvent_->AddPointerItem(item);
    pointerEvent_->SetPointerId(seatSlot);
    return true;
}

bool TouchScreenHandler::OnEventTouchUp(const std::shared_ptr<AbsEvent> absEvent)
{
    CHKPF(absEvent);
    int64_t time = GetSysClockTime();
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    PointerEvent::PointerItem item;
    int32_t seatSlot = absEvent->GetCurSlot();
    if (!(pointerEvent_->GetPointerItem(seatSlot, item))) {
        MMI_HILOGE("Get pointer parameter failed, seatSlot:%{public}d", seatSlot);
        return false;
    }
    item.SetPressed(false);
    pointerEvent_->UpdatePointerItem(seatSlot, item);
    pointerEvent_->SetPointerId(seatSlot);
    return true;
}

bool TouchScreenHandler::OnEventTouchMotion(const std::shared_ptr<AbsEvent> absEvent)
{
    CHKPF(absEvent);
    CHKPF(inputDevice_);
    pointerEvent_->SetActionTime(GetSysClockTime());
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    int32_t deviceId = inputDevice_->GetDeviceId();
    EventTouch touchInfo;
    int32_t logicalDisplayId = pointerEvent_->GetTargetDisplayId();
    if (!WinMgr->TouchPointToDisplayPoint(deviceId, absEvent, touchInfo, logicalDisplayId)) {
        MMI_HILOGE("Get TouchPointToDisplayPoint failed");
        return false;
    }
    int32_t seatSlot = absEvent->GetCurSlot();
    PointerEvent::PointerItem item;
    if (!(pointerEvent_->GetPointerItem(seatSlot, item))) {
        MMI_HILOGE("Get pointer parameter failed, seatSlot:%{public}d", seatSlot);
        return false;
    }
    double pressure = 0;     //TO DO...
    int32_t longAxis = 0;    //TO DO...
    int32_t shortAxis = 0;   //TO DO...
    item.SetPressure(pressure);
    item.SetLongAxis(longAxis);
    item.SetShortAxis(shortAxis);
    item.SetDisplayX(touchInfo.point.x);
    item.SetDisplayY(touchInfo.point.y);
    item.SetToolDisplayX(touchInfo.toolRect.point.x);
    item.SetToolDisplayY(touchInfo.toolRect.point.y);
    item.SetToolWidth(touchInfo.toolRect.width);
    item.SetToolHeight(touchInfo.toolRect.height);
    pointerEvent_->UpdatePointerItem(seatSlot, item);
    pointerEvent_->SetPointerId(seatSlot);
    return true;
}
} // namespace MMI
} // namespace OHOS