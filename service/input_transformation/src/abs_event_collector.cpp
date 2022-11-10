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

#include "abs_event_collector.h"

#include <linux/input.h>

#include "abs_event.h"
#include "time_utils.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AbsEventCollector" };
};
AbsEventCollector::AbsEventCollector(int32_t devIndex, int32_t sourceType)
    : sourceType_(sourceType), curSlot_(0), absEvent_(new AbsEvent(devIndex, sourceType))
{}

void AbsEventCollector::HandleAbsEvent(int32_t code, int32_t value)
{
    CALL_DEBUG_ENTER;
    switch (code) {
        case ABS_MT_SLOT:
            HandleMtSlot(value);
            break;
        case ABS_MT_TOUCH_MAJOR:
        case ABS_MT_TOUCH_MINOR:
        case ABS_MT_WIDTH_MAJOR:
        case ABS_MT_WIDTH_MINOR:
        case ABS_MT_ORIENTATION:
            break;
        case ABS_MT_POSITION_X:
            HandleMtPositionX(value);
            break;
        case ABS_MT_POSITION_Y:
            HandleMtPositionY(value);
            break;
        case ABS_MT_TOOL_TYPE:
        case ABS_MT_BLOB_ID:
            break;
        case ABS_MT_TRACKING_ID:
            HandleMtTrackingId(value);
            break;
        case ABS_MT_PRESSURE:
        case ABS_MT_DISTANCE:
        case ABS_MT_TOOL_X:
        case ABS_MT_TOOL_Y:
        default:
            break;
    }
}

const std::shared_ptr<AbsEvent> AbsEventCollector::HandleSyncEvent(int32_t code, int32_t value)
{
    return FinishPointer();
}

void AbsEventCollector::AfterProcessed()
{
    RemoveReleasedPointer();
}

int32_t AbsEventCollector::SetSourceType(int32_t sourceType)
{
    CALL_DEBUG_ENTER;
    if (sourceType <= AbsEvent::SOURCE_TYPE_NONE || sourceType >= AbsEvent::SOURCE_TYPE_END) {
        return -1;
    }

    if (sourceType_ > AbsEvent::SOURCE_TYPE_NONE && sourceType_ < AbsEvent::SOURCE_TYPE_END) {
        return -1;
    }
    sourceType_ = sourceType;
    absEvent_->SetSourceType(sourceType_);

    return 0;
}

void AbsEventCollector::SetAxisInfo(std::shared_ptr<IDevice::AxisInfo> xInfo,
    std::shared_ptr<IDevice::AxisInfo> yInfo)
{
    xInfo_ = xInfo;
    yInfo_ = yInfo;
}

std::shared_ptr<AbsEvent::Pointer> AbsEventCollector::GetCurrentPointer(bool createIfNotExist)
{
    if (curSlot_ < 0) {
        MMI_HILOGE("Leave, curSlot_ < 0");
        return nullptr;
    }

    auto it = pointers_.find(curSlot_);
    if (it != pointers_.end()) {
        return it->second;
    }

    if (!createIfNotExist) {
        MMI_HILOGD("Leave, null pointer and !createIfNotExist");
        return nullptr;
    }

    auto pointer = std::make_shared<AbsEvent::Pointer>();
    pointers_[curSlot_] = pointer;
    return pointer;
}

const std::shared_ptr<AbsEvent> AbsEventCollector::FinishPointer()
{
    CALL_DEBUG_ENTER;
    auto pointer = GetCurrentPointer(false);
    if (!pointer) {
        MMI_HILOGE("pointer is null. Leave.");
        return {};
    }
    auto nowTime = TimeUtils::GetTimeStampMs();
    if (absEventAction_ == AbsEvent::ACTION_DOWN) {
           auto retCode = absEvent_->AddPointer(pointer);
            if (retCode < 0) {
                MMI_HILOGE("Leave, absAction:%{public}s AddPointer Failed", AbsEvent::ActionToString(absEventAction_));
                return {};
            }
        pointer->SetDownTime(nowTime);
    }
    absEvent_->SetAction(absEventAction_);
    absEvent_->SetCurSlot(curSlot_);
    absEvent_->SetActionTime(nowTime);
    absEvent_->SetAxisInfo(xInfo_, yInfo_);
    return absEvent_;
}

void AbsEventCollector::HandleMtSlot(int32_t value)
{
    if (value >= slotNum_) {
        MMI_HILOGE("exceeded slot count (%{public}d of max %{public}d)", value, slotNum_);
        curSlot_ = slotNum_ - 1;
        return;
    }
    curSlot_ = value;
}

void AbsEventCollector::HandleMtPositionX(int32_t value)
{
    auto pointer = GetCurrentPointer(true);
    if (!pointer) {
        MMI_HILOGE("Leave, null pointer");
        return;
    }

    pointer->SetX(value);
    absEventAction_ = AbsEvent::ACTION_MOVE;
}

void AbsEventCollector::HandleMtPositionY(int32_t value)
{
    auto pointer = GetCurrentPointer(true);
    if (!pointer) {
        MMI_HILOGE("Leave, null pointer");
        return;
    }

    pointer->SetY(value);
    absEventAction_ = AbsEvent::ACTION_MOVE;
}

void AbsEventCollector::HandleMtTrackingId(int32_t value)
{
    if (value < 0) {
        absEventAction_ = AbsEvent::ACTION_UP;
    } else {
        absEventAction_ = AbsEvent::ACTION_DOWN;
    }
}

void AbsEventCollector::RemoveReleasedPointer()
{
    if (absEvent_->GetAction() != AbsEvent::ACTION_UP) {
        return;
    }
    if (pointers_.find(curSlot_) != pointers_.end()) {
        pointers_.erase(curSlot_);
    }
}
} // namespace MMI
} // namespace OHOS