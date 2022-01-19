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

#include "multimodal_input_connect_service.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.h"
#include "multimodal_input_connect_def_parcel.h"
#include "singleton.h"
#include "string_ex.h"
#include "system_ability.h"

namespace OHOS {
namespace MMI {
namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MultimodalInputConnectService" };
}
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<MultimodalInputConnectService>::GetInstance().get());

int32_t MultimodalInputConnectService::AllocSocketFd(const std::string &programName, const int moduleType,
                                                     int &toReturnClientFd)
{
    MMI_LOG_I("MultimodalInputConnectService::AllocSocketFd enter, programName: %{public}s, moduleType: %{public}d",
             programName.c_str(), moduleType);
    if (udsServer_ == nullptr) {
        MMI_LOG_E("called, udsServer_ is nullptr.");
        return RET_ERR;
    }
    toReturnClientFd = INVALID_SOCKET_FD;
    int serverFd = INVALID_SOCKET_FD;
    int32_t uid = GetCallingUid();
    int32_t pid = GetCallingPid();
    const int32_t ret = udsServer_->AddSocketPairInfo(programName, moduleType, serverFd, toReturnClientFd, uid, pid);
    if (ret != RET_OK) {
        MMI_LOG_E("call AddSocketPairInfo return %{public}d.", ret);
        return RET_ERR;
    }

    MMI_LOGIK("leave, programName: %{public}s, moduleType: %{public}d, alloc success.",
        programName.c_str(), moduleType);

    return RET_OK;
}

int32_t MultimodalInputConnectService::AddInputEventFilter(sptr<IEventFilter> filter)
{
    MMI_LOG_F("enter, this code is discarded, and it runs with Weston");
    return RET_ERR;
}

MultimodalInputConnectService::MultimodalInputConnectService()
    : SystemAbility(MULTIMODAL_INPUT_CONNECT_SERVICE_ID, true), state_(ServiceRunningState::STATE_NOT_START)
{
    MMI_LOG_I("MultimodalInputConnectService()");
}

MultimodalInputConnectService::~MultimodalInputConnectService()
{
    MMI_LOG_I("~MultimodalInputConnectService()");
}

void MultimodalInputConnectService::OnStart()
{
    MMI_LOG_T("enter");
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        MMI_LOG_I("MultimodalInputConnectService has already started!");
        return;
    }
    MMI_LOG_I("MultimodalInputConnectService is starting");
    if (!Initialize()) {
        MMI_LOG_E("Failed to initialize");
        return;
    }
    bool ret = Publish(DelayedSingleton<MultimodalInputConnectService>::GetInstance().get());
    if (!ret) {
        MMI_LOG_E("Failed to publish service!");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    MMI_LOGIK("Congratulations, MultimodalInputConnectService start successfully!");
}

void MultimodalInputConnectService::OnStop()
{
    MMI_LOG_T("enter");
    state_ = ServiceRunningState::STATE_NOT_START;
}

void MultimodalInputConnectService::OnDump()
{
    MMI_LOG_T("enter");
}

bool MultimodalInputConnectService::Initialize() const
{
    MMI_LOG_T("enter");
    return true;
}

int32_t MultimodalInputConnectService::HandleAllocSocketFd(MessageParcel& data, MessageParcel& reply)
{
    MMI_LOG_T("enter");
    int32_t ret;

    sptr<ConnectDefReqParcel> req = data.ReadParcelable<ConnectDefReqParcel>();
    if (req == nullptr) {
        MMI_LOG_E("read data error.");
        return RET_ERR;
    }

    MMI_LOGIK("clientName = %{public}s, moduleId = %{public}d", req->data.clientName.c_str(), req->data.moduleId);
    if (!IsAuthorizedCalling()) {
        MMI_LOG_E("permission denied");
        return RET_ERR;
    }

    if (udsServer_ == nullptr) {
        MMI_LOG_E("udsServer_ is nullptr.");
        return RET_ERR;
    }

    int clientFd = INVALID_SOCKET_FD;
    ret = AllocSocketFd(req->data.clientName, req->data.moduleId, clientFd);
    if (ret != RET_OK) {
        MMI_LOG_E("call AddSocketPairInfo return %{public}d.", ret);
        reply.WriteInt32(RET_ERR);
        return RET_ERR;
    }

    MMI_LOG_I("call AllocSocketFd success.");

    reply.WriteInt32(RET_OK);
    reply.WriteFileDescriptor(clientFd);

    MMI_LOG_I("send clientFd to client, clientFd = %d", clientFd);
    close(clientFd);
    clientFd = -1;
    MMI_LOG_I(" clientFd = %d, has closed in server", clientFd);

    return RET_OK;
}

void MultimodalInputConnectService::SetUdsServer(IUdsServer *server)
{
    MMI_LOG_T("enter");
    udsServer_ = server;
}

int32_t MultimodalInputConnectServiceSetUdsServer(IUdsServer* server)
{
    MMI_LOG_T("enter");
    auto s = DelayedSingleton<MultimodalInputConnectService>::GetInstance();
    if (s == nullptr) {
        MMI_LOG_E("MultimodalInputConnectService not initialize.");
        return RET_ERR;
    }

    s->SetUdsServer(server);
    return RET_OK;
}

int32_t MultimodalInputConnectServiceStart()
{
    MMI_LOG_T("enter");
    auto s = DelayedSingleton<MultimodalInputConnectService>::GetInstance();
    if (s == nullptr) {
        MMI_LOG_E("MultimodalInputConnectService not initialize.");
        return RET_ERR;
    }

    s->OnStart();

    return RET_OK;
}

int32_t MultimodalInputConnectServiceStop()
{
    MMI_LOG_T("enter");
    auto s = DelayedSingleton<MultimodalInputConnectService>::GetInstance();
    if (s == nullptr) {
        MMI_LOG_E("MultimodalInputConnectService not initialize.");
        return RET_ERR;
    }

    s->OnStop();

    return RET_OK;
}
} // namespace Security
} // namespace OHOS
