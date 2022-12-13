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

#include "input_filter.h"

#include "input_connect_manager.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputFilter" };
constexpr size_t MAX_FILTER_NUM = 4;
} // namespace

int32_t InputFilter::AddInputEventFilter(std::shared_ptr<IInputEventFilter> filter, int32_t priority)
{
    CALL_INFO_TRACE;
    std::lock_guard<std::mutex> guard(mtx_);
    CHKPR(filter, RET_ERR);
    if (eventFilterServices_.size() >= MAX_FILTER_NUM) {
        MMI_HILOGE("Too many filters, size:%{public}zu", eventFilterServices_.size());
        return RET_ERR;
    }
    sptr<IEventFilter> service = new (std::nothrow) EventFilterService(filter);
    CHKPR(service, RET_ERR);
    const int32_t filterId = EventFilterService::GetNextId();
    int32_t ret = MultimodalInputConnMgr->AddInputEventFilter(service, filterId, priority);
    if (ret != RET_OK) {
        MMI_HILOGE("AddInputEventFilter has send to server failed, priority:%{public}d, ret:%{public}d", priority, ret);
        service = nullptr;
        return RET_ERR;
    }
    auto it =  eventFilterServices_.emplace(filterId, std::make_tuple(service, priority));
    if (!it.second) {
        MMI_HILOGW("Filter id duplicate");
    }
    return filterId;
}

int32_t InputFilter::RemoveInputEventFilter(int32_t filterId)
{
    CALL_INFO_TRACE;
    std::lock_guard<std::mutex> guard(mtx_);
    if (eventFilterServices_.empty()) {
        MMI_HILOGE("Filters is empty, size:%{public}zu", eventFilterServices_.size());
        return RET_OK;
    }
    std::map<int32_t, std::tuple<sptr<IEventFilter>, int32_t>>::iterator it;
    if (filterId != -1) {
        it = eventFilterServices_.find(filterId);
        if (it == eventFilterServices_.end()) {
            MMI_HILOGE("Filter not found");
            return RET_OK;
        }
    }
    int32_t ret = MultimodalInputConnMgr->RemoveInputEventFilter(filterId);
    if (ret != RET_OK) {
        MMI_HILOGE("Remove filter failed, filter id:%{public}d, ret:%{public}d", filterId, ret);
        return RET_ERR;
    }
    if (filterId != -1) {
        eventFilterServices_.erase(it);
    } else {
        eventFilterServices_.clear();
    }
    MMI_HILOGI("Filter remove success");
    return RET_OK;
}

void InputFilter::OnConnected()
{
    CALL_INFO_TRACE;
    if (eventFilterServices_.size() > MAX_FILTER_NUM) {
        MMI_HILOGE("Too many filters, size:%{public}zu", eventFilterServices_.size());
        return;
    }
    for (const auto &[filterId, t] : eventFilterServices_) {
        const auto &[service, priority] = t;
        int32_t ret = MultimodalInputConnMgr->AddInputEventFilter(service, filterId, priority);
        if (ret != RET_OK) {
            MMI_HILOGE("AddInputEventFilter has send to server failed, filterId:%{public}d, priority:%{public}d, ret:%{public}d",
                filterId, priority, ret);
        }
    }
}
} // namespace MMI
} // namespace OHOS
