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

#ifndef VIRTUAL_KEYBOARD_EXT_H
#define VIRTUAL_KEYBOARD_EXT_H
#include "virtual_device.h"

namespace OHOS {
namespace MMI {
class VirtualKeyboardExt : public VirtualDevice {
public:
    VirtualKeyboardExt();
    ~VirtualKeyboardExt();
    DISALLOW_COPY_AND_MOVE(VirtualKeyboardExt);

protected:
    const std::vector<uint32_t>& GetEventTypes() const override;
    const std::vector<uint32_t>& GetKeys() const override;
    const std::vector<uint32_t>& GetMiscellaneous() const override;
    const std::vector<uint32_t>& GetRepeats() const override;
};
} // namespace MMI
} // namespace OHOS
#endif // VIRTUAL_KEYBOARD_EXT_H