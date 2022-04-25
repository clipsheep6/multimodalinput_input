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
#include "remote_msg_handler.h"

#include "string_ex.h"

#include "i_multimodal_input_connect.h"
#include "input_event_handler.h"
#include "mmi_func_callback.h"
#include "mmi_log.h"
#include "multimodal_input_connect_def_parcel.h"
#include "time_cost_chk.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr int32_t SYSTEM_UID = 1000;
constexpr int32_t ROOT_UID = 0;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "RemoteMsgHandler"};
} // namespace

void RemoteMsgHandler::Init(UDSServer& udsServer)
{
    udsServer_ = &udsServer;
    callbacks_ = {
        {IMultimodalInputConnect::ALLOC_SOCKET_FD, MsgCallbackBind3(&RemoteMsgHandler::OnAllocSocketFd, this)},
        {IMultimodalInputConnect::ADD_INPUT_EVENT_FILTER, 
            MsgCallbackBind3(&RemoteMsgHandler::OnAddInputEventFilter, this)},
    };
}

void RemoteMsgHandler::OnMsgHandler(int32_t uid, int32_t pid, uint32_t code, MessageParcel& data,
    MessageParcel& reply)
{
    TimeCostChk chk("IPC-RemoteHandler", "overtime 300(us)", MAX_OVER_TIME, pid,
        static_cast<int64_t>(code));
    if (pid <= 0) {
        MMI_HILOGE("invalid clientIdx:%{public}d", pid);
        return;
    }
    auto callback = GetMsgCallback(code);
    if (callback == nullptr) {
        MMI_HILOGE("Unknown remote msg id:%{public}d,errCode:%{public}d", id, UNKNOWN_MSG_ID);
        return;
    }
    auto session = udsServer_->GetSessionByPid(pid);
    if (session == nullptr && code != IMultimodalInputConnect::ALLOC_SOCKET_FD) {
        MMI_HILOGE("invalid request session=nullptr");
        return;
    }
    lastClientPid_ = pid;
    lastClientUid_ = uid;
    (*callback)(session, data, reply);
}

void RemoteMsgHandler::OnAllocSocketFd(SessionPtr session, MessageParcel &data, MessageParcel &reply)
{
    if (session != nullptr) {
        MMI_HILOGE("Repeat request clientIdx:%{public}d", lastClientPid_);
        return;
    }
    sptr<ConnectReqParcel> req = data.ReadParcelable<ConnectReqParcel>();
    CHKPV(req);
    MMI_HILOGD("clientName:%{public}s,moduleId:%{public}d", req->data.clientName.c_str(), req->data.moduleId);

    int32_t clientFd = INVALID_SOCKET_FD;
    int32_t ret = udsServer_->AllocSocketFd(req->data.clientName, req->data.moduleId, clientFd);
    if (ret != RET_OK) {
        MMI_HILOGE("call AddSocketPairInfo return %{public}d", ret);
        reply.WriteInt32(RET_ERR);
        return;
    }
    MMI_HILOGI("call AllocSocketFd success");

    reply.WriteInt32(RET_OK);
    reply.WriteFileDescriptor(clientFd);

    MMI_HILOGI("send clientFd to client, clientFd = %d", clientFd);
    close(clientFd);
}

void RemoteMsgHandler::OnAddInputEventFilter(SessionPtr session, MessageParcel& data, MessageParcel& reply)
{
    CALL_LOG_ENTER;
    CHKPV(session);
    if (lastClientUid_ != SYSTEM_UID && lastClientUid_ != ROOT_UID) {
        MMI_HILOGE("check failed, uid is not root or system");
        return;
    }

    sptr<IRemoteObject> client = data.ReadRemoteObject();
    if (client == nullptr) {
        MMI_HILOGE("mouse client is nullptr");
        return;
    }
    sptr<IEventFilter> filter = iface_cast<IEventFilter>(client);
    if (filter == nullptr) {
        MMI_HILOGE("filter is nullptr");
        return;
    }
    MMI_HILOGD("filter iface_cast succeeded");
    int32_t ret = InputHandler->AddInputEventFilter(filter);
    if (!reply.WriteInt32(ret)) {
        MMI_HILOGE("WriteInt32:%{public}d fail", ret);
        return;
    }
}
} // namespace MMI
} // namespace OHOS