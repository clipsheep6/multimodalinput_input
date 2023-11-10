/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "event_resample.h"

#include "event_log_helper.h"
#include "mmi_log.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MMI_LOG_DOMAIN, "EventResample" };
} // namespace

EventResample::EventResample(){};
EventResample::~EventResample(){};

std::shared_ptr<PointerEvent> EventResample::OnEventConsume(std::shared_ptr<PointerEvent> pointerEvent,
                                                            int64_t frameTime, bool &deferred, ErrCode &status)
{
    MotionEvent* outEvent = nullptr;
    ErrCode result = ERR_OK;

    deferred = false;
    status = ERR_OK;
    if (ERR_OK != InitializeInputEvent(pointerEvent, frameTime)) {
        status = ERR_WOULD_BLOCK;
        return pointerEvent;
    }

    do {
        // All events are dispathed so consume batches
        if (PointerEvent::POINTER_ACTION_UNKNOWN == inputEvent_.pointerAction) {
            if (msgDeferred_ == true) {
                msgDeferred_ = false;
                deferred = false;
                frameTime_ = 0;
                outEvent = &deferredEvent_;
                result = ERR_OK;
                break;
            }
            result = ConsumeBatch(frameTime_, &outEvent);
            frameTime_ = 0;
            if ((ERR_OK == result) && (NULL != outEvent)) {
                status = result;
                break;
            } else {
                status = result;
                return nullptr;
            }
        }

        // Add event into batch
        if (UpdateBatch(&outEvent, result, deferred)) {
            break;
        }

        // Update touch state object
        EventDump("UpdateTouchState", inputEvent_);
        UpdateTouchState(inputEvent_);
//        outEvent = &inputEvent_;
        return pointerEvent_;
    } while (0);

    if ((ERR_OK == result) && (NULL != outEvent)) {
        // Update pointer event
        UpdatePointerEvent(outEvent);
        return pointerEvent_;
    }

    return nullptr;
}

std::shared_ptr<PointerEvent> EventResample::GetPointerEvent()
{
    return pointerEvent_;
}

void EventResample::EventDump(const char *msg, MotionEvent &event)
{
    MMI_HILOGI("%{public}s: a=%{public}d t=%{public}" PRId64 " c=%{public}d s=%{public}d d=%{public}d e=%{public}d",
               msg, event.pointerAction, event.actionTime, event.pointerCount,
               event.sourceType, event.deviceId, event.eventId);
    for (auto &it : event.pointers) {
        MMI_HILOGI("ID %{public}d: x=%{public}d y=%{public}d (%{public}d)",
                   it.second.id, it.second.coordX, it.second.coordY, it.second.toolType);
    }
}

ErrCode EventResample::InitializeInputEvent(std::shared_ptr<PointerEvent> pointerEvent, int64_t frameTime)
{
    int32_t pointerAction = PointerEvent::POINTER_ACTION_UNKNOWN;

    if (pointerEvent != nullptr) {
        pointerEvent_ = pointerEvent;
    }

    if (frameTime_ <= 0) {
        if (0 != frameTime) {
            frameTime_ = frameTime;
        } else if (nullptr != pointerEvent) {
            frameTime_ = GetSysClockTime();
        } else {
            frameTime_ = 0;
        }
    }

    // Check that event can be consumed and initialize motion event.
    if (nullptr != pointerEvent) {
        pointerAction = pointerEvent->GetPointerAction();
        MMI_HILOGD("pointerAction:%{public}d %{public}" PRId64 " %{public}" PRId64,
                   pointerAction, pointerEvent->GetActionTime(), frameTime_);
        switch (pointerAction) {
            case PointerEvent::POINTER_ACTION_DOWN:
            case PointerEvent::POINTER_ACTION_MOVE:
            case PointerEvent::POINTER_ACTION_UP:
            case PointerEvent::POINTER_ACTION_CANCEL:
                break;
            default:
                return ERR_WOULD_BLOCK;
        }
        inputEvent_.Reset();
        inputEvent_.InitializeFrom(pointerEvent);

        EventDump("Input Event", inputEvent_);
    } else {
        inputEvent_.Reset();
    }

    return ERR_OK;
}

