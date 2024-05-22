/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef VIRTUAL_MOUSE_H
#define VIRTUAL_MOUSE_H

#include "virtual_device.h"
#include "v_input_device.h"

namespace OHOS {
namespace MMI {
class VirtualMouse final : public VirtualDevice {
public:
    VirtualMouse();
    ~VirtualMouse() = default;
    DISALLOW_COPY_AND_MOVE(VirtualMouse);

    bool SetUp() override;
    void SendEvent(uint16_t type, uint16_t code, int32_t value);

    std::string GetDevPath() const;

    static std::string GetDeviceName();

private:
    std::unique_ptr<VInputDevice> vMouse_;
};
} // namespace MMI
} // namespace OHOS
#endif // VIRTUAL_MOUSE_H