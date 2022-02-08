/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "touch_transform_point_processor.h"
#include "log.h"

namespace OHOS::MMI {
namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "TouchTransformPointProcessor"};
}

TouchTransformPointProcessor::TouchTransformPointProcessor(int32_t deviceId) : deviceId_(deviceId)
{
    pointerEvent_ = PointerEvent::Create();
}

TouchTransformPointProcessor::~TouchTransformPointProcessor() {}

void TouchTransformPointProcessor::SetPointEventSource(int32_t sourceType)
{
    pointerEvent_->SetSourceType(sourceType);
}

bool TouchTransformPointProcessor::OnEventTouchDown(libinput_event *event)
{
    MMI_LOGD("Enter onEventTouchDown");
    CHKP(event, PARAM_INPUT_INVALID);
    auto data = libinput_event_get_touch_event(event);
    auto time = libinput_event_touch_get_time(data);
    auto seatSlot = libinput_event_touch_get_seat_slot(data);
    auto pressure = libinput_event_get_touch_pressure(event);
    int32_t logicalY = -1;
    int32_t logicalX = -1;
    int32_t logicalDisplayId = -1;
    Direction temp = direction_;
    if (!WinMgr->TransformTouchPointToDisplayPoint(data, temp, logicalDisplayId, logicalX, logicalY)){
        return false;
    }
    auto pointIds = pointerEvent_->GetPointersIdList();
    auto time = libinput_event_touch_get_time(data);
    pointerEvent_->SetActionTime(time);
    if (pointIds.empty()) {
        pointerEvent_->SetActionStartTime(time);
        pointerEvent_->SetTargetDisplayId(logicalDisplayId);
        direction_ = temp;
    } else if (temp != direction_){
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
        return true;
    }
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    PointerEvent::PointerItem pointer;
    pointer.SetPointerId(seatSlot);
    pointer.SetDownTime(time);
    pointer.SetPressed(true);
    pointer.SetGlobalX(logicalX);
    pointer.SetGlobalY(logicalY);
    pointer.SetLocalX(1);
    pointer.SetLocalY(1);
    pointer.SetWidth(1);
    pointer.SetHeight(1);
    pointer.SetPressure(pressure);
    pointer.SetDeviceId(deviceId_);
    pointerEvent_->SetDeviceId(deviceId_);
    pointerEvent_->AddPointerItem(pointer);
    pointerEvent_->SetPointerId(seatSlot);
    MMI_LOGD("logicalX:%{public}d, logicalY:%{public}d, logicalDisplayId:%{public}d",
             logicalX, logicalY, logicalDisplayId);
    MMI_LOGD("Leave onEventTouchDown");
	return true;
}

bool TouchTransformPointProcessor::OnEventTouchMotion(libinput_event *event)
{
    MMI_LOGD("Enter onEventTouchMotion");
    CHKP(event, PARAM_INPUT_INVALID);
    auto data = libinput_event_get_touch_event(event);
    auto time = libinput_event_touch_get_time(data);
    auto seatSlot = libinput_event_touch_get_seat_slot(data);
    auto pressure = libinput_event_get_touch_pressure(event);
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    int32_t logicalY = -1;
    int32_t logicalX = -1;
    int32_t logicalDisplayId = pointerEvent_->GetTargetDisplayId();
    Direction temp = direction_;
    if (!WinMgr->TransformTouchPointToDisplayPoint(data, temp, logicalDisplayId, logicalX, logicalY)){
        return false;
    }
    if (temp != direction_){
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
        return true;
    }
    PointerEvent::PointerItem pointer;
    if (!pointerEvent_->GetPointerItem(seatSlot, pointer)) {
        return false;
    }
    pointer.SetPressure(pressure);
    pointer.SetGlobalX(logicalX);
    pointer.SetGlobalY(logicalY);
    pointerEvent_->UpdatePointerItem(seatSlot, pointer);
    pointerEvent_->SetPointerId(seatSlot);
    MMI_LOGD("Leave onEventTouchMotion");
	return true;
}

bool TouchTransformPointProcessor::OnEventTouchUp(libinput_event *event)
{
    MMI_LOGD("Enter onEventTouchUp");
    CHKP(event, PARAM_INPUT_INVALID);
    auto data = libinput_event_get_touch_event(event);
    auto time = libinput_event_touch_get_time(data);
    auto seatSlot = libinput_event_touch_get_seat_slot(data);
    PointerEvent::PointerItem pointer;
    if (!pointerEvent_->GetPointerItem(seatSlot, pointer)) {
        return false;
    }
    if (pointerEvent_->GetPointerAction() == PointerEvent::POINTER_ACTION_CANCEL) {
        pointerEvent_->RemovePointerItem(seatSlot);
        if (pointerEvent_->GetPointersIdList().empty()) {
            pointerEvent_->Init();
        }
        return false;
    }
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_UP);
    pointer.SetPressed(false);
    pointerEvent_->UpdatePointerItem(seatSlot, pointer);
    pointerEvent_->SetPointerId(seatSlot);
    MMI_LOGD("Leave onEventTouchUp");
	return true;
}

std::shared_ptr<PointerEvent> TouchTransformPointProcessor::OnLibinputTouchEvent(libinput_event *event)
{
    CHKPR(event, PARAM_INPUT_INVALID, nullptr);
    MMI_LOGD("call  onLibinputTouchEvent begin");
    if (pointerEvent_ == nullptr) {
        MMI_LOGE("pointerEvent_ is nullptr");
        return nullptr;
    }
    pointerEvent_->UpdateId();
    auto type = libinput_event_get_type(event);
    switch (type) {
        case LIBINPUT_EVENT_TOUCH_DOWN: {
            if(!OnEventTouchDown(event)) {
                return nullptr;
            }
            break;
        }
        case LIBINPUT_EVENT_TOUCH_UP: {
            if(!OnEventTouchUp(event)) {
                return nullptr;
            }
            break;
        }
        case LIBINPUT_EVENT_TOUCH_MOTION: {
            if(!OnEventTouchMotion(event)) {
                return nullptr;
            }
            break;
        }
        default: {
            MMI_LOGE("Unknown event type, touchType:%{public}d", type);
            return nullptr;
        }
    }
    MMI_LOGD("call  onLibinputTouchEvent end");
    return pointerEvent_;
}
}

