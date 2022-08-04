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

#include "event_cooperate_service.h"

#include <cstring>

#include <sys/types.h>
#include <unistd.h>

#include "string_ex.h"

#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventCooperateService" };
} // namespace

EventCooperateService::EventCooperateService()
{
    CALL_DEBUG_ENTER;
}

EventCooperateService::~EventCooperateService()
{
    CALL_DEBUG_ENTER;
}

int32_t EventCooperateService::SetCooperateMessageListener(std::function<void(std::string, CooperateMessages)> listener)
{
    CALL_DEBUG_ENTER;
    msgCooperateListener_ = listener;
    return RET_OK;
}

int32_t EventCooperateService::SetCooperateStateCallback(std::function<void(bool)> callback)
{
    CALL_DEBUG_ENTER;
    CHKPR(callback, RET_ERR);
    stateCooperateCallback_ = callback;
    return RET_OK;
}

bool EventCooperateService::OnCooperateMessage(const std::string &deviceId, const CooperateMessages &msg)
{
    CALL_DEBUG_ENTER;
    CHKPR(msgCooperateListener_, false);
    msgCooperateListener_(deviceId, msg);
    return true;
}

bool EventCooperateService::OnCooperateState(bool state)
{
    CALL_DEBUG_ENTER;
    CHKPR(stateCooperateCallback_, false);
    stateCooperateCallback_(state);
    return true;
}
} // namespace MMI
} // namespace OHOS
