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
#include "abs_event.h"

#include <algorithm>
#include <linux/input.h>

#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AbsEvent" };
};

AbsEvent::AbsEvent(int32_t devIndex, int32_t sourceType)
    : KernelEventBase(devIndex, ACTION_NONE), sourceType_(sourceType)
{}

void AbsEvent::SetAxisInfo(std::shared_ptr<IDevice::AxisInfo> xInfo,
    std::shared_ptr<IDevice::AxisInfo> yInfo)
{
    xInfo_ = xInfo;
    yInfo_ = yInfo;
}

std::tuple<std::shared_ptr<IDevice::AxisInfo>, std::shared_ptr<IDevice::AxisInfo>> AbsEvent::GetAxisInfo() const
{
    return { xInfo_, yInfo_ };
}

int32_t AbsEvent::SetSourceType(int32_t sourceType)
{
    if ((sourceType <= SOURCE_TYPE_NONE) || (sourceType >= SOURCE_TYPE_END)) {
        return RET_ERR;
    }
    sourceType_ = sourceType;
    return RET_OK;
}

int32_t AbsEvent::SetPointer(const std::shared_ptr<Pointer> pointer)
{
    if (!pointer) {
        MMI_HILOGE("null pointer");
        return RET_ERR;
    }
    curPointer_ = pointer;
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS