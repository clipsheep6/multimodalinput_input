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

#include "setinputdevicetoscreen_fuzzer.h"

#include "input_manager.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "SetInputDeviceToScreenFuzzTest" };
} // namespace

void SetInputDeviceToScreenFuzzTest(const uint8_t* data, size_t  size)
{
    if (data == nullptr) {
        return;
    }
    MMI_HILOGD("SetInputDeviceToScreenFuzzTest");
    const std::string dhid(reinterpret_cast<const char*>(data), size);
    const std::string screenId(reinterpret_cast<const char*>(data), size);
    InputManager::GetInstance()->SetInputDeviceToScreen(dhid, screenId);
}
} // MMI
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::MMI::SetInputDeviceToScreenFuzzTest(data, size);
    return 0;
}

