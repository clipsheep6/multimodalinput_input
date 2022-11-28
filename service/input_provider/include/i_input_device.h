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

#ifndef I_INPUT_DEVICE
#define I_INPUT_DEVICE

#include <memory>
#include <string>

namespace OHOS {
namespace MMI {
class IInputDevice {
public:
    explicit IInputDevice(uint32_t devIndex) : id_(devIndex) {}
    virtual ~IInputDevice() = default;
    virtual int32_t GetId() const { return id_; }
    virtual int32_t GetDeviceId() const = 0;
    virtual int32_t Enable() = 0;
    virtual int32_t Disable() = 0;
    virtual std::string GetName() = 0;
    virtual int32_t GetCapabilities() = 0;
    virtual std::string GetPath() = 0;
private:
    uint32_t id_;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_DEVICE