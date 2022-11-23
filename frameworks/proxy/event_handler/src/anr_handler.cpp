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

#include "anr_handler.h"

#include <cinttypes>

#include "define_multimodal.h"

#include "input_manager_impl.h"
#include "multimodal_input_connect_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "ANRHandler" };
constexpr int64_t MAX_MARK_PROCESS_DELAY_TIME = 3500000;
constexpr int64_t MIN_MARK_PROCESS_DELAY_TIME = 50000;
} // namespace

ANRHandler::ANRHandler() {}
ANRHandler::~ANRHandler() {}

void ANRHandler::UpdateLastEventId(int32_t eventType, int32_t eventId, uint64_t actionTime)
{
    CALL_DEBUG_ENTER;
    anrMtx_.lock();
    if (eventInfos_[eventType].lastEventId > eventId) {
        anrMtx_.unlock();
        MMI_HILOGE("Event type:%{public}d, id %{public}d less then last processed lastEventId %{public}d",
            eventType, eventId, eventInfos_[eventType].lastEventId);
        return;
    }
    eventInfos_[eventType].lastEventId = eventId;
    bool isExistTask = isExistTask_;
    anrMtx_.unlock();

    MMI_HILOGE("Event type:%{public}d, id %{public}d", eventType, eventId);

    if (isExistTask) {
        return;
    }
    AddMarkProcessedTask(actionTime);
}

void ANRHandler::MarkProcessedTask()
{
    CALL_DEBUG_ENTER;
    std::vector<int32_t> toReportIds;
    anrMtx_.lock();
    toReportIds.reserve(ANR_EVENT_TYPE_BUTT);
    toReportIds.push_back(GetLastReportId(ANR_EVENT_TYPE_DISPATCH));
    toReportIds.push_back(GetLastReportId(ANR_EVENT_TYPE_MONITOR));
    isExistTask_ = false;
    anrMtx_.unlock();
    int32_t ret = MultimodalInputConnMgr->MarkProcessed(toReportIds);
    if (ret != 0) {
        MMI_HILOGE("Send to server failed, ret:%{public}d", ret);
    }
    // TODO: RETRY
}

int32_t ANRHandler::GetLastReportId(int32_t eventType)
{
    if ((eventInfos_[eventType].lastEventId == INVALID_OR_PROCESSED_ID)
        || (eventInfos_[eventType].lastEventId <= eventInfos_[eventType].lastReportId)) {
        MMI_HILOGD("Invalid or processed event type:%{public}d, lastEventId:%{public}d, lastReportId:%{public}d",
            eventType, eventInfos_[eventType].lastEventId, eventInfos_[eventType].lastReportId);
        return INVALID_OR_PROCESSED_ID;
    }
    eventInfos_[eventType].lastEventId = eventInfos_[eventType].lastEventId;
    return eventInfos_[eventType].lastEventId;
}

void ANRHandler::AddMarkProcessedTask(int64_t actionTime)
{
    CALL_DEBUG_ENTER;
    int64_t currentTime = GetSysClockTime();
    int64_t timeoutTime = INPUT_UI_TIMEOUT_TIME - (currentTime - actionTime);
    auto delayMs = [=]() -> int64_t {
        if (timeoutTime < MIN_MARK_PROCESS_DELAY_TIME) {
            return 0;
        } else if (timeoutTime >= MAX_MARK_PROCESS_DELAY_TIME) {
            return MAX_MARK_PROCESS_DELAY_TIME / 1000;
        }
        return timeoutTime / 1000;
    }();

    MMI_HILOGD("Processed actionTime:%{public}" PRId64 ", currentTime:%{public}" PRId64
        ", timeoutTime:%{public}" PRId64 ", delayMs:%{public}" PRId64,
        actionTime, currentTime, timeoutTime, delayMs);

    auto eventHandler = InputMgrImpl.GetEventHandler();
    CHKPV(eventHandler);
    std::function<void()> eventFunc = std::bind(&ANRHandler::MarkProcessedTask, this);
    if (!eventHandler->PostHighPriorityTask(eventFunc, delayMs)) {
        MMI_HILOGE("Send event failed");
        return;
    }
    anrMtx_.lock();
    isExistTask_ = true;
    anrMtx_.unlock();
}
} // namespace MMI
} // namespace OHOS