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

#ifndef ANR_HANDLER_H
#define ANR_HANDLER_H

#include <atomic>

#include "singleton.h"

#include "proto.h"

namespace OHOS {
namespace MMI {
class ANRHandler final {
    DECLARE_DELAYED_SINGLETON(ANRHandler);
public:
    DISALLOW_COPY_AND_MOVE(ANRHandler);
    void UpdateLastEventId(int32_t eventType, int32_t eventId, uint64_t actionTime);
    void MarkProcessedTask();
private:
    void AddMarkProcessedTask(int64_t actionTime);
    int32_t GetLastReportId(int32_t eventType);
private:
    std::mutex anrMtx_;
    struct ANREventInfo {
        int32_t lastEventId { -1 };
        int32_t lastReportId { -1 };
    };
    bool isExistTask_ = false;
    ANREventInfo eventInfos_[ANR_EVENT_TYPE_BUTT];
};
#define ANRHdl ::OHOS::DelayedSingleton<ANRHandler>::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // ANR_HANDLER_H