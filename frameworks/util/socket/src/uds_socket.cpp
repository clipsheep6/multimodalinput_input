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
#include "uds_socket.h"
#include "log.h"

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "UDSSocket" };
    }
}

OHOS::MMI::UDSSocket::UDSSocket()
{
}

OHOS::MMI::UDSSocket::~UDSSocket()
{
    Close();
}

int32_t OHOS::MMI::UDSSocket::Socket(int32_t domain, int32_t type)
{
    fd_ = socket(domain, type, 0);
    if (fd_ < 0) {
        MMI_LOGE("UDSSocket::Socket socket return %{public}d", fd_);
    }
    return fd_;
}

int32_t OHOS::MMI::UDSSocket::Close()
{
    int rf = RET_OK;
    if (fd_ >= 0) {
        rf = close(fd_);
        if (rf > 0) {
            MMI_LOGE("Socket close failed rf:%{public}d", rf);
        }
    }
    fd_ = -1;
    return rf;
}

int32_t OHOS::MMI::UDSSocket::Bind(const std::string& path)
{
    int32_t ret = 0;
    const size_t cmdStrSize = 520;
    char cmdStr[cmdStrSize] = {0};
    if (access(DEF_MMI_DATA_ROOT, 0) != 0) {
        if (sprintf_s(cmdStr, cmdStrSize, "mkdir -p %s", DEF_MMI_DATA_ROOT) == -1) {
            MMI_LOGE("in %{public}s, sprintf_s error cmd: mkdir -p %{public}s", __func__, DEF_MMI_DATA_ROOT);
            return RET_ERR;
        }
        system(cmdStr);
    }

    sockaddr_un addr = {};
    addr.sun_family = AF_LOCAL;
    if (path.empty()) {
        ret = sprintf_s(addr.sun_path, sizeof(addr.sun_path), "%s%llu.skd", 
            DEF_MMI_DATA_ROOT, getpid());
    } else {
        ret = sprintf_s(addr.sun_path, sizeof(addr.sun_path), "%s", path.c_str());
    }
    CHKR(ret >= 0, SPRINTF_S_SEC_FUN_FAIL, RET_ERR);
    bindPath_ = addr.sun_path;
    auto len = static_cast<socklen_t>(offsetof(struct sockaddr_un, sun_path) + strlen(addr.sun_path));
    unlink(addr.sun_path);
    ret = bind(fd_, reinterpret_cast<const sockaddr *>(&addr), len);
    if (ret < 0) {
        Close();
        MMI_LOGE("UDSSocket::Bind bind return %{public}d", ret);
    }
    return ret;
}

int32_t OHOS::MMI::UDSSocket::Connect(const std::string& path)
{
    CHKR(!path.empty(), PARAM_INPUT_INVALID, RET_ERR);
    sockaddr_un addr = {};
    addr.sun_family = AF_LOCAL;
    CHKR(sprintf_s(addr.sun_path, sizeof(addr.sun_path), "%s", path.c_str()) >= 0, SPRINTF_S_SEC_FUN_FAIL, RET_ERR);
    connPath_ = addr.sun_path;
    auto len = static_cast<socklen_t>(sizeof(addr));
    auto ret = connect(fd_, reinterpret_cast<struct sockaddr *>(&addr), len);
    if (ret < 0) {
        Close();
        MMI_LOGE("UDSSocket::Connect connect return %{public}d path:%{public}s", ret, addr.sun_path);
    }
    return ret;
}

int32_t OHOS::MMI::UDSSocket::Listen(int32_t n)
{
    CHKR(fd_ >= 0, VAL_NOT_EXP, RET_ERR);
    auto ret = listen(fd_, n);
    if (ret < 0) {
        Close();
        MMI_LOGE("UDSSocket::Listen listen return %{public}d size:%{public}d", ret, n);
    }
    return ret;
}

int32_t OHOS::MMI::UDSSocket::Accept(struct sockaddr_un& addr)
{
    auto len = static_cast<socklen_t>(sizeof(addr));
    auto newFd = accept(fd_, reinterpret_cast<struct sockaddr *>(&addr), &len);
    auto addrLen = strlen(addr.sun_path);
    if (newFd < 0 || addrLen <= 0) {
        MMI_LOGE("UDSSocket::Accept accept return %{public}d addrLen:%{public}d", newFd, addrLen);
        Close();
        return RET_ERR;
    }
    return newFd;
}

size_t OHOS::MMI::UDSSocket::Read(char *buf, size_t size)
{
    CHKR(buf, NULL_POINTER, -1);
    CHKR(size > 0, PARAM_INPUT_INVALID, -1);
    CHKR(fd_ >= 0, PARAM_INPUT_INVALID, -1);
    auto ret = read(fd_, static_cast<void *>(buf), size);
    if (ret < 0) {
        MMI_LOGE("UDSSocket::Read read return %{public}d", ret);
    }
    return ret;
}

