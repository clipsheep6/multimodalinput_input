/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mmi_client.h"

#include "anr_handler.h"
#include "mmi_fd_listener.h"
#include "multimodal_input_connect_manager.h"
#include "qos.h"
#include "parameters.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "MMIClient"

namespace OHOS {
namespace MMI {
namespace {
const std::string THREAD_NAME { "OS_mmi_EventHdr" };
static const bool USE_FILE_DESCRIPTION = system::GetBoolParameter("const.sys.param_file_description_monitor", false);
static const bool USE_ISOLATE_DISPATCH_THREAD =
    system::GetBoolParameter("const.multimodalinput.use_isolate_dispatch_thread", false);
} // namespace

using namespace AppExecFwk;
using namespace OHOS::QOS;
MMIClient::~MMIClient()
{
    CALL_DEBUG_ENTER;
    Stop();
}

void MMIClient::SetEventHandler(EventHandlerPtr eventHandler)
{
    if (eventHandler == nullptr) {
        MMI_HILOGW("eventHandler is null");
        return;
    }
    // use the new thread untill eventhandler use poll thread
    eventHandler_ = eventHandler;
}

void MMIClient::MarkIsEventHandlerChanged(EventHandlerPtr eventHandler)
{
    CHKPV(eventHandler);
    CHKPV(eventHandler_);
    auto currentRunner = eventHandler_->GetEventRunner();
    CHKPV(currentRunner);
    auto newRunner = eventHandler->GetEventRunner();
    CHKPV(newRunner);
    isEventHandlerChanged_ = false;
    if (currentRunner->GetRunnerThreadName() != newRunner->GetRunnerThreadName()) {
        isEventHandlerChanged_ = true;
        MMI_HILOGD("Event handler changed");
    }
    MMI_HILOGD("Current handler name:%{public}s, New handler name:%{public}s",
        currentRunner->GetRunnerThreadName().c_str(), newRunner->GetRunnerThreadName().c_str());
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
    CALL_INFO_TRACE;
    msgHandler_.Init();
    auto callback = [this] (const UDSClient& client, NetPacket& pkt) { return msgHandler_.OnMsgHandler(client, pkt); };
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
    MMI_HILOGD("Client started successfully");
    return true;
}

void MMIClient::SetScheduler()
{
    CALL_INFO_TRACE;
    SetThreadQos(QosLevel::QOS_USER_INTERACTIVE);
}

bool MMIClient::StartEventRunner()
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    bool selfCreateEventHandle = false;
    if (USE_ISOLATE_DISPATCH_THREAD || eventHandler_ == nullptr || !USE_FILE_DESCRIPTION) {
        MMI_HILOGI("USE_ISOLATE_DISPATCH_THREAD:%{public}d, eventHandler_:%{public}d",
            USE_ISOLATE_DISPATCH_THREAD, (eventHandler_ == nullptr));
        auto runner = AppExecFwk::EventRunner::Create(THREAD_NAME);
        eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
        eventHandler_->PostTask([this] { this->SetScheduler(); });
        selfCreateEventHandle = true;
    }

