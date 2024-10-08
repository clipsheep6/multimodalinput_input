/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "getallsystemhotkeys_fuzzer.h"

#include "input_manager.h"
#include "mmi_log.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "GetAllSystemHotkeysFuzzTest"

namespace OHOS {
namespace MMI {
void GetAllSystemHotkeysFuzzTest(const uint8_t* data, size_t /* size */)
{
    std::vector<std::unique_ptr<KeyOption>> keyOptions;
    int32_t count = 0;
    OHOS::MMI::InputManager::GetInstance()->GetAllSystemHotkeys(keyOptions, count);
    if (!keyOptions.empty()) {
        MMI_HILOGD("Get all system hot key success");
    } else {
        MMI_HILOGD("Get all system hot key failed");
    }
}
} // MMI
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::MMI::GetAllSystemHotkeysFuzzTest(data, size);
    return 0;
}

