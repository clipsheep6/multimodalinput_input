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

#include "smoothen_motion.h"

#include "error_multimodal.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "SmoothenMotion" };
constexpr int64_t UNIT_MS = 1000;
constexpr int64_t RESAMPLE_RANGE = 6 * UNIT_MS;
constexpr int64_t RESAMPLE_MIN_DELTA = 2 + UNIT_MS;
constexpr int64_t RESAMPLE_MAX_PREDICTION = 8 * UNIT_MS;
constexpr int64_t RESAMPLE_MAX_DELTA = 30 * UNIT_MS;
constexpr int32_t EVENT_MAX_COUNT = 100;

enum RET_CODE {
    RET_LACK_OF_DATA_CODE = 100,
    RET_TIMEOUT_DATA_CODE,
    RET_EXCEPTION_DATA_CODE,
    RET_NOT_NEED_HANDLE_CODE,
};
} // namespace

bool SmoothenMotion::IsSmoothenSwitch()
{
    return switch_;
}

void SmoothenMotion::EnableSmoothenSwitch(bool smoothenSwitch)
{
    switch_ = smoothenSwitch;
}

int32_t SmoothenMotion::HandleEvent(std::shared_ptr<PointerEvent>& point)
{
    CHKPR(point, ERROR_NULL_POINTER);

    if (!switch_) {
        return RET_NOT_NEED_HANDLE_CODE;
    }
    if (point->GetSourceType() != PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        return RET_NOT_NEED_HANDLE_CODE;
    }
    std::vector<int32_t> pointerIds = point->GetPointersIdList();
    MMI_HILOGD("Smoothen Event%{public}d,ActionTime:%{public}" PRId64 ","
               "PointerAction:%{public}s,SourceType:%{public}s,PointerCount:%{public}zu",
               point->GetId(), point->GetActionTime(), point->DumpPointerAction(),
               point->DumpSourceType(), pointerIds.size());
    for (const auto& pointerId : pointerIds) {
        PointerEvent::PointerItem item;
        if (!point->GetPointerItem(pointerId, item)) {
            MMI_HILOGE("Invalid pointer: %{public}d", pointerId);
            return RET_ERR;
        }
        MMI_HILOGD("PointerId:%{public}d,DownTime:%{public}" PRId64 ","
                   "GlobalX:%{public}d,GlobalY:%{public}d",
                   item.GetPointerId(), item.GetDownTime(),
                   item.GetGlobalX(), item.GetGlobalY());
    }

    std::lock_guard<std::mutex> guard(mutex_);
    if (eventBatch_.size() > EVENT_MAX_COUNT) {
        eventBatch_.pop_front();
    }
    eventBatch_.push_back(point);

    return RET_OK;
}

bool SmoothenMotion::UpdateDownState(std::shared_ptr<PointerEvent>& point)
{
    if (auto it = histories_.find(point->GetDeviceId()); it != histories_.end()) {
        PointerEvent::PointerItem item;
        if (!(point->GetPointerItem(point->GetPointerId(), item))) {
            MMI_HILOGE("Get pointer(%{public}d) item error", point->GetPointerId());
            return false;
        }
        CHKPF(it->second.samples_);
        it->second.samples_->UpdatePointerItem(item.GetPointerId(), item);
    } else {
        BatchEvent batch;
        batch.samples_ = point;
        batch.AddHistory(point);
        auto iter = histories_.insert(std::make_pair(point->GetDeviceId(), batch));
        if (!iter.second) {
            MMI_HILOGE("Device:%{public}d insertion failed", point->GetDeviceId());
            return false;
        }
    }
    return true;
}

bool SmoothenMotion::UpdateMoveState(std::shared_ptr<PointerEvent>& point)
{
    if (auto it = histories_.find(point->GetDeviceId()); it != histories_.end()) {
        PointerEvent::PointerItem item;
        if (!(point->GetPointerItem(point->GetPointerId(), item))) {
            MMI_HILOGE("Get pointer(%{public}d) item error", point->GetPointerId());
            return false;
        }
        it->second.AddHistory(point);
        return true;
    }
    MMI_HILOGE("Device:%{public}d press event not found", point->GetDeviceId());
    return false;
}

