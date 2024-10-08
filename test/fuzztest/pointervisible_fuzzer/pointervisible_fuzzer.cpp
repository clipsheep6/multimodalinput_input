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

#include "pointervisible_fuzzer.h"

#include "securec.h"

#include "input_manager.h"
#include "mmi_log.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "PointerVisibleFuzzTest"

namespace OHOS {
namespace MMI {
template<class T>
size_t GetObject(const uint8_t *data, size_t size, T &object)
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

void PointerVisibleFuzzTest(const uint8_t* data, size_t size)
{
    int32_t random = 0;
    size_t startPos = 0;
    startPos += GetObject<int32_t>(data + startPos, size - startPos, random);
    bool visible = (random % 2) ? false : true;
    if (InputManager::GetInstance()->SetPointerVisible(visible) == RET_ERR) {
        MMI_HILOGD("Set pointer visible failed");
    }
    if (!InputManager::GetInstance()->IsPointerVisible()) {
        MMI_HILOGD("The pointer is not visible");
    }
}
} // namespace MMI
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::MMI::PointerVisibleFuzzTest(data, size);
    return 0;
}
