/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "subscribeswitchevent_fuzzer.h"

#include "securec.h"

#include "input_manager.h"
#include "mmi_log.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "SubscribeSwitchEventFuzzTest"

namespace OHOS {
namespace MMI {
template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    errno_t ret = memcpy_s(&object, objectSize, data, objectSize);
    if (ret != EOK) {
        return 0;
    }
    return objectSize;
}

void SubscribeSwitchEventFuzzTest(const uint8_t* data, size_t size)
{
    size_t startPos = 0;
    int32_t rowsBefore;
    startPos += GetObject<int32_t>(rowsBefore, data + startPos, size - startPos);
    auto fun = [](std::shared_ptr<SwitchEvent> event) {
        MMI_HILOGD("Subscribe SwitchEvent success");
    };
    int32_t subscribeId = InputManager::GetInstance()->SubscribeSwitchEvent(fun);
    InputManager::GetInstance()->UnsubscribeSwitchEvent(subscribeId);
}
} // namespace MMI
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::MMI::SubscribeSwitchEventFuzzTest(data, size);
    return 0;
}

