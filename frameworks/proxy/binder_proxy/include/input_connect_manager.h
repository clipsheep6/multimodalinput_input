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

#ifndef INPUT_CONNECT_MANAGER_H
#define INPUT_CONNECT_MANAGER_H

#include <map>
#include <memory>
#include <set>
#include <string>

#include "nocopyable.h"

#include "i_input_connect.h"

namespace OHOS {
namespace MMI {
class InputConnectManager final : public std::enable_shared_from_this<InputConnectManager> {
public:
    ~InputConnectManager() = default;
    static std::shared_ptr<InputConnectManager> GetInstance();
    int32_t AllocSocketPair(const int32_t moduleType);
    int32_t GetClientSocketFdOfAllocedSocketPair() const;
    int32_t GetTokenType() const
    {
        return tokenType_;
    }
    int32_t AddInputEventFilter(sptr<IEventFilter> filter, int32_t filterId, int32_t priority);
    int32_t RemoveInputEventFilter(int32_t filterId);
    int32_t SetPointerVisible(bool visible);
    int32_t IsPointerVisible(bool &visible);
    int32_t SetPointerSpeed(int32_t speed);
    int32_t GetPointerSpeed(int32_t &speed);
    int32_t SetPointerStyle(int32_t windowId, int32_t pointerStyle);
    int32_t GetPointerStyle(int32_t windowId, int32_t &pointerStyle);
    int32_t SupportKeys(int32_t deviceId, std::vector<int32_t> &keys, std::vector<bool> &keystroke);
    int32_t GetDeviceIds(std::vector<int32_t> &ids);
    int32_t GetDevice(int32_t deviceId, std::shared_ptr<InputDevice> &inputDevice);
    int32_t RegisterDevListener();
    int32_t UnregisterDevListener();
    int32_t GetKeyboardType(int32_t deviceId, int32_t &keyboardType);
    int32_t AddInterceptorHandler(HandleEventType eventType, int32_t priority, uint32_t deviceTags);
    int32_t RemoveInterceptorHandler(HandleEventType eventType, int32_t priority, uint32_t deviceTags);
    int32_t AddMonitorHandler(HandleEventType eventType);
    int32_t RemoveMonitorHandler(HandleEventType eventType);
    int32_t MarkEventConsumed(int32_t eventId);
    int32_t MoveMouseEvent(int32_t offsetX, int32_t offsetY);
    int32_t InjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent);
    int32_t SubscribeKeyEvent(int32_t subscribeId, const std::shared_ptr<KeyOption> option);
    int32_t UnsubscribeKeyEvent(int32_t subscribeId);
    int32_t InjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent);
    int32_t SetAnrObserver();
    int32_t RegisterCooperateListener();
    int32_t UnregisterCooperateListener();
    int32_t EnableInputDeviceCooperate(int32_t userData, bool enabled);
    int32_t StartInputDeviceCooperate(int32_t userData, const std::string &sinkDeviceId, int32_t srcInputDeviceId);
    int32_t StopDeviceCooperate(int32_t userData);
    int32_t GetInputDeviceCooperateState(int32_t userData, const std::string &deviceId);
    int32_t SetInputDevice(const std::string& dhid, const std::string& screenId);
    int32_t GetFunctionKeyState(int32_t funcKey, bool &state);
    int32_t SetFunctionKeyState(int32_t funcKey, bool enable);
    int32_t SetPointerLocation(int32_t x, int32_t y);

private:
    InputConnectManager() = default;
    DISALLOW_COPY_AND_MOVE(InputConnectManager);

    bool ConnectInputService();
    void OnDeath();
    void Clean();
    void NotifyDeath();
    sptr<IInputConnect> inputConnectService_ { nullptr };
    sptr<IRemoteObject::DeathRecipient> inputConnectRecipient_ { nullptr };
    int32_t socketFd_ { IInputConnect::INVALID_SOCKET_FD };
    int32_t tokenType_ { -1 };
};
} // namespace MMI
} // namespace OHOS
#define MultimodalInputConnMgr InputConnectManager::GetInstance()
#endif // INPUT_CONNECT_MANAGER_H