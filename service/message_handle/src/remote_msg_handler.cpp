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

#include "input_event_handler.h"
#include "mmi_func_callback.h"
#include "multimodal_input_connect_def_parcel.h"
#include "multimodal_input_connect_stub.h"
#include "time_cost_chk.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr int32_t SYSTEM_UID = 1000;
constexpr int32_t ROOT_UID = 0;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "RemoteMsgHandler"};
} // namespace

void RemoteMsgHandler::Init(IMultimodalInputConnect& multStub)
{
    multStub_ = &multStub;
    callbacks_ = {
        {IMultimodalInputConnect::ALLOC_SOCKET_FD, MsgCallbackBind2(&RemoteMsgHandler::OnAllocSocketFd, this)},
        {IMultimodalInputConnect::ADD_INPUT_EVENT_FILTER, 
            MsgCallbackBind2(&RemoteMsgHandler::OnAddInputEventFilter, this)},
    };
}

void RemoteMsgHandler::OnMsgHandler(int32_t taskId, int32_t uid, int32_t pid, uint64_t stid, uint32_t code,
    MessageParcel& data, MessageParcel& reply)
{
    TimeCostChk chk("IPC-RemoteHandler", "overtime 300(us)", MAX_OVER_TIME, pid,
        static_cast<int64_t>(code));
    CHKPV(multStub_);
    if (pid <= 0) {
        MMI_HILOGE("invalid clientIdx:%{public}d", pid);
        return;
    }
    auto callback = GetMsgCallback(code);
    if (callback == nullptr) {
        MMI_HILOGE("Unknown remote msg id:%{public}d", code);
        return;
    }
    auto multStub = static_cast<MultimodalInputConnectStub*>(multStub_);
    CHKPV(multStub);
    lastSession_ = multStub->GetSessionByPid(pid);
    if (lastSession_ == nullptr && code != IMultimodalInputConnect::ALLOC_SOCKET_FD) {
        MMI_HILOGE("invalid request session=nullptr");
        return;
    }
    lastClientPid_ = pid;
    lastClientUid_ = uid;
    MMI_HILOGD("RemoteRequest handler uid:%{public}d taskId:%{public}d code:%{public}d tid:%{public}" PRId64 ""
        " stid:%{public}" PRId64 "", uid, taskId, code, GetThisThreadId(), stid);
    (*callback)(data, reply);
}

void RemoteMsgHandler::OnAllocSocketFd(MessageParcel &data, MessageParcel &reply)
{
    CALL_LOG_ENTER;
    if (lastSession_ != nullptr) {
        MMI_HILOGE("Repeat request clientIdx:%{public}d", lastClientPid_);
        return;
    }
    CHKPV(multStub_);
    sptr<ConnectReqParcel> req = data.ReadParcelable<ConnectReqParcel>();
    CHKPV(req);
    MMI_HILOGD("clientName:%{public}s,moduleId:%{public}d", req->data.clientName.c_str(), req->data.moduleId);

    int32_t clientFd = -1;
    int32_t ret = multStub_->AllocSocketFd(req->data.clientName, req->data.moduleId, clientFd,
        lastClientPid_, lastClientUid_);
    if (ret != RET_OK) {
        MMI_HILOGE("call AddSocketPairInfo return %{public}d", ret);
        if (!reply.WriteInt32(RET_ERR)) {
            MMI_HILOGE("WriteInt32 RET_ERR fail");
        }
        return;
    }
    MMI_HILOGI("call AllocSocketFd success");

    if (!reply.WriteInt32(RET_OK)) {
        MMI_HILOGE("WriteInt32 RET_OK fail");
    }
    if (!reply.WriteFileDescriptor(clientFd)) {
        MMI_HILOGE("WriteFileDescriptor clientFd:%{public}d fail", clientFd);
    }
    MMI_HILOGI("send clientFd to client, clientFd = %{public}d", clientFd);
    close(clientFd);
}

void RemoteMsgHandler::OnAddInputEventFilter(MessageParcel& data, MessageParcel& reply)
{
    CALL_LOG_ENTER;
    CHKPV(lastSession_);
    CHKPV(multStub_);
    if (lastClientUid_ != SYSTEM_UID && lastClientUid_ != ROOT_UID) {
        MMI_HILOGE("check failed, uid is not root or system");
        return;
    }
    sptr<IRemoteObject> client = data.ReadRemoteObject();
    CHKPV(client);
    sptr<IEventFilter> filter = iface_cast<IEventFilter>(client);
    CHKPV(filter);
    MMI_HILOGD("filter iface_cast succeeded");
    int32_t ret = multStub_->AddInputEventFilter(filter);
    if (!reply.WriteInt32(ret)) {
        MMI_HILOGE("WriteInt32 ret:%{public}d fail", ret);
        return;
    }
}

void RemoteMsgHandler::StubSetPointerVisible(MessageParcel& data, MessageParcel& reply)
{
    CALL_LOG_ENTER;
    CHKPV(lastSession_);
    CHKPV(multStub_);
    bool visible;
    if (!data.ReadBool(visible)) {
        MMI_HILOGE("data ReadBool fail");
        return;
    }
    int32_t ret = multStub_->SetPointerVisible(visible);
    if (!reply.WriteInt32(ret)) {
        MMI_HILOGE("WriteInt32:%{public}d fail", ret);
        return;
    }
}
} // namespace MMI
} // namespace OHOS