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
#ifndef I_INPUT_DEVICE_IMPL_H
#define I_INPUT_DEVICE_IMPL_H

#include <functional>
#include <vector>

#include "nocopyable.h"
#include "input_device_impl_type.h"

namespace OHOS {
namespace MMI {
class IInputDeviceImpl {
public:
    static IInputDeviceImpl& GetInstance();
    DISALLOW_COPY_AND_MOVE(IInputDeviceImpl);
    ~IInputDeviceImpl() = default;

    void RegisterInputDeviceMonitor(std::function<void(std::string, int32_t)> listening);
    void UnRegisterInputDeviceMonitor();

    void GetInputDeviceIdsAsync(std::function<void(int32_t, std::vector<int32_t>&)> callback);
    void GetInputDeviceAsync(int32_t deviceId,
        std::function<void(int32_t, std::shared_ptr<InputDeviceInfo>)> callback);
    void SupportKeys(int32_t deviceId, std::vector<int32_t> keyCodes,
        std::function<void(std::vector<bool>&)> callback);
    void GetKeyboardTypeAsync(int32_t deviceId, std::function<void(int32_t, int32_t)> callback);
    void OnInputDevice(int32_t userData, std::shared_ptr<InputDeviceInfo> devData);
    void OnInputDeviceIds(int32_t userData, std::vector<int32_t> &ids);
    void OnSupportKeys(int32_t userData, const std::vector<bool> &keystrokeAbility);
    void OnDevMonitor(std::string type, int32_t deviceId);
    void OnKeyboardType(int32_t userData, int32_t keyboardType);
    int32_t GetUserData();
private:
    IInputDeviceImpl() = default;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_DEVICE_IMPL_H