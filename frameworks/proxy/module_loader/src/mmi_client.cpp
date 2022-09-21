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
const std::string THREAD_NAME = "mmi_EventHdr";
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

void MMIClient::SetEventHandler(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler)
{
    CHKPV(inputEventConsumer);
    CHKPV(eventHandler);
    std::lock_guard<std::mutex> guard(mtx_);
    inputEventConsumer_ = inputEventConsumer;
    eventHandler_ = eventHandler;
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
    std::lock_guard<std::mutex> guard(mtx_);
    if (eventHandler_ == nullptr) {
        auto runner = AppExecFwk::EventRunner::Create(THREAD_NAME);
        eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
        CHKPF(eventHandler_);
    }

    if (isConnected_ && fd_ >= 0) {
        if (isListening_) {
            MMI_HILOGI("File fd is in listening");
            return true;
        }
        if (!AddFdListener(fd_)) {
            MMI_HILOGE("Add fd listener failed");
            return false;
        }
        msgHandler_.SetInputConsumer(inputEventConsumer_);
    } else {
        if (!eventHandler_->PostTask(std::bind(&MMIClient::OnReconnect, this), CLIENT_RECONNECT_COOLING_TIME)) {
            MMI_HILOGE("Send reconnect event failed");
            return false;
        }
    }
    return true;
}

void MMIClient::StopOldEventHandler(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler)
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    CHKPV(eventHandler);
    CHKPV(eventHandler_);
    auto currentRunner = eventHandler_->GetEventRunner();
    CHKPV(currentRunner);
    MMI_HILOGI("Current thread name:%{public}s", currentRunner->GetRunnerThreadName().c_str());
    auto newRunner = eventHandler->GetEventRunner();
    CHKPV(newRunner);
    MMI_HILOGI("New thread name:%{public}s", newRunner->GetRunnerThreadName().c_str());
    if (currentRunner->GetThreadId() == newRunner->GetThreadId()) {
        MMI_HILOGI("The eventRunners are the same");
        return;
    }

    if (fd_ >= 0) {
        eventHandler_->RemoveFileDescriptorListener(fd_);
    }

    if (currentRunner->GetRunnerThreadName() == THREAD_NAME) {
        currentRunner->Stop();
    }

    if (!eventHandler->PostImmediateTask(std::bind(&MMIClient::StartNewEventHandler, this, inputEventConsumer, eventHandler))) {
        MMI_HILOGE("Send new eventHandler task failed");
        return;
    }
}

void MMIClient::StartNewEventHandler(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler)
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    eventHandler_ = eventHandler;
    if (isConnected_ && fd_ >= 0) {
        if (!AddFdListener(fd_)) {
            MMI_HILOGE("Add new fd listener failed");
            return;
        }
        msgHandler_.SetInputConsumer(inputEventConsumer);
    } else {
        if (!eventHandler_->PostTask(std::bind(&MMIClient::OnReconnect, this), CLIENT_RECONNECT_COOLING_TIME)) {
            MMI_HILOGE("Send new reconnect event failed");
            return;
        }
    }
    MMI_HILOGI("Switch eventHandler successfully");
}

void MMIClient::SwitchEventHandler(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler)
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    CHKPV(inputEventConsumer);
    CHKPV(eventHandler);
    std::lock_guard<std::mutex> guard(mtx_);
    if (eventHandler_ != nullptr) {
        if (!eventHandler_->PostImmediateTask(std::bind(&MMIClient::StopOldEventHandler, this, inputEventConsumer, eventHandler))) {
            MMI_HILOGE("Send stop old eventHandler task failed");
            return;
        }
    }
}

bool MMIClient::AddFdListener(int32_t fd)
{
    CALL_DEBUG_ENTER;
    if (fd < 0) {
        MMI_HILOGE("Invalid fd:%{public}d", fd);
        return false;
    }
    CHKPF(eventHandler_);
    auto fdListener = std::make_shared<MMIFdListener>(GetSharedPtr());
    CHKPF(fdListener);
    auto errCode = eventHandler_->AddFileDescriptorListener(fd, FILE_DESCRIPTOR_INPUT_EVENT, fdListener);
    if (errCode != ERR_OK) {
        MMI_HILOGE("Add fd listener failed,fd:%{public}d code:%{public}u str:%{public}s", fd, errCode,
            GetErrorStr(errCode).c_str());
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
    CHKPF(eventHandler_);
    auto runner = eventHandler_->GetEventRunner();
    CHKPF(runner);
    if (runner->GetRunnerThreadName() == THREAD_NAME) {
        eventHandler_->RemoveAllEvents();
        eventHandler_->RemoveFileDescriptorListener(fd);
    }
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

void MMIClient::OnReconnect()
{
    if (Reconnect() == RET_OK) {
        return;
    }
    CHKPV(eventHandler_);
    if (!eventHandler_->PostTask(std::bind(&MMIClient::OnReconnect, this), CLIENT_RECONNECT_COOLING_TIME)) {
        MMI_HILOGE("Post reconnect event failed");
    }
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
    if (!isExit && eventHandler_ != nullptr) {
        if (!eventHandler_->PostTask(std::bind(&MMIClient::OnReconnect, this), CLIENT_RECONNECT_COOLING_TIME)) {
            MMI_HILOGE("Send reconnect event task failed");
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
    if (!isExit && !isRunning_ && fd_ >= 0 && eventHandler_ != nullptr) {
        if (!AddFdListener(fd_)) {
            MMI_HILOGE("Add fd listener failed");
        }
        msgHandler_.SetInputConsumer(inputEventConsumer_);
        isListening_ = true;
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
    if (eventHandler_ != nullptr) {
        auto runner = eventHandler_->GetEventRunner();
        CHKPV(runner);
        if (runner->GetRunnerThreadName() == THREAD_NAME) {
            runner->Stop();
            eventHandler_->RemoveAllEvents();
            eventHandler_->RemoveAllFileDescriptorListeners();
        }
    }
}

const std::string& MMIClient::GetErrorStr(ErrCode code) const
{
    const static std::string defErrString = "Unknown event handler error!";
    const static std::map<ErrCode, std::string> mapStrings = {
        {ERR_OK, "ERR_OK."},
        {EVENT_HANDLER_ERR_INVALID_PARAM, "Invalid parameters"},
        {EVENT_HANDLER_ERR_NO_EVENT_RUNNER, "Have not set event runner yet"},
        {EVENT_HANDLER_ERR_FD_NOT_SUPPORT, "Not support to listen file descriptors"},
        {EVENT_HANDLER_ERR_FD_ALREADY, "File descriptor is already in listening"},
        {EVENT_HANDLER_ERR_FD_FAILED, "Failed to listen file descriptor"},
        {EVENT_HANDLER_ERR_RUNNER_NO_PERMIT, "No permit to start or stop deposited event runner"},
        {EVENT_HANDLER_ERR_RUNNER_ALREADY, "Event runner is already running"}
    };
    auto it = mapStrings.find(code);
    if (it != mapStrings.end()) {
        return it->second;
    }
    return defErrString;
}
} // namespace MMI
} // namespace OHOS
