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

#include "i_input_device_impl.h"

#include "error_multimodal.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "IInputDeviceImpl"};
} // namespace

IInputDeviceImpl& IInputDeviceImpl::GetInstance()
{
    static IInputDeviceImpl instance;
    return instance;
}

void IInputDeviceImpl::RegisterInputDeviceMonitor(std::function<void(std::string, int32_t)> listening)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceImpl::UnRegisterInputDeviceMonitor()
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceImpl::OnDevMonitor(std::string type, int32_t deviceId)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceImpl::GetInputDeviceIdsAsync(std::function<void(int32_t, std::vector<int32_t>&)> callback)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceImpl::GetInputDeviceAsync(int32_t deviceId,
    std::function<void(int32_t, std::shared_ptr<InputDeviceInfo>)> callback)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceImpl::SupportKeys(int32_t deviceId, std::vector<int32_t> keyCodes,
    std::function<void(std::vector<bool>&)> callback)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceImpl::GetKeyboardTypeAsync(int32_t deviceId, std::function<void(int32_t, int32_t)> callback)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceImpl::OnInputDevice(int32_t userData, std::shared_ptr<InputDeviceInfo> devData)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceImpl::OnInputDeviceIds(int32_t userData, std::vector<int32_t> &ids)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceImpl::OnSupportKeys(int32_t userData, const std::vector<bool> &keystrokeAbility)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceImpl::OnKeyboardType(int32_t userData, int32_t keyboardType)
{
    MMI_HILOGW("device manager dose not support");
}

int32_t IInputDeviceImpl::GetUserData()
{
    MMI_HILOGW("device manager dose not support");
    return ERROR_UNSUPPORT;
}
} // namespace MMI
} // namespace OHOS