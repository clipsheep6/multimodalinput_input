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
 
#include <cstring>

#include <dirent.h>

#include "device_manager.h"
// #include "IoUtils.h"
// #include "Log.h"
#include "mmi_log.h"
#include "device.h"
#include "i_seat_manager.h"
// #include "InputErrorCode.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "DeviceManager" };
};
std::unique_ptr<DeviceManager> DeviceManager::CreateInstance(IInputContext* context) 
{
    if (context == nullptr) {
        // errno = EINVAL;
        return nullptr;
    }
    return std::unique_ptr<DeviceManager>(new DeviceManager(context));
}

DeviceManager::DeviceManager(IInputContext* context)
    : context_(context)
{
}

DeviceManager::~DeviceManager() 
{
}

std::shared_ptr<IInputDevice> DeviceManager::GetDevice(int32_t id) const
{
    auto it = inputDevices_.find(id);
    if (it == inputDevices_.end()) {
        return nullptr;
    }
    return it->second;
}

std::list<int32_t> DeviceManager::GetDeviceIdList() const
{
    std::list<int32_t> result;
    for (auto it = inputDevices_.begin(); it != inputDevices_.end(); ++it) {
        result.push_back(it->first);
    }
    return result;
}

bool DeviceManager::AddDevice(const std::shared_ptr<IInputDevice>& device)
{
    MMI_HILOGD("Enter");
    if (!device) {
        MMI_HILOGE("Leave, null device");
        return false;
    }

    auto id = device->GetId();
    if (id < 0) {
        MMI_HILOGE("Leave, id < 0");
        return false;
    }

    if (GetDevice(id)) {
        MMI_HILOGE("Leave, repeat id");
        return false;
    }

    inputDevices_[id] = device;

    NotifyDeviceAdded(device);
    MMI_HILOGD("Leave");
    return true;
}

std::shared_ptr<IInputDevice> DeviceManager::RemoveDevice(int32_t id)
{
    MMI_HILOGD("Enter id:%{public}d", id);
    auto it = inputDevices_.find(id);
    if (it == inputDevices_.end()) {
        MMI_HILOGE("Leave id:%{public}d not exist device", id);
        return nullptr;
    }

    std::shared_ptr<IInputDevice> device = it->second;
    inputDevices_.erase(it);

    NotifyDeviceRemoved(device);

    MMI_HILOGD("Leave id:%{public}d", id);
    return device;
}

// std::shared_ptr<IInputDevice> DeviceManager::RemoveDevice(const std::string& deviceFile)
// {
//     MMI_HILOGD("Enter deviceFile:%{public}s", deviceFile.c_str());
//     for (auto it = inputDevices_.begin(); it != inputDevices_.end(); ) {
//         std::shared_ptr<IInputDevice> inputDevice = it->second;
//         if (!inputDevice) {
//             MMI_HILOGW("null inputDevice, remove it");
//             it = inputDevices_.erase(it);
//             continue;
//         }

//         if (inputDevice->GetDeviceFile() == deviceFile) {
//             inputDevices_.erase(it);
//             NotifyDeviceRemoved(inputDevice);
//             MMI_HILOGD("Leave");
//             return inputDevice;
//         }

//         ++it;
//     }
    
//     MMI_HILOGE("Leave deviceFile:%{public}s", deviceFile.c_str());
//     return nullptr;
// }

const std::unique_ptr<ISeatManager>& DeviceManager::GetSeatManager() const
{
    MMI_HILOGD("Enter");
    if (context_ == nullptr) {
        MMI_HILOGE("Leave, null context_");
        return ISeatManager::NULL_VALUE;
    }

    const auto& seatManager = context_->GetSeatManager();
    if (!seatManager) {
        MMI_HILOGE("Leave, null seatManager");
        return seatManager;
    }

    MMI_HILOGD("Leave");
    return seatManager;
}

void DeviceManager::NotifyDeviceAdded(const std::shared_ptr<IInputDevice>& device)
{
    MMI_HILOGD("Enter");
    if (!device) {
        MMI_HILOGE("Leave, null device");
        return;
    }

    const auto& seatManager = context_->GetSeatManager();
    if (!seatManager) {
        MMI_HILOGE("Leave, null seatManager");
        return;
    }

    seatManager->OnInputDeviceAdded(device);
}

void DeviceManager::NotifyDeviceRemoved(const std::shared_ptr<IInputDevice>& device)
{
    MMI_HILOGD("Enter");
    if (!device) {
        MMI_HILOGE("Leave, null device");
        return;
    }

    const auto& seatManager = context_->GetSeatManager();
    if (!seatManager) {
        MMI_HILOGE("Leave, null seatManager");
        return;
    }

    seatManager->OnInputDeviceRemoved(device);
}
} // namespace MMI
} // namespace OHOS