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
#include "uds_server.h"
#include "safe_keeper.h"
#include "util.h"
#include "util_ex.h"
#include "uds_command_queue.h"

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "UDSServer" };
    }
}

OHOS::MMI::UDSServer::UDSServer()
{
}

OHOS::MMI::UDSServer::~UDSServer()
{
    MMI_LOGD("enter");
    Stop();
}

int32_t OHOS::MMI::UDSServer::Init(const std::string& path)
{
    CHKR(!path.empty(), SOCKET_PATH_INVALID, SOCKET_PATH_INVALID);
    CHKR(Socket(AF_LOCAL, SOCK_STREAM) >= 0, SOCKET_CREATE_FAIL, SOCKET_CREATE_FAIL);
    CHKR(Bind(path.c_str()) >= 0, SOCKET_BIND_FAIL, SOCKET_BIND_FAIL);
    CHKR(Listen(MAX_LIST_SIZE) >= 0, SOCKET_LISTEN_FAIL, SOCKET_LISTEN_FAIL);
    if (EpollCreat(MAX_EVENT_SIZE) < 0) {
        Close();
        MMI_LOGE("Epoll creation failed... errCode:%{public}d", EPOLL_CREATE_FAIL);
        return EPOLL_CREATE_FAIL;
    }

    epoll_event ev = {};
    ev.events = EPOLLIN;
    ev.data.fd = fd_;
    if (EpollCtl(fd_, EPOLL_CTL_ADD, ev) < 0) {
        Close();
        MMI_LOGE("Epoll ctrl add failed... errCode:%{public}d", EPOLL_MODIFY_FAIL);
        return EPOLL_MODIFY_FAIL;
    }
    return RET_OK;
}

void OHOS::MMI::UDSServer::Stop()
{
    LOCKGUARD(mux_);
    Close();
    isRun_ = false;
    if (epollFd_ != -1) {
        close(epollFd_);
        epollFd_ = -1;
    }
    for (auto it : sessionsMap_) {
        it.second->Close();
    }
    sessionsMap_.clear();
    if (t_.joinable()) {
        t_.join();
    }
}

bool OHOS::MMI::UDSServer::SendMsg(int32_t fd, NetPacket& pkt)
{
    LOCKGUARD(mux_);
    CHKF(fd >= 0, PARAM_INPUT_INVALID);
    auto ses = GetSession(fd);
    if (!ses) {
        MMI_LOGE("SendMsg fd:%{public}d not found, The message was discarded! errCode:%{public}d",
                 fd, SESSION_NOT_FOUND);
        return false;
    }
    return ses->SendMsg(pkt);
}

void OHOS::MMI::UDSServer::Broadcast(NetPacket& pkt)
{
    LOCKGUARD(mux_);
    for (auto it : sessionsMap_) {
        it.second->SendMsg(pkt);
    }
}

void OHOS::MMI::UDSServer::Multicast(const IdsList& fdList, NetPacket& pkt)
{
    for (auto it : fdList) {
        SendMsg(it, pkt);
    }
}

void OHOS::MMI::UDSServer::Dump(int32_t fd)
{
    LOCKGUARD(mux_);
    mprintf(fd, "Sessions: count=%d", sessionsMap_.size());
    std::string strTmp;
    if (sessionsMap_.size() > 0) {
        strTmp = "fds:[";
    }
    for (auto& it : sessionsMap_) {
        strTmp += std::to_string(it.second->GetFd()) + ",";
    }
    if (sessionsMap_.size() > 0) {
        strTmp.resize(strTmp.size()-1);
        strTmp += "]";
    }
    mprintf(fd, "\t%s", strTmp.c_str());
}

void OHOS::MMI::UDSServer::OnConnected(SessionPtr s)
{
    MMI_LOGI("UDSServer::OnConnected addr:%{public}s fd:%{public}d", s->GetAddr()->sun_path, s->GetFd());
}

void OHOS::MMI::UDSServer::OnDisconnected(SessionPtr s)
{
    MMI_LOGI("UDSServer::OnDisconnected addr:%{public}s fd:%{public}d", s->GetAddr()->sun_path, s->GetFd());
}

void OHOS::MMI::UDSServer::SetRecvFun(MsgServerFunCallback fun)
{
    recvFun_ = fun;
}

bool OHOS::MMI::UDSServer::StartServer()
{
    isRun_ = true;
    t_ = std::thread(std::bind(&UDSServer::OnThread, this));
    t_.detach();
    return true;
}

OHOS::MMI::SessionPtr OHOS::MMI::UDSServer::OnAccept()
{
    struct sockaddr_un addr = {};
    auto newFd = Accept(addr);
    if (newFd < 0) {
        MMI_LOGE("Accept return %{public}d errCode:%{public}d", newFd, FD_ACCEPT_FAIL);
        return nullptr;
    }
    int32_t ret = 0;
    epoll_event nev = {};
    nev.events = EPOLLIN;
    nev.data.fd = newFd;
    if ((ret = EpollCtl(newFd, EPOLL_CTL_ADD, nev)) < 0) {
        close(newFd);
        MMI_LOGE("epoll_ctl EPOLL_CTL_ADD return %{public}d errCode:%{public}d", ret, EPOLL_MODIFY_FAIL);
        return nullptr;
    }
    return std::make_shared<UDSSession>(newFd, addr);
}

