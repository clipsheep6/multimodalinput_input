/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef HWC_POINTER_MANAGER_H
#define HWC_POINTER_MANAGER_H

#include <mutex>
#include <iservmgr_hdi.h>
#include <v1_0/include/idisplay_composer_interface.h>
#include "singleton.h"

namespace OHOS {
namespace MMI {
using namespace OHOS::HDI::Display::Composer::V1_0;
// my--my
class HwcPointerManager {
public:
    HwcPointerManager() = default;
    ~HwcPointerManager() = default;
    int32_t SetTargetDevice(uint32_t devId);
    bool IsSupported(uint32_t id, uint64_t& value);
    int32_t SetPosition(int32_t x, int32_t y);
    int32_t EnableStats(bool enable);
    int32_t QueryStats(uint32_t frameCount, uint32_t vsyncCount);
private:
    bool isEnableState_ { false };
    bool isEnable_ { false };
    uint32_t devId_ { 0 };
    sptr<IDisplayComposerInterface> powerInterface_ = nullptr;
};
} // namespace MMI
} // namespace OHOS
#endif // HWC_POINTER_MANAGER_H
