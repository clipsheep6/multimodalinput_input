/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#ifndef MOCK_H
#define MOCK_H

#include <memory>
#include <string>
#include <gmock/gmock.h>

#include "input_device.h"
#include "input_device_manager.h"

namespace OHOS {
namespace MMI {
class DfsMessageParcel {
public:
    virtual ~DfsMessageParcel() = default;
public:
    virtual bool HasCapability(uint32_t deviceTags) = 0;
    virtual std::shared_ptr<InputDevice> GetInputDevice(int32_t deviceId, bool checked) = 0;
public:
    static inline std::shared_ptr<DfsMessageParcel> messageParcel = nullptr;
};

class MessageParcelMock : public DfsMessageParcel {
public:
    MOCK_METHOD1(HasCapability, bool(uint32_t deviceTags));
    MOCK_METHOD2(GetInputDevice, std::shared_ptr<InputDevice>(int32_t deviceId, bool checked));
};
} // namespace MMI
} // namespace OHOS
#endif