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

#include "event_log_helper.h"
#include "input_device_manager.h"
#include "input_event_handler.h"

#include "crown_transform_processor.h"

#undef MMI_LOG_DOMAIN
#define MMI_LOG_DOMAIN MMI_LOG_DISPATCH
#undef MMI_LOG_TAG
#define MMI_LOG_TAG "CrownTransformProcessor"

namespace OHOS {
namespace MMI {
#ifdef OHOS_BUILD_ENABLE_CROWN
namespace {
constexpr double SCALE_RATIO = static_cast<double>(360) / 532;
constexpr uint64_t MICROSECONDS_PER_SECOND = 1000 * 1000;
}

CrownTransformProcessor::CrownTransformProcessor(int32_t deviceId)
    : pointerEvent_(PointerEvent::Create()), deviceId_(deviceId)
{}

std::shared_ptr<PointerEvent> CrownTransformProcessor::GetPointerEvent() const
{
    return pointerEvent_;
}

int32_t CrownTransformProcessor::NormalizeKeyEvent(const struct libinput_event *event)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);

    struct libinput_event_keyboard *keyEvent = libinput_event_get_keyboard_event(event);
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    auto key = libinput_event_keyboard_get_key(keyEvent);
    MMI_HILOGI("CrownTransformProcessor normalize KeyEvent:%{public}d, and next handle with Keyboard logic", key);

    return RET_OK;
}

int32_t CrownTransformProcessor::NormalizeRotateEvent(const struct libinput_event *event)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);

    struct libinput_event_pointer *rawPointerEvent = libinput_event_get_pointer_event(event);
    CHKPR(rawPointerEvent, ERROR_NULL_POINTER);
    libinput_pointer_axis_source source = libinput_event_pointer_get_axis_source(rawPointerEvent);
    if (source == LIBINPUT_POINTER_AXIS_SOURCE_WHEEL) {
        if (TimerMgr->IsExist(timerId_)) {
            HandleCrownRotateUpdate(rawPointerEvent);
            TimerMgr->ResetTimer(timerId_);
            MMI_HILOGD("Wheel axis update, crown rotate update");
        } else {
            static constexpr int32_t timeout = 100;
            std::weak_ptr<CrownTransformProcessor> weakPtr = shared_from_this();
            timerId_ = TimerMgr->AddTimer(timeout, 1, [weakPtr]() {
                CALL_DEBUG_ENTER;
                auto sharedProcessor = weakPtr.lock();
                CHKPV(sharedProcessor);
                MMI_HILOGI("Timer:%{public}d", sharedProcessor->timerId_);
                sharedProcessor->timerId_ = -1;
                auto pointerEvent = sharedProcessor->GetPointerEvent();
                CHKPV(pointerEvent);
                HandleCrownRotateEnd();
                MMI_HILOGI("Wheel axis end, crown rotate end");
                auto inputEventNormalizeHandler = InputHandler->GetEventNormalizeHandler();
                CHKPV(inputEventNormalizeHandler);
                inputEventNormalizeHandler->HandlePointerEvent(pointerEvent);
            });
            HandleCrownRotateBegin(rawPointerEvent);
            MMI_HILOGI("Wheel axis begin, crown rotate begin");
        }

        auto eventMonitorHandler = InputHandler->GetMonitorHandler();
        CHKPR(eventMonitorHandler, ERROR_NULL_POINTER);
        eventMonitorHandler->OnHandleEvent(pointerEvent);
        DumpInner();
        return RET_OK;
    } else {
        MMI_HILOGE("The source is invalid, source: %{public}d", source);
        return RET_ERR;
    }
}

int32_t CrownTransformProcessor::HandleCrownRotateBegin(const struct libinput_event_pointer *rawPointerEvent)
{
    CALL_DEBUG_ENTER;
    return HandleCrownRotateBeginAndUpdate(rawPointerEvent, POINTER_ACTION_CROWN_ROTATE_BEGIN);
}

int32_t CrownTransformProcessor::HandleCrownRotateUpdate(const struct libinput_event_pointer *rawPointerEvent)
{
    CALL_DEBUG_ENTER;
    return HandleCrownRotateBeginAndUpdate(rawPointerEvent, POINTER_ACTION_CROWN_ROTATE_UPDATE);
}

int32_t CrownTransformProcessor::HandleCrownRotateEnd()
{
    CALL_DEBUG_ENTER;
    lastTime_ = 0;
    return HandleCrownRotatePostInner(0.0, 0.0, PointerEvent::POINTER_ACTION_CROWN_ROTATE_END);
}

int32_t CrownTransformProcessor::HandleCrownRotateBeginAndUpdate(const struct libinput_event_pointer *rawPointerEvent,
    int32_t action)
{
    CALL_DEBUG_ENTER;
    CHKPR(rawPointerEvent, ERROR_NULL_POINTER);

    uint64_t currentTime = libinput_event_pointer_get_time_usec(rawPointerEvent);
    double scrollValue = libinput_event_pointer_get_scroll_value_v120(rawPointerEvent,
        LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
    double degree = scrollValue * SCALE_RATIO;
    double angularVelocity = 0.0;
    
    if (action == PointerEvent::POINTER_ACTION_CROWN_ROTATE_BEGIIN) {
        lastTime_ = currentTime;
    } else if (action == PointerEvent::POINTER_ACTION_CROWN_ROTATE_UPDATE) {
        uint64_t intervalTime = currentTime - lastTime_;
        if (intervalTime > 0) {
            angularVelocity = (degree * MICROSECONDS_PER_SECOND) / intervalTime);
        } else {
            degree = 0.0;
        }
        lastTime_ = currentTime;
    } else {
        MMI_HILOGE("The action is invalid, action: %{public}d", action);
        return RET_ERR;
    }

    return HandleCrownRotatePostInner(angularVelocity, degree, action);
}

void CrownTransformProcessor::HandleCrownRotatePostInner(double angularVelocity, double degree, int32_t action)
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
            pointerEvent_->GetCrownAngularVelocity(), pointerEvent_->GetCrownDegree(),
            pointerEvent_->GetAgentWindowId(), pointerEvent_->GetTargetWindowId());
}
#endif // OHOS_BUILD_ENABLE_CROWN
} // namespace MMI
} // namespace OHOS