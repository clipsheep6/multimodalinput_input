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

#include "plugin_context.h"

#include <fstream>

namespace OHOS {
namespace MMI {
namespace {
// constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "PluginContext"};
int32_t max_ { 0 };
int32_t raw_ { 0 };
int64_t sum_ { 0 };
int32_t times_ { 0 };
} // namespace

void PluginContext::SetEventHandler(std::shared_ptr<IInputEventHandler> handler)
{
    handler_ = handler;
}

std::shared_ptr<IInputEventHandler> PluginContext::GetEventHandler()
{
    return handler_;
}

void PluginContext::TimeStat(TimeStatFlag flag)
{
    auto time = GetSysClockTime();
    if (flag == TimeStatFlag::BEGIN) {
        raw_ = time;
    } else {
        int32_t t = GetSysClockTime() - raw_;
        max_ = std::max(t, max_);
        sum_ += t;
        times_++;
    }
}

void PluginContext::OnReport(int32_t &max, int32_t &avg)
{
    max = max_;
    avg = sum_ / times_;
    max_ = 0;
    sum_ = 0;
    times_  = 0;
}

std::shared_ptr<IInputDeviceManager> PluginContext::GetInputDeviceManager()
{
    return inputDeviceMgr_;
}

void PluginContext::StatBegin()
{
    TimeStat(TimeStatFlag::BEGIN);
}

void PluginContext::StatEnd()
{
    TimeStat(TimeStatFlag::END);
}
} // namespace MMI
} // namespace OHOS