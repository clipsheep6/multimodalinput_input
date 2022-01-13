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

#include "mmi_client.h"
#include "log.h"
#include "multimodal_event_handler.h"
#include "multimodal_input_connect_manager.h"
#include "proto.h"
#include "util.h"

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MMIClient" };
    }
}

OHOS::MMI::MMIClient::MMIClient()
{
}

OHOS::MMI::MMIClient::~MMIClient()
{
    MMI_LOGD("enter");
}

bool OHOS::MMI::MMIClient::SendMessage(const NetPacket &pkt) const
{
    return SendMsg(pkt);
}

bool OHOS::MMI::MMIClient::GetCurrentConnectedStatus() const
{
    return GetConnectedStatus();
}

bool OHOS::MMI::MMIClient::Start(IClientMsgHandlerPtr msgHdl, bool detachMode)
{
    MMI_LOGT("enter");
    EventManager.SetClientHandle(GetPtr());
    CHKF(msgHdl->Init(), MSG_HANDLER_INIT_FAIL);
    auto msgHdlImp = static_cast<ClientMsgHandler *>(msgHdl.get());
    CHKF(msgHdlImp, MSG_HANDLER_INIT_FAIL);
    auto callback = std::bind(&ClientMsgHandler::OnMsgHandler, msgHdlImp, std::placeholders::_1, std::placeholders::_2);
    CHKF(StartClient(callback, detachMode), START_CLI_FAIL);

    return true;
}

void OHOS::MMI::MMIClient::RegisterConnectedFunction(ConnectCallback fun)
{
    funConnected_ = fun;
}

void OHOS::MMI::MMIClient::RegisterDisconnectedFunction(ConnectCallback fun)
{
    funDisconnected_ = fun;
}

void OHOS::MMI::MMIClient::VirtualKeyIn(RawInputEvent virtualKeyEvent)
{
    NetPacket ckt(MmiMessageId::ON_VIRTUAL_KEY);
    ckt << virtualKeyEvent;
    SendMsg(ckt);
}

void OHOS::MMI::MMIClient::ReplyMessageToServer(MmiMessageId idMsg, uint64_t clientTime, uint64_t endTime) const
{
    NetPacket ckt(MmiMessageId::CHECK_REPLY_MESSAGE);
    ckt << idMsg << clientTime << endTime;
    SendMsg(ckt);
}

void OHOS::MMI::MMIClient::SdkGetMultimodeInputInfo()
{
    TagPackHead tagPackHead = {MmiMessageId::GET_MMI_INFO_REQ, {0}};
    NetPacket ckt(MmiMessageId::GET_MMI_INFO_REQ);
    ckt << tagPackHead;
    SendMsg(ckt);
}

void OHOS::MMI::MMIClient::OnDisconnected()
{
    MMI_LOGD("Disconnected from server... fd:%{public}d", GetFd());
    if (funDisconnected_) {
        funDisconnected_(*this);
    }
    isConnected_ = false;
    EventManager.ClearAll();
}

void OHOS::MMI::MMIClient::OnConnected()
{
    MMI_LOGD("Connection to server succeeded... fd:%{public}d", GetFd());
    if (funConnected_) {
        funConnected_(*this);
    }
    isConnected_ = true;
}

int32_t OHOS::MMI::MMIClient::Socket()
{
    MMI_LOGT("enter");
    const int32_t ret = MultimodalInputConnectManager::GetInstance()->
                        AllocSocketPair(IMultimodalInputConnect::CONNECT_MODULE_TYPE_MMI_CLIENT);
    if (ret != RET_OK) {
        MMI_LOGE("UDSSocket::Socket, call MultimodalInputConnectManager::AllocSocketPair return %{public}d", ret);
    }
    fd_ = MultimodalInputConnectManager::GetInstance()->GetClientSocketFdOfAllocedSocketPair();
    if (fd_ == IMultimodalInputConnect::INVALID_SOCKET_FD) {
        MMI_LOGE("UDSSocket::Socket, call MultimodalInputConnectManager::GetClientSocketFdOfAllocedSocketPair"
                 " return invalid fd.");
    } else {
        MMI_LOGT("UDSSocket::Socket, call MultimodalInputConnectManager::GetClientSocketFdOfAllocedSocketPair"
                 " return fd = %{public}d.", fd_);
    }

    return fd_;
}

