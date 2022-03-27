/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "define_multimodal.h"
#include "dinput_manager.h"
#include "distributed_input_kit.h"
#include "input_device_manager.h"

namespace OHOS {
namespace MMI {
namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "DInputManager" };
    }

void DInputManager::SetMouseLocation(DMouseLocation info)
{
    mouseLocation = info;
}

DMouseLocation& DInputManager::GetMouseLocation()
{
    return this->mouseLocation;
}
std::string DInputManager::GetDeviceId()
{
    return deviceId_;
}

int32_t DInputManager::PrepareRemoteInput(const std::string& deviceId, sptr<PrepareDInputCallback> callback)
{
    MMI_LOGI("DInputManager::PrepareRemoteInput deviceId = %{public}s", deviceId.c_str());
    if (!deviceId.empty()) {
        deviceId_ = deviceId;
    } else {
        std::vector<std::string> ids = InputDevMgr->GetAllNodeDeviceInfoFromDM();
        if (ids.size() > 0) {
            deviceId_ = ids[0];
        }
    }

    return OHOS::DistributedHardware::DistributedInput::DistributedInputKit::PrepareRemoteInput(deviceId, callback);
}

int32_t DInputManager::UnPrepareRemoteInput(const std::string& deviceId, sptr<UnprepareDInputCallback> callback)
{ 
    deviceId_ = "";
    return OHOS::DistributedHardware::DistributedInput::DistributedInputKit::UnprepareRemoteInput(deviceId, callback);
}

int32_t DInputManager::StartRemoteInput(const std::string& deviceId, sptr<StartDInputCallback> callback)
{
    return OHOS::DistributedHardware::DistributedInput::DistributedInputKit::StartRemoteInput(deviceId, callback);
}

int32_t DInputManager::StopRemoteInput(const std::string& deviceId, sptr<StopDInputCallback> callback)
{
    return OHOS::DistributedHardware::DistributedInput::DistributedInputKit::StopRemoteInput(deviceId, callback);
}
}
}
