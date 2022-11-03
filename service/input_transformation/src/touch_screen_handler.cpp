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

#include "event_log_helper.h"
#include "i_input_context.h"
#include "input_device_manager.h"
#include "input_event_handler.h"
#include "input_event_handler.h"
#include "input_windows_manager.h"
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

int32_t TouchScreenHandler::BindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice)
{
    CALL_DEBUG_ENTER;
    if (!inputDevice) {
        MMI_HILOGE("Leave, null InputDevice");
        return -1;
    }

    std::shared_ptr<IInputDevice::AxisInfo> xInfo = inputDevice->GetAxisInfo(IInputDevice::AXIS_MT_X);
    if (!xInfo) {
        MMI_HILOGE("Leave, null AxisInfo Of AXIS_MT_X");
        return -1;
    }

    if (xInfo->GetMinimum() >= xInfo->GetMaximum()) {
        MMI_HILOGE("Leave, xInfo->GetMinimum():%{public}d >= xInfo->GetMaximum():%{public}d",
                    xInfo->GetMinimum(), xInfo->GetMaximum());
        return -1;
    }

    std::shared_ptr<IInputDevice::AxisInfo> yInfo = inputDevice->GetAxisInfo(IInputDevice::AXIS_MT_Y);
    if (!yInfo) {
        MMI_HILOGE("Leave, null AxisInfo Of AXIS_MT_Y");
        return -1;
    }

    if (yInfo->GetMinimum() >= yInfo->GetMaximum()) {
        MMI_HILOGE("Leave, yInfo->GetMinimum():%{public}d >= yInfo->GetMaximum():%{public}d",
                    yInfo->GetMinimum(), yInfo->GetMaximum());
        return -1;
    }

    inputDevice_ = inputDevice;
    xInfo_ = xInfo;
    yInfo_ = yInfo;
    return 0;
}

int32_t TouchScreenHandler::UnbindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice)
{
    CALL_DEBUG_ENTER;
    if (inputDevice != inputDevice_) {
        MMI_HILOGE("Leave, inputDevice != inputDevice_");
        return -1;
    }

    inputDevice_.reset();
    return 0;
}

void TouchScreenHandler::ResetTouchUpEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPV(pointerEvent);
    pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
    if (pointerEvent->GetPointerIds().empty()) {
        MMI_HILOGE("This touch event is final finger up remove this finger");
        pointerEvent->Reset();
    }
}

void TouchScreenHandler::OnInputEvent(const std::shared_ptr<const AbsEvent>& event)
{
    CALL_DEBUG_ENTER;
    int32_t pointerAction = PointerEvent::POINTER_ACTION_UNKNOWN;
    int64_t actionTime = 0;

    if (pointerEvent_ == nullptr) {
        pointerEvent_ = PointerEvent::Create();
        CHKPV(pointerEvent_);
    }

    auto ret = ConvertPointer(event, pointerAction, actionTime);
    if (!ret) {
        MMI_HILOGE("Leave ConvertPointer Failed");
    }
    auto inputEventNormalizeHandler = InputHandler->GetEventNormalizeHandler();
    CHKPV(inputEventNormalizeHandler);   
    CHKPV(pointerEvent_);
    inputEventNormalizeHandler->HandleTouchEvent(pointerEvent_);
    if (pointerAction == PointerEvent::POINTER_ACTION_UP) {
        ResetTouchUpEvent(pointerEvent_);
    }
}

