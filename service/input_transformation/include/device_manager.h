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

#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <memory>
#include <map>

#include "i_input_define.h"
#include "i_device_manager.h"

namespace OHOS {
namespace MMI {
class IInputContext;
class ISeatManager;
class DeviceManager : public NonCopyable, public IDeviceManager {
public:
    static std::unique_ptr<DeviceManager> CreateInstance(IInputContext* context);

public:
    virtual ~DeviceManager();
    virtual std::shared_ptr<IInputDevice> GetDevice(int32_t id) const override;
    virtual std::list<int32_t> GetDeviceIdList() const override;

    virtual bool AddDevice(const std::shared_ptr<IInputDevice>& device) override;
    virtual std::shared_ptr<IInputDevice> RemoveDevice(int32_t id) override;

protected:
    DeviceManager(IInputContext* context);

private:
    const std::unique_ptr<ISeatManager>& GetSeatManager() const;
    void NotifyDeviceAdded(const std::shared_ptr<IInputDevice>& device);
    void NotifyDeviceRemoved(const std::shared_ptr<IInputDevice>& device);

private:
    IInputContext* const context_;
    std::map<int32_t, std::shared_ptr<IInputDevice>> inputDevices_;
};
} // namespace MMI
} // namespace OHOS
#endif // DEVICE_MANAGER_H