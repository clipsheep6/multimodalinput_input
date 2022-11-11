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
#include "error_multimodal.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AbsEventCollector" };
};
AbsEventCollector::AbsEventCollector(int32_t devIndex, int32_t sourceType)
    : curSlot_(0), absEvent_(std::make_shared<AbsEvent>(devIndex, sourceType))
{}

void AbsEventCollector::HandleAbsEvent(int32_t code, int32_t value)
{
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
            MMI_HILOGW("Unknown type: %{public}d", code);
            break;
    }
}

const std::shared_ptr<AbsEvent> AbsEventCollector::HandleSyncEvent(int32_t code, int32_t value)
{
    return FinishPointer();
}

void AbsEventCollector::AfterProcessed()
{
    if (absEvent_->GetAction() != AbsEvent::ACTION_UP) {
        return;
    }
    auto it = pointers_.find(curSlot_);
    if (it != pointers_.end()) {
        pointers_.erase(it);
    }
}

int32_t AbsEventCollector::SetSourceType(int32_t sourceType)
{
    CALL_DEBUG_ENTER;
    CHKPR(absEvent_, ERROR_NULL_POINTER);
    return absEvent_->SetSourceType(sourceType);
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
        MMI_HILOGE("curSlot_(%{public}d) < 0", curSlot_);
        return nullptr;
    }
    auto it = pointers_.find(curSlot_);
    if (it != pointers_.end()) {
        return it->second;
    }
    if (!createIfNotExist) {
        MMI_HILOGD("Return null pointer, because no need to create");
        return nullptr;
    }
    auto pointer = std::make_shared<AbsEvent::Pointer>();
    CHKPP(pointer);
    pointers_.insert(std::make_pair(curSlot_, pointer));
    return pointer;
}

const std::shared_ptr<AbsEvent> AbsEventCollector::FinishPointer()
{
    CALL_DEBUG_ENTER;
    CHKPP(absEvent_);
    auto pointer = GetCurrentPointer(false);
    if (!pointer) {
        MMI_HILOGE("pointer is null. Leave.");
        return {};
    }
    auto timeNs = std::chrono::steady_clock::now().time_since_epoch();
    auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeNs).count();
    if (absEventAction_ == AbsEvent::ACTION_DOWN) {
        auto retCode = absEvent_->AddPointer(pointer);
        if (retCode < 0) {
            MMI_HILOGE("absAction:%{public}s AddPointer Failed", AbsEvent::ActionToString(absEventAction_));
            return {};
        }
        pointer->SetDownTime(timeMs);
    }
    absEvent_->SetAction(absEventAction_);
    absEvent_->SetCurSlot(curSlot_);
    absEvent_->SetActionTime(timeMs);
    absEvent_->SetAxisInfo(xInfo_, yInfo_);
    return absEvent_;
}

void AbsEventCollector::HandleMtSlot(int32_t value)
{
    if (value >= slotNum_) {
        MMI_HILOGW("Exceeded slot count (%{public}d of max %{public}d)", value, slotNum_);
        curSlot_ = slotNum_ - 1;
        return;
    }
    curSlot_ = value;
}

void AbsEventCollector::HandleMtPositionX(int32_t value)
{
    auto pointer = GetCurrentPointer(true);
    if (!pointer) {
        MMI_HILOGE("null pointer");
        return;
    }
    pointer->SetX(value);
    absEventAction_ = AbsEvent::ACTION_MOVE;
}

void AbsEventCollector::HandleMtPositionY(int32_t value)
{
    auto pointer = GetCurrentPointer(true);
    if (!pointer) {
        MMI_HILOGE("null pointer");
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
} // namespace MMI
} // namespace OHOS