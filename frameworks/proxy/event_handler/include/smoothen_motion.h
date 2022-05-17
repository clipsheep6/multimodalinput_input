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

#ifndef SMOOTHEN_MOTION_H
#define SMOOTHEN_MOTION_H

#include <array>
#include <list>
#include <map>
#include <mutex>

#include "parcel.h"
#include "pointer_event.h"
#include "singleton.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr int32_t sampleCount = 2;
} // namespace
class SmoothenMotion : public DelayedSingleton<SmoothenMotion> {
public:
    SmoothenMotion() = default;
    ~SmoothenMotion() = default;
    DISALLOW_COPY_AND_MOVE(SmoothenMotion);

    bool IsSmoothenSwitch();
    void EnableSmoothenSwitch(bool smoothenSwitch = false);
    int32_t HandleEvent(std::shared_ptr<PointerEvent>& point);
    int32_t ConsumeInputEvent(int64_t frameTime, std::shared_ptr<PointerEvent>& pointEvent);
private:
    inline int32_t Calculate(float a, float b, float ratio)
    {
        return static_cast<int32_t>(a + ratio * (b - a));
    }
    struct BatchEvent {
        std::shared_ptr<PointerEvent> samples_;
        std::array<std::shared_ptr<PointerEvent>, sampleCount> history_;
        int32_t historyIndex { 1 };
        void AddHistory(std::shared_ptr<PointerEvent>& point)
        {
            historyIndex ^= 1;
            history_[historyIndex] = point;
        }
        std::shared_ptr<PointerEvent> GetHistory(size_t index) const
        {
            return history_[(historyIndex + index) & 1];
        }
        void ClearHistory()
        {
            for (auto& iter : history_) {
                iter = nullptr;
            }
        }
    };
    bool UpdateUpState(std::shared_ptr<PointerEvent>& point);
    bool UpdateMoveState(std::shared_ptr<PointerEvent>& point);
    bool UpdateDownState(std::shared_ptr<PointerEvent>& point);
    bool UpdatePointerState(std::shared_ptr<PointerEvent>& point);
    int32_t ResampleTouchState(int64_t sampleTime,
                               const BatchEvent& batch,
                               const std::shared_ptr<PointerEvent>& nextEvent,
                               std::shared_ptr<PointerEvent>& event);
    int32_t UpdateSamplePointItem(float ratio,
                                  const std::shared_ptr<PointerEvent>& current,
                                  const std::shared_ptr<PointerEvent>& other,
                                  std::shared_ptr<PointerEvent>& samplePoint);
    void RemoveMotionEventById(int32_t deviceId);
    bool FindBatchEvent(int32_t deviceId, BatchEvent& batch);
    std::list<std::shared_ptr<PointerEvent>>::iterator FindSampleEventPrev(int64_t sampleTime);
private:
    std::list<std::shared_ptr<PointerEvent>> eventBatch_;
    std::map<int32_t, BatchEvent> histories_;

    bool switch_ { false };
    std::mutex mutex_;
};
#define smoothenMotion SmoothenMotion::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // SMOOTHEN_MOTION_H