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

#ifndef UEVENT_DEVICE_MANAGER_H
#define UEVENT_DEVICE_MANAGER_H

#include <map>
#include <memory>

#include <nocopyable.h>
#include <singleton.h>
#include "device.h"

namespace OHOS {
namespace MMI {
namespace UEVENT {

class DeviceManager {
    DECLARE_DELAYED_SINGLETON(DeviceManager);

public:
    DISALLOW_COPY_AND_MOVE(DeviceManager);

    std::shared_ptr<Device> AddDevice(std::shared_ptr<UEvent> uevent, const std::string &devnode);
    std::shared_ptr<Device> AddInputDevice(std::shared_ptr<UEvent> uevent, const std::string &syspath);
    std::shared_ptr<Device> RemoveDevice(const std::string &devnode);
    std::shared_ptr<Device> FindDevice(const std::string &devnode);

private:
    bool IsSyspath(const std::string &syspath) const;
    bool IsInputDevice(const std::string &syspath) const;

private:
    std::map<std::string, std::shared_ptr<Device>> devices_;
};

#define UEDevMgr DelayedSingleton<DeviceManager>::GetInstance()
} // UEVENT
} // MMI
} // OHOS

#endif // UEVENT_DEVICE_MANAGER_H
