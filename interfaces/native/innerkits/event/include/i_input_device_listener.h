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

#ifndef I_INPUT_DEVICE_LISTENER_H
#define I_INPUT_DEVICE_LISTENER_H

#include <string>

namespace OHOS {
namespace MMI {
class IInputDeviceListener {
public:
    IInputDeviceListener() = default;
    virtual ~IInputDeviceListener() = default;
    virtual void OnDeviceAdded(int32_t deviceId, const std::string &type) = 0;
    virtual void OnDeviceRemoved(int32_t deviceId, const std::string &type) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_DEVICE_LISTENER_H