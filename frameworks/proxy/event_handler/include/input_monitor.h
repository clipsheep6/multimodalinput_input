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

#ifndef INPUT_MONITOR_H
#define INPUT_MONITOR_H

#include <map>
#include <mutex>

#include "i_input_event_consumer.h"
#include "i_mmi_client.h"
#include "input_proxy_def.h"

namespace OHOS {
namespace MMI {
class InputMonitor final {
public:
    InputMonitor();
    DISALLOW_COPY_AND_MOVE(InputMonitor);
    ~InputMonitor() = default;
    void SetMMIClient(MMIClientPtr &client);
    int32_t AddMonitor(std::shared_ptr<IInputEventConsumer> monitor,
        HandleEventType eventType = HANDLE_EVENT_TYPE_ALL);
    void RemoveMonitor(int32_t monitorId);
    void MarkConsumed(int32_t monitorId, int32_t eventId);
    void OnConnected();
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    int32_t ReportMonitorKey(NetPacket& pkt);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    int32_t ReportMonitorPointer(NetPacket& pkt);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
private:
    int32_t AddLocal(int32_t handlerId, HandleEventType eventType,
        std::shared_ptr<IInputEventConsumer> monitor);
    int32_t AddToServer(HandleEventType eventType);
    int32_t RemoveLocal(int32_t handlerId);
    void RemoveFromServer(HandleEventType eventType);
    int32_t GetNextId();
    bool HasHandler(int32_t handlerId);
    HandleEventType GetEventType() const;
    void OnDispatchEventProcessed(int32_t eventId);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    void OnInputEvent(std::shared_ptr<KeyEvent> keyEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    void OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    void GetConsumerInfos(std::shared_ptr<PointerEvent> pointerEvent,
        std::map<int32_t, std::shared_ptr<IInputEventConsumer>> &consumerInfos);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
private:
    struct MonitorHandler {
        int32_t handlerId_ { 0 };
        HandleEventType eventType_ { HANDLE_EVENT_TYPE_ALL };
        std::shared_ptr<IInputEventConsumer> consumer_ { nullptr };
    };
    MMIClientPtr client_ { nullptr };
    std::function<void(int32_t)> monitorCallback_ { nullptr };
    std::map<int32_t, MonitorHandler> inputHandlers_;
    std::map<int32_t, int32_t> processedEvents_;
    std::mutex mtxHandlers_;
    int32_t nextId_ { 1 };
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_MONITOR_H