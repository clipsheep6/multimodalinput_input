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

#include "hardware_cursor_pointer_manager.h"
#include <thread>
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "HardwareCursorPointerManager" };
} // namespace

int32_t HardwareCursorPointerManager::SetTargetDevice(uint32_t devId)
{
    if (devId != devId_) {
        devId_ = devId;
        MMI_HILOGI("SetTargetDevice changed.");
        isEnableState_ = false;
    }
    return RET_OK;
}

bool HardwareCursorPointerManager::IsSupported(uint32_t id, uint64_t& value)
{
    MMI_HILOGD("IsSupported enter.");
    if (isEnableState_) {
        MMI_HILOGI("IsSupported already checked.");
        return false;
    }
    powerInterface_ = OHOS::HDI::Display::Composer::V1_2::IDisplayComposerInterface::Get();
    if (powerInterface_ == nullptr) {
        MMI_HILOGE("The hdf interface is null");
        return false;
    }
    if (powerInterface_->GetDisplayProperty(devId_,
        HDI::Display::Composer::V1_2::CAPABILITY_HARDWARE_CURSOR, value)
        != HDI::Display::Composer::V1_2::DISPLAY_SUCCESS) {
        MMI_HILOGE("Get display property is error.");
        return false;
    }
    if (value) {
        MMI_HILOGI("Get display property is support.");
        isEnable_ = true;
    }
    isEnableState_ = true;
    return isEnable_;
}

int32_t HardwareCursorPointerManager::SetPosition(int32_t x, int32_t y)
{
    if (powerInterface_->SetHardwareCursorPosition(devId_, x, y) != DISPLAY_SUCCESS) {
        MMI_HILOGE("Set hardware cursor position is error.");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t HardwareCursorPointerManager::EnableStats(bool enable)
{
    if (powerInterface_->EnableHardwareCursorStats(devId_, enable) != DISPLAY_SUCCESS) {
        MMI_HILOGE("Enable hardware cursor stats is error.");
        return RET_ERR;
    }
    MMI_HILOGE("my-my, EnableStats, enable:%{public}d", enable);
    return RET_OK;
}

int32_t HardwareCursorPointerManager::QueryStats(uint32_t &frameCount, uint32_t &vsyncCount)
{
    if (powerInterface_->GetHardwareCursorStats(devId_, frameCount, vsyncCount) != DISPLAY_SUCCESS) {
        MMI_HILOGE("Get hardware cursor stats is error.");
        return RET_ERR;
    }
    MMI_HILOGE("Get hardware cursor stats, frameCount:%{public}d, vsyncCount:%{public}d", frameCount, vsyncCount);
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
