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
#include "mtimer.h"
#include "libmmi_util.h"
#include "util.h"

namespace OHOS {
namespace MMI {

MTimer::MTimer(int32_t id, uint32_t outTime, const TimerFun &fun, void *data)
    : id_(id), outTime_(outTime), startTime_(GetSysClockTime()), fun_(fun), userData_(data)
{
}

MTimer::~MTimer()
{
    if (userData_) {
        free(userData_);
        userData_ = nullptr;
    }
}

bool MTimer::Process(uint64_t time)
{
    if (time == 0) {
        time = GetSysClockTime();
    }
    if (IsTimeOut(time)) {
        fun_(userData_);
        return true;
    }
    return false;
}
}
}