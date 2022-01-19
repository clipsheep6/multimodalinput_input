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

#include "touchpad_transform_point_processor.h"
#include "log.h"

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN,
            "TouchPadTransformPointProcessor"};
    }
}

OHOS::MMI::TouchPadTransformPointProcessor::TouchPadTransformPointProcessor()
{
    this->pointerEvent_ = PointerEvent::Create();
}

OHOS::MMI::TouchPadTransformPointProcessor::~TouchPadTransformPointProcessor() {}

void OHOS::MMI::TouchPadTransformPointProcessor::setPointEventSource(int32_t sourceType)
{
    pointerEvent_->SetSourceType(sourceType);
}

void OHOS::MMI::TouchPadTransformPointProcessor::onEventTouchPadDown(libinput_event *event)
{
    CHK(event, PARAM_INPUT_INVALID);
    MMI_LOG_D("this touch pad event is down begin");
    auto data = libinput_event_get_touchpad_event(event);
    auto time = libinput_event_touchpad_get_time(data);
    auto seat_slot = libinput_event_touchpad_get_seat_slot(data);
    auto logicalX = libinput_event_touchpad_get_x(data);
    auto logicalY = libinput_event_touchpad_get_y(data);

    int32_t id = 1;
    auto pointIds = pointerEvent_->GetPointersIdList();
    if (pointIds.size() == 0) {
        pointerEvent_->SetActionStartTime(time);
    }
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_DOWN);
    PointerEvent::PointerItem pointer;
    pointer.SetPointerId(seat_slot);
    pointer.SetDownTime(time);
    pointer.SetPressed(true);
    pointer.SetGlobalX((int32_t)logicalX);
    pointer.SetGlobalY((int32_t)logicalY);
    pointer.SetDeviceId(id);
    pointerEvent_->SetDeviceId(id);
    pointerEvent_->AddPointerItem(pointer);
    pointerEvent_->SetPointerId(seat_slot);
    MMI_LOG_D("this touch pad event is down end");
}

void OHOS::MMI::TouchPadTransformPointProcessor::onEventTouchPadMotion(libinput_event *event)
{
    CHK(event, PARAM_INPUT_INVALID);
    MMI_LOG_D("this touch pad event is motion begin");
    auto data = libinput_event_get_touchpad_event(event);
    auto time = libinput_event_touchpad_get_time(data);
    auto seat_slot = libinput_event_touchpad_get_seat_slot(data);
    auto logicalX = libinput_event_touchpad_get_x(data);
    auto logicalY = libinput_event_touchpad_get_y(data);

    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_MOVE);
    PointerEvent::PointerItem pointer;
    pointerEvent_->GetPointerItem(seat_slot, pointer);
    pointer.SetGlobalX((int32_t)logicalX);
    pointer.SetGlobalY((int32_t)logicalY);
    pointerEvent_->RemovePointerItem(seat_slot);
    pointerEvent_->AddPointerItem(pointer);
    pointerEvent_->SetPointerId(seat_slot);
    MMI_LOG_D("this touch pad event is motion end");
}

void OHOS::MMI::TouchPadTransformPointProcessor::onEventTouchPadUp(libinput_event *event)
{
    CHK(event, PARAM_INPUT_INVALID);
    MMI_LOG_D("this touch pad event is up begin");
    auto data = libinput_event_get_touchpad_event(event);
    auto time = libinput_event_touchpad_get_time(data);
    auto seat_slot = libinput_event_touchpad_get_seat_slot(data);
    auto logicalX = libinput_event_touchpad_get_x(data);
    auto logicalY = libinput_event_touchpad_get_y(data);

    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_UP);

    PointerEvent::PointerItem pointer;
    pointerEvent_->GetPointerItem(seat_slot, pointer);
    pointerEvent_->RemovePointerItem(seat_slot);
    pointer.SetPressed(false);
    pointer.SetGlobalX((int32_t)logicalX);
    pointer.SetGlobalY((int32_t)logicalY);
    pointerEvent_->AddPointerItem(pointer);
    pointerEvent_->SetPointerId(seat_slot);
    MMI_LOG_D("this touch pad event is up end");
}

std::shared_ptr<OHOS::MMI::PointerEvent> OHOS::MMI::TouchPadTransformPointProcessor::onLibinputTouchPadEvent(
    libinput_event *event)
{
    CHKR(event, PARAM_INPUT_INVALID, nullptr);
    MMI_LOG_D("call  onLibinputTouchPadEvent begin");
    auto type = libinput_event_get_type(event);
    switch (type) {
        case LIBINPUT_EVENT_TOUCHPAD_DOWN: {
            onEventTouchPadDown(event);
            break;
        }
        case LIBINPUT_EVENT_TOUCHPAD_UP: {
            onEventTouchPadUp(event);
            break;
        }
        case LIBINPUT_EVENT_TOUCHPAD_MOTION: {
            onEventTouchPadMotion(event);
            break;
        }
        default: {
            return nullptr;
        }
    }
    MMI_LOG_D("call  onLibinputTouchPadEvent end");
    return this->pointerEvent_;
}
