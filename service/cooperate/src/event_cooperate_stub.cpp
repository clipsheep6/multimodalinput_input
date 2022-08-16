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

#include "event_cooperate_stub.h"

#include <map>
#include "string_ex.h"

#include "ipc_skeleton.h"
#include "time_cost_chk.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventCooperateStub" };
using CooperateFunc = int32_t (EventCooperateStub::*)(MessageParcel& data, MessageParcel& reply);
} // namespace

int32_t EventCooperateStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    CALL_DEBUG_ENTER;
    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != IEventCooperate::GetDescriptor()) {
        MMI_HILOGE("Get unexpect descriptor:%{public}s", Str16ToStr8(descriptor).c_str());
        return ERR_INVALID_STATE;
    }
    const static std::map<int32_t, CooperateFunc> mapCooperateFunc = {
        {IEventCooperate::ON_COOPERATE_MESSAGE_EVENT, &EventCooperateStub::StubOnCooperateMessage},
        {IEventCooperate::ON_COOPERATE_STATE_EVENT, &EventCooperateStub::StubOnCooperateState},
    };
    auto it = mapCooperateFunc.find(code);
    if (it != mapCooperateFunc.end()) {
        return (this->*it->second)(data, reply);
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t EventCooperateStub::StubOnCooperateMessage(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    std::string deviceId;
    READSTRING(data, deviceId, ERR_INVALID_VALUE);
    int32_t nType;
    READINT32(data, nType, ERR_INVALID_VALUE);
    CooperationState msg = CooperationState(nType);
    bool ret = OnCooperateMessage(deviceId, msg);
    WRITEBOOL(reply, ret, RET_ERR);
    return RET_OK;
}

int32_t EventCooperateStub::StubOnCooperateState(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    bool state;
    READBOOL(data, state, ERR_INVALID_VALUE);
    bool ret = OnCooperateState(state);
    WRITEBOOL(reply, ret, RET_ERR);
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS