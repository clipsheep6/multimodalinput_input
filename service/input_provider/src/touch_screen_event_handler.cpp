/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "touch_screen_event_handler.h"
#include "define_multimodal.h"
#include "input-event-codes.h"
#include "input_event_handler.h"
#include "input_windows_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "TouchScreenEventHandler" };
};

TouchScreenEventHandler::TouchScreenEventHandler()
{
    mtdev_ = input_mtdev_open();
}

void TouchScreenEventHandler::HandleEvent(const struct input_event& event)
{
    if (mtdev_ == nullptr) {
        ProcessEvent(event);
        return;
    }
    mtdev_put_event(mtdev_, &event);
    if (!EventIsCode(event, EV_SYN, SYN_REPORT)) {
        return;
    }
    while (!mtdev_empty(mtdev_)) {
        struct input_event e = {};
        mtdev_get_event(mtdev_, &e);
        ProcessEvent(e);
    }
}

int32_t TouchScreenEventHandler::EventIsCode(const struct input_event& ev, unsigned int type, unsigned int code)
{
    if (!EventIsType(ev, type)) {
        return RET_OK;
    }
    int max = EventTypeGetMax(type);
    return ((max > -1) && (code <= static_cast<unsigned int>(max)) && (ev.code == code));
}

int32_t TouchScreenEventHandler::EventIsType(const struct input_event& ev, unsigned int type)
{
    return ((type < EV_CNT) && (ev.type == type));
}

int32_t TouchScreenEventHandler::EventTypeGetMax(unsigned int type)
{
    if (type > EV_MAX) {
        MMI_HILOGE("Error, type:%{public}u > EV_MAX", type);
        return RET_ERR;
    }
    return ev_max[type];
}

void TouchScreenEventHandler::ProcessEvent(const struct input_event& event)
{
    auto type = event.type;
    auto code = event.code;
    auto value = event.value;
    switch (type) {
        case EV_ABS: {
            HandleAbsEvent(code, value);
            break;
        }
        case EV_SYN: {
            HandleSyncEvent();
            break;
        }
        case EV_KEY:
        case EV_REL:
        case EV_MSC:
        case EV_SW:
        case EV_LED:
        case EV_SND:
        case EV_REP:
        case EV_FF:
        case EV_PWR:
        case EV_FF_STATUS: {
            break;
        }
        default: {
            break;
        }
    }
}

void TouchScreenEventHandler::HandleSyncEvent()
{
    CALL_DEBUG_ENTER;
    for (auto iter = slots_.begin(); iter != slots_.end();) {
        auto slot = iter->second;
        if (slot == nullptr || slot->pointer == nullptr) {
            slots_.erase(iter++);
            continue;
        }
        if (!slot->isDirty) {
            iter++;
            continue;
        }
        slot->isDirty = false;
        slot->id = iter->first;
        auto timeNs = std::chrono::steady_clock::now().time_since_epoch();
        auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeNs).count();
        if (slot->pointerAction == ACTION_DOWN) {
            slot->pointer->SetDownTime(timeMs);
        }
        OnInputEvent(slot);
        if (slot->pointerAction == ACTION_UP) {
             slots_.erase(iter++);
             continue;
        }
        iter++;
    }
}

void TouchScreenEventHandler::HandleAbsEvent(int code, int value)
{
    switch (code) {
        case ABS_MT_SLOT: {
            HandleMtSlot(value);
            break;
        }
        case ABS_MT_POSITION_X: {
            HandleMtPositionX(value);
            break;
        }
        case ABS_MT_POSITION_Y: {
            HandleMtPositionY(value);
            break;
        }
        case ABS_MT_TRACKING_ID: {
            HandleMtTrackingId(value);
            break;
        }
        case ABS_MT_TOUCH_MAJOR:
        case ABS_MT_TOUCH_MINOR:
        case ABS_MT_WIDTH_MAJOR:
        case ABS_MT_WIDTH_MINOR:
        case ABS_MT_ORIENTATION: {
            break;
        }
        case ABS_MT_PRESSURE:
        case ABS_MT_DISTANCE:
        case ABS_MT_TOOL_X:
        case ABS_MT_TOOL_Y: {
            break;
        }
        default: {
            MMI_HILOGW("Unknown type: %{public}d", code);
            break;
        }
    }
}

void TouchScreenEventHandler::HandleMtSlot(int value)
{
    if (value >= slotNum_) {
        MMI_HILOGW("Exceeded slot count (%{public}d of max %{public}d)", value, slotNum_);
        curSlot_ = slotNum_ - 1;
        return;
    }
    curSlot_ = value;
}

void TouchScreenEventHandler::HandleMtPositionX(int value)
{
    auto pointer = GetCurrentPointer(true);
    CHKPV(pointer);
    CHKPV(pointer->pointer);
    pointer->pointer->SetRawDx(value);
    pointer->pointerAction = ACTION_MOVE;
    pointer->isDirty = true;
}

void TouchScreenEventHandler::HandleMtPositionY(int value)
{
    auto pointer = GetCurrentPointer(true);
    CHKPV(pointer);
    CHKPV(pointer->pointer);
    pointer->pointer->SetRawDy(value);
    pointer->pointerAction = ACTION_MOVE;
    pointer->isDirty = true;
}

void TouchScreenEventHandler::HandleMtTrackingId(int value)
{
    auto pointer = GetCurrentPointer(true);
    CHKPV(pointer);
    if (value < 0) {
        pointer->pointerAction = ACTION_UP;
    } else {
        pointer->pointerAction = ACTION_DOWN;
    }
    pointer->isDirty = true;
}

