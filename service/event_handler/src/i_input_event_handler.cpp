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

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "IInputEventHandler" };
}
int32_t IInputEventHandler::HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Keyboard device dose not support");
        return RET_OK;
    }
    return nextHandler_->HandleTouchEvent(pointerEvent);
}
int32_t IInputEventHandler::HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("Pointer device dose not support");
        return RET_OK;
    }
    return nextHandler_->HandleTouchEvent(pointerEvent);
}
int32_t IInputEventHandler::HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    if (nextHandler_ == nullptr) {
        MMI_HILOGW("touch device dose not support");
        return RET_OK;
    }
    return nextHandler_->HandleTouchEvent(pointerEvent);
}

int32_t IInputEventHandler::AddMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    std::shared_ptr<IInputEventHandler> cur = this;
    bool 
    while (cur != nullptr) {
        if (cur->GetHandlerType() == HandlerType::MONITOR) {
            cur->AddMonitor(handlerId, handlerType, session);
            return;
        }
        cur = cur->nextHandler_;
    }
}

int32_t IInputEventHandler::AddMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    std::shared_ptr<IInputEventHandler> cur = this;
    bool 
    while (cur != nullptr) {
        if (cur->GetHandlerType() == HandlerType::MONITOR) {
            cur->AddMonitor(handlerId, handlerType, session);
            return;
        }
        cur = cur->nextHandler_;
    }
}

int32_t IInputEventHandler::AddSubscriber(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    std::shared_ptr<IInputEventHandler> cur = this;
    bool 
    while (cur != nullptr) {
        if (cur->GetHandlerType() == HandlerType::SUBSCRIBER) {
            auto subscriber = static_cast<ISubscriberEventHandler>(cur);
            return subscriber->SubscribeKeyEvent(handlerId, handlerType, session);
        }
        cur = cur->nextHandler_;
    }
    return RET_OK;
}

void IInputEventHandler::SetNext(std::shared_ptr<IInputEventHandler> nextHandler)
{
    std::shared_ptr<IInputEventHandler> cur = this;
    std::shared_ptr<IInputEventHandler> next = cur->nextHandler_;

    if (next == nullptr) {
        cur->nextHandler_ = nextHandler;
        return;
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
}
} // namespace MMI
} // namespace OHOS