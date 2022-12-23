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

#ifndef INPUT_WINDOW_TRANSFER_H
#define INPUT_WINDOW_TRANSFER_H

#include "nocopyable.h"

#include "window_info.h"
#include "i_input_event_consumer.h"
#include "i_mmi_client.h"

namespace OHOS {
namespace MMI {
class InputWindowTransfer final {
public:
    InputWindowTransfer() = default;
    DISALLOW_MOVE(InputWindowTransfer);
    ~InputWindowTransfer() = default;
    int32_t GetDisplayBindInfo(DisplayBindInfos &infos);
    int32_t SetDisplayBind(int32_t deviceId, int32_t displayId, std::string &msg);
    void UpdateDisplayInfo(const DisplayGroupInfo &displayGroupInfo);
    void OnConnected();
    void SetWindowInputEventConsumer(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
        std::shared_ptr<AppExecFwk::EventHandler> eventHandler);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    int32_t OnKeyEvent(NetPacket& pkt);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    int32_t OnPointerEvent(NetPacket& pkt);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
private:
    void SendDisplayInfo();
    int32_t PackDisplayData(NetPacket &pkt);
    int32_t PackWindowInfo(NetPacket &pkt);
    int32_t PackDisplayInfo(NetPacket &pkt);
    void PrintDisplayInfo();
    void InitProcessedCallback();
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    void HandlerKeyEvent(std::shared_ptr<KeyEvent> keyEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    void HandlerPointerEvent(std::shared_ptr<PointerEvent> pointerEvent);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    void HandlerKeyEventTask(std::shared_ptr<IInputEventConsumer> consumer,
        std::shared_ptr<KeyEvent> keyEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    void HandlerPointerEventTask(std::shared_ptr<IInputEventConsumer> consumer,
        std::shared_ptr<PointerEvent> pointerEvent);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
    void OnDispatchEventProcessed(int32_t eventId, int64_t actionTime);
private:
    std::mutex mtx_;
    DisplayGroupInfo displayGroupInfo_ {};
    std::function<void(int32_t, int64_t)> dispatchCallback_ { nullptr };
    std::shared_ptr<IInputEventConsumer> consumer_ { nullptr };
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ { nullptr };
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_WINDOW_TRANSFER_H