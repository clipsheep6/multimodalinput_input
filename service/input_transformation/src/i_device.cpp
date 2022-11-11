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

#include "i_device.h"

#include <linux/input-event-codes.h>

namespace OHOS {
namespace MMI {
const std::string IDevice::AxisToString(int32_t axis)
{
    switch(axis) {
        CASE_STR(AXIS_NONE);
        CASE_STR(AXIS_MT_X);
        CASE_STR(AXIS_MT_Y);
        CASE_STR(AXIS_MAX);
        default:
        return "AXIS_UNKNOWN";
    }
}
} // namespace MMI
} // namespace OHOS