size_t OHOS::MMI::UDSSocket::Write(const char *buf, size_t size)
{
    CHKR(buf, NULL_POINTER, -1);
    CHKR(size > 0, PARAM_INPUT_INVALID, -1);
    CHKR(fd_ >= 0, PARAM_INPUT_INVALID, -1);
    auto ret = write(fd_, buf, size);
    if (ret < 0) {
        MMI_LOGE("UDSSocket::Write write return %{public}d", ret);
    }
    return ret;
}

size_t OHOS::MMI::UDSSocket::Send(const char *buf, size_t size, int32_t flags)
{
    CHKR(buf, NULL_POINTER, -1);
    CHKR(size > 0, PARAM_INPUT_INVALID, -1);
    auto ret = send(fd_, buf, size, flags);
    if (ret < 0) {
        MMI_LOGE("UDSSocket::Send send return %{public}d", ret);
    }
    return ret;
}

size_t OHOS::MMI::UDSSocket::Recv(char *buf, size_t size, int32_t flags)
{
    CHKR(buf, NULL_POINTER, -1);
    CHKR(size > 0, PARAM_INPUT_INVALID, -1);
    auto ret = recv(fd_, static_cast<void *>(buf), size, flags);
    if (ret < 0) {
        MMI_LOGE("UDSSocket::Recv recv return %{public}d", ret);
    }
    return ret;
}

size_t OHOS::MMI::UDSSocket::Recvfrom(char *buf, size_t size, uint32_t flags, sockaddr *addr, size_t *addrlen)
{
    CHKR(buf, NULL_POINTER, -1);
    CHKR(size > 0, PARAM_INPUT_INVALID, -1);
    CHKR(fd_ >= 0, PARAM_INPUT_INVALID, -1);
    auto ret = recvfrom(fd_, static_cast<void *>(buf), size, flags, addr, reinterpret_cast<socklen_t *>(addrlen));
    if (ret < 0) {
        MMI_LOGE("UDSSocket::Recvfrom recvfrom return %{public}d", ret);
    }
    return ret;
}

size_t OHOS::MMI::UDSSocket::Sendto(const char *buf, size_t size, uint32_t flags, sockaddr *addr, size_t addrlen)
{
    CHKR(buf, NULL_POINTER, -1);
    CHKR(size > 0, PARAM_INPUT_INVALID, -1);
    CHKR(fd_ >= 0, PARAM_INPUT_INVALID, -1);
    auto ret = sendto(fd_, static_cast<const void *>(buf), size, flags, addr, static_cast<socklen_t>(addrlen));
    if (ret < 0) {
        MMI_LOGE("UDSSocket::Sendto sendto return %{public}d", ret);
    }
    return ret;
}

int32_t OHOS::MMI::UDSSocket::EpollCreat(int32_t size)
{
    epollFd_ = epoll_create(size);
    if (epollFd_ < 0) {
        MMI_LOGE("UDSSocket::EpollCreat epoll_create retrun %{public}d", epollFd_);
    }
    return epollFd_;
}

int32_t OHOS::MMI::UDSSocket::EpollCtl(int32_t fd, int32_t op, epoll_event& event)
{
    CHKR(epollFd_ >= 0, PARAM_INPUT_INVALID, RET_ERR);
    CHKR(fd >= 0, PARAM_INPUT_INVALID, RET_ERR);
    auto ret = epoll_ctl(epollFd_, op, fd, &event);
    if (ret < 0) {
        const int errnoSaved = errno;
        MMI_LOGE("UDSSocket::EpollCtl epoll_ctl retrun %{public}d epollFd_:%{public}d, op:%{public}d fd:%{public}d errno:%{public}d error msg: %{public}s",
            ret, epollFd_, op, fd, errnoSaved, strerror(errnoSaved));
    }
    return ret;
}

int32_t OHOS::MMI::UDSSocket::EpollWait(epoll_event& events, int32_t maxevents, int32_t timeout)
{
    CHKR(epollFd_ >= 0, PARAM_INPUT_INVALID, RET_ERR);
    auto ret = epoll_wait(epollFd_, &events, maxevents, timeout);
    if (ret < 0) {
        MMI_LOGE("UDSSocket::EpollWait epoll_wait retrun %{public}d", ret);
    }
    return ret;
}

void OHOS::MMI::UDSSocket::EpollClose()
{
    if (epollFd_ >= 0) {
        close(epollFd_);
        epollFd_ = -1;
    }
}