void OHOS::MMI::UDSServer::OnRecv(int32_t fd, const char *buf, size_t size)
{
    CHK(fd >= 0 && buf, PARAM_INPUT_INVALID);
    auto sess = GetSession(fd);
    CHK(sess, NULL_POINTER);
    int32_t readIdx = 0;
    int32_t packSize = 0;
    const auto headSize = static_cast<int32_t>(sizeof(PackHead));
    CHK(size >= headSize, VAL_NOT_EXP);
    while (size > 0 && recvFun_) {
        CHK(size >= headSize, VAL_NOT_EXP);
        auto head = (PackHead *)&buf[readIdx];
        CHK(head->size[0] < static_cast<int32_t>(size), VAL_NOT_EXP);
        packSize = headSize + head->size[0];

        NetPacket pkt(head->idMsg);
        if (head->size[0] > 0) {
            CHK(pkt.Write(&buf[readIdx + headSize], head->size[0]), STREAM_BUF_WRITE_FAIL);
        }
        recvFun_(sess, pkt);
        size -= packSize;
        readIdx += packSize;
    }
}

void OHOS::MMI::UDSServer::OnEvent(const epoll_event& ev, CLMAP<int32_t, StreamBufData>& bufMap)
{
    const int32_t maxCount = static_cast<int32_t>(MAX_STREAM_BUF_SIZE / MAX_PACKET_BUF_SIZE) + 1;
    CHK(maxCount > 0, VAL_NOT_EXP);
    char szBuf[MAX_PACKET_BUF_SIZE] = {};
    auto fd = ev.data.fd;
    if ((ev.events & EPOLLERR) || (ev.events & EPOLLHUP)) {
        auto secPtr = GetSession(fd);
        if (secPtr) {
            OnDisconnected(secPtr);
            DelSession(fd);
        }
        close(fd);
        return;
    }

    if (fd == fd_) {
        auto ptr = OnAccept();
        if (!ptr) {
            MMI_LOGE("Failed to accept client connection... errCode:%{public}d", NULL_POINTER);
            return;
        }
        OnConnected(ptr);
        CHK(AddSession(ptr), SESSION_ADD_FAIL);
    } else if (ev.events & EPOLLIN) {
        auto bufData = &bufMap[fd];
        if (bufData->isOverflow) {
            return;
        }
        for (auto j = 0; j < maxCount; j++) {
            auto size = read(fd, (void*)szBuf, MAX_PACKET_BUF_SIZE);
            if (size > 0 && bufData->sBuf.Write(szBuf, size) == false) {
                bufData->isOverflow = true;
                break;
            }
            if (size < MAX_PACKET_BUF_SIZE) {
                break;
            }
        }
    }
}

void OHOS::MMI::UDSServer::DumpSession(const char* title)
{
    printf("in %s: %s\n", __func__, title);
    int i = 0;
    for (auto& r : sessionsMap_) {
        printf("%d, fd: %d --- session: %p\n", i, r.first, r.second.get());
        i++;
    }
}

bool OHOS::MMI::UDSServer::AddSession(SessionPtr ses)
{
    CHKF(ses, OHOS::NULL_POINTER);
    auto fd = ses->GetFd();
    CHKF(fd >= 0, VAL_NOT_EXP);
    sessionsMap_[fd] = ses;
    return true;
}

void OHOS::MMI::UDSServer::DelSession(int32_t fd)
{
    sessionsMap_.erase(fd);
    DumpSession("DelSession");
}

void OHOS::MMI::UDSServer::OnThread()
{
    auto tid = GetThisThreadIdOfLL();
    CHK(tid > 0, VAL_NOT_EXP);
    MMI_LOGI("begin... fd:%{public}d tid:%{public}llu", fd_, tid);
    SafeKpr->RegisterEvent(tid, "UDSServer::_OnThread");

    CLMAP<int32_t, StreamBufData> bufMap;
    epoll_event ev[MAX_EVENT_SIZE] = {};
    while (isRun_) {
        auto count = EpollWait(*ev, MAX_EVENT_SIZE, DEFINE_EPOLL_TIMEOUT);
        if (count > 0) {
            bufMap.clear();
            for (auto i = 0; i < count; i++) {
                OnEvent(ev[i], bufMap);
            }
            for (auto& it : bufMap) {
                if (it.second.isOverflow) {
                    continue;
                }
                OnRecv(it.first, it.second.sBuf.Data(), it.second.sBuf.Size());
            }
        }
#ifdef OHOS_BUILD_MMI_DEBUG
        HandleCommandQueue();
#endif // OHOS_BUILD_MMI_DEBUG
        SafeKpr->ReportHealthStatus(tid);
    }
    MMI_LOGI("end");
}

#ifdef OHOS_BUILD_MMI_DEBUG
void OHOS::MMI::UDSServer::HandleCommandQueue()
{
    UdsCommandQueue &commandQueue = Singleton<UdsCommandQueue>::GetInstance();
    if (commandQueue.GetSize() == 0) {
        return;
    }
    const int maxProcessTenCommandPerOneTime = 10;
    for (int i = 0; i < maxProcessTenCommandPerOneTime; ++i) {
        std::string command = commandQueue.PopCommand();
        if (command.empty()) {
            continue;
        }
        if (command == "dump session") {
            DumpSession("** dump sesion**\n");
        }
    }
}
#endif // OHOS_BUILD_MMI_DEBUG