std::shared_ptr<Slot> TouchScreenEventHandler::GetCurrentPointer(bool createIfNotExist)
{
    if (curSlot_ < 0) {
        MMI_HILOGE("curSlot_(%{public}d) < 0", curSlot_);
        return nullptr;
    }
    auto it = slots_.find(curSlot_);
    if (it != slots_.end()) {
        return it->second;
    }
    if (!createIfNotExist) {
        MMI_HILOGD("Return null pointer, because no need to create");
        return nullptr;
    }

    auto pt = std::make_shared<Slot>();
    CHKPP(pt);
    auto item = std::make_shared<PointerEvent::PointerItem>();
    CHKPP(item);
    pt->pointer = item;
    slots_.insert(std::make_pair(curSlot_, pt));
    return pt;
}

std::shared_ptr<TouchScreenEventHandler> TouchScreenEventHandler::CreateInstance()
{
    return std::shared_ptr<TouchScreenEventHandler>(new TouchScreenEventHandler());
}

void TouchScreenEventHandler::OnInputEvent(std::shared_ptr<Slot> slot)
{
    CALL_DEBUG_ENTER;
    CHKPV(slot);
    if (pointerEvent_ == nullptr) {
        pointerEvent_ = PointerEvent::Create();
        CHKPV(pointerEvent_);
    }
    auto pointerEvent = TransformToPointerEvent(slot);
    if (!pointerEvent) {
        MMI_HILOGE("TransformToPointerEvent Failed");
        return;
    }
    CHKPV(iEventHandlerMgr_);
    iEventHandlerMgr_->DispatchEvent(pointerEvent);
}

const std::shared_ptr<PointerEvent> TouchScreenEventHandler::TransformToPointerEvent(std::shared_ptr<Slot> slot)
{
    CALL_DEBUG_ENTER;
    CHKPP(slot);
    int32_t action = slot->pointerAction;
    switch (action) {
        case ACTION_DOWN: {
            if (!OnEventTouchDown(slot)) {
                MMI_HILOGE("Get OnEventTouchDown failed");
                return nullptr;
            }
            break;
        }
        case ACTION_UP: {
            if (!OnEventTouchUp(slot)) {
                MMI_HILOGE("Get OnEventTouchUp failed");
                return nullptr;
            }
            break;
        }
        case ACTION_MOVE: {
            if (!OnEventTouchMotion(slot)) {
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

bool TouchScreenEventHandler::OnEventTouchDown(std::shared_ptr<Slot> slot)
{
    CHKPF(slot);
    auto pointer = slot->pointer;
    CHKPF(pointer);
    EventTouch touchInfo;
    int32_t logicalDisplayId = -1;
    auto axis = std::make_tuple(xInfo_, yInfo_);
    auto raw = std::make_tuple(pointer->GetRawDx(), pointer->GetRawDy());
    auto deviceId = GetDeviceId();
    if (!WinMgr->TouchPointToDisplayPoint(axis, raw, touchInfo, logicalDisplayId, deviceId)) {
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
    int32_t longAxis = 0;   //TO DO ...
    int32_t shortAxis = 0;  //TO DO ...
    int32_t toolType = 0;   //TO DO ...
    item.SetPressure(pressure);
    item.SetLongAxis(longAxis);
    item.SetShortAxis(shortAxis);
    item.SetToolType(toolType);
    item.SetPointerId(slot->id);
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
    pointerEvent_->SetPointerId(slot->id);
    return true;
}

bool TouchScreenEventHandler::OnEventTouchUp(std::shared_ptr<Slot> slot)
{
    CHKPF(slot);
    int64_t time = GetSysClockTime();
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    PointerEvent::PointerItem item;
    int curSlot = slot->id;
    if (!pointerEvent_->GetPointerItem(curSlot, item)) {
        MMI_HILOGE("Get pointer parameter failed, curSlot:%{public}d", curSlot);
        return false;
    }
    item.SetPressed(false);
    pointerEvent_->UpdatePointerItem(curSlot, item);
    pointerEvent_->SetPointerId(curSlot);
    return true;
}

bool TouchScreenEventHandler::OnEventTouchMotion(std::shared_ptr<Slot> slot)
{
    CHKPF(slot);
    auto pointer = slot->pointer;
    CHKPF(pointer);
    pointerEvent_->SetActionTime(GetSysClockTime());
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    EventTouch touchInfo;
    int32_t logicalDisplayId = pointerEvent_->GetTargetDisplayId();
    auto axis = std::make_tuple(xInfo_, yInfo_);
    auto raw = std::make_tuple(pointer->GetRawDx(), pointer->GetRawDy());
    auto deviceId = GetDeviceId();
    if (!WinMgr->TouchPointToDisplayPoint(axis, raw, touchInfo, logicalDisplayId, deviceId)) {
        MMI_HILOGE("Get TouchPointToDisplayPoint failed");
        return false;
    }
    PointerEvent::PointerItem item;
    int curSlot = slot->id;
    if (!(pointerEvent_->GetPointerItem(curSlot, item))) {
        MMI_HILOGE("Get pointer parameter failed, curSlot:%{public}d", curSlot);
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
    pointerEvent_->UpdatePointerItem(curSlot, item);
    pointerEvent_->SetPointerId(curSlot);
    return true;
}

void TouchScreenEventHandler::SetAxisInfo(std::shared_ptr<IInputDevice::AxisInfo> xInfo, std::shared_ptr<IInputDevice::AxisInfo> yInfo)
{
    xInfo_ = xInfo;
    yInfo_ = yInfo;
}
} // namespace MMI
} // namespace OHOS