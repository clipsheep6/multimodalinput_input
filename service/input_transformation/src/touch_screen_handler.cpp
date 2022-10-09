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

int32_t TouchScreenSeat::BindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice)
{
    MMI_HILOGD("Enter");
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
        MMI_HILOGE("Leave, xInfo->GetMinimum():$s >= xInfo->GetMaximum():$s", xInfo->GetMinimum(), xInfo->GetMaximum());
        return -1;
    }

    std::shared_ptr<IInputDevice::AxisInfo> yInfo = inputDevice->GetAxisInfo(IInputDevice::AXIS_MT_Y);
    if (!yInfo) {
        MMI_HILOGE("Leave, null AxisInfo Of AXIS_MT_Y");
        return -1;
    }

    if (yInfo->GetMinimum() >= yInfo->GetMaximum()) {
        MMI_HILOGE("Leave, yInfo->GetMinimum():$s >= yInfo->GetMaximum():$s", yInfo->GetMinimum(), yInfo->GetMaximum());
        return -1;
    }

    inputDevice_ = inputDevice;
    xInfo_ = xInfo;
    yInfo_ = yInfo;
    MMI_HILOGD("Leave");
    return 0;
}

int32_t TouchScreenSeat::UnbindInputDevice(const std::shared_ptr<IInputDevice>& inputDevice)
{
    MMI_HILOGD("Enter");
    if (inputDevice != inputDevice_) {
        LOG_E("Leave, inputDevice != inputDevice_");
        return -1;
    }

    inputDevice_.reset();
    MMI_HILOGD("Leave");
    return 0;
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

    if (pointerEvent_ == nullptr) {
        pointerEvent_ = PointerEvent::Create();
        CHKPP(pointerEvent_);
    }

    auto action = ConvertAction(absEvent->GetAction());
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

int32_t TouchScreenHandler::TransformX(int32_t xPos, int32_t width, int32_t logicalWidth) const
{
    if (width <= 0) {
        return -1;
    }

    int64_t one = 1;
    auto result = (int32_t)(one * xPos * logicalWidth / width);
    LOG_D("xPos:$s width:$s logicalWidth:$s result:$s", xPos, width, logicalWidth, result);
    return result;
}

int32_t TouchScreenHandler::TransformY(int32_t yPos, int32_t height, in32_t logicalHeight) const
{
    if (height <= 0) {
        return -1;
    }

    int64_t one = 1;
    auto result = (int32_t)(one * yPos * logicalHeight / height);
    LOG_D("yPos:$s height:$s logicalHeight:$s result:$s", yPos, height, logicalHeight, result);
    return result;
}

int32_t TouchScreenHandler::TransformToPhysicalDisplayCoordinate(const DisplayInfo& info,
        int32_t tpX, int32_t tpY, int32_t& displayX, int32_t& displayY) const
{
    if (!xInfo_) {
        LOG_E("Leave, null xInfo_");
        return -1;
    }

    if (!yInfo_) {
        LOG_E("Leave, null yInfo_");
        return -1;
    }

    if (!display_) {
        LOG_E("Leave, null display_");
        return -1;
    }

    int32_t deltaX = tpX  - xInfo_->GetMinimum();
    int32_t width = xInfo_->GetMaximum() - xInfo_->GetMinimum() + 1;
    displayX = TransformX(deltaX, width, info.width);

    int32_t deltaY = tpY - yInfo_->GetMinimum();
    int32_t height = yInfo_->GetMaximum() - yInfo_->GetMinimum() + 1;
    displayY = TransformY(deltaY, height, info.height);

    LOG_D("Leave");
    return 0;
}

void TouchScreenHandler::GetPhysicalDisplayCoord(std::shared_ptr<const AbsEvent>& absEvent,
    const DisplayInfo& info, EventTouch& touchInfo)
{
    const auto& absEventPointer = absEvent->GetPointer();
    if (!absEventPointer) {
        LOG_E("Leave, null absEventPointer");
        return;
    }

    int32_t physicalDisplayX = -1;
    int32_t physicalDisplayY = -1;
    auto retCode = TransformToPhysicalDisplayCoordinate(info, absEventPointer->GetX(), absEventPointer->GetY(),
            physicalDisplayX, physicalDisplayY);
    if (retCode < 0) {
        LOG_E("Leave, TransformToPhysicalDisplayCoordinate Failed");
        return;
    }
}

bool TouchScreenHandler::TouchPointToDisplayPoint(int32_t deviceId, std::shared_ptr<const AbsEvent>& absEvent,
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

bool TouchScreenHandler::OnEventTouchDown(std::shared_ptr<const AbsEvent>& absEvent)
{
    CALL_DEBUG_ENTER;
    CHKPF(absEvent);
    EventTouch touchInfo;
    int32_t logicalDisplayId = -1;
    // if (!WinMgr->TouchPointToDisplayPoint(deviceId_, absEvent, touchInfo, logicalDisplayId)) {
    //     MMI_HILOGE("TouchDownPointToDisplayPoint failed");
    //     return false;
    // }
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
    double pressure = 0;//TO DO ...
    int32_t seatSlot = 0;//TO DO ...
    int32_t longAxis = 0;//TO DO ...
    int32_t shortAxis = 0;//TO DO ...
    item.SetPressure(pressure);
    item.SetLongAxis(longAxis);
    item.SetShortAxis(shortAxis);
    int32_t toolType = 0;  //GetTouchToolType(touch, device);//TO DO ...
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
    return true;
}

bool TouchScreenHandler::OnEventTouchUp(std::shared_ptr<const AbsEvent>& absEvent)
{
    CALL_DEBUG_ENTER;
    CHKPF(event);
    auto touch = libinput_event_get_touch_event(event);
    CHKPF(touch);
    int64_t time = GetSysClockTime();
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_UP);

    PointerEvent::PointerItem item;
    int32_t seatSlot = libinput_event_touch_get_seat_slot(touch);
    if (!(pointerEvent_->GetPointerItem(seatSlot, item))) {
        MMI_HILOGE("Get pointer parameter failed");
        return false;
    }
    item.SetPressed(false);
    pointerEvent_->UpdatePointerItem(seatSlot, item);
    pointerEvent_->SetPointerId(seatSlot);
    return true;
}

bool TouchScreenHandler::OnEventTouchMotion(std::shared_ptr<const AbsEvent>& absEvent)
{
    CALL_DEBUG_ENTER;
    CHKPF(event);
    auto touch = libinput_event_get_touch_event(event);
    CHKPF(touch);
    int64_t time = GetSysClockTime();
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    EventTouch touchInfo;
    int32_t logicalDisplayId = pointerEvent_->GetTargetDisplayId();
    if (!WinMgr->TouchPointToDisplayPoint(deviceId_, touch, touchInfo, logicalDisplayId)) {
        MMI_HILOGE("Get TouchMotionPointToDisplayPoint failed");
        return false;
    }
    PointerEvent::PointerItem item;
    int32_t seatSlot = libinput_event_touch_get_seat_slot(touch);
    if (!(pointerEvent_->GetPointerItem(seatSlot, item))) {
        MMI_HILOGE("Get pointer parameter failed");
        return false;
    }
    double pressure = libinput_event_touch_get_pressure(touch);
    int32_t longAxis = libinput_event_get_touch_contact_long_axis(touch);
    int32_t shortAxis = libinput_event_get_touch_contact_short_axis(touch);
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
    EventLogHelper::PrintEventData(pointerEvent_, pointerEvent_->GetPointerAction(),
        pointerEvent_->GetPointerIds().size());
    return true;
}
} // namespace MMI
} // namespace OHOS