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

#include "common_method.h"
#include "define_multimodal.h"
#include "input_manager.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "UpdateDisplayInfoFuzzTest" };
} // namespace

size_t GetString(const uint8_t *data, size_t size, char *object, size_t objectSize)
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


void UpdateHotAreas(const uint8_t* data, size_t size, WindowInfo &windowInfo)
{
    size_t startPos = 0;
    std::vector<Rect> defaultHotAreasInfo;
    std::vector<Rect> pointerHotAreasInfo;
    for (size_t j = 0; j < WindowInfo::MAX_HOTAREA_COUNT; ++j) {
        Rect defaultRect;
        startPos += GetObject<int32_t>(defaultRect.height, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(defaultRect.width, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(defaultRect.x, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(defaultRect.y, data + startPos, size - startPos);
        defaultHotAreasInfo.push_back(defaultRect);
        Rect pointerRect;
        startPos += GetObject<int32_t>(pointerRect.height, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(pointerRect.width, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(pointerRect.x, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(pointerRect.y, data + startPos, size - startPos);
        pointerHotAreasInfo.push_back(pointerRect);
    }
    windowInfo.defaultHotAreas = defaultHotAreasInfo;
    windowInfo.pointerHotAreas = pointerHotAreasInfo;
}

void UpdateDisplayInfoFuzzTest(const uint8_t* data, size_t size)
{
    DisplayGroupInfo displayGroupInfo;
    size_t startPos = 0;
    size_t stringSize = 4;
    startPos += GetObject<int32_t>(displayGroupInfo.width, data + startPos, size - startPos);
    startPos += GetObject<int32_t>(displayGroupInfo.height, data + startPos, size - startPos);
    startPos += GetObject<int32_t>(displayGroupInfo.focusWindowId, data + startPos, size - startPos);
    std::vector<WindowInfo> windowsInfo;
    std::vector<DisplayInfo> displaysInfo;
    for (size_t i = 0; i < WindowInfo::MAX_HOTAREA_COUNT + 1; ++i) {
        WindowInfo windowInfo;
        startPos += GetObject<int32_t>(windowInfo.id, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(windowInfo.pid, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(windowInfo.uid, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(windowInfo.area.x, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(windowInfo.area.y, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(windowInfo.area.width, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(windowInfo.area.height, data + startPos, size - startPos);
        UpdateHotAreas(data, size, windowInfo);
        windowsInfo.push_back(windowInfo);

        DisplayInfo displayInfo;
        startPos += GetObject<int32_t>(displayInfo.id, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(displayInfo.x, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(displayInfo.y, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(displayInfo.width, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(displayInfo.height, data + startPos, size - startPos);
        startPos += GetObject<int32_t>(displayInfo.dpi, data + startPos, size - startPos);
        char name[] = "name";
        startPos += GetString(data + startPos, size - startPos, name, stringSize);
        displayInfo.name = name;
        char uniq[] = "uniq";
        startPos += GetString(data + startPos, size - startPos, uniq, stringSize);
        displayInfo.uniq = uniq;
        displaysInfo.push_back(displayInfo);
    }
    displayGroupInfo.windowsInfo = windowsInfo;
    displayGroupInfo.displaysInfo = displaysInfo;
    InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo);
    MMI_HILOGD("Update display info success");
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
