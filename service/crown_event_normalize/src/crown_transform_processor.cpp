/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "crown_transform_processor.h"

#include "event_log_helper.h"
#include "input_event_handler.h"
#include "input_device_manager.h"

namespace OHOS {
namespace MMI {
#ifdef OHOS_BUILD_ENABLE_CROWN
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "CrownTransformProcessor"};
}

CrownTransformProcessor::CrownTransformProcessor(int32_t deviceId)
    : pointerEvent_(PointerEvent::Create()), deviceId_(deviceId)
{}

CrownTransformProcessor::~CrownTransformProcessor()
{}

std::shared_ptr<PointerEvent> CrownTransformProcessor::GetPointerEvent() const
{
    return pointerEvent_;
}

int32_t CrownTransformProcessor::NormalizeKeyEvent(struct libinput_event *event)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);

    struct libinput_event_keyboard *keyEvent = libinput_event_get_keyboard_event(event);
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    auto key = libinput_event_keyboard_get_key(keyEvent);
    MMI_HILOGI("CrownTransformProcessor normalize KeyEvent:%{public}d, and next handle with Keyboard logic", key);

    return RET_OK;
}

int32_t CrownTransformProcessor::NormalizeRotateEvent(struct libinput_event *event)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    CHKPR(pointerEvent_, ERROR_NULL_POINTER);

    struct libinput_event_pointer *rawPointerEvent = libinput_event_get_pointer_event(event);
    CHKPR(rawPointerEvent, ERROR_NULL_POINTER);
    libinput_pointer_axis_source source = libinput_event_pointer_get_axis_source(rawPointerEvent);
    if (source == LIBINPUT_POINTER_AXIS_SOURCE_WHEEL) {
        MMI_HILOGI("Libinput event axis source type is wheel");
        double scrollValue = libinput_event_pointer_get_scroll_value_v120(rawPointerEvent, 
            LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
        double degree = scrollValue * (360 / 532);
        double angularVelocity = 0.0;

        uint64_t currentTime = libinput_event_pointer_get_time_usec(rawPointerEvent);

        if (TimerMgr->IsExist(timerId_)) {
            angularVelocity = (degree * (1000 * 1000)) / (currentTime - lastTime_);
            HandleCrownRotatePostInner(angularVelocity, degree, PointerEvent::POINTER_ACTION_CROWN_ROTATE_UPDATE);
            TimerMgr->ResetTimer(timerId_);
            MMI_HILOGD("Wheel axis update, crown rotate update");
        } else {
            static constexpr int32_t timeout = 100;
            std::weak_ptr<CrownTransformProcessor> weakPtr = shared_from_this();
            timerId_ = TimerMgr->AddTimer(timeout, 1, [weakPtr](){
                CALL_DEBUG_ENTER;
                auto sharedPtr = weakPtr.lock();
                CHKPV(sharedPtr);
                MMI_HILOGI("Timer:%{public}d", sharedPtr->timerId_);
                sharedPtr->timerId_ = -1;
                auto pointerEvent = sharedPtr->GetPointerEvent();
                CHKPV(pointerEvent);
                HandleCrownRotatePostInner(0.0, 0.0, PointerEvent::POINTER_ACTION_CROWN_ROTATE_END);
                MMI_HILOGI("Wheel axis end, crown rotate end");
                lastTime_ = 0;
                auto inputEventNormalizeHandler = InputHandler->GetEventNormalizeHandler();
                CHKPV(inputEventNormalizeHandler);
                inputEventNormalizeHandler->HandlePointerEvent(pointerEvent);
            });
            HandleCrownRotatePostInner(angularVelocity, degree, PointerEvent::POINTER_ACTION_CROWN_ROTATE_BEGIN);
            MMI_HILOGI("Wheel axis begin, crown rotate begin");
        }

        InputHandler->GetMonitorHandler()->OnHandleEvent(pointerEvent);
        DumpInner();
        lastTime_ = currentTime;
    }

    return RET_OK;
}

void CrownTransformProcessor::HandleCrownRotatePostInner(double angularVelocity, double degree, int32_t actoin)
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent_);

    pointerEvent_->SetCrownAngularVelocity(angularVelocity);
    pointerEvent_->SetCrownDegree(degree);
    pointerEvent_->SetPointerAction(action);
    int64_t time = GetSysClockTime();
    pointerEvent_->SetActionTime(time);
    pointerEvent_->SetActionStartTime(time);
    pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_CROWN);
    pointerEvent_->SetButtonId(PointerEvent::BUTTON_NONE);
    pointerEvent_->UpdateId();
    pointerEvent_->SetPointerId(0);
    pointerEvent_->SetDeviceId(deviceId_);
    pointerEvent_->SetTargetDisplayId(-1);
    pointerEvent_->SetTargetWindowId(-1);
    pointerEvent_->SetAgentWindowId(-1);
}

void CrownTransformProcessor::DumpInner()
{
    EventLogHelper::PrintEventData(pointerEvent_);
    auto device = InputDevMgr->GetInputDevice(pointerEvent_->GetDeviceId());
    CHKPV(device);
    MMI_HILOGI("The crown device id: %{public}d, event created by: %{public}s", pointerEvent_->GetId(), 
        device->GetName().c_str());
}

void CrownTransformProcessor::Dump(int32_t fd, const std::vector<std::string> &args)
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent_);
    mprintf(fd, "Crown device state information:\t");
    mprintf(fd,
            "PointerId:%d | SourceType:%s | PointerAction:%s | ActionTime:%" PRId64 " | CrownAngularVelocity:%f "
            "| CrownDegree:%s | AgentWindowId:%d | TargetWindowId:%d\t", pointerEvent_->GetPointerId(), 
            pointerEvent_->DumpSourceType(), pointerEvent_->DumpPointerAction(), pointerEvent_->GetActionTime(), 
            pointerEvent_->GetCrownAngularVelocity(), pointerEvent_->GetCrownDegree(), pointerEvent_->GetAgentWindowId(),
            pointerEvent_->GetTargetWindowId());
}
#endif // OHOS_BUILD_ENABLE_CROWN
} // namespace MMI
} // namespace OHOS