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

#ifndef DEVICE_COLLECTOR_H
#define DEVICE_COLLECTOR_H

#include <memory>
#include <map>

#include "i_device_collector.h"
#include "i_input_define.h"
#include "i_touch_screen_handler.h"

namespace OHOS {
namespace MMI {
class IInputContext;
class DeviceCollector : public NonCopyable, public IDeviceCollector {
public:
    static std::unique_ptr<DeviceCollector> CreateInstance(IInputContext* context);

public:
    virtual ~DeviceCollector();
    virtual std::shared_ptr<IInputDevice> GetDevice(int32_t id) const override;
    virtual std::list<int32_t> GetDeviceIdList() const override;

    virtual bool AddDevice(const std::shared_ptr<IInputDevice>& device) override;
    virtual std::shared_ptr<IInputDevice> RemoveDevice(int32_t id) override;

protected:
    DeviceCollector(IInputContext* context);

private:
    void NotifyDeviceAdded(const std::shared_ptr<IInputDevice>& device);
    void NotifyDeviceRemoved(const std::shared_ptr<IInputDevice>& device);

private:
    IInputContext* const context_;
    std::shared_ptr<ITouchScreenHandler> touchScreenHandler_;
    std::map<int32_t, std::shared_ptr<IInputDevice>> inputDevices_;
};
} // namespace MMI
} // namespace OHOS
#endif // DEVICE_COLLECTOR_H