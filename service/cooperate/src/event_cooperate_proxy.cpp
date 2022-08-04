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

#include "event_cooperate_proxy.h"

#include "message_option.h"
#include "string_ex.h"

#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventCooperateProxy" };
} // namespace

EventCooperateProxy::EventCooperateProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IEventCooperate>(impl)
{
}

EventCooperateProxy::~EventCooperateProxy()
{
}

bool EventCooperateProxy::OnCooperateMessage(const std::string &deviceId, const CooperateMessages &msg)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(EventCooperateProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return false;
    }
    WRITESTRING(data, deviceId, ERR_INVALID_VALUE);
    WRITEINT32(data, static_cast<int32_t>(msg), ERR_INVALID_VALUE);
    sptr<IRemoteObject> remote = Remote();
    CHKPF(remote);
    int32_t ret = remote->SendRequest(IEventCooperate::ON_COOPERATE_MESSAGE_EVENT, data, reply, option);
    if (ret != NO_ERROR) {
        MMI_HILOGE("Send request fail, ret:%{public}d", ret);
        return false;
    }
    bool result = false;
    READBOOL(reply, result, false);
    return result;
}

bool EventCooperateProxy::OnCooperateState(bool state)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(EventCooperateProxy::GetDescriptor())) {
        MMI_HILOGE("Failed to write descriptor");
        return false;
    }
    WRITEBOOL(data, state, ERR_INVALID_VALUE);
    sptr<IRemoteObject> remote = Remote();
    CHKPF(remote);
    int32_t ret = remote->SendRequest(IEventCooperate::ON_COOPERATE_STATE_EVENT, data, reply, option);
    if (ret != NO_ERROR) {
        MMI_HILOGE("Send request fail, ret:%{public}d", ret);
        return false;
    }
    bool result = false;
    READBOOL(reply, result, false);
    return result;
}
} // namespace MMI
} // namespace OHOS