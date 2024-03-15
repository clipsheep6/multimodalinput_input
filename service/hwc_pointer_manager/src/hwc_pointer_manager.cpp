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

#include "hwc_pointer_manager.h"
#include <thread>
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "HwcPointerManager" };
} // namespace
// my--my

int32_t HwcPointerManager::SetTargetDevice(uint32_t devId)
{
    if (devId != devId_) {
        devId_ = devId;
        isEnableState_ = false;
    }
    return RET_OK;
}

bool HwcPointerManager::IsSupported(uint32_t id, uint64_t& value)
{
    MMI_HILOGD("my--my HwcPointerManager::IsEnable enter.");
    if (isEnableState_) {
        MMI_HILOGD("my--my HwcPointerManager::IsEnable already checked.");
        return isEnable_;
    }
    powerInterface_ = IDisplayComposerInterface::Get();
    if (powerInterface_ == nullptr) {
        MMI_HILOGE("The hdf interface is null");
        return RET_ERR;
    }
     MMI_HILOGD("my--my HwcPointerManager::IsEnable 11111111111111.");
    // GetDisplayProperty参数devId 和 id不明确
    if (powerInterface_->GetDisplayProperty(devId_, id, value) != DISPLAY_SUCCESS) {
        MMI_HILOGE("Get display property is error.");
        return false;
    }
    if (value) {
        MMI_HILOGD("Get display property is support.");
        isEnable_ = true;
    }
    isEnableState_ = true;
    return isEnable_;
}

int32_t HwcPointerManager::SetPosition(int32_t x, int32_t y)
{
    /*if (powerInterface_->SetHardwareCursorPosition(devId, x, y) != DISPLAY_SUCCESS) {
        MMI_HILOGE("Set hardware cursor position is error.");
        return RET_ERR;
    }*/
    return RET_OK;
}

int32_t HwcPointerManager::EnableStats(bool enable)
{
    /*if (powerInterface_->EnableHardwareCursorStats(devId, enable) != DISPLAY_SUCCESS) {
        MMI_HILOGE("Enable hardware cursor stats is error.");
        return RET_ERR;
    }*/
    return RET_OK;
}

int32_t HwcPointerManager::QueryStats(uint32_t frameCount, uint32_t vsyncCount)
{
    /*if (powerInterface_->GetHardwareCursorStats(devId, frameCount, vsyncCount) != DISPLAY_SUCCESS) {
        MMI_HILOGE("Get hardware cursor stats is error.");
        return RET_ERR;
    }*/
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
