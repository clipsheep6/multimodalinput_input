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
#include <fstream>
#include<regex>
#include "plugin_context.h"
namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "PluginContext"};
// int64_t time { 0 };
// std::list<uint64_t, int32_t> timeAndCostTimes_;
// int32_t avg_ { 0 };
int32_t max_ { 0 };
int32_t raw_ { 0 };
int64_t sum_ { 0 };
int32_t times_ { 0 };

int32_t memMax_ { 0 };
int32_t memSum_ { 0 };
int32_t count_ { 0 };
int32_t pss_ { 0 };
int32_t dirty_ { 0 };
int32_t swap_ { 0 };
int32_t size_ { 0 };

int32_t GetNum(std::string str)
{
    std::smatch match;
    bool isValueNumber = std::regex_search(str, match, std::regex("\\d+"));
    if (isValueNumber) {
        return stoi(match[0]);
    } else {
        MMI_HILOGE("numberValue : is error");
    }
    return 0;
}
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

void PluginContext::OnReport(int32_t &max, int32_t &avg, int32_t &memMax, int32_t &memAvg)
{

    max = max_;
    avg = sum_ / times_;
    memMax = memMax_;
    memAvg = memSum_ / count_;
    memMax_ = 0;
    memSum_ = 0;
    count_ = 0;
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

void PluginContext::chengmem()
{
    std::string strPid = std::to_string(GetPid());
    MMI_HILOGE("1111111111111111 PID is %{public}s",strPid.data());
    std::string path = "/proc/" + strPid +"/smaps";
    std::ifstream mem(path);
    if (!mem.is_open()) {
        MMI_HILOGE("Failed to open config file");
        return;
    }
    std::string tmp;
    bool dataStatus = false;
    while (std::getline(mem, tmp)) {
        if (tmp.find("libinput_touch_2_key_handler.z.so") != std::string::npos) {
            dataStatus = true;
        }
        if (dataStatus) {
            if (tmp.find("Size") != std::string::npos) {
                size_ += GetNum(tmp);
            } else if (tmp.find("Pss") != std::string::npos) {
                pss_ += GetNum(tmp);
            } else if (tmp.find("Private_Dirty") != std::string::npos) {
                dirty_ += GetNum(tmp);
            } else if (tmp.find("Swap") != std::string::npos) {
                swap_ += GetNum(tmp);
            } else if (tmp.find("VmFlags") != std::string::npos) {
                dataStatus = false;
                MMI_HILOGE("%{public}s", tmp.data());
            } else {}
        }
    }
    mem.close();
    MMI_HILOGE("Size = %{public}d; pss = %{public}d; dirty = %{public}d; swap = %{public}d;", size_, pss_, dirty_, swap_);
    memMax_ = std::max(dirty_, memMax_);
    memSum_ += dirty_;
    count_++;
    size_ = 0;
    pss_ = 0;
    dirty_ = 0;
    swap_ = 0;
}
} // namespace MMI
} // namespace OHOS