bool SmoothenMotion::UpdateUpState(std::shared_ptr<PointerEvent>& point)
{
    if (auto it = histories_.find(point->GetDeviceId()); it != histories_.end()) {
        CHKPF(it->second.samples_);
        it->second.samples_->RemovePointerItem(point->GetPointerId());

        if (it->second.samples_->GetPointersIdList().empty()) {
            it->second.ClearHistory();
            histories_.erase(it);
        }
        return true;
    }
    MMI_HILOGE("Device:%{public}d press event not found", point->GetDeviceId());
    return false;
}

bool SmoothenMotion::UpdatePointerState(std::shared_ptr<PointerEvent>& point)
{
    CHKPF(point);

    switch (point->GetPointerAction()) {
        case PointerEvent::POINTER_ACTION_DOWN: {
            return UpdateDownState(point);
        }
        case PointerEvent::POINTER_ACTION_MOVE: {
            return UpdateMoveState(point);
        }
        case PointerEvent::POINTER_ACTION_CANCEL:
        case PointerEvent::POINTER_ACTION_UP: {
            return UpdateUpState(point);
        }
        default: {
            MMI_HILOGW("Unknown touch screen action: %{pulic}s", point->DumpPointerAction());
            return false;
        }
    }
}

int32_t SmoothenMotion::ConsumeInputEvent(int64_t frameTime, std::shared_ptr<PointerEvent>& outEvent)
{
    outEvent = nullptr;
    std::lock_guard<std::mutex> guard(mutex_);
    if (eventBatch_.empty()) {
        return RET_ERR;
    }
    int64_t sampleTime = frameTime - RESAMPLE_RANGE;
    int32_t deviceId = -1;
    auto next = eventBatch_.end();
    auto prev = FindSampleEventPrev(sampleTime);
    if (prev == eventBatch_.end()) {
        MMI_HILOGW("No sampling time for the previous event");
        return RET_ERR;
    }
    deviceId = (*prev)->GetDeviceId();
    next = prev;
    do {
        ++next;
    } while ((next != eventBatch_.end()) &&
                (*next)->GetDeviceId() != (*prev)->GetDeviceId());
    if ((next != eventBatch_.end()) && (*next)->GetDeviceId() != (*prev)->GetDeviceId()) {
        next = eventBatch_.end();
        *next = nullptr;
    }
    prev++;

    for (auto iter = eventBatch_.begin(); iter != prev;) {
        if (!UpdatePointerState(*iter)) {
            MMI_HILOGE("Update pointer state failed");
            return RET_ERR;
        }
        outEvent = *iter;
        iter = eventBatch_.erase(iter);
        if (outEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_CANCEL ||
            outEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_UP) {
            return RET_OK;
        }
    }
    BatchEvent batch;
    if (!FindBatchEvent(deviceId, batch)) {
        MMI_HILOGE("The device(%{public}d) not found", deviceId);
        return RET_ERR;
    }
    if (ResampleTouchState(sampleTime, batch, *next, outEvent) == RET_ERR) {
        MMI_HILOGE("Resampling failed");
        outEvent = nullptr;
        return RET_ERR;
    }
    return RET_OK;
}