bool EventResample::UpdateBatch(MotionEvent** outEvent, ErrCode &result, bool &deferred)
{
    ssize_t batchIndex = FindBatch(inputEvent_.deviceId, inputEvent_.sourceType);
    if (batchIndex >= 0) {
        Batch& batch = batches_.at(batchIndex);
        if (CanAddSample(batch, inputEvent_)) {
            batch.samples.push_back(inputEvent_);
            MMI_HILOGD("Event added to batch: %{public}d %{public}d %{public}d",
                       inputEvent_.deviceId, inputEvent_.sourceType, inputEvent_.pointerAction);
            return true;
        } else if (PointerEvent::POINTER_ACTION_UP == inputEvent_.pointerAction) {
//             MMI_HILOGD("Deferred event: %{public}d %{public}d %{public}d",
//                        inputEvent_.deviceId, inputEvent_.sourceType, inputEvent_.pointerAction);
//             deferredEvent_.InitializeFrom(inputEvent_);
//             msgDeferred_ = true;
//             deferred = true;
//             result = ConsumeSamples(batch, batch.samples.size(), outEvent);
//             batches_.erase(batches_.begin() + batchIndex);
//             UpdateTouchState(deferredEvent_);
//             return true;
            return false;
        }
    }

    // Start a new batch
    if (PointerEvent::POINTER_ACTION_MOVE == inputEvent_.pointerAction) {
        Batch batch;
        batch.samples.push_back(inputEvent_);
        batches_.push_back(std::move(batch));
        return true;
    }

    return false;
}

void EventResample::UpdatePointerEvent(MotionEvent* outEvent)
{
    EventDump("Output Event", *outEvent);
    pointerEvent_->SetActionTime(outEvent->actionTime);
    pointerEvent_->SetPointerAction(outEvent->pointerAction);
    pointerEvent_->SetActionTime(outEvent->actionTime);
    pointerEvent_->SetId(outEvent->eventId);

    for (auto &it : outEvent->pointers) {
        PointerEvent::PointerItem item;
        if (pointerEvent_->GetPointerItem(it.first, item)) {
            item.SetDisplayX(it.second.coordX);
            item.SetDisplayY(it.second.coordY);
            if (PointerEvent::POINTER_ACTION_MOVE == outEvent->pointerAction) {
                item.SetPressed(true);
            } else if (PointerEvent::POINTER_ACTION_UP == outEvent->pointerAction) {
                item.SetPressed(false);
            } else {
//                MMI_HILOGD("Output event: Pointer action: %{public}d", outEvent->pointerAction);
            }
            pointerEvent_->UpdatePointerItem(it.first, item);
        }
    }
}

ErrCode EventResample::ConsumeBatch(int64_t frameTime, MotionEvent** outEvent)
{
    int32_t result;
    for (size_t i = batches_.size(); i > 0;) {
        i--;
        Batch& batch = batches_.at(i);
        if (frameTime < 0) {
            result = ConsumeSamples(batch, batch.samples.size(), outEvent);
            batches_.erase(batches_.begin() + i);
            return result;
        }

        int64_t sampleTime = frameTime;
        if (resampleTouch_) {
            sampleTime -= RESAMPLE_LATENCY;
        }
        ssize_t split = FindSampleNoLaterThan(batch, sampleTime);
        if (split < 0) {
            continue;
        }

        result = ConsumeSamples(batch, split + 1, outEvent);
        const MotionEvent* next;
        if (batch.samples.empty()) {
            batches_.erase(batches_.begin() + i);
            next = NULL;
        } else {
            next = &batch.samples.at(0);
        }
        if (!result && resampleTouch_) {
            ResampleTouchState(sampleTime, static_cast<MotionEvent*>(*outEvent), next);
        }
        return result;
    }

    return ERR_WOULD_BLOCK;
}

