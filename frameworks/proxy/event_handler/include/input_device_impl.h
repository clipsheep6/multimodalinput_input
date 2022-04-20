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
#ifndef OHOS_INPUT_DEVICE_EVENT_H
#define OHOS_INPUT_DEVICE_EVENT_H

#include <functional>
#include <map>
#include <mutex>

#include "nocopyable.h"

#include "mmi_event_handler.h"

namespace OHOS {
namespace MMI {
class InputDeviceImpl {
public:
    static InputDeviceImpl& GetInstance();
    DISALLOW_COPY_AND_MOVE(InputDeviceImpl);
    ~InputDeviceImpl() = default;

    struct InputDeviceInfo {
        InputDeviceInfo(int32_t id, std::string name, uint32_t devcieType) : id(id),
            name(name), devcieType(devcieType) {}
        int32_t id;
        std::string name;
        uint32_t devcieType;
    };

    using FunInputDevInfo = std::function<void(int32_t, std::shared_ptr<InputDeviceInfo>)>;
    using CppFunInputDevInfo = std::function<void(std::shared_ptr<InputDeviceInfo>)>;
    using FunInputDevIds = std::function<void(int32_t, std::vector<int32_t>)>;
    using CppFunInputDevIds = std::function<void(std::vector<int32_t>)>;
    using FunInputDevKeys = std::function<void(int32_t, std::map<int32_t, bool>)>;
    using CppFunInputDevKeys = std::function<void(std::map<int32_t, bool>)>;
    using FunInputDevMonitor = std::function<void(std::string, int32_t)>;
    using DevInfo = std::pair<EventHandlerPtr, FunInputDevInfo>;
    using DevIds = std::pair<EventHandlerPtr, FunInputDevIds>;
    using DevKeys = std::pair<EventHandlerPtr, FunInputDevKeys>;
    using DevMonitor = std::pair<EventHandlerPtr, FunInputDevMonitor>;
    struct InputDeviceData {
        DevInfo inputDevice;
        DevIds ids;
        DevKeys keys;
        CppFunInputDevInfo cppDev = nullptr;
        CppFunInputDevIds cppIds = nullptr;
        CppFunInputDevKeys cppKeys = nullptr;
    };

    void RegisterInputDeviceMonitor(std::function<void(std::string, int32_t)> listening);
    void UnRegisterInputDeviceMonitor();

    void GetInputDeviceIdsAsync(std::function<void(int32_t, std::vector<int32_t>)> callback);
    void GetInputDeviceAsync(int32_t deviceId,
        std::function<void(int32_t, std::shared_ptr<InputDeviceInfo>)> callback);
    void GetKeystrokeAbility(int32_t deviceId, std::vector<int32_t> keyCodes,
        std::function<void(int32_t, std::map<int32_t, bool>)> callback);
    void GetKeystrokeAbility(int32_t deviceId, std::vector<int32_t> keyCodes,
        std::function<void(std::map<int32_t, bool>)> callback);
    void OnInputDevice(int32_t userData, int32_t id, const std::string &name, int32_t deviceId);
    void OnInputDeviceIds(int32_t userData, const std::vector<int32_t> &ids);
    void OnKeystrokeAbility(int32_t userData, const std::map<int32_t, bool> &keystrokeAbility);
    void OnDevMonitor(std::string type, int32_t deviceId);
    int32_t GetUserData();

private:
    const DevInfo* GetDeviceInfo(int32_t) const;
    const DevIds* GetDeviceIds(int32_t) const;
    const DevKeys* GetDeviceKeys(int32_t) const;
    void OnInputDeviceTask(InputDeviceImpl::DevInfo devInfo, int32_t userData,
        int32_t id, std::string name, int32_t deviceId);
    void OnInputDeviceIdsTask(InputDeviceImpl::DevIds devIds, int32_t userData, std::vector<int32_t> ids);
    void OnKeystrokeAbilityTask(InputDeviceImpl::DevKeys devKeys, int32_t userData,
        std::map<int32_t, bool> keystrokeAbility);
    void OnDevMonitorTask(DevMonitor devMonitor, std::string type, int32_t deviceId);

private:
    InputDeviceImpl() = default;
    std::map<int32_t, InputDeviceData> inputDevices_;
    DevMonitor devMonitor_;
    std::mutex mtx_;
    int32_t userData_ {0};
};
} // namespace MMI
} // namespace OHOS
#define InputDevImp OHOS::MMI::InputDeviceImpl::GetInstance()
#endif // OHOS_INPUT_DEVICE_EVENT_H