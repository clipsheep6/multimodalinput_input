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

#ifndef INTERCEPTOR_HANDLER_GLOBAL_H
#define INTERCEPTOR_HANDLER_GLOBAL_H

#include <set>

#include "i_input_event_handler.h"
#include "i_input_event_monitor_handler.h"
#include "input_handler_type.h"
#include "nocopyable.h"
#include "singleton.h"
#include "uds_session.h"

namespace OHOS {
namespace MMI {
class InterceptorHandlerGlobal : public IInputEventHandler {
public:
    InterceptorHandlerGlobal() = default;
    DISALLOW_COPY_AND_MOVE(InterceptorHandlerGlobal);
    ~InterceptorHandlerGlobal() = default;
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    void HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent) override;
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#ifdef OHOS_BUILD_ENABLE_POINTER
    void HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent) override;
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    void HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent) override;
#endif // OHOS_BUILD_ENABLE_TOUCH
    int32_t AddInputHandler(int32_t handlerId, InputHandlerType handlerType, SessionPtr session);
    void RemoveInputHandler(int32_t handlerId, InputHandlerType handlerType, SessionPtr session);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    bool HandleEvent(std::shared_ptr<KeyEvent> keyEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    bool HandleEvent(std::shared_ptr<PointerEvent> pointerEvent);

private:
    void InitSessionLostCallback();
    void OnSessionLost(SessionPtr session);

private:
    struct SessionHandler {
        SessionHandler(int32_t id, InputHandlerType handlerType, SessionPtr session)
            : id_(id), handlerType_(handlerType), session_(session) { }
        void SendToClient(std::shared_ptr<KeyEvent> keyEvent) const;
        void SendToClient(std::shared_ptr<PointerEvent> pointerEvent) const;
        bool operator<(const SessionHandler& other) const
        {
            if (id_ != other.id_) {
                return (id_ < other.id_);
            }
            if (handlerType_ != other.handlerType_) {
                return (handlerType_ < other.handlerType_);
            }
            return (session_ < other.session_);
        }
        int32_t id_;
        InputHandlerType handlerType_;
        SessionPtr session_ = nullptr;
    };

    struct InterceptorCollection : public IInputEventMonitorHandler, protected NoCopyable {
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
        virtual bool HandleEvent(std::shared_ptr<KeyEvent> keyEvent) override;
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
        virtual bool HandleEvent(std::shared_ptr<PointerEvent> pointerEvent) override;
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
        int32_t AddInterceptor(const SessionHandler& interceptor);
        void RemoveInterceptor(const SessionHandler& interceptor);
        void OnSessionLost(SessionPtr session);

        std::set<SessionHandler> interceptors_;
    };

private:
    bool sessionLostCallbackInitialized_ { false };
    InterceptorCollection interceptors_;
};
} // namespace MMI
} // namespace OHOS
#endif // INTERCEPTOR_HANDLER_GLOBAL_H