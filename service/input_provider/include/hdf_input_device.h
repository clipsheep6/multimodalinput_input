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

#ifndef HDF_INPUT_DEVICE
#define HDF_INPUT_DEVICE

#include <memory>
#include <map>
#include "i_input_device.h"

namespace OHOS {
namespace MMI {
class HDFInputDevice : public IInputDevice {
public:
    HDFInputDevice(int32_t devIndex);
    virtual ~HDFInputDevice();
    DISALLOW_COPY_AND_MOVE(HDFInputDevice);

    virtual int32_t GetDeviceId() const override;
    virtual int32_t Enable() override;
    virtual int32_t Disable() override;
    virtual std::string GetName() override;
    virtual int32_t GetCapabilities() override;
    virtual std::string GetPath() override;
// private:
//     int32_t deviceId_ { -1 };
//     int32_t capabilities_ { IDevice::CAPABILITY_UNKNOWN };
};
} // namespace MMI
} // namespace OHOS
#endif // HDF_INPUT_DEVICE