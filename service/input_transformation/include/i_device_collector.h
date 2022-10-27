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

#ifndef I_DEVICE_COLLECTOR_H
#define I_DEVICE_COLLECTOR_H

#include <memory>
#include <list>

#include "i_input_device.h"

namespace OHOS {
namespace MMI {
class IInputContext;
class IDeviceCollector {
public:
    static std::shared_ptr<IDeviceCollector> CreateInstance(IInputContext* context);

    virtual ~IDeviceCollector() = default;
    virtual std::shared_ptr<IInputDevice> GetDevice(int32_t id) const = 0;
    virtual std::list<int32_t> GetDeviceIdList() const = 0;

    virtual bool AddDevice(const std::shared_ptr<IInputDevice>& device) = 0;
    virtual std::shared_ptr<IInputDevice> RemoveDevice(int32_t id) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_DEVICE_COLLECTOR_H