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

#include "device_manager.h"
#include "discriminator.h"
#include "utility.h"
#include "mmi_log.h"


namespace OHOS {
namespace MMI {
namespace UEVENT {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, ::OHOS::MMI::MMI_LOG_DOMAIN, "UeventDevMgr" };
}

DeviceManager::DeviceManager() {}
DeviceManager::~DeviceManager() {}

std::shared_ptr<Device> DeviceManager::AddDevice(std::shared_ptr<UEvent> uevent, const std::string &devnode)
{
    CALL_INFO_TRACE;
    std::shared_ptr<Device> dev = FindDevice(devnode);
    if (dev != nullptr) {
        MMI_HILOGD("Already exists: %{public}s", devnode.c_str());
        return dev;
    }
    const std::string devpath { DEV_INPUT_PATH + devnode };
    struct stat statbuf;

    if (stat(devpath.c_str(), &statbuf) != 0) {
        MMI_HILOGD("Invalid device path: %{public}s", devpath.c_str());
        return nullptr;
    }
    if (!S_ISCHR(statbuf.st_mode)) {
        MMI_HILOGD("Not character device: %{public}s", devpath.c_str());
        return nullptr;
    }

    const std::string lsyspath { SYS_INPUT_PATH + devnode };
    char rpath[PATH_MAX];
    if (realpath(lsyspath.c_str(), rpath) == nullptr) {
        MMI_HILOGD("Invalid syspath: %{public}s", lsyspath.c_str());
        return nullptr;
    }

    dev = std::make_shared<Device>(uevent);
    CHKPP(dev);
    dev->SetDevnode(devpath);
    dev->SetSyspath(std::string(rpath));

    auto discriminator { UEDiscriminator };
    CHKPP(discriminator);
    if (!discriminator->UpdateDeviceCapability(dev)) {
        MMI_HILOGE("Failed to update device capability: \'%{public}s\'", dev->GetDevnode());
        return nullptr;
    }

    auto [tIter, isOk] = devices_.emplace(devnode, dev);
    return (isOk ? dev : nullptr);
}

std::shared_ptr<Device> DeviceManager::AddInputDevice(std::shared_ptr<UEvent> uevent, const std::string &syspath)
{
    CALL_DEBUG_ENTER;
    if (!IsSyspath(syspath)) {
        MMI_HILOGD("Not syspath: %{public}s", syspath.c_str());
        return nullptr;
    }
    if (!IsInputDevice(syspath)) {
        MMI_HILOGD("Not input device: %{public}s", syspath.c_str());
        return nullptr;
    }
    std::string::size_type tpos { syspath.rfind('/') };
    if (tpos == std::string::npos) {
        return nullptr;
    }
    std::string devnode { syspath.substr(tpos + 1) };
    if (!Utility::StartWith(devnode, std::string("event"))) {
        MMI_HILOGD("Not device node: %{public}s", devnode.c_str());
        return nullptr;
    }
    return AddDevice(uevent, devnode);
}

std::shared_ptr<Device> DeviceManager::RemoveDevice(const std::string &devnode)
{
    CALL_DEBUG_ENTER;
    auto tIter = devices_.find(devnode);
    if (tIter == devices_.end()) {
        return nullptr;
    }
    std::shared_ptr<Device> dev { tIter->second };
    devices_.erase(tIter);
    return dev;
}

std::shared_ptr<Device> DeviceManager::FindDevice(const std::string &devnode)
{
    auto tIter = devices_.find(devnode);
    return (tIter != devices_.end() ? tIter->second : nullptr);
}

bool DeviceManager::IsSyspath(const std::string &syspath) const
{
    if (!Utility::StartWith(syspath, std::string("/sys"))) {
        return false;
    }
    char rpath[PATH_MAX];
    if (realpath(syspath.c_str(), rpath) == nullptr) {
        return false;
    }
    std::string sfname { Utility::ConcatAsString(rpath, "/uevent") };
    struct stat statbuf;
    return (stat(sfname.c_str(), &statbuf) == 0);
}

bool DeviceManager::IsInputDevice(const std::string &syspath) const
{
    char fullpath[PATH_MAX];
    std::string::size_type ncopied { syspath.copy(fullpath, sizeof(fullpath) - 1) };
    fullpath[ncopied] = '\0';

    for (char *pos = strrchr(fullpath, '/'); pos != nullptr; pos = strrchr(fullpath, '/')) {
        if (strcmp(pos, "/input") == 0) {
            return true;
        }
        *pos = '\0';
    }
    return false;
}
} // namespace UEVENT
} // namespace MMI
} // namespace OHOS
