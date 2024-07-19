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

#undef MMI_LOG_DOMAIN
#define MMI_LOG_DOMAIN MMI_LOG_CURSOR
#undef MMI_LOG_TAG
#define MMI_LOG_TAG "HardwareCursorPointerManager"

namespace OHOS {
namespace MMI {
void HardwareCursorPointerManager::SetTargetDevice(uint32_t devId)
{
    if (devId != devId_) {
        devId_ = devId;
        MMI_HILOGI("SetTargetDevice devId_ changed");
        isEnableState_ = false;
    }
}

bool HardwareCursorPointerManager::IsSupported()
{
    if (isEnableState_) {
        MMI_HILOGI("IsSupported already checked");
        return true;
    }
    if (isEnable_ && isDeviceChange_ && (isEnableState_ == false)) {
        MMI_HILOGI("Not Support");
        return false;
    }
    if (!isEnable_) {
        powerInterface_ = OHOS::HDI::Display::Composer::V1_2::IDisplayComposerInterface::Get(false);
        if (powerInterface_ == nullptr) {
            MMI_HILOGE("The hdf interface is null");
            return false;
        }
        isEnable_ = true;
    }
    CHKPR(powerInterface_, false);
    uint64_t value = 0;
    if (powerInterface_->GetDisplayProperty(devId_,
        HDI::Display::Composer::V1_2::DISPLAY_CAPBILITY_HARDWARE_CURSOR, value)
        != HDI::Display::Composer::V1_2::DISPLAY_SUCCESS) {
        MMI_HILOGE("Get display property is error");
        return false;
    }
    if (value) {
        MMI_HILOGI("Get display property is support");
        isEnableState_ = true;
        isDeviceChange_ = true;
    }
    return isEnableState_;
}

int32_t HardwareCursorPointerManager::SetPosition(int32_t x, int32_t y)
{
    CHKPR(powerInterface_, RET_ERR);
    if (powerInterface_->SetHardwareCursorPosition(devId_, x, y) != HDI::Display::Composer::V1_2::DISPLAY_SUCCESS) {
        MMI_HILOGE("Set hardware cursor position is error.");
        return RET_ERR;
    }
    MMI_HILOGD("SetPosition, x:%{public}d, y:%{public}d", x, y);
    return RET_OK;
}

int32_t HardwareCursorPointerManager::EnableStats(bool enable)
{
    CALL_DEBUG_ENTER;
    CHKPR(powerInterface_, RET_ERR);
    MMI_HILOGE("devId is:%{public}d", devId_);
    if (powerInterface_->EnableHardwareCursorStats(devId_, enable) != HDI::Display::Composer::V1_2::DISPLAY_SUCCESS) {
        MMI_HILOGE("Enable hardware cursor stats is error.");
        return RET_ERR;
    }
    MMI_HILOGD("EnableStats, enable:%{public}d", enable);
    return RET_OK;
}

int32_t HardwareCursorPointerManager::GetCursorStats(uint32_t &frameCount, uint32_t &vsyncCount)
{
    CALL_DEBUG_ENTER;
    CHKPR(powerInterface_, RET_ERR);
    MMI_HILOGE("devId is:%{public}d", devId_);
    if (powerInterface_->GetHardwareCursorStats(devId_, frameCount, vsyncCount) !=
        HDI::Display::Composer::V1_2::DISPLAY_SUCCESS) {
        MMI_HILOGE("Get hardware cursor stats is error.");
        return RET_ERR;
    }
    MMI_HILOGD("Get hardware cursor stats, frameCount:%{public}d, vsyncCount:%{public}d", frameCount, vsyncCount);
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
