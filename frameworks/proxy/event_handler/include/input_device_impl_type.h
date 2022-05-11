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
#ifndef INPUT_DEVICE_IMPL_TYPE_H
#define INPUT_DEVICE_IMPL_TYPE_H

#include <vector>
#include <string>

namespace OHOS {
namespace MMI {
    struct ImplAxisInfo {
        int32_t axisType = 0;
        int32_t min = 0;
        int32_t max = 0;
        int32_t fuzz = 0;
        int32_t flat = 0;
        int32_t resolution = 0;
    };
    struct InputDeviceInfo {
        int32_t id = -1;
        std::string name = "null";
        uint32_t deviceType = 0;
        int32_t busType = 0;
        int32_t product = 0;
        int32_t vendor = 0;
        int32_t version = 0;
        std::string phys = "null";
        std::string uniq = "null";
        std::vector<ImplAxisInfo> axis;
    };
} // namespace MMI
} // namespace OHOS
#endif // INPUT_DEVICE_IMPL_TYPE_H