int32_t SmoothenMotion::ResampleTouchState(int64_t sampleTime,
                                           const BatchEvent& batch,
                                           const std::shared_ptr<PointerEvent>& nextEvent,
                                           std::shared_ptr<PointerEvent>& event)
{
    float ratio = 0.0;
    std::shared_ptr<PointerEvent> other { nullptr };
    std::shared_ptr<PointerEvent> current = batch.GetHistory(0);
    CHKPR(current, RET_ERR);
    if (nextEvent != nullptr) {
        other = nextEvent;
        int64_t delta = other->GetActionTime() - current->GetActionTime();
        if (delta < RESAMPLE_MIN_DELTA) {
            MMI_HILOGW("The data interval is too small"
                       ",cTime:%{public}" PRId64 ", oTime:%{public}" PRId64,
                       current->GetActionTime(), other->GetActionTime());
            return RET_EXCEPTION_DATA_CODE;
        }
        ratio = static_cast<float>((sampleTime - current->GetActionTime()) / delta);
    } else {
        other = batch.GetHistory(1);
        if (other == nullptr) {
            MMI_HILOGW("Required data missing");
            return RET_LACK_OF_DATA_CODE;
        }
        int64_t delta = current->GetActionTime() - other->GetActionTime();
        if (delta < RESAMPLE_MIN_DELTA) {
            MMI_HILOGW("The data interval is too small,"
                       "cTime:%{public}" PRId64 ", oTime:%{public}" PRId64,
                       current->GetActionTime(), other->GetActionTime());
            return RET_EXCEPTION_DATA_CODE;
        } else if (delta > RESAMPLE_MAX_DELTA) {
            MMI_HILOGW("The data interval is too large"
                       ",cTime:%{public}" PRId64 ", oTime:%{public}" PRId64,
                       current->GetActionTime(), other->GetActionTime());
            return RET_TIMEOUT_DATA_CODE;
        }
        int64_t maxPredict = current->GetActionTime() + std::min(delta / 2, RESAMPLE_MAX_PREDICTION);
        if (sampleTime > maxPredict) {
            sampleTime = maxPredict;
        }
        ratio = static_cast<float>((current->GetActionTime() - sampleTime) / delta);
    }
    std::shared_ptr<PointerEvent> samplePoint { batch.samples_ };
    if (UpdateSamplePointItem(ratio, current, other, samplePoint) == RET_ERR) {
        return RET_ERR;
    }
    event = samplePoint;
    return RET_OK;
}

int32_t SmoothenMotion::UpdateSamplePointItem(float ratio,
                                              const std::shared_ptr<PointerEvent>& current,
                                              const std::shared_ptr<PointerEvent>& other,
                                              std::shared_ptr<PointerEvent>& samplePoint)
{
    CHKPR(samplePoint, RET_ERR);
    auto pointers = samplePoint->GetPointersIdList();
    for (auto pointId : pointers) {
        PointerEvent::PointerItem currentItem;
        PointerEvent::PointerItem otherItem;
        PointerEvent::PointerItem item;
        if (!samplePoint->GetPointerItem(pointId, item)) {
            MMI_HILOGE("Get pointer(%{public}d) item error", pointId);
            return RET_ERR;
        }
        if (current->GetPointerItem(pointId, currentItem)) {
            if (other->GetPointerItem(pointId, otherItem)) {
                item.SetGlobalX(Calculate(currentItem.GetGlobalX(), otherItem.GetGlobalX(), ratio));
                item.SetGlobalY(Calculate(currentItem.GetGlobalY(), otherItem.GetGlobalY(), ratio));
            } else {
                item.SetGlobalX(currentItem.GetGlobalX());
                item.SetGlobalY(currentItem.GetGlobalY());
            }
        } else {
            if (other->GetPointerItem(pointId, otherItem)) {
                item.SetGlobalX(otherItem.GetGlobalX());
                item.SetGlobalY(otherItem.GetGlobalY());
            } else {
                MMI_HILOGE("Get pointer(%{public}d) item error", pointId);
                continue;
            }
        }
        samplePoint->UpdatePointerItem(pointId, item);
    }
    return RET_OK;
}

void SmoothenMotion::RemoveMotionEventById(int32_t deviceId)
{
    auto iter = eventBatch_.begin();
    while (iter != eventBatch_.end()) {
        if ((*iter)->GetDeviceId() == deviceId) {
            iter = eventBatch_.erase(iter);
        } else {
            iter++;
        }
    }
}

std::list<std::shared_ptr<PointerEvent>>::iterator SmoothenMotion::FindSampleEventPrev(int64_t sampleTime)
{
    auto sampleEventPrev = eventBatch_.end();
    for (auto iter = eventBatch_.begin(); iter != eventBatch_.end(); ++iter) {
        if ((*iter)->GetActionTime() > sampleTime) {
            break;
        } else {
            sampleEventPrev = iter;
        }
    }
    return sampleEventPrev;
}

bool SmoothenMotion::FindBatchEvent(int32_t deviceId, BatchEvent& batch)
{
    auto iter = histories_.find(deviceId);
    if (iter != histories_.end()) {
        batch = iter->second;
        return true;
    }
    return false;
}
} // namespace MMI
} // namespace OHOS
