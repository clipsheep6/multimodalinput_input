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
AbsEventCollector::AbsEventCollector(int32_t devIndex, int32_t sourceType, OnCollectCallback callback)
    : curSlot_(0), absEvent_(std::make_shared<AbsEvent>(devIndex, sourceType)), collectCallback_(callback)
{}

void AbsEventCollector::HandleAbsEvent(int32_t code, int32_t value)
{
    switch (code) {
        case ABS_MT_SLOT: {
            HandleMtSlot(value);
            break;
        }
        case ABS_MT_TOUCH_MAJOR:
        case ABS_MT_TOUCH_MINOR:
        case ABS_MT_WIDTH_MAJOR:
        case ABS_MT_WIDTH_MINOR:
        case ABS_MT_ORIENTATION: {
            break;
        }
        case ABS_MT_POSITION_X: {
            HandleMtPositionX(value);
            break;
        }
        case ABS_MT_POSITION_Y: {
            HandleMtPositionY(value);
            break;
        }
        case ABS_MT_TOOL_TYPE:
        case ABS_MT_BLOB_ID: {
            break;
        }
        case ABS_MT_TRACKING_ID: {
            HandleMtTrackingId(value);
            break;
        }
        case ABS_MT_PRESSURE:
        case ABS_MT_DISTANCE:
        case ABS_MT_TOOL_X:
        case ABS_MT_TOOL_Y:
        default: {
            MMI_HILOGW("Unknown type: %{public}d", code);
            break;
        }
    }
}


void AbsEventCollector::HandleAbsAbsoluteEvent(int32_t code, int32_t value)
{
    switch (code) {
        case ABS_X: {  
            auto pointer = GetCurrentPointer(true);
            if (pointer != nullptr) {
                pointer->SetX(value);
                action_ = AbsEvent::ACTION_MOVE;   
            }           
            break;
        }
        case ABS_Y: {            
            auto pointer = GetCurrentPointer(true);
            if (pointer != nullptr) {
                pointer->SetY(value);
                action_ = AbsEvent::ACTION_MOVE;   
            }           
            break;
        }
        case ABS_PRESSURE:{    
                break;
        }
        default: {
            MMI_HILOGW("Unknown type: %{public}d", code);
            break;
        }
    }
}

void AbsEventCollector::HandleKeyEvent(int32_t code, int32_t value)
{
    if (code == BTN_TOUCH) {
        if (!isMt_) {
            action_ =  (value == 1 ) ? AbsEvent::ACTION_DOWN : AbsEvent::ACTION_UP;
        }
    }
}

void AbsEventCollector::HandleSyncEvent()
{
    FinishPointer();
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

void AbsEventCollector::FinishPointer()
{
    CALL_DEBUG_ENTER;
    CHKPV(absEvent_);
    if (!isMt_) {
        if (action_ == AbsEvent::ACTION_NONE) {
            return;
        }
        if (action_ == AbsEvent::ACTION_DOWN) {
            FinishAbsoluteDown();
        } else if (action_ == AbsEvent::ACTION_UP) {
            FinishAbsoluteUp();
        } else {
            FinishAbsoluteMotion();
        }
    } else {
        for (auto iter = pointers_.begin(); iter != pointers_.end();) {
            if (iter->second == nullptr) {
                pointers_.erase(iter++);
                continue;
            }
            auto pointer = iter->second;
            if (!pointer->IsDirty()) {
                iter++;
                continue;
            }
            pointer->MarkIsDirty(false);
            absEvent_->SetPointer(pointer);
            absEvent_->SetAction(pointer->GetAction());      
            absEvent_->SetCurSlot(iter->first);
            absEvent_->SetAxisInfo(xInfo_, yInfo_);
            auto timeNs = std::chrono::steady_clock::now().time_since_epoch();
            auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeNs).count();
            if (pointer->GetAction() == AbsEvent::ACTION_DOWN) {
                pointer->SetDownTime(timeMs);
            }
            absEvent_->SetActionTime(timeMs);
            collectCallback_(absEvent_);
            if (pointer->GetAction() == AbsEvent::ACTION_UP) {
                pointers_.erase(iter++);
                continue;
            }
            iter++;
        }
    }
}

