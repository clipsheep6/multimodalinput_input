/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "call_dinput_service.h"

#include <cstring>
#include <sys/types.h>
#include <unistd.h>

#include "error_multimodal.h"
#include "mmi_log.h"
#include "string_ex.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "CallDinputService" };
} // namespace

CallDinputService::CallDinputService()
{
    CALL_LOG_ENTER;
}

CallDinputService::~CallDinputService()
{
    CALL_LOG_ENTER;
}

void CallDinputService::SetPrepareCallback(const std::function<void(int32_t)> callback)
{
    prepareCallback_ = callback;
}
void CallDinputService::SetUnprepareCallback(const std::function<void(int32_t)> callback)
{
    unprepareCallback_ = callback;
}
void CallDinputService::SetStartCallback(const std::function<void(int32_t)> callback)
{
    startCallback_ = callback;
}

void CallDinputService::SetStopCallback(const std::function<void(int32_t)> callback)
{
    stopCallback_ = callback;
}

void CallDinputService::SetRemoteAbilityCallback(std::function<void(std::set<int32_t>)> callback)
{
    remoteAbilityCallback_ = callback;
}

int32_t CallDinputService::HandlePrepareDinput(std::string deviceId, int32_t status)
{
    CHKPR(prepareCallback_, ERROR_NULL_POINTER);
    prepareCallback_(status);
    return RET_OK;
}
int32_t CallDinputService::HandleUnprepareDinput(std::string deviceId, int32_t status)
{
    CHKPR(unprepareCallback_, ERROR_NULL_POINTER);
    unprepareCallback_(status);
    return RET_OK;
}
int32_t CallDinputService::HandleStartDinput(std::string deviceId, uint32_t inputTypes, int32_t status)
{
    CHKPR(startCallback_, ERROR_NULL_POINTER);
    startCallback_(status);
    return RET_OK;
}
int32_t CallDinputService::HandleStopDinput(std::string deviceId, uint32_t inputTypes, int32_t status)
{
    CHKPR(stopCallback_, ERROR_NULL_POINTER);
    stopCallback_(status);
    return RET_OK;
}

int32_t CallDinputService::HandleRemoteInputAbility(const std::set<int32_t> remoteInputAbility)
{
    CHKPR(remoteAbilityCallback_, ERROR_NULL_POINTER);
    remoteAbilityCallback_(remoteInputAbility);
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
