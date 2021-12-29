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
#ifndef OHOS_MTIMER_MANAGER_H
#define OHOS_MTIMER_MANAGER_H
#include <atomic>
#include <mutex>
#include <thread>
#include "mtimer.h"
#include "c_singleton.h"

namespace OHOS {
namespace MMI {

using MTimerMap = std::map<int32_t, sptr<MTimer>>;
class MTimerManager : public CSingleton<MTimerManager> {
public:
    MTimerManager();
    virtual ~MTimerManager();
    bool Init();
    void Stop();
    bool IsExist(int32_t id);
    bool AddTimer(int32_t id, uint32_t timeOut, const TimerFun &fun, void *data/* = nullptr*/);
    void DelTimer(int32_t id);

protected:
    bool ChkTimer(int32_t id) const;
    void ProcessTimers();
    void OnThread();

protected:
    std::mutex mux_;
    std::thread t_;
    std::atomic_bool isRun_ = false;
    MTimerMap timers_;
};
}
}
#define TimerMgr OHOS::MMI::MTimerManager::GetInstance()
#endif