/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
    MMI_HILOGD("Enter code:%{public}s value:%{public}d", EnumUtils::InputEventAbsCodeToString(code), value);
    RemoveReleasedPointer();

    switch (code) {
        case ABS_MT_SLOT: 
        MMI_HILOGD("songliy e->code == ABS_MT_SLOT");
            return HandleMtSlot(value);
        case ABS_MT_TOUCH_MAJOR:
        case ABS_MT_TOUCH_MINOR:
        case ABS_MT_WIDTH_MAJOR:
        case ABS_MT_WIDTH_MINOR:
        case ABS_MT_ORIENTATION:
            break;
        case ABS_MT_POSITION_X:
            MMI_HILOGD("songliy ABS_MT_POSITION_X");
            HandleMtPositionX(value);
            break;
        case ABS_MT_POSITION_Y:
            MMI_HILOGD("songliy ABS_MT_POSITION_Y");
            HandleMtPositionY(value);
            break;
        case ABS_MT_TOOL_TYPE:
        case ABS_MT_BLOB_ID:
            break;
        case ABS_MT_TRACKING_ID:
            MMI_HILOGD("songliy ABS_MT_TRACKING_ID");
            return HandleMtTrackingId(value);
        case ABS_MT_PRESSURE:
        case ABS_MT_DISTANCE:
        case ABS_MT_TOOL_X:
        case ABS_MT_TOOL_Y:
        default:
            break;
    }

    MMI_HILOGD("Leave code:%{public}s value:%{public}d", EnumUtils::InputEventAbsCodeToString(code), value);
    return AbsEvent::NULL_VALUE;
}

const std::shared_ptr<AbsEvent>& AbsEventCollector::HandleSyncEvent(int32_t code, int32_t value)
{
    MMI_HILOGD("Enter HandleSyncEvent FinishPointer code:%{public}s value:%{public}d", EnumUtils::InputEventSynCodeToString(code), value);
    const auto& absEvent = FinishPointer();
    MMI_HILOGD("Leave code:%{public}s value:%{public}d absEvent:%{public}p", EnumUtils::InputEventSynCodeToString(code), value, absEvent.get());
    return absEvent;
}
 
void AbsEventCollector::AfterProcessed()
{
    RemoveReleasedPointer();
}

int32_t AbsEventCollector::SetSourceType(int32_t sourceType)
{
    MMI_HILOGD("Enter, sourceType_:%{public}s, sourceType:%{public}s", 
            AbsEvent::SourceToString(sourceType_),
            AbsEvent::SourceToString(sourceType));

    if (sourceType <= AbsEvent::SOURCE_TYPE_NONE || sourceType >= AbsEvent::SOURCE_TYPE_END) {
        return -1;
    }

    if (sourceType_ > AbsEvent::SOURCE_TYPE_NONE && sourceType_ < AbsEvent::SOURCE_TYPE_END) {
        return -1;
    }
    sourceType_ = sourceType;
    absEvent_->SetSourceType(sourceType_);

    MMI_HILOGD("Leave, sourceType_:%{public}s, sourceType:%{public}s", 
            AbsEvent::SourceToString(sourceType_),
            AbsEvent::SourceToString(sourceType));
    return 0;
}

void AbsEventCollector::SetAction(int32_t action) {
    // absEventAction_ = action;
}

std::shared_ptr<AbsEvent::Pointer> AbsEventCollector::GetCurrentPointer(bool createIfNotExist)
{
    if (curSlot_ < 0) {
        MMI_HILOGE("Leave, curSlot_ < 0");
        return nullptr;
    }

    if (curPointer_) {
        return curPointer_;
    }

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
    if (!curPointer_) {
        MMI_HILOGE("Leave");
        return AbsEvent::NULL_VALUE;
    }
    MMI_HILOGD("curPointer_:%{public}p, GetId()%{public}d", curPointer_.get(), curPointer_->GetId());
    auto action = absEventAction_;
    absEventAction_ = AbsEvent::ACTION_NONE;
    auto nowTime = TimeUtils::GetTimeStampMs();
    if (action == AbsEvent::ACTION_DOWN) {
        if (curPointer_->GetId() < 0) {
            curPointer_->SetId(nextId_++);
            auto retCode = absEvent_->AddPointer(curPointer_);
            if (retCode < 0) {
                MMI_HILOGE("Leave, absAction:%{public}s AddPointer Failed", AbsEvent::ActionToString(action));
                return AbsEvent::NULL_VALUE;
            }
            curPointer_->SetDownTime(nowTime);
        }
    } else if (action == AbsEvent::ACTION_MOVE) {
        // if (curPointer_->GetId() < 0) {
        //     curPointer_->SetId(nextId_++);
        //     auto retCode = absEvent_->AddPointer(curPointer_);
        //     if (retCode < 0) {
        //         MMI_HILOGE("Leave, absAction:%{public}s AddPointer Failed", AbsEvent::ActionToString(action));
        //         return AbsEvent::NULL_VALUE;
        //     }
        //     curPointer_->SetDownTime(nowTime);
        //     // action = AbsEvent::ACTION_DOWN;
        // }
    } else if (action == AbsEvent::ACTION_UP) {
        if (curPointer_->GetId() < 0) {
            MMI_HILOGE("lisong, FinishPointer absAction:%{public}s id < 0 on up", AbsEvent::ActionToString(action));
            return AbsEvent::NULL_VALUE;
        }
        MMI_HILOGE("lisong, FinishPointer absAction:%{public}s id", AbsEvent::ActionToString(action));
        if (absEvent_->GetPointerIdList().empty()) {
            MMI_HILOGE("lisong, FinishPointer absEvent_->GetPointerIdList().empty()");
            nextId_ = 0;
        }
    } else {
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
    MMI_HILOGD("Enter HandleMtSlot FinishPointer");
    // const auto& absEvent = FinishPointer();
    curSlot_ = value;
    curPointer_ = AbsEvent::Pointer::NULL_VALUE;
    absEventAction_ = AbsEvent::ACTION_NONE;
    MMI_HILOGE("Reset curPointer_");
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
        MMI_HILOGD("action up, FinishPointer");
        // return FinishPointer();
    } else {
        absEventAction_ = AbsEvent::ACTION_DOWN;
        MMI_HILOGD("action down, FinishPointer");
        // return FinishPointer();
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
}
} // namespace MMI
} // namespace OHOS