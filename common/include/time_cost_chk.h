/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_TIME_COST_CHK_H
#define OHOS_TIME_COST_CHK_H

#define MAX_INPUT_EVENT_TIME (1000)
#define MAX_OVER_TIME (300)

namespace OHOS::MMI {
class TimeCostChk {
    static inline constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "TimeCostChk" };
public:
    TimeCostChk(const std::string& strReason, const std::string& strOutputStr, uint32_t tmChk = 10,
                int64_t llParam1 = 0, int64_t llParam2 = 0):
        beginTime_(std::chrono::high_resolution_clock::now()),
        strOutput_(strOutputStr),
        strReason_(strReason),
        uiTime_(tmChk),
        llParam1_(llParam1),
        llParam2_(llParam2) {
    }

    ~TimeCostChk(void)
    {
        auto ullCost = GetElapsed_micro(); // microsecond
        if ((ullCost > uiTime_) && strReason_.size() > 0 && strOutput_.size() > 0) {
            if (0 != llParam1_ || 0 != llParam2_) {
                MMI_LOGW("Time cost overtime (%{public}llu(us)>%{public}u(us)) when Reason:%{public}s chk:%{public}s "
                         "param1:%{public}llu param2:%{public}llu",
                         ullCost, uiTime_, strReason_.c_str(), strOutput_.c_str(), llParam1_, llParam2_);
            } else {
                MMI_LOGW("TimeCostChk Overtime(%{public}llu(us)>%{public}u(us)) when Reason:%{public}s chk:%{public}s",
                         ullCost, uiTime_, strReason_.c_str(), strOutput_.c_str());
            }
        }
    }

    // microseconds
    uint64_t GetElapsed_micro() const
    {
        auto tm64Cost = std::chrono::duration_cast<std::chrono::microseconds>(
                            std::chrono::high_resolution_clock::now() - beginTime_
                        ).count();
        return static_cast<uint64_t>(tm64Cost);
    }

private:
    const std::chrono::time_point<std::chrono::high_resolution_clock> beginTime_;
    const std::string strOutput_ = ""; // output string
    const std::string strReason_ = ""; // reason string
    const uint32_t uiTime_ = 0;
    const int64_t llParam1_ = 0;
    const int64_t llParam2_ = 0;
};
}

#define TimeCostChk(reason, outstring)                   OHOS::MMI::TimeCostChk _chk_(reason, outstring, 10)
#define TimeCostChkp1(reason, outstring, p1)             OHOS::MMI::TimeCostChk _chk_(reason, outstring, 10, p1)
#define TimeCostChkp2(reason, outstring, p1, p2)         OHOS::MMI::TimeCostChk _chk_(reason, outstring, 10, p1, p2)
#define TimeCostChkf(reason, outstring, tmchk)           OHOS::MMI::TimeCostChk _chk_(reason, outstring, tmchk)
#define TimeCostChkfp1(reason, outstring, tmchk, p1)     OHOS::MMI::TimeCostChk _chk_(reason, outstring, tmchk, p1)
#define TimeCostChkfp2(reason, outstring, tmchk, p1, p2) OHOS::MMI::TimeCostChk _chk_(reason, outstring, tmchk, p1, p2)

#endif
