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
#ifndef I_INPUT_WINDOWS_MANAGER_H
#define I_INPUT_WINDOWS_MANAGER_H

#include <tuple>
#include "struct_multimodal.h"
#include "i_input_device.h"

namespace OHOS {
namespace MMI {
class IInputWindowsManager {
public:
#ifdef OHOS_BUILD_ENABLE_TOUCH
    virtual bool TouchPointToDisplayPoint(
        std::tuple<std::shared_ptr<IInputDevice::AxisInfo>, std::shared_ptr<IInputDevice::AxisInfo>> axisInfo,
        std::tuple<int32_t, int32_t> raw, EventTouch& touchInfo, int32_t& physicalDisplayId, int32_t deviceId) = 0;
#endif // OHOS_BUILD_ENABLE_TOUCH
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_WINDOWS_MANAGER_H
