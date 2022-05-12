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

#ifndef I_INTERCEPTOR_HANDLER_GLOBAL_H
#define I_INTERCEPTOR_HANDLER_GLOBAL_H

#include "nocopyable.h"
#include "singleton.h"

#include "i_interceptor_event_handler.h"
#include "input_handler_type.h"
#include "pointer_event.h"
#include "uds_session.h"

namespace OHOS {
namespace MMI {
class IInterceptorHandlerGlobal : public IInterceptorEventHandler {
public:
    IInterceptorHandlerGlobal(int32_t priority) : IInterceptorEventHandler(priority) {};
    ~IInterceptorHandlerGlobal() = default;
    DISALLOW_COPY_AND_MOVE(IInterceptorHandlerGlobal);
    EventHandlerType GetHandlerType() const override { return EventHandlerType::INTERCEPTOR; }
#ifdef OHOS_BUILD_ENABLE_POINTER
    int32_t HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent) override;
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    int32_t HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent) override;
#endif // OHOS_BUILD_ENABLE_TOUCH
    static std::shared_ptr<IInterceptorHandlerGlobal> CreateInstance(int32_t priority);
    int32_t AddInputHandler(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
    void RemoveInputHandler(int32_t handlerId, InputHandlerType handlerType, SessionPtr session) override;
private:
    bool HandleEvent(std::shared_ptr<KeyEvent> keyEvent);
    bool HandleEvent(std::shared_ptr<PointerEvent> pointerEvent);
};
} // namespace MMI
} // namespace OHOS
#endif // I_INTERCEPTOR_HANDLER_GLOBAL_H