ErrCode EventResample::ConsumeSamples(Batch& batch, size_t count, MotionEvent** outEvent)
{
    outputEvent_.Reset();

    for (size_t i = 0; i < count; i++) {
        MotionEvent& event = batch.samples.at(i);
        UpdateTouchState(event);
        if (i > 0) {
            AddSample(&outputEvent_, &event);
        } else {
            outputEvent_.InitializeFrom(event);
        }
    }
    batch.samples.erase(batch.samples.begin(), batch.samples.begin() + count);

    *outEvent = &outputEvent_;

    return ERR_OK;
}

void EventResample::AddSample(MotionEvent* outEvent, const MotionEvent* event)
{
    outEvent->actionTime = event->actionTime;
    for (auto &it : event->pointers) {
        outEvent->pointers[it.first] = it.second;
    }
}

void EventResample::UpdateTouchState(MotionEvent &event)
{
    int32_t deviceId = event.deviceId;
    int32_t source = event.sourceType;

    switch (event.pointerAction) {
        case PointerEvent::POINTER_ACTION_DOWN: {
            ssize_t idx = FindTouchState(deviceId, source);
            if (idx < 0) {
                TouchState newState;
                touchStates_.push_back(newState);
                idx = touchStates_.size() - 1;
            }
            TouchState& touchState = touchStates_.at(idx);
            touchState.Initialize(deviceId, source);
            touchState.AddHistory(event);
            break;
        }
        case PointerEvent::POINTER_ACTION_MOVE: {
            ssize_t idx = FindTouchState(deviceId, source);
            if (idx >= 0) {
                TouchState& touchState = touchStates_.at(idx);
                touchState.AddHistory(event);
                RewriteMessage(touchState, event);
            }
            break;
        }
        case PointerEvent::POINTER_ACTION_UP:
        case PointerEvent::POINTER_ACTION_CANCEL: {
            ssize_t idx = FindTouchState(deviceId, source);
            if (idx >= 0) {
                TouchState& touchState = touchStates_.at(idx);
                RewriteMessage(touchState, event);
                touchStates_.erase(touchStates_.begin() + idx);
            }
            frameTime_ = 0;
            idx = FindBatch(deviceId, source);
            if (idx >= 0) {
                batches_.erase(batches_.begin() + idx);
            }
            break;
        }
        default: {
            break;
        }
    }
}

void EventResample::ResampleTouchState(int64_t sampleTime, MotionEvent* event, const MotionEvent* next)
{
    if (!resampleTouch_ || (PointerEvent::SOURCE_TYPE_TOUCHSCREEN != event->sourceType)
                        || (PointerEvent::POINTER_ACTION_MOVE != event->pointerAction)) {
        return;
    }

    ssize_t idx = FindTouchState(event->deviceId, event->sourceType);
    if (idx < 0) {
        return;
    }

    TouchState &touchState = touchStates_.at(idx);
    if (touchState.historySize < 1) {
        return;
    }

    // Ensure that the current sample has all of the pointers that need to be reported.
    const History* current = touchState.GetHistory(0);
    for (auto &it : event->pointers) {
        if (!current->HasPointerId(it.first)) {
            return;
        }
    }

    // Find the data to use for resampling.
    const History* other;
    History future;
    float alpha;
    if (next) {
        // Interpolate between current sample and future sample.
        // So current->actionTime <= sampleTime <= future.actionTime.
        future.InitializeFrom(*next);
        other = &future;
        int64_t delta = future.actionTime - current->actionTime;
        if (delta < RESAMPLE_MIN_DELTA) {
            return;
        }
        alpha = static_cast<float>(sampleTime - current->actionTime) / delta;
    } else if (touchState.historySize >= HISTORY_SIZE_MAX) {
        // Extrapolate future sample using current sample and past sample.
        // So other->actionTime <= current->actionTime <= sampleTime.
        other = touchState.GetHistory(1);
        int64_t delta = current->actionTime - other->actionTime;
        if (delta < RESAMPLE_MIN_DELTA) {
            return;
        } else if (delta > RESAMPLE_MAX_DELTA) {
            return;
        }
        int64_t maxPredict = current->actionTime + std::min(delta / 2, RESAMPLE_MAX_PREDICTION);
        if (sampleTime > maxPredict) {
            sampleTime = maxPredict;
        }
        alpha = static_cast<float>(current->actionTime - sampleTime) / delta;
    } else {
        return;
    }

    // Resample touch coordinates.
    ResampleCoordinates(sampleTime, event, touchState, current, other, alpha);
}

