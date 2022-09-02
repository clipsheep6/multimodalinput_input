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
#include "mmi_client.h"

#include <cinttypes>
#include <condition_variable>

#include "mmi_log.h"
#include "proto.h"
#include "util.h"

#include "input_manager_impl.h"
#include "mmi_fd_listener.h"
#include "multimodal_event_handler.h"
#include "multimodal_input_connect_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MMIClient" };
} // namespace

using namespace AppExecFwk;
MMIClient::MMIClient()
{
    CALL_DEBUG_ENTER;
}

MMIClient::~MMIClient()
{
    CALL_DEBUG_ENTER;
    Stop();
}

bool MMIClient::SendMessage(const NetPacket &pkt) const
{
    return SendMsg(pkt);
}

bool MMIClient::GetCurrentConnectedStatus() const
{
    return GetConnectedStatus();
}

MMIClientPtr MMIClient::GetSharedPtr()
{
    return shared_from_this();
}

bool MMIClient::Start()
{
    CALL_DEBUG_ENTER;
    msgHandler_.Init();
    EventManager.SetClientHandle(GetSharedPtr());
    auto callback = std::bind(&ClientMsgHandler::OnMsgHandler, &msgHandler_,
        std::placeholders::_1, std::placeholders::_2);
    if (!StartClient(callback)) {
        MMI_HILOGE("Client startup failed");
        Stop();
        return false;
    }
    if (!StartEventRunner()) {
        MMI_HILOGE("Start runner failed");
        Stop();
        return false;
    }
    MMI_HILOGI("Client started successfully");
    return true;
}

bool MMIClient::StartEventRunner()
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    if (!InputMgrImpl.InitEventHandler()) {
        MMI_HILOGE("Init event handler failed");
        Stop();
        return false;
    }

    static constexpr int32_t outTime = 3;
    std::unique_lock <std::mutex> lck(mtx_);
    recvThread_ = std::thread(std::bind(&MMIClient::OnRecvThread, this));
    recvThread_.detach();
    if (cv_.wait_for(lck, std::chrono::seconds(outTime)) == std::cv_status::timeout) {
        MMI_HILOGE("Recv thread start timeout");
        Stop();
        return false;
    }
    return true;
}

void MMIClient::OnRecvThread()
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    SetThreadName("mmi_client_RecvEventHdr");
    auto runner = EventRunner::Create(false);
    CHKPV(runner);
    {
        std::lock_guard<std::mutex> lck(mtx_);
        recvEventHandler_ = std::make_shared<MMIEventHandler>(runner, GetSharedPtr());
        CHKPV(recvEventHandler_);
        if (isConnected_ && fd_ >= 0) {
            if (!AddFdListener(fd_)) {
                MMI_HILOGE("Add fd listener failed");
                return;
            }
        } else {
            if (!recvEventHandler_->SendEvent(MMI_EVENT_HANDLER_ID_RECONNECT, 0, CLIENT_RECONNECT_COOLING_TIME)) {
                MMI_HILOGE("Send reconnect event failed");
                return;
            }
        }
        cv_.notify_one();
    }
    runner->Run();
}

bool MMIClient::AddFdListener(int32_t fd)
{
    CALL_DEBUG_ENTER;
    if (fd < 0) {
        MMI_HILOGE("Invalid fd:%{public}d", fd);
        return false;
    }
    CHKPF(recvEventHandler_);
    auto fdListener = std::make_shared<MMIFdListener>(GetSharedPtr());
    CHKPF(fdListener);
    auto errCode = recvEventHandler_->AddFileDescriptorListener(fd, FILE_DESCRIPTOR_INPUT_EVENT, fdListener);
    if (errCode != ERR_OK) {
        MMI_HILOGE("Add fd listener failed,fd:%{public}d code:%{public}u str:%{public}s", fd, errCode,
            recvEventHandler_->GetErrorStr(errCode).c_str());
        return false;
    }
    isRunning_ = true;
    MMI_HILOGI("serverFd:%{public}d was listening,mask:%{public}u pid:%{public}d threadId:%{public}" PRIu64,
        fd, FILE_DESCRIPTOR_INPUT_EVENT, GetPid(), GetThisThreadId());
    return true;
}

