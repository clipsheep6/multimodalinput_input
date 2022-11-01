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
#include "enum_utils.h"
#include "time_utils.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AbsEventCollector" };
};
AbsEventCollector::AbsEventCollector(int32_t deviceId, int32_t sourceType)
    : deviceId_(deviceId), sourceType_(sourceType), curSlot_(0), nextId_(0), 
    absEvent_(new AbsEvent(deviceId, sourceType))
{
}

const std::shared_ptr<AbsEvent>& AbsEventCollector::HandleAbsEvent(int32_t code, int32_t value)
{
    CALL_DEBUG_ENTER;

    switch (code) {
        case ABS_MT_SLOT:
            return HandleMtSlot(value);
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
            return HandleMtTrackingId(value);
        case ABS_MT_PRESSURE:
        case ABS_MT_DISTANCE:
        case ABS_MT_TOOL_X:
        case ABS_MT_TOOL_Y:
        default:
            break;
    }

    return AbsEvent::NULL_VALUE;
}

const std::shared_ptr<AbsEvent>& AbsEventCollector::HandleSyncEvent(int32_t code, int32_t value)
{
    const auto& absEvent = FinishPointer();
    return absEvent;
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

std::shared_ptr<AbsEvent::Pointer> AbsEventCollector::GetCurrentPointer(bool createIfNotExist)
{
    if (curSlot_ < 0) {
        MMI_HILOGE("Leave, curSlot_ < 0");
        return nullptr;
    }

    // if (curPointer_) {
    //     return curPointer_;
    // }

    auto it = pointers_.find(curSlot_);
    if (it != pointers_.end()) {
        curPointer_ = it->second;
        return it->second;
    }

    if (!createIfNotExist) {
        MMI_HILOGE("Leave, null pointer and !createIfNotExist");
        return nullptr;
    }

    curPointer_ = std::make_shared<AbsEvent::Pointer>();
    pointers_[curSlot_] = curPointer_;
    return curPointer_;
}

const std::shared_ptr<AbsEvent>& AbsEventCollector::FinishPointer()
{
    CALL_DEBUG_ENTER;
    if (!curPointer_) {
        MMI_HILOGE("curPointer is null. Leave.");
        return AbsEvent::NULL_VALUE;
    }
    auto action = absEventAction_;
    absEventAction_ = AbsEvent::ACTION_NONE;
    auto nowTime = TimeUtils::GetTimeStampMs();
    if (action == AbsEvent::ACTION_DOWN) {
        if (curPointer_->GetId() < 0) {
            MMI_HILOGE("ACTION_DOWN is coming, nextId_ = %{public}d", nextId_);
            curPointer_->SetId(nextId_++);
            auto retCode = absEvent_->AddPointer(curPointer_);
            if (retCode < 0) {
                MMI_HILOGE("Leave, absAction:%{public}s AddPointer Failed", AbsEvent::ActionToString(action));
                return AbsEvent::NULL_VALUE;
            }
            curPointer_->SetDownTime(nowTime);
        }
    } else if (action == AbsEvent::ACTION_UP) {
        if (curPointer_->GetId() < 0) {
            return AbsEvent::NULL_VALUE;
        }
        // if (absEvent_->GetPointerIdList().empty()) {
        //     MMI_HILOGE("ACTION_UP is all over, nextId_ = %{public}d", nextId_);
        //     nextId_ = 0;
        // }
    } else if (action != AbsEvent::ACTION_MOVE) {
        return AbsEvent::NULL_VALUE;
    }

    absEvent_->SetPointerId(curPointer_->GetId());
    absEvent_->SetAction(action);
    absEvent_->SetCurSlot(curSlot_);
    absEvent_->SetActionTime(nowTime);
    return absEvent_;
}

const std::shared_ptr<AbsEvent>& AbsEventCollector::HandleMtSlot(int32_t value)
{
    if (curSlot_ == value) {
        return AbsEvent::NULL_VALUE;
    }
    curSlot_ = value;
    curPointer_ = AbsEvent::Pointer::NULL_VALUE;
    curPointer_ = GetCurrentPointer(true);
    if (!curPointer_) {
        MMI_HILOGE("Leave, null pointer");
        return AbsEvent::NULL_VALUE;
    }
    absEventAction_ = AbsEvent::ACTION_NONE;
    return AbsEvent::NULL_VALUE;
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

const std::shared_ptr<AbsEvent>& AbsEventCollector::HandleMtTrackingId(int32_t value)
{
    if (value < 0) {
        MMI_HILOGF("MT_TRACKING_ID -1");
        absEventAction_ = AbsEvent::ACTION_UP;
    } else {
        absEventAction_ = AbsEvent::ACTION_DOWN;
    }

    return AbsEvent::NULL_VALUE;
}

void AbsEventCollector::RemoveReleasedPointer()
{
    if (absEvent_->GetAction() != AbsEvent::ACTION_UP) {
        return;
    }
    absEvent_->SetAction(AbsEvent::ACTION_NONE);
    const auto& pointer = absEvent_->GetPointer();
    if (!pointer) {
        MMI_HILOGE("Leave, null pointer");
        return;
    }

    auto retCode = absEvent_->RemovePointer(pointer);
    if (retCode < 0) {
        MMI_HILOGE("Leave, null pointer failed");
        return;
    }
    pointer->SetId(-1);
    if (absEvent_->GetPointerIdList().empty()) {
        MMI_HILOGE("ACTION_UP is all over, nextId_ = %{public}d", nextId_);
        nextId_ = 0;
    }
}
} // namespace MMI
} // namespace OHOS