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

#ifndef I_INPUT_EVENT_HANDLER_H
#define I_INPUT_EVENT_HANDLER_H

#include <memory>

#include "define_multimodal.h"
#include "i_event_filter.h"
#include "input_handler_type.h"
#include "key_event.h"
#include "key_option.h"
#include "pointer_event.h"
#include "uds_session.h"

struct libinput_event;

namespace OHOS {
namespace MMI {
class IInputEventHandler : public std::enable_shared_from_this<IInputEventHandler> {
public:
    enum class EventHandlerType {
        NORMAL,
        INTERCEPTOR,
        SUBSCRIBER,
        MONITOR,
        FILTER
    };
public:
    IInputEventHandler(int32_t priority = 0);
    DISALLOW_COPY_AND_MOVE(IInputEventHandler);
    virtual ~IInputEventHandler() = default;
    virtual EventHandlerType GetHandlerType() const
    {
        return EventHandlerType::NORMAL; 
    }
    virtual int32_t HandleLibinputEvent(libinput_event* event);
    virtual int32_t HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent);
    virtual int32_t HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent);
    virtual int32_t HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent);

    int32_t GetPriority() const;
    int32_t AddMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session);
    void RemoveMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session);
    void TouchMonitorHandlerMarkConsumed(int32_t monitorId, int32_t eventId, SessionPtr sess);
    int32_t AddInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session);
    void RemoveInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session);
    void AddKeyInterceptor(int32_t sourceType, int32_t id, SessionPtr session) ;
    void RemoveKeyInterceptor(int32_t id) ;
    int32_t AddSubscriber(SessionPtr sess, int32_t subscribeId, const std::shared_ptr<KeyOption> keyOption);
    int32_t RemoveSubscriber(SessionPtr sess, int32_t subscribeId) ;
    int32_t AddFilter(sptr<IEventFilter> filter);

    template<class T>
    uint32_t AddConstructHandler(int32_t priority = 0);
    template<class T>
    uint32_t AddInstanceHandler(int32_t priority = 0);
protected:
    uint32_t SetNext(std::shared_ptr<IInputEventHandler> nextHandler);
protected:
    std::shared_ptr<IInputEventHandler> nextHandler_ = nullptr;
private:
    int32_t priority_;
};

template<class T>
uint32_t IInputEventHandler::AddConstructHandler(int32_t priority)
{
    auto handler = std::make_shared<T>(priority);
    if (handler == nullptr) {
        return RET_ERR;
    }
    return SetNext(handler);
}

template<class T>
uint32_t IInputEventHandler::AddInstanceHandler(int32_t priority)
{
    auto handler = T::CreateInstance(priority);
    if (handler == nullptr) {
        return RET_ERR;
    }
    return SetNext(handler);
}
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_EVENT_HANDLER_H