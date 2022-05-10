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

#ifndef INPUT_SOUTH_EVENT_HANDLER_H
#define INPUT_SOUTH_EVENT_HANDLER_H

#include <memory>

#include "nocopyable.h"
#include "singleton.h"

#include "event_dispatch.h"
#include "i_event_filter.h"
#include "i_input_event_handler.h"
#include "i_interceptor_handler_global.h"
#include "i_interceptor_manager_global.h"
#include "input_handler_manager_global.h"
#include "key_event_subscriber.h"
#include "mouse_event_handler.h"
#include "event_filter_wrap.h"
#include "msg_handler.h"
#include "key_event_handler.h"
#include "pointer_event_handler.h"
#include "touch_event_handler.h"

namespace OHOS {
namespace MMI {
class IInputSouthEventHandler {
public:
    virtual int32_t HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent) = 0;
    virtual int32_t HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent) = 0;
    virtual int32_t HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent) = 0;

    virtual int32_t AddMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual int32_t AddInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) = 0;
    virtual int32_t AddSubscriber(SessionPtr sess, int32_t subscribeId, const std::shared_ptr<KeyOption> keyOption) = 0;
    virtual int32_t AddFilter(SessionPtr sess, int32_t subscribeId, const std::shared_ptr<KeyOption> keyOption) = 0;

    std::shared_ptr<KeyEvent> GetKeyEvent() const;
    std::shared_ptr<IInputEventHandler> GetInputEventHandler() const;  

    // std::shared_ptr<KeyEventHandler> GetKeyEventHandler() const;
    // std::shared_ptr<PointerEventHandler> GetPointerEventHandler() const;
    // std::shared_ptr<TouchEventHandler> GetTouchEventHandler() const;
    
    std::shared_ptr<IInterceptorManagerGlobal> GetKeyInterceptorHandler() const;
    std::shared_ptr<KeyEventSubscriber> GetKeySubscriberHandler() const;
    std::shared_ptr<InputHandlerManagerGlobal> GetKeyMonitorHandler() const;

    std::shared_ptr<EventFilterWrap> GetPointerEventFilterHanlder() const;
    std::shared_ptr<IInterceptorHandlerGlobal> GetPointerInterceptorHandler() const;
    std::shared_ptr<InputHandlerManagerGlobal> GetPointerMonitorHandler() const;

    std::shared_ptr<EventFilterWrap> GetTouchEventFilterHandler() const;
    std::shared_ptr<IInterceptorHandlerGlobal> GetTouchInterceptorHandler() const;
    std::shared_ptr<InputHandlerManagerGlobal> GetTouchMonitorHandler() const;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_SOUTH_EVENT_HANDLER_H