bool MMIClient::DelFdListener(int32_t fd)
{
    CALL_DEBUG_ENTER;
    if (fd < 0) {
        MMI_HILOGE("Invalid fd:%{public}d", fd);
        return false;
    }
    CHKPF(recvEventHandler_);
    recvEventHandler_->RemoveAllEvents();
    recvEventHandler_->RemoveFileDescriptorListener(fd);
    isRunning_ = false;
    return true;
}

void MMIClient::OnPacket(NetPacket& pkt)
{
    recvFun_(*this, pkt);
}

void MMIClient::OnRecvMsg(const char *buf, size_t size)
{
    CHKPV(buf);
    if (size == 0 || size > MAX_PACKET_BUF_SIZE) {
        MMI_HILOGE("Invalid input param size. size:%{public}zu", size);
        return;
    }
    if (!circBuf_.Write(buf, size)) {
        MMI_HILOGW("Write data failed. size:%{public}zu", size);
    }
    OnReadPackets(circBuf_, std::bind(&MMIClient::OnPacket, this, std::placeholders::_1));
}

int32_t MMIClient::Reconnect()
{
    return ConnectTo();
}

void MMIClient::OnDisconnect()
{
    OnDisconnected();
}

void MMIClient::RegisterConnectedFunction(ConnectCallback fun)
{
    funConnected_ = fun;
}

void MMIClient::RegisterDisconnectedFunction(ConnectCallback fun)
{
    funDisconnected_ = fun;
}

void MMIClient::OnDisconnected()
{
    CALL_DEBUG_ENTER;
    MMI_HILOGI("Disconnected from server, fd:%{public}d", fd_);
    isConnected_ = false;
    if (funDisconnected_) {
        funDisconnected_(*this);
    }
    if (!DelFdListener(fd_)) {
        MMI_HILOGE("Delete fd listener failed");
    }
    Close();
    if (!isExit && recvEventHandler_ != nullptr) {
        if (!recvEventHandler_->SendEvent(MMI_EVENT_HANDLER_ID_RECONNECT, 0, CLIENT_RECONNECT_COOLING_TIME)) {
            MMI_HILOGE("Send reconnect event return false");
        }
    }
}

void MMIClient::OnConnected()
{
    CALL_DEBUG_ENTER;
    MMI_HILOGI("Connection to server succeeded, fd:%{public}d", GetFd());
    isConnected_ = true;
    msgHandler_.InitProcessedCallback();
    if (funConnected_) {
        funConnected_(*this);
    }
    if (!isExit && !isRunning_ && fd_ >= 0 && recvEventHandler_ != nullptr) {
        if (!AddFdListener(fd_)) {
            MMI_HILOGE("Add fd listener failed");
        }
    }
}

int32_t MMIClient::Socket()
{
    CALL_DEBUG_ENTER;
    int32_t ret = MultimodalInputConnMgr->AllocSocketPair(IMultimodalInputConnect::CONNECT_MODULE_TYPE_MMI_CLIENT);
    if (ret != RET_OK) {
        MMI_HILOGE("Call AllocSocketPair return %{public}d", ret);
        return RET_ERR;
    }
    fd_ = MultimodalInputConnMgr->GetClientSocketFdOfAllocedSocketPair();
    if (fd_ == IMultimodalInputConnect::INVALID_SOCKET_FD) {
        MMI_HILOGE("Call GetClientSocketFdOfAllocedSocketPair return invalid fd");
    } else {
        MMI_HILOGI("Call GetClientSocketFdOfAllocedSocketPair return fd:%{public}d", fd_);
    }
    return fd_;
}

void MMIClient::Stop()
{
    CALL_DEBUG_ENTER;
    UDSClient::Stop();
    if (recvEventHandler_ != nullptr) {
        recvEventHandler_->SendSyncEvent(MMI_EVENT_HANDLER_ID_STOP, 0, EventHandler::Priority::IMMEDIATE);
    }
    auto eventHandler = InputMgrImpl.GetEventHandler();
    CHKPV(eventHandler);
    eventHandler->SendSyncEvent(MMI_EVENT_HANDLER_ID_STOP, 0, EventHandler::Priority::IMMEDIATE);
}
} // namespace MMI
} // namespace OHOS