bool TouchScreenHandler::ConvertPointer(const std::shared_ptr<const AbsEvent>& absEvent,
        int32_t& pointerAction, int64_t& actionTime)
{
    CALL_DEBUG_ENTER;
    CHKPF(absEvent);

    auto action = ConvertAction(absEvent->GetAction());
    pointerAction = action;
    if (action == PointerEvent::POINTER_ACTION_UNKNOWN) {
        MMI_HILOGE("Leave, ConvertAction Failed");
        return false;
    }
    switch (action) {
        case PointerEvent::POINTER_ACTION_DOWN: {
            if (!OnEventTouchDown(absEvent)) {
                MMI_HILOGE("Get OnEventTouchDown failed");
                return false;
            }
            break;
        }
        case PointerEvent::POINTER_ACTION_UP: {
            if (!OnEventTouchUp(absEvent)) {
                MMI_HILOGE("Get OnEventTouchUp failed");
                return false;
            }
            break;
        }
        case PointerEvent::POINTER_ACTION_MOVE: {
            if (!OnEventTouchMotion(absEvent)) {
                MMI_HILOGE("Get OnEventTouchMotion failed");
                return false;
            }
            break;
        }
        default: {
            MMI_HILOGE("Leave, unknown absEvent Action:%{public}d", action);
            return false;
        }
    }

    pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent_->UpdateId();
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

int32_t TouchScreenHandler::TransformX(int32_t xPos, int32_t width, int32_t logicalWidth) const
{
    if (width <= 0) {
        return -1;
    }

    int64_t one = 1;
    auto result = (int32_t)(one * xPos * logicalWidth / width);
    MMI_HILOGD("xPos:%{public}d width:%{public}d logicalWidth:%{public}d result:%{public}d",
            xPos, width, logicalWidth, result);
    return result;
}

int32_t TouchScreenHandler::TransformY(int32_t yPos, int32_t height, int32_t logicalHeight) const
{
    if (height <= 0) {
        return -1;
    }

    int64_t one = 1;
    auto result = (int32_t)(one * yPos * logicalHeight / height);
    MMI_HILOGD("yPos:%{public}d height:%{public}d logicalHeight:%{public}d result:%{public}d",
            yPos, height, logicalHeight, result);
    return result;
}

int32_t TouchScreenHandler::TransformToPhysicalDisplayCoordinate(const DisplayInfo& info,
        int32_t tpX, int32_t tpY, int32_t& displayX, int32_t& displayY) const
{
    CALL_DEBUG_ENTER;
    if (!xInfo_) {
        MMI_HILOGE("Leave, null xInfo_");
        return -1;
    }

    if (!yInfo_) {
        MMI_HILOGE("Leave, null yInfo_");
        return -1;
    }

    int32_t deltaX = tpX  - xInfo_->GetMinimum();
    int32_t width = xInfo_->GetMaximum() - xInfo_->GetMinimum() + 1;
    displayX = TransformX(deltaX, width, info.width);

    int32_t deltaY = tpY - yInfo_->GetMinimum();
    int32_t height = yInfo_->GetMaximum() - yInfo_->GetMinimum() + 1;
    displayY = TransformY(deltaY, height, info.height);
    return 0;
}

void TouchScreenHandler::GetPhysicalDisplayCoord(const std::shared_ptr<const AbsEvent>& absEvent,
    const DisplayInfo& info, EventTouch& touchInfo)
{
    const auto& absEventPointer = absEvent->GetPointer();
    if (!absEventPointer) {
        MMI_HILOGE("Leave, null absEventPointer");
        return;
    }

    int32_t physicalDisplayX = -1;
    int32_t physicalDisplayY = -1;
    auto retCode = TransformToPhysicalDisplayCoordinate(info, absEventPointer->GetX(), absEventPointer->GetY(),
            physicalDisplayX, physicalDisplayY);
    if (retCode < 0) {
        MMI_HILOGE("Leave, TransformToPhysicalDisplayCoordinate Failed");
        return;
    }
    touchInfo.point.x = physicalDisplayX;
    touchInfo.point.y = physicalDisplayY;
    touchInfo.toolRect.point.x = 0;   //TO DO...
    touchInfo.toolRect.point.y = 0;   //TO DO...
    touchInfo.toolRect.width = 0;     //TO DO...
    touchInfo.toolRect.height = 0;    //TO DO...
}

bool TouchScreenHandler::TouchPointToDisplayPoint(int32_t deviceId, const std::shared_ptr<const AbsEvent>& absEvent,
                                                  EventTouch& touchInfo, int32_t& physicalDisplayId)
{
    CHKPF(absEvent);
    std::string screenId = InputDevMgr->GetScreenId(deviceId);
    if (screenId.empty()) {
        screenId = "default0";
    }
    auto info = WinMgr->FindPhysicalDisplayInfo(screenId);
    CHKPF(info);
    physicalDisplayId = info->id;
    if ((info->width <= 0) || (info->height <= 0)) {
        MMI_HILOGE("Get DisplayInfo is error");
        return false;
    }
    GetPhysicalDisplayCoord(absEvent, *info, touchInfo);
    return true;
}

bool TouchScreenHandler::OnEventTouchDown(const std::shared_ptr<const AbsEvent>& absEvent)
{
    CALL_DEBUG_ENTER;
    CHKPF(absEvent);
    EventTouch touchInfo;
    int32_t logicalDisplayId = -1;
    CHKPF(inputDevice_);
    int32_t deviceId = inputDevice_->GetDeviceId();
    if (!TouchPointToDisplayPoint(deviceId, absEvent, touchInfo, logicalDisplayId)) {
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

bool TouchScreenHandler::OnEventTouchUp(const std::shared_ptr<const AbsEvent>& absEvent)
{
    CALL_DEBUG_ENTER;
    CHKPF(absEvent);
    int64_t time = GetSysClockTime();
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_UP);

    PointerEvent::PointerItem item;
    int32_t seatSlot = absEvent->GetCurSlot();
    if (!(pointerEvent_->GetPointerItem(seatSlot, item))) {
        MMI_HILOGE("Get pointer parameter failed");
        return false;
    }
    item.SetPressed(false);
    pointerEvent_->UpdatePointerItem(seatSlot, item);
    pointerEvent_->SetPointerId(seatSlot);
    return true;
}

bool TouchScreenHandler::OnEventTouchMotion(const std::shared_ptr<const AbsEvent>& absEvent)
{
    CALL_DEBUG_ENTER;
    CHKPF(absEvent);
    int64_t time = GetSysClockTime();
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    EventTouch touchInfo;
    int32_t deviceId = inputDevice_->GetDeviceId();
    int32_t logicalDisplayId = pointerEvent_->GetTargetDisplayId();
    if (!TouchPointToDisplayPoint(deviceId, absEvent, touchInfo, logicalDisplayId)) {
        MMI_HILOGE("Get TouchMotionPointToDisplayPoint failed");
        return false;
    }
    PointerEvent::PointerItem item;
    int32_t seatSlot = absEvent->GetCurSlot();
    if (!(pointerEvent_->GetPointerItem(seatSlot, item))) {
        MMI_HILOGE("Get pointer parameter failed");
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