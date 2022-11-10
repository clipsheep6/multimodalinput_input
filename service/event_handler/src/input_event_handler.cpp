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

#include "input_event_handler.h"

#include <cstdio>
#include <cstring>
#include <functional>
#include <vector>
#include <cinttypes>

#include <sys/stat.h>
#include <unistd.h>

#include "libinput.h"
#ifdef OHOS_BUILD_ENABLE_COOPERATE
#include "input_device_cooperate_sm.h"
#endif // OHOS_BUILD_ENABLE_COOPERATE
#include "key_command_handler.h"
#include "timer_manager.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputEventHandler" };
class EventMeasureWrapper
{
public:
    EventMeasureWrapper(uint64_t &idSeed, uint32_t type)
    {
        ++idSeed;
        const uint64_t maxUInt64 = (std::numeric_limits<uint64_t>::max)() - 1;
        if (idSeed >= maxUInt64) {
            MMI_HILOGE("The value is flipped. id:%{public}" PRId64, idSeed);
            idSeed = 1;
        }
        idSeed_ = idSeed;
        int64_t beginTime = GetSysClockTime();
        MMI_HILOGD("Event reporting. id:%{public}" PRId64 ",tid:%{public}" PRId64 ", type:%{public}d,"
                "beginTime:%{public}" PRId64, idSeed_, GetThisThreadId(), type, beginTime);      
    }

    ~EventMeasureWrapper()
    {
        int64_t endTime = GetSysClockTime();
        int64_t lostTime = endTime - beginTime_;
        MMI_HILOGD("Event handling completed. id:%{public}" PRId64 ",endTime:%{public}" PRId64
                ",lostTime:%{public}" PRId64, idSeed_, endTime, lostTime);
    }
private:
    uint64_t idSeed_;
    int64_t beginTime_;
};
} // namespace

InputEventHandler::InputEventHandler()
{
    udsServer_ = nullptr;
}

InputEventHandler::~InputEventHandler() {}

void InputEventHandler::Init(UDSServer& udsServer)
{
    udsServer_ = &udsServer;
    BuildInputHandlerChain();
}

#ifdef OHOS_BUILD_HDF
void InputEventHandler::HandleHDFDeviceStatusEvent(const HDFDeviceStatusEvent &event)
{
    EventMeasureWrapper measure(idSeed_, event.devType);
    CHKPV(eventNormalizeHandler_);
    eventNormalizeHandler_->HandleHDFDeviceStatusEvent(event);
}

void InputEventHandler::HandleHDFDeviceInputEvent(const HDFDeviceInputEvent &event)
{
    EventMeasureWrapper measure(idSeed_, event.type);
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    InputDevCooSM->HandleHDFDeviceInputEvent(event);
#else
    CHKPV(eventNormalizeHandler_);
    eventNormalizeHandler_->HandleHDFDeviceInputEvent(event);
#endif
}
#endif // OHOS_BUILD_HDF

void InputEventHandler::OnEvent(void *event)
{
    CHKPV(event);
    auto *lpEvent = static_cast<libinput_event *>(event);
    CHKPV(lpEvent);
    int32_t eventType = libinput_event_get_type(lpEvent);
    EventMeasureWrapper measure(idSeed_, eventType);
    CHKPV(eventNormalizeHandler_);
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    InputDevCooSM->HandleEvent(lpEvent);
#else
    eventNormalizeHandler_->HandleEvent(lpEvent);
#endif // OHOS_BUILD_ENABLE_COOPERATE
}

int32_t InputEventHandler::BuildInputHandlerChain()
{
    eventNormalizeHandler_ = std::make_shared<EventNormalizeHandler>();
    CHKPR(eventNormalizeHandler_, ERROR_NULL_POINTER);
#if !defined(OHOS_BUILD_ENABLE_KEYBOARD) && !defined(OHOS_BUILD_ENABLE_POINTER) && !defined(OHOS_BUILD_ENABLE_TOUCH)
    return RET_OK;
#endif // !OHOS_BUILD_ENABLE_KEYBOARD && !OHOS_BUILD_ENABLE_POINTER && !OHOS_BUILD_ENABLE_TOUCH

    std::shared_ptr<IInputEventHandler> handler = eventNormalizeHandler_;
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    eventFilterHandler_ = std::make_shared<EventFilterHandler>();
    CHKPR(eventFilterHandler_, ERROR_NULL_POINTER);
    handler->SetNext(eventFilterHandler_);
    handler = eventFilterHandler_;
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
    eventInterceptorHandler_  = std::make_shared<EventInterceptorHandler>();
    CHKPR(eventInterceptorHandler_, ERROR_NULL_POINTER);
    handler->SetNext(eventInterceptorHandler_);
    handler = eventInterceptorHandler_;
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
#ifdef OHOS_BUILD_ENABLE_COMBINATION_KEY
    auto keyCommandHandler = std::make_shared<KeyCommandHandler>();
    CHKPR(keyCommandHandler, ERROR_NULL_POINTER);
    handler->SetNext(keyCommandHandler);
    handler = keyCommandHandler;
#endif // OHOS_BUILD_ENABLE_COMBINATION_KEY
    eventSubscriberHandler_ = std::make_shared<KeySubscriberHandler>();
    CHKPR(eventSubscriberHandler_, ERROR_NULL_POINTER);
    handler->SetNext(eventSubscriberHandler_);
    handler = eventSubscriberHandler_;
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#ifdef OHOS_BUILD_ENABLE_MONITOR
    eventMonitorHandler_ = std::make_shared<EventMonitorHandler>();
    CHKPR(eventMonitorHandler_, ERROR_NULL_POINTER);
    handler->SetNext(eventMonitorHandler_);
    handler = eventMonitorHandler_;
#endif // OHOS_BUILD_ENABLE_MONITOR
    auto dispatchHandler = std::make_shared<EventDispatchHandler>();
    CHKPR(dispatchHandler, ERROR_NULL_POINTER);
    handler->SetNext(dispatchHandler);
    return RET_OK;
}

UDSServer* InputEventHandler::GetUDSServer() const
{
    return udsServer_;
}

std::shared_ptr<EventNormalizeHandler> InputEventHandler::GetEventNormalizeHandler() const
{
    return eventNormalizeHandler_;
}

std::shared_ptr<EventInterceptorHandler> InputEventHandler::GetInterceptorHandler() const
{
    return eventInterceptorHandler_;
}

std::shared_ptr<KeySubscriberHandler> InputEventHandler::GetSubscriberHandler() const
{
    return eventSubscriberHandler_;
}

std::shared_ptr<EventMonitorHandler> InputEventHandler::GetMonitorHandler() const
{
    return eventMonitorHandler_;
}

std::shared_ptr<EventFilterHandler> InputEventHandler::GetFilterHandler() const
{
    return eventFilterHandler_;
}

#ifdef OHOS_BUILD_ENABLE_COOPERATE
void InputEventHandler::SetJumpInterceptState(bool isJump)
{
    isJumpIntercept_ = isJump;
}

bool InputEventHandler::GetJumpInterceptState() const
{
    return isJumpIntercept_;
}
#endif // OHOS_BUILD_ENABLE_COOPERATE
} // namespace MMI
} // namespace OHOS