void AbsEventCollector::FinishAbsoluteDown() 
{   
    CHKPV(absEvent_);
	int seat_slot = __builtin_ffs(~slot_map_) - 1;

	if (seat_slot == -1) {
        return;
    }
    curSlot_ = seat_slot;
    slot_map_ |= (1UL << seat_slot);
    auto pointer = GetCurrentPointer(false);
    CHKPV(pointer);
    absEvent_->SetPointer(pointer);
    absEvent_->SetAction(AbsEvent::ACTION_DOWN);      
    absEvent_->SetCurSlot(seat_slot);
    absEvent_->SetAxisInfo(xInfo_, yInfo_);
    auto timeNs = std::chrono::steady_clock::now().time_since_epoch();
    auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeNs).count();
    absEvent_->SetActionTime(timeMs);
    collectCallback_(absEvent_);
}

void AbsEventCollector::FinishAbsoluteMotion()
{
    CHKPV(absEvent_);
	int seat_slot = curSlot_;
	if (seat_slot == -1) {
        return;
    }
    auto pointer = GetCurrentPointer(false);
    CHKPV(pointer);
    absEvent_->SetPointer(pointer);
    absEvent_->SetAction(AbsEvent::ACTION_MOVE);      
    absEvent_->SetCurSlot(seat_slot);
    absEvent_->SetAxisInfo(xInfo_, yInfo_);
    auto timeNs = std::chrono::steady_clock::now().time_since_epoch();
    auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeNs).count();
    absEvent_->SetActionTime(timeMs);
    collectCallback_(absEvent_);
}

void AbsEventCollector::FinishAbsoluteUp() 
{
    CHKPV(absEvent_);
	int seat_slot = curSlot_;
	curSlot_ = -1;
	if (seat_slot == -1) {
	    return;
    }
	slot_map_ &= ~(1UL << seat_slot);
    auto pointer = GetCurrentPointer(false);
    CHKPV(pointer);
    absEvent_->SetPointer(pointer);
    absEvent_->SetAction(AbsEvent::ACTION_UP);      
    absEvent_->SetCurSlot(seat_slot);
    absEvent_->SetAxisInfo(xInfo_, yInfo_);
    auto timeNs = std::chrono::steady_clock::now().time_since_epoch();
    auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeNs).count();
    absEvent_->SetActionTime(timeMs);
    collectCallback_(absEvent_);
    if (pointer->GetAction() == AbsEvent::ACTION_UP) {
        auto iter = pointers_.find(curSlot_);
        if (iter != pointers_.end()) {
            pointers_.erase(iter);
        }
    }
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
    pointer->SetAction(AbsEvent::ACTION_MOVE);
    pointer->MarkIsDirty(true);
}

void AbsEventCollector::HandleMtPositionY(int32_t value)
{
    auto pointer = GetCurrentPointer(true);
    if (!pointer) {
        MMI_HILOGE("null pointer");
        return;
    }
    pointer->SetY(value);
    pointer->SetAction(AbsEvent::ACTION_MOVE);
    pointer->MarkIsDirty(true);
}

void AbsEventCollector::HandleMtTrackingId(int32_t value)
{
    auto pointer = GetCurrentPointer(true);
    if (!pointer) {
        MMI_HILOGE("null pointer");
        return;
    }
    if (value < 0) {
        pointer->SetAction(AbsEvent::ACTION_UP);
    } else {
        pointer->SetAction(AbsEvent::ACTION_DOWN);
    }
    pointer->MarkIsDirty(true);
}
} // namespace MMI
} // namespace OHOS