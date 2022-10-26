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

 #include "device_collector.h"

#include <cstddef>
#include <cstring>
#include <dirent.h>

#include "device.h"
#include "kernel_event_handler_bridge.h"
#include "mmi_log.h"
namespace OHOS {
namespace MMI {

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "DeviceCollector" };
};
std::unique_ptr<DeviceCollector> DeviceCollector::CreateInstance(IInputContext* context) 
{
    if (context == nullptr) {
        errno = EINVAL;
        return nullptr;
    }
    return std::unique_ptr<DeviceCollector>(new DeviceCollector(context));
}

DeviceCollector::DeviceCollector(IInputContext* context)
    : context_(context)
{
}

DeviceCollector::~DeviceCollector() 
{
}

std::shared_ptr<IInputDevice> DeviceCollector::GetDevice(int32_t id) const
{
    auto it = inputDevices_.find(id);
    if (it == inputDevices_.end()) {
        return nullptr;
    }
    return it->second;
}

std::list<int32_t> DeviceCollector::GetDeviceIdList() const
{
    std::list<int32_t> result;
    for (auto it = inputDevices_.begin(); it != inputDevices_.end(); ++it) {
        result.push_back(it->first);
    }
    return result;
}

bool DeviceCollector::AddDevice(const std::shared_ptr<IInputDevice>& device)
{
    CALL_DEBUG_ENTER;
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
    return true;
}

std::shared_ptr<IInputDevice> DeviceCollector::RemoveDevice(int32_t id)
{
    CALL_DEBUG_ENTER;
    auto it = inputDevices_.find(id);
    if (it == inputDevices_.end()) {
        MMI_HILOGE("Leave id:%{public}d not exist device", id);
        return nullptr;
    }
    std::shared_ptr<IInputDevice> device = it->second;
    inputDevices_.erase(it);
    NotifyDeviceRemoved(device);
    return device;
}

void DeviceCollector::NotifyDeviceAdded(const std::shared_ptr<IInputDevice>& device)
{
    CALL_DEBUG_ENTER;
    if (!device) {
        MMI_HILOGE("Leave, null device");
        return;
    }
    if (!device->HasCapability(IInputDevice::CAPABILITY_TOUCHSCREEN)) {
        MMI_HILOGE("Leave, invalid device!");
        return;
    }
    touchScreenHandler_ = ITouchScreenHandler::CreateInstance(context_);
    if (!touchScreenHandler_) {
        MMI_HILOGE("Leave , Create Failed");
        return;
    }
    auto retCode = touchScreenHandler_->BindInputDevice(device);
    if (retCode < 0) {
        MMI_HILOGE("Leave, BindInputDevice Failed");
        return;
    }
    auto handler = KernelEventHandlerBridge::CreateInstance(touchScreenHandler_);
    if (!handler) {
        MMI_HILOGE("Leave, null bridge handler");
        return;
    }
    device->StartReceiveEvents(handler);
}

void DeviceCollector::NotifyDeviceRemoved(const std::shared_ptr<IInputDevice>& device)
{
    CALL_DEBUG_ENTER;
    if (!device) {
        MMI_HILOGE("Leave, null device");
        return;
    }
    auto retCode = device->StopReceiveEvents();
    if (retCode < 0) {
        MMI_HILOGW("Leave, inputDevice StopReceiveEvents Failed");
    }

    if (touchScreenHandler_ != nullptr) {
        retCode = touchScreenHandler_->UnbindInputDevice(device);
        if (retCode < 0) {
            MMI_HILOGE("Leave, BindInputDevice Failed");
            return;
        }
    }
    MMI_HILOGD("Leave");
}
} // namespace MMI
} // namespace OHOS