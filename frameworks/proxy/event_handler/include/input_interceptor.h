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

#ifndef INPUT_INTERCEPTOR_H
#define INPUT_INTERCEPTOR_H

#include <map>
#include <mutex>

#include "i_input_event_consumer.h"
#include "input_proxy_def.h"
#include "net_packet.h"

namespace OHOS {
namespace MMI {
class InputInterceptor final {
public:
    InputInterceptor() = default;
    DISALLOW_COPY_AND_MOVE(InputInterceptor);
    ~InputInterceptor() = default;
    int32_t AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptor,
        HandleEventType eventType = HANDLE_EVENT_TYPE_ALL);
    void RemoveInterceptor(int32_t interceptorId);
    void OnConnected();
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    int32_t ReportInterceptorKey(NetPacket& pkt);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    int32_t ReportInterceptorPointer(NetPacket& pkt);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
private:
    int32_t AddLocal(int32_t handlerId, HandleEventType eventType,
        std::shared_ptr<IInputEventConsumer> monitor);
    int32_t AddToServer(HandleEventType eventType);
    int32_t RemoveLocal(int32_t handlerId);
    void RemoveFromServer(HandleEventType eventType);
    int32_t GetNextId();
    HandleEventType GetEventType() const;
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    void OnInputEvent(std::shared_ptr<KeyEvent> keyEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    void OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
private:
    struct InterceptorHandler {
        int32_t handlerId_ { 0 };
        HandleEventType eventType_ { HANDLE_EVENT_TYPE_ALL };
        std::shared_ptr<IInputEventConsumer> consumer_ { nullptr };
    };
    std::map<int32_t, InterceptorHandler> inputHandlers_;
    std::mutex mtxHandlers_;
    int32_t nextId_ { 1 };
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_INTERCEPTOR_H
