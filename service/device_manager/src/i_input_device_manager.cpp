/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "i_input_device_manager.h"

#include "mmi_log.h"
#include "error_multimodal.h"

namespace OHOS {
namespace MMI {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "IInputDeviceManager"};
void IInputDeviceManager::OnInputDeviceAdded(struct libinput_device *inputDevice)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceManager::OnInputDeviceRemoved(struct libinput_device *inputDevice)
{
    MMI_HILOGW("device manager dose not support");
}

std::vector<int32_t> IInputDeviceManager::GetInputDeviceIds() const
{
    MMI_HILOGW("device manager dose not support");
    return {};
}

std::shared_ptr<InputDevice> IInputDeviceManager::GetInputDevice(int32_t id) const
{
    MMI_HILOGW("device manager dose not support");
    return nullptr;
}

std::vector<bool> IInputDeviceManager::SupportKeys(int32_t deviceId, std::vector<int32_t> &keyCodes)
{
    MMI_HILOGW("device manager dose not support");
    return {};
}

int32_t IInputDeviceManager::FindInputDeviceId(struct libinput_device *inputDevice)
{
    MMI_HILOGW("device manager dose not support");
    return ERROR_UNSUPPORT;
}

void IInputDeviceManager::AddDevMonitor(SessionPtr sess, std::function<void(std::string, int32_t)> callback)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceManager::RemoveDevMonitor(SessionPtr sess)
{
    MMI_HILOGW("device manager dose not support");
}

void IInputDeviceManager::Attach(std::shared_ptr<IDeviceObserver> observer)
{
    MMI_HILOGW("device manager dose not support");
    observers_.push_back(observer);
}

void IInputDeviceManager::Detach(std::shared_ptr<IDeviceObserver> observer)
{
    MMI_HILOGW("device manager dose not support");
    observers_.remove(observer);
}

void IInputDeviceManager::NotifyPointerDevice(bool hasPointerDevice)
{
    MMI_HILOGI("observers_ size:%{public}zu", observers_.size());
    for (auto observer = observers_.begin(); observer != observers_.end(); observer++) {
        (*observer)->UpdatePointerDevice(hasPointerDevice);
    }
}

#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
bool IInputDeviceManager::HasPointerDevice()
{
    MMI_HILOGW("device manager dose not support");
    return false;
}
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING
} // namespace MMI
} // namespace OHOS