void EventResample::ResampleCoordinates(int64_t sampleTime, MotionEvent* event, TouchState &touchState,
                                        const History* current, const History* other, float alpha)
{
    History oldLastResample;
    oldLastResample.InitializeFrom(touchState.lastResample);
    touchState.lastResample.actionTime = sampleTime;

    for (auto &it : event->pointers) {
        uint32_t id = it.first;
        if (oldLastResample.HasPointerId(id) && touchState.RecentCoordinatesAreIdentical(id)) {
            auto lastItem = touchState.lastResample.pointers.find(id);
            if (lastItem != touchState.lastResample.pointers.end()) {
                auto oldLastItem = oldLastResample.pointers.find(id);
                lastItem->second.CopyFrom(oldLastItem->second);
            }
            continue;
        }

        Pointer resampledCoords;
        const Pointer& currentCoords = current->GetPointerById(id);
        resampledCoords.CopyFrom(currentCoords);
        auto item = event->pointers.find(id);
        if (item == event->pointers.end()) {
            return;
        }
        if (other->HasPointerId(id) && ShouldResampleTool(item->second.toolType)) {
            const Pointer& otherCoords = other->GetPointerById(id);
            resampledCoords.coordX = CalcCoord(currentCoords.coordX, otherCoords.coordX, alpha);
            resampledCoords.coordY = CalcCoord(currentCoords.coordY, otherCoords.coordY, alpha);
        }
        item->second.CopyFrom(resampledCoords);
        event->actionTime = sampleTime;
    }
}

ssize_t EventResample::FindBatch(int32_t deviceId, int32_t source) const
{
    ssize_t idx = 0;
    for (auto it = batches_.begin(); it < batches_.end(); ++it, ++idx) {
        const MotionEvent& head = it->samples.at(0);
        if ((head.deviceId == deviceId) && (head.sourceType == source)) {
            return idx;
        }
    }
    return -1;
}

ssize_t EventResample::FindTouchState(int32_t deviceId, int32_t source) const
{
    ssize_t idx = 0;
    for (auto it = touchStates_.begin(); it < touchStates_.end(); ++it, ++idx) {
        if ((it->deviceId == deviceId) && (it->source == source)) {
            return idx;
        }
    }
    return -1;
}

bool EventResample::CanAddSample(const Batch &batch, MotionEvent &event)
{
    const MotionEvent& head = batch.samples.at(0);
    uint32_t pointerCount = event.pointerCount;
    int32_t pointerAction = event.pointerAction;
    if ((head.pointerCount != pointerCount) || (head.pointerAction != pointerAction)) {
        return false;
    }

    return true;
}

void EventResample::RewriteMessage(TouchState& state, MotionEvent &event)
{
    for (auto &it : event.pointers) {
        uint32_t id = it.first;
        if (state.lastResample.HasPointerId(id)) {
            if ((event.actionTime < state.lastResample.actionTime) || state.RecentCoordinatesAreIdentical(id)) {
                Pointer& msgCoords = it.second;
                const Pointer& resampleCoords = state.lastResample.GetPointerById(id);
                msgCoords.CopyFrom(resampleCoords);
            } else {
                state.lastResample.pointers.erase(id);
            }
        }
    }
}

ssize_t EventResample::FindSampleNoLaterThan(const Batch& batch, int64_t time)
{
    size_t numSamples = batch.samples.size();
    size_t idx = 0;
    while ((idx < numSamples) && (batch.samples.at(idx).actionTime <= time)) {
        idx += 1;
    }
    return ssize_t(idx) - 1;
}

bool EventResample::ShouldResampleTool(int32_t toolType)
{
    switch (toolType) {
        case PointerEvent::TOOL_TYPE_FINGER:
        case PointerEvent::TOOL_TYPE_PEN:
            return true;
        default:
            return false;
    }
}

} // namespace MMI
} // namespace OHOS