    auto runner = eventHandler_->GetEventRunner();
    MMI_HILOGI("Current event handler, thread name:%{public}s",
        runner->GetRunnerThreadName().c_str());
    if (isConnected_ && fd_ >= 0) {
        if (isListening_) {
            MMI_HILOGI("File fd is in listening");
            return true;
        }
        if (!AddFdListener(fd_, selfCreateEventHandle)) {
            MMI_HILOGE("Add fd listener failed");
            return false;
        }
    } else {
        if (!eventHandler_->PostTask([this] { return this->OnReconnect(); }, CLIENT_RECONNECT_COOLING_TIME)) {
            MMI_HILOGE("Send reconnect event failed");
            return false;
        }
    }
    return true;
}

bool MMIClient::AddFdListener(int32_t fd, bool selfCreate)
{
    CALL_DEBUG_ENTER;
    if (fd < 0) {
        MMI_HILOGE("Invalid fd:%{public}d", fd);
        return false;
    }
    CHKPF(eventHandler_);
    auto fdListener = std::make_shared<MMIFdListener>(GetSharedPtr());
    if (!USE_ISOLATE_DISPATCH_THREAD && !selfCreate && USE_FILE_DESCRIPTION) {
        fdListener->SetDeamonWaiter();
    }
    
    auto errCode = eventHandler_->AddFileDescriptorListener(fd, FILE_DESCRIPTOR_INPUT_EVENT, fdListener, "MMITask",
        AppExecFwk::EventQueue::Priority::VIP);
    if (errCode != ERR_OK) {
        MMI_HILOGE("Add fd listener failed,fd:%{public}d code:%{public}u str:%{public}s", fd, errCode,
            GetErrorStr(errCode).c_str());
        return false;
    }
    isRunning_ = true;
    MMI_HILOGI("Server was listening");
    return true;
}

bool MMIClient::DelFdListener(int32_t fd)
{
    CALL_DEBUG_ENTER;
    CHKPF(eventHandler_);
    if (fd >= 0) {
        eventHandler_->RemoveFileDescriptorListener(fd);
        MMI_HILOGI("Remove file descriptor listener success");
    } else {
        MMI_HILOGE("Invalid fd:%{public}d", fd);
    }
    eventHandler_->RemoveAllEvents();
    MMI_HILOGI("Remove all events success");
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
    OnReadPackets(circBuf_, [this] (NetPacket& pkt) { return this->OnPacket(pkt); });
}

int32_t MMIClient::Reconnect()
{
    if (ConnectTo() != RET_OK) {
        MMI_HILOGE("Client connection failed");
        return RET_ERR;
    }
    if (funConnected_) {
        funConnected_(*this);
    }
    return RET_OK;
}

void MMIClient::OnReconnect()
{
    if (Reconnect() == RET_OK) {
        MMI_HILOGI("Reconnect ok");
        return;
    }
    CHKPV(eventHandler_);
    if (!eventHandler_->PostTask([this] { return this->OnReconnect(); }, CLIENT_RECONNECT_COOLING_TIME)) {
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
    isListening_ = false;
    ANRHDL->ResetAnrArray();
    if (funDisconnected_) {
        funDisconnected_(*this);
    }
    if (!DelFdListener(fd_)) {
        MMI_HILOGE("Delete fd listener failed");
    }
    Close();
    if (!isExit && eventHandler_ != nullptr) {
        if (!eventHandler_->PostTask([this] { return this->OnReconnect(); }, CLIENT_RECONNECT_COOLING_TIME)) {
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
    if (!isExit && !isRunning_ && fd_ >= 0 && eventHandler_ != nullptr) {
        if (!AddFdListener(fd_)) {
            MMI_HILOGE("Add fd listener failed");
            return;
        }
        isListening_ = true;
    }
}

int32_t MMIClient::Socket()
{
    CALL_DEBUG_ENTER;
    int32_t ret =
        MULTIMODAL_INPUT_CONNECT_MGR->AllocSocketPair(MultimodalInputConnectManager::CONNECT_MODULE_TYPE_MMI_CLIENT);
    if (ret != RET_OK) {
        MMI_HILOGE("Call AllocSocketPair return %{public}d", ret);
        return MultimodalInputConnectManager::INVALID_SOCKET_FD;
    }
    fd_ = MULTIMODAL_INPUT_CONNECT_MGR->GetClientSocketFdOfAllocedSocketPair();
    if (fd_ == MultimodalInputConnectManager::INVALID_SOCKET_FD) {
        MMI_HILOGE("Call GetClientSocketFdOfAllocedSocketPair return invalid fd");
    } else {
        MMI_HILOGD("Call GetClientSocketFdOfAllocedSocketPair return fd:%{public}d", fd_);
    }
    fdsan_exchange_owner_tag(fd_, 0, TAG);
    return fd_;
}

EventHandlerPtr MMIClient::GetEventHandler() const
{
    CHKPP(eventHandler_);
    return eventHandler_;
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
            MMI_HILOGI("Remove all file descriptor listeners success");
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
