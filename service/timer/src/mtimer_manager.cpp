/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "mtimer_manager.h"
#include <algorithm>
#include "libmmi_util.h"
#include "safe_keeper.h"

namespace OHOS {
namespace MMI {
namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "MTimerManager"};
}
MTimerManager::MTimerManager()
{
}

MTimerManager::~MTimerManager()
{
    timers_.clear();
}

bool MTimerManager::Init()
{
    isRun_ = true;
    t_ = std::thread(std::bind(&MTimerManager::OnThread, this));
    t_.detach();
    return true;
}

void MTimerManager::Stop()
{
    std::lock_guard<std::mutex> lock(mux_);
    isRun_ = false;
    if (t_.joinable()) {
        t_.join();
    }
}

bool MTimerManager::IsExist(int32_t id)
{
    std::lock_guard<std::mutex> lock(mux_);
    return ChkTimer(id);
}

bool MTimerManager::AddTimer(int32_t id, uint32_t timeOut, const TimerFun &fun, void *data)
{
    CHKF(id > 0, OHOS::PARAM_INPUT_INVALID);
    CHKF(timeOut > 0, OHOS::PARAM_INPUT_INVALID);
    CHKF(fun != nullptr, OHOS::PARAM_INPUT_INVALID);
    std::lock_guard<std::mutex> lock(mux_);
    if (ChkTimer(id)) {
        return true;
    }
    timers_[id] = sptr<MTimer>(new MTimer(id, timeOut, fun, data));
    return true;
}

void MTimerManager::DelTimer(int32_t id)
{
    CHK(id > 0, OHOS::PARAM_INPUT_INVALID);
    std::lock_guard<std::mutex> lock(mux_);
    auto it = timers_.find(id);
    if (it != timers_.end()) {
        timers_.erase(it);
    }
}

bool MTimerManager::ChkTimer(int32_t id) const
{
    auto it = timers_.find(id);
    if (it == timers_.end()) {
        return false;
    }
    return true;
}

void MTimerManager::ProcessTimers()
{
    std::lock_guard<std::mutex> lock(mux_);
    auto curTime = GetSysClockTime();
    for (auto it = timers_.begin(); it != timers_.end();) {
        if (it->second->Process(curTime)) {
            it = timers_.erase(it);
        } else {
            it++;
        }
    }
}

void MTimerManager::OnThread()
{
    uint64_t tid = GetThisThreadIdOfLL();
    CHK(tid > 0, OHOS::VAL_NOT_EXP);
    SafeKpr->RegisterEvent(tid, "MTimerManager::_OnThread");

    while (isRun_) {
        ProcessTimers();
        SafeKpr->ReportHealthStatus(tid);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    MMI_LOGD("MTimerManager::OnThread end...");
}
}
}