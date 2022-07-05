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

#include "updatedisplayinfo_fuzzer.h"

#include <string>

#include "securec.h"

#include "define_multimodal.h"
#include "input_manager.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "UpdateDisplayInfoFuzzTest" };
} // namespace
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

size_t GetString(size_t objectSize, const uint8_t *data, size_t size, std::string &object)
{
    if (objectSize > size) {
        return 0;
    }
    errno_t ret = memcpy_s(&object, objectSize, data, objectSize);
    if (ret != EOK) {
        return 0;
    }
    return objectSize;
}

void UpdateDisplayInfoFuzzTest(const uint8_t* data, size_t size)
{
    DisplayGroupInfo displayGroupInfo;
    size_t startPos = 0;
    startPos += GetObject<int32_t>(data + startPos, size - startPos, displayGroupInfo.width);
    startPos += GetObject<int32_t>(data + startPos, size - startPos, displayGroupInfo.height);
    startPos += GetObject<int32_t>(data + startPos, size - startPos, displayGroupInfo.focusWindowId);
    std::vector<WindowInfo> windowsInfos;
    std::vector<DisplayInfo> displaysInfos;
    for (size_t i = 0; i < WindowInfo::MAX_HOTAREA_COUNT + 1; ++i) {
        WindowInfo windowInfo;
        startPos += GetObject<int32_t>(data + startPos, size - startPos, windowInfo.id);
        startPos += GetObject<int32_t>(data + startPos, size - startPos, windowInfo.pid);
        startPos += GetObject<int32_t>(data + startPos, size - startPos, windowInfo.uid);
        startPos += GetObject<int32_t>(data + startPos, size - startPos, windowInfo.area.x);
        startPos += GetObject<int32_t>(data + startPos, size - startPos, windowInfo.area.y);
        startPos += GetObject<int32_t>(data + startPos, size - startPos, windowInfo.area.width);
        startPos += GetObject<int32_t>(data + startPos, size - startPos, windowInfo.area.height);
        windowsInfos.push_back(windowInfo);

        DisplayInfo displayInfo;
        startPos += GetObject<int32_t>(data + startPos, size - startPos, displayInfo.id);
        startPos += GetObject<int32_t>(data + startPos, size - startPos, displayInfo.x);
        startPos += GetObject<int32_t>(data + startPos, size - startPos, displayInfo.y);
        startPos += GetObject<int32_t>(data + startPos, size - startPos, displayInfo.width);
        startPos += GetObject<int32_t>(data + startPos, size - startPos, displayInfo.height);

        size_t objectSize = 0;
        std::string name = "";
        size_t ret = 0;
        ret = GetString(objectSize, data, size, name);
        if (ret == 0) {
            MMI_HILOGD("%{public}s:%{public}d The return value is 0", __func__, __LINE__);
            return;
        }
        displayInfo.name = name;
        std::string uniq = "";
        ret = GetString(objectSize, data, size, uniq);
        if (ret == 0) {
            MMI_HILOGD("%{public}s:%{public}d The return value is 0", __func__, __LINE__);
            return;
        }
        displayInfo.uniq = uniq;
        displaysInfos.push_back(displayInfo);
    }
    displayGroupInfo.windowsInfo = windowsInfos;
    displayGroupInfo.displaysInfo = displaysInfos;
    InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo);
    MMI_HILOGD("update display info success");
}
} // namespace MMI
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::MMI::UpdateDisplayInfoFuzzTest(data, size);
    return 0;
}
