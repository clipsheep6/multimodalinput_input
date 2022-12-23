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

#ifndef ANR_COLLECTER_H
#define ANR_COLLECTER_H

#include <vector>
#include <mutex>

#include "nocopyable.h"

#include "i_anr_observer.h"
#include "net_packet.h"

namespace OHOS {
namespace MMI {
class AnrCollecter final {
public:
    AnrCollecter() = default;
    DISALLOW_MOVE(AnrCollecter);
    ~AnrCollecter() = default;
    void SetAnrObserver(std::shared_ptr<IAnrObserver> &observer);
    void OnConnected();
    int32_t OnAnr(NetPacket& pkt);
    void SetLastProcessedEventId(int32_t eventType, int32_t eventId, uint64_t actionTime);
    void MarkProcessed(int32_t eventType);
private:
    void HandlerAnr(int32_t pid);
    void UpdateLastProcessedEventId(int32_t eventType, int32_t eventId);
    void SetLastProcessedEventStatus(int32_t eventType, bool status);
    int32_t GetLastProcessedEventId(int32_t eventType);
    void SendEvent(int32_t eventType, int64_t delayTime);
private:
    struct ANREvent {
        bool sendStatus { false };
        int32_t lastEventId { -1 };
        int32_t lastReportId { -1 };
    };
    ANREvent event_[ANR_EVENT_TYPE_NUM];
    std::mutex mtx_;
    std::vector<std::shared_ptr<IAnrObserver>> anrObservers_;
};
} // namespace MMI
} // namespace OHOS
#endif // ANR_COLLECTER_H