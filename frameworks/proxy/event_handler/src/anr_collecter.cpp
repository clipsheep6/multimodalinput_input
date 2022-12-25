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

#include "anr_collecter.h"

#include "input_connect_manager.h"
#include "input_manager_impl.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AnrCollecter" };
constexpr int64_t MAX_MARK_PROCESS_DELAY_TIME = 3500000;
constexpr int64_t MIN_MARK_PROCESS_DELAY_TIME = 50000;
constexpr int32_t INVALID_OR_PROCESSED_ID = -1;
} // namespace

void AnrCollecter::SetAnrObserver(std::shared_ptr<IAnrObserver> &observer)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    for (auto iter = anrObservers_.begin(); iter != anrObservers_.end(); ++iter) {
        if (*iter == observer) {
            MMI_HILOGE("Observer already exist");
            return;
        }
    }
    anrObservers_.push_back(observer);
    int32_t ret = MultimodalInputConnMgr->SetAnrObserver();
    if (ret != RET_OK) {
        MMI_HILOGE("Set anr observer failed, ret:%{public}d", ret);
    }
}

void AnrCollecter::HandlerAnr(int32_t pid)
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    std::lock_guard<std::mutex> guard(mtx_);
    for (const auto &observer : anrObservers_) {
        CHKPC(observer);
        observer->OnAnr(pid);
    }
    MMI_HILOGI("ANR noticed pid:%{public}d", pid);
}

void AnrCollecter::OnConnected()
{
    CALL_DEBUG_ENTER;
    if (anrObservers_.empty()) {
        return;
    }
    int32_t ret = MultimodalInputConnMgr->SetAnrObserver();
    if (ret != RET_OK) {
        MMI_HILOGE("Set anr observerfailed, ret:%{public}d", ret);
    }
}

int32_t AnrCollecter::OnAnr(NetPacket& pkt)
{
    CALL_DEBUG_ENTER;
    int32_t pid;
    pkt >> pid;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet read data failed");
        return RET_ERR;
    }
    MMI_HILOGI("Client pid:%{public}d", pid);
    HandlerAnr(pid);
    return RET_OK;
}

void AnrCollecter::SetLastProcessedEventStatus(int32_t eventType, bool status)
{
    std::lock_guard<std::mutex> guard(mtx_);
    event_[eventType].sendStatus = status;
}

void AnrCollecter::UpdateLastProcessedEventId(int32_t eventType, int32_t eventId)
{
    std::lock_guard<std::mutex> guard(mtx_);
    event_[eventType].lastEventId = eventId;
}

void AnrCollecter::SetLastProcessedEventId(int32_t eventType, int32_t eventId, uint64_t actionTime)
{
    CALL_DEBUG_ENTER;
    if (event_[eventType].lastEventId > eventId) {
        MMI_HILOGE("Event type:%{public}d, id %{public}d less then last processed lastEventId %{public}d",
            eventType, eventId, event_[eventType].lastEventId);
        return;
    }
    UpdateLastProcessedEventId(eventType, eventId);

    int64_t currentTime = GetSysClockTime();
    int64_t timeoutTime = INPUT_UI_TIMEOUT_TIME - (currentTime - actionTime);
    MMI_HILOGD("Processed event type:%{public}d, id:%{public}d, actionTime:%{public}" PRId64 ", "
        "currentTime:%{public}" PRId64 ", timeoutTime:%{public}" PRId64,
        eventType, eventId, actionTime, currentTime, timeoutTime);

    if (!event_[eventType].sendStatus) {
        if (timeoutTime < MIN_MARK_PROCESS_DELAY_TIME) {
            SendEvent(eventType, 0);
        } else {
            int64_t delayTime = 0;
            if (timeoutTime >= MAX_MARK_PROCESS_DELAY_TIME) {
                delayTime = MAX_MARK_PROCESS_DELAY_TIME / 1000;
            } else {
                delayTime = timeoutTime / 1000;
            }
            SendEvent(eventType, delayTime);
        }
    }
}

int32_t AnrCollecter::GetLastProcessedEventId(int32_t eventType)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (event_[eventType].lastEventId == INVALID_OR_PROCESSED_ID
        || event_[eventType].lastEventId <= event_[eventType].lastReportId) {
        MMI_HILOGD("Invalid or processed event type:%{public}d, lastEventId:%{public}d, lastReportId:%{public}d",
            eventType, event_[eventType].lastEventId, event_[eventType].lastReportId);
        return INVALID_OR_PROCESSED_ID;
    }

    event_[eventType].lastReportId = event_[eventType].lastEventId;
    MMI_HILOGD("Processed event type:%{public}d, lastEventId:%{public}d, lastReportId:%{public}d",
        eventType, event_[eventType].lastEventId, event_[eventType].lastReportId);
    return event_[eventType].lastEventId;
}

void AnrCollecter::MarkProcessed(int32_t eventType)
{
    CALL_DEBUG_ENTER;
    int32_t eventId = GetLastProcessedEventId(eventType);
    if (eventId == INVALID_OR_PROCESSED_ID) {
        return;
    }
    MMI_HILOGD("Processed event type:%{public}d, id:%{public}d", eventType, eventId);
    int32_t ret = MultimodalInputConnMgr->MarkProcessed(eventType, eventId);
    if (ret != 0) {
        MMI_HILOGE("Send to server failed, ret:%{public}d", ret);
    }
    SetLastProcessedEventStatus(eventType, false);
}

void AnrCollecter::SendEvent(int32_t eventType, int64_t delayTime)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGD("Event type:%{public}d, delayTime:%{public}" PRId64, eventType, delayTime);
    SetLastProcessedEventStatus(eventType, true);
    auto eventHandler = InputMgrImpl.GetEventHandler();
    CHKPV(eventHandler);
    std::function<void()> eventFunc = std::bind(&AnrCollecter::MarkProcessed, this, eventType);
    if (!eventHandler->PostHighPriorityTask(eventFunc, delayTime)) {
        MMI_HILOGE("Send dispatch event failed");
    }
}
} // namespace MMI
} // namespace OHOS
