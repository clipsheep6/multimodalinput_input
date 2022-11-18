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
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AnrCollecter" };
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
} // namespace MMI
} // namespace OHOS
