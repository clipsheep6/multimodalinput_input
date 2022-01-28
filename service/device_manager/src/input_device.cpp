/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "input_device.h"

namespace OHOS {
namespace MMI {
void InputDevice::SetId(int32_t deviceId)
{
    id = deviceId;
}

int32_t InputDevice::GetId()
{
    return id;
}

void InputDevice::SetName(std::string name)
{
    this->name = name;
}

std::string InputDevice::GetName()
{
    return name;
}

void InputDevice::SetType(int32_t deviceType)
{
    this->deviceType = deviceType;
}

int32_t InputDevice::GetType()
{
    return deviceType;
}
}
}