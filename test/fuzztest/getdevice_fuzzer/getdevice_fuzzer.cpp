/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "getdevice_fuzzer.h"

#include "input_manager.h"
#include "mmi_log.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "GetDeviceFuzzTest"

namespace OHOS {
namespace MMI {
void GetDeviceFuzzTest(const uint8_t* data, size_t /* size */)
{
    int32_t deviceId = *(reinterpret_cast<const int32_t*>(data));
    auto callback = [](std::shared_ptr<InputDevice> inputDevice) {
        MMI_HILOGD("Get device success");
    };
    InputManager::GetInstance()->GetDevice(deviceId, callback);
}
} // namespace MMI
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return 0;
    }
    if (size < sizeof(int32_t)) {
        return 0;
    }
    OHOS::MMI::GetDeviceFuzzTest(data, size);
    return 0;
}

