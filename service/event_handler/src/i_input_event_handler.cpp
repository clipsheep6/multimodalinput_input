/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "i_input_event_handler.h"

#include "error_multimodal.h"
#include "i_filter_event_handler.h"
#include "i_interceptor_event_handler.h"
#include "i_interceptor_manager_event_handler.h"
#include "i_subscriber_event_handler.h"
#include "i_monitor_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "IInputEventHandler" };
}

IInputEventHandler::IInputEventHandler(int32_t priority) 
{
    priority_ = priority;
};

int32_t IInputEventHandler::HandleLibinputEvent(libinput_event* event)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("nextHandler_ is nullptr");
        return RET_OK;
    }
    return nextHandler_->HandleLibinputEvent(event);
}

int32_t IInputEventHandler::HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Keyboard device does not support");
        return RET_OK;
    }
    return nextHandler_->HandleKeyEvent(keyEvent);
}

int32_t IInputEventHandler::HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Pointer device does not support");
        return RET_OK;
    }
    return nextHandler_->HandlePointerEvent(pointerEvent);
}

int32_t IInputEventHandler::HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("touch device does not support");
        return RET_OK;
    }
    return nextHandler_->HandleTouchEvent(pointerEvent);
}

int32_t IInputEventHandler::GetPriority() const
{
    return priority_;
}

int32_t IInputEventHandler::AddInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    std::shared_ptr<IInputEventHandler> cur = std::shared_ptr<IInputEventHandler>(this);
    while (cur != nullptr) {
        if (cur->GetHandlerType() == EventHandlerType::INTERCEPTOR) {
            auto interceptor = std::static_pointer_cast<IInterceptorEventHandler>(cur);
            return interceptor->AddInputHandler(handlerId, handlerType, session);
        }
        cur = cur->nextHandler_;
    }
    return RET_OK;
}

void IInputEventHandler::TouchMonitorHandlerMarkConsumed(int32_t monitorId, int32_t eventId, SessionPtr sess)
{
    std::shared_ptr<IInputEventHandler> cur = std::shared_ptr<IInputEventHandler>(this);
    while (cur != nullptr) {
        if (cur->GetHandlerType() == EventHandlerType::MONITOR) {
            auto monitor = std::static_pointer_cast<IMonitorEventHandler>(cur);
            monitor->MarkConsumed(monitorId, eventId, sess);
            return;
        }
        cur = cur->nextHandler_;
    }
}

void IInputEventHandler::RemoveInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    std::shared_ptr<IInputEventHandler> cur = std::shared_ptr<IInputEventHandler>(this);
    while (cur != nullptr) {
        if (cur->GetHandlerType() == EventHandlerType::INTERCEPTOR) {
            auto interceptor = std::static_pointer_cast<IInterceptorEventHandler>(cur);
            interceptor->RemoveInputHandler(handlerId, handlerType, session);
            return;
        }
        cur = cur->nextHandler_;
    }
}

void IInputEventHandler::AddKeyInterceptor(int32_t sourceType, int32_t id, SessionPtr session)
{
    std::shared_ptr<IInputEventHandler> cur = std::shared_ptr<IInputEventHandler>(this);
    while (cur != nullptr) {
        if (cur->GetHandlerType() == EventHandlerType::INTERCEPTOR) {
            auto interceptor = std::static_pointer_cast<IIncterceptorManagerEventHandler>(cur);
            interceptor->OnAddInterceptor(sourceType, id, session);
            return;
        }
        cur = cur->nextHandler_;
    }
}

void IInputEventHandler::RemoveKeyInterceptor(int32_t id)
{
    std::shared_ptr<IInputEventHandler> cur = std::shared_ptr<IInputEventHandler>(this);
    while (cur != nullptr) {
        if (cur->GetHandlerType() == EventHandlerType::INTERCEPTOR) {
            auto interceptor = std::static_pointer_cast<IIncterceptorManagerEventHandler>(cur);
            interceptor->OnRemoveInterceptor(id);
            return;
        }
        cur = cur->nextHandler_;
    }
}

int32_t IInputEventHandler::AddMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    std::shared_ptr<IInputEventHandler> cur = std::shared_ptr<IInputEventHandler>(this);
    while (cur != nullptr) {
        if (cur->GetHandlerType() == EventHandlerType::MONITOR) {
            auto monitor = std::static_pointer_cast<IMonitorEventHandler>(cur);
            return monitor->AddInputHandler(handlerId, handlerType, session);

        }
        cur = cur->nextHandler_;
    }
    return RET_OK;
}

void IInputEventHandler::RemoveMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    std::shared_ptr<IInputEventHandler> cur = std::shared_ptr<IInputEventHandler>(this);
    while (cur != nullptr) {
        if (cur->GetHandlerType() == EventHandlerType::MONITOR) {
            auto monitor = std::static_pointer_cast<IMonitorEventHandler>(cur);
            monitor->RemoveInputHandler(handlerId, handlerType, session);
            return;
        }
        cur = cur->nextHandler_;
    }
}

int32_t IInputEventHandler::AddSubscriber(SessionPtr sess, int32_t subscribeId, const std::shared_ptr<KeyOption> keyOption)
{
    std::shared_ptr<IInputEventHandler> cur = std::shared_ptr<IInputEventHandler>(this);
    while (cur != nullptr) {
        if (cur->GetHandlerType() == EventHandlerType::SUBSCRIBER) {
            auto subscriber = std::static_pointer_cast<ISubscriberEventHandler>(cur);
            return subscriber->SubscribeKeyEvent(sess, subscribeId, keyOption);
        }
        cur = cur->nextHandler_;
    }
    return RET_OK;
}

int32_t IInputEventHandler::RemoveSubscriber(SessionPtr sess, int32_t subscribeId)
{
    std::shared_ptr<IInputEventHandler> cur = std::shared_ptr<IInputEventHandler>(this);
    while (cur != nullptr) {
        if (cur->GetHandlerType() == EventHandlerType::SUBSCRIBER) {
            auto subscriber = std::static_pointer_cast<ISubscriberEventHandler>(cur);
            return subscriber->UnSubscribeKeyEvent(sess, subscribeId);
        }
        cur = cur->nextHandler_;
    }
    return RET_OK;
}


int32_t IInputEventHandler::AddFilter(sptr<IEventFilter> filter)
{
    std::shared_ptr<IInputEventHandler> cur = std::shared_ptr<IInputEventHandler>(this);
    while (cur != nullptr) {
        if (cur->GetHandlerType() == EventHandlerType::FILTER) {
            auto filterHalder = std::static_pointer_cast<IFilterEventHandler>(cur);
            return filterHalder->AddInputEventFilter(filter);
        }
        cur = cur->nextHandler_;
    }
    return RET_OK;
}

uint32_t IInputEventHandler::SetNext(std::shared_ptr<IInputEventHandler> nextHandler)
{
    std::shared_ptr<IInputEventHandler> cur = std::shared_ptr<IInputEventHandler>(this);
    std::shared_ptr<IInputEventHandler> next = cur->nextHandler_;

    if (next == nullptr) {
        cur->nextHandler_ = nextHandler;
        return RET_OK;
    }

    auto tmpPrioty = nextHandler->GetPriority();
    while (next != nullptr) {
        if (tmpPrioty < next->GetPriority()) {
            next->nextHandler_ = nextHandler;
            break;
        }

        cur = next;
        next = next->nextHandler_;

        if (next == nullptr) {
            cur->nextHandler_ = nextHandler;
            break;
        }        
    }

    return RET_OK;
}

} // namespace MMI
} // namespace OHOS