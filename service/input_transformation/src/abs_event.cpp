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
const char* AbsEvent::SourceToString(int32_t sourceType)
{
    switch (sourceType) {
    CASE_STR(SOURCE_TYPE_NONE);
    CASE_STR(SOURCE_TYPE_TOUCHSCREEN);
    CASE_STR(SOURCE_TYPE_TOUCHPAD);
    CASE_STR(SOURCE_TYPE_END);
    default:
        return "SOURCE_TYPE_UNKNOWN";
    }
}

const char* AbsEvent::ActionToString(int32_t action)
{
    switch(action) {
    CASE_STR(ACTION_NONE);
    CASE_STR(ACTION_DOWN);
    CASE_STR(ACTION_MOVE);
    CASE_STR(ACTION_UP);
    default:
        return "ACTION_UNKOWN";
    }
}

AbsEvent::AbsEvent(int32_t devIndex, int32_t sourceType)
    : KernelEventBase(devIndex, ACTION_NONE), pointerId_(-1), sourceType_(sourceType)
{}

int32_t AbsEvent::GetSourceType() const
{
    return sourceType_;
}

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

std::shared_ptr<AbsEvent::Pointer> AbsEvent::GetPointer() const
{
    return curPointer_;
}

void AbsEvent::SetCurSlot(int32_t curSlot)
{
    curSlot_ = curSlot;
}

int32_t AbsEvent::GetCurSlot() const
{
    return curSlot_;
}

int32_t AbsEvent::SetSourceType(int32_t sourceType)
{
    if ((sourceType <= SOURCE_TYPE_NONE) || (sourceType >= SOURCE_TYPE_END)) {
        MMI_HILOGE("Invalid sourceType:%{public}s", SourceToString(sourceType));
        return RET_ERR;
    }
    sourceType_ = sourceType;
    return RET_OK;
}

int32_t AbsEvent::AddPointer(const std::shared_ptr<Pointer> pointer)
{
    if (!pointer) {
        MMI_HILOGE("null pointer");
        return -1;
    }
    curPointer_ = pointer;
    return 0;
}

std::string AbsEvent::ActionToStr(int32_t action) const
{
    return ActionToString(action);
}

std::ostream& operator<<(std::ostream& os, const AbsEvent& r)
{
    os << '{' << "KernelEventBase:" 
        << static_cast<const KernelEventBase &>(r)
        << ','
        << "pointerId:" << r.pointerId_ << ','
        << "sourceType:" << AbsEvent::SourceToString(r.sourceType_) << ','
        << '}';
    return os;
}

std::ostream& operator<<(std::ostream& os, const AbsEvent::Pointer& r)
{
    os << '{'
        << "id:" << r.id_ << ','
        << "x:" << r.x_ << ','
        << "y:" << r.y_
        << '}';
    return os;
}
} // namespace MMI
} // namespace OHOS