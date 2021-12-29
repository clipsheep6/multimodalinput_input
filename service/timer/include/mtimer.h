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
#ifndef OHOS_TIMER_H
#define OHOS_TIMER_H
#include <map>
#include <functional>
#include <refbase.h>

namespace OHOS {
namespace MMI {
using TimerFun = std::function<void(void *data)>;
class MTimer : public RefBase {
public:
    MTimer(int32_t id, uint32_t timeOut, const TimerFun &fun, void *data/* = nullptr*/);
    virtual ~MTimer();

    int32_t GetID() const
    {
        return id_;
    }
    bool Process(uint64_t time = 0);

protected:
    bool IsTimeOut(uint64_t time) const
    {
        return (time >= startTime_ + outTime_);
    }

protected:
    int32_t id_ = 0;
    uint32_t outTime_ = 0;
    uint64_t startTime_ = 0;
    const TimerFun fun_ = nullptr;
    void *userData_ = nullptr;
};
}
}
#endif