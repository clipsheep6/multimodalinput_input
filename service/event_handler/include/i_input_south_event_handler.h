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

#ifndef I_INPUT_SOUTH_EVENT_HANDLER_H
#define I_INPUT_SOUTH_EVENT_HANDLER_H

#include <memory>

#include "i_event_filter.h"
#include "input_handler_type.h"
#include "key_event.h"
#include "key_option.h"
#include "pointer_event.h"
#include "uds_session.h"

namespace OHOS {
namespace MMI {
class IInputSouthEventHandler {
public:
    virtual int32_t HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent) = 0;
    virtual int32_t HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent) = 0;
    virtual int32_t HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent) = 0;

    virtual int32_t AddKeyInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual void RemoveKeyInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual int32_t AddPointerInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual void RemovePointerInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual int32_t AddTouchInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual void RemoveTouchInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;  
 
    virtual void AddKeyMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual void RemoveKeyMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual void AddPointerMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual void RemovePointerMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual void AddTouchMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual void RemoveTouchMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual void MarkTouchConsumed(int32_t monitorId, int32_t eventId, SessionPtr sess) = 0;
    virtual int32_t AddKeySubscriber(SessionPtr sess, int32_t subscribeId, const std::shared_ptr<KeyOption> keyOption) = 0;
    virtual int32_t RemoveKeySubscriber(SessionPtr sess, int32_t subscribeId) = 0;

    virtual int32_t AddFilter(sptr<IEventFilter> filter) = 0;
    virtual std::shared_ptr<KeyEvent> GetKeyEvent() const = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_SOUTH_EVENT_HANDLER_H
