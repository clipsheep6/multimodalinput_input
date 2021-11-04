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
#include "libmmi_st_client.h"
#include "util.h"
#include "log.h"
#include "proto.h"
#include "st_msg_handler.h"
#include "multimodal_input_connect_manager.h"

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "STClient" };
    }
}

OHOS::MMI::STClient::STClient()
{
    MMI_LOGD("enter");
}

OHOS::MMI::STClient::~STClient()
{
    MMI_LOGD("enter");
}

bool OHOS::MMI::STClient::GetCurrentConnectedStatus() const
{
    return GetConnectedStatus();
}

bool OHOS::MMI::STClient::Start(IClientMsgHandlerPtr msgHdl, bool detachMode)
{
    auto msgHdlImp = static_cast<OHOS::MMI::STMsgHandler*>(msgHdl.get());
    CHKF(msgHdlImp, MSG_HANDLER_INIT_FAIL)
    auto callback = std::bind(&OHOS::MMI::STMsgHandler::OnMsgHandler, msgHdlImp, std::placeholders::_1, std::placeholders::_2);
    CHKF(StartClient(callback, detachMode), START_CLI_FAIL);
    
    return true;
}

void OHOS::MMI::STClient::RegisterConnectedFunction(ConnectCallback fun)
{
    funConnected_ = fun;
}

void OHOS::MMI::STClient::RegisterDisconnectedFunction(ConnectCallback fun)
{
    funDisconnected_ = fun;
}

void OHOS::MMI::STClient::OnDisconnected()
{
    MMI_LOGD("Disconnected from server... fd:%{public}d", GetFd());
    if (funDisconnected_) {
        funDisconnected_(*this);
    }
    isConnected_ = false;
}

void OHOS::MMI::STClient::OnConnected()
{
    MMI_LOGD("Connection to server succeeded... fd:%{public}d", GetFd());
    if (funConnected_) {
        funConnected_(*this);
    }
    isConnected_ = true;
}

int32_t OHOS::MMI::STClient::Socket()
{
    MMI_LOGT("enter");
    const int32_t ret = MultimodalInputConnectManager::GetInstance()->AllocSocketPair(IMultimodalInputConnect::CONNECT_MODULE_TYPE_ST_TEST);
    if(ret != RET_OK){
        MMI_LOGE("UDSSocket::Socket, call MultimodalInputConnectManager::AllocSocketPair return %{public}d",ret);
    }
    fd_ = MultimodalInputConnectManager::GetInstance()->GetClientSocketFdOfAllocedSocketPair();
    if(fd_ == IMultimodalInputConnect::INVALID_SOCKET_FD){
        MMI_LOGE("UDSSocket::Socket, call MultimodalInputConnectManager::GetClientSocketFdOfAllocedSocketPair return invalid fd.");
    } else{
        MMI_LOGT("UDSSocket::Socket, call MultimodalInputConnectManager::GetClientSocketFdOfAllocedSocketPair return fd = %{public}d.",fd_);
    }

    return fd_;
}

bool OHOS::MMI::STClient::SendMessage(const NetPacket& pkt) const
{
    return SendMsg(pkt);
}

