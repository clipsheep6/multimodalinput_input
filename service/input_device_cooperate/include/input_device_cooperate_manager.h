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

#ifndef INPUT_DEVICE_COOPERATE_MANAGER_H
#define INPUT_DEVICE_COOPERATE_MANAGER_H

#include "input_device_cooperate_sm.h"
#include "singleton.h"

namespace OHOS {
namespace MMI {

class InputDeviceCooperateManager final : public InputDeviceCooperateSM {
    DECLARE_DELAYED_SINGLETON(InputDeviceCooperateManager);
public:
    DISALLOW_COPY_AND_MOVE(InputDeviceCooperateManager);
    int32_t StartInputDeviceCooperate(const std::string &remoteNetworkId, int32_t startInputDeviceId) override;
    int32_t StopInputDeviceCooperate(int32_t stopInputDeviceId);
private:
    void OnPrepareDistributedInput(bool isSuccess, int32_t startInputDeviceId);
    int32_t StartRemoteInput(int32_t startInputDeviceId);
    void OnStartRemoteInput(bool isSuccess);
    void OnStopRemoteInput(bool isSuccess);
};

#define InputDevCooManager ::OHOS::DelayedSingleton<InputDeviceCooperateManager>::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // INPUT_DEVICE_COOPERATE_SM_H
