/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "container_ipc_manager.h"

#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "ContainerIpcMgr" };
constexpr char HMOS_SOCKET_PATH[] = "/data/virt_service/containerized_aosp/anco_android_run/hmos_event";
constexpr int MICRO_SECOND_INTERVAL = 5000;  // 5ms
constexpr int MAX_CLIENT_NUM = 1;
constexpr int MAX_EVENT_NUM = 1000;
constexpr int MAX_EVENT_DATA_LENGTH = 2044;
} // namespace

ContainerIpcMgr::ContainerIpcMgr() : serverFd_(-1), clientFd_(-1)
{
}

ContainerIpcMgr::~ContainerIpcMgr()
{
    Stop();
}

int32_t ContainerIpcMgr::SendPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    std::lock_guard<std::mutex> guard(mux_);
    if (clientFd_ < 0) {
        MMI_HILOGD("no client accepted, ignore this event");
        return RET_ERR;
    }
    if (eventQueue_.size() >= MAX_EVENT_NUM) {
        MMI_HILOGD("event number has reached limit, ignore this event");
        return RET_ERR;
    }
    eventQueue_.push(static_cast<std::shared_ptr<InputEvent>>(pointerEvent));
    return RET_OK;
}

int32_t ContainerIpcMgr::SendKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
{
    std::lock_guard<std::mutex> guard(mux_);
    if (clientFd_ < 0) {
        MMI_HILOGD("no client accepted, ignore this event");
        return RET_ERR;
    }
    if (eventQueue_.size() >= MAX_EVENT_NUM) {
        MMI_HILOGD("event number has reached limit, ignore this event");
        return RET_ERR;
    }
    eventQueue_.push(static_cast<std::shared_ptr<InputEvent>>(keyEvent));
    return RET_OK;
}

int32_t ContainerIpcMgr::Init(ContainerType type)
{
    MMI_HILOGD("ContainerIpcMgr Init...  type:%{public}d", type);
    switch (type) {
        case ContainerType::IPC_NONE:
            Stop();
            break;
        case ContainerType::IPC_HMOS:
            Stop();
            return Start(HMOS_SOCKET_PATH);
            break;
        case ContainerType::IPC_LINUX:
        /*
            unsupported now;
        */
        default:
            MMI_HILOGE("unknown ContainerType... type:%{public}d", type);
    }
    return RET_ERR;
}

int32_t ContainerIpcMgr::Start(const char path[])
{
    serverFd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverFd_ < 0) {
        MMI_HILOGE("server socket create error, socket is %{public}d", serverFd_);
        return RET_ERR;
    }
    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, path, sizeof(address.sun_path) - 1);
    if (bind(serverFd_, (struct sockaddr*)&address, sizeof(sockaddr_un)) < 0) {
        MMI_HILOGE("server socket BIND error");
        close(serverFd_);
        serverFd_ = -1;
        return RET_ERR;
    }
    if (listen(serverFd_, MAX_CLIENT_NUM) < 0) {
        MMI_HILOGE("server socket LISTEN error");
        close(serverFd_);
        serverFd_ = -1;
        return RET_ERR;
    }
    std::thread threadAcceptor(&ContainerIpcMgr::AcceptClient, this);
    MMI_HILOGD("create thread for accepting client connectin");
    threadAcceptor.detach();
    return RET_OK;
}

void ContainerIpcMgr::AcceptClient()
{
    while (serverFd_ >= 0) {
        struct sockaddr_un address;
        socklen_t addressLength = sizeof(sockaddr_un);
        clientFd_ = accept(serverFd_, (struct sockaddr*)&address, &addressLength);
        if (clientFd_ >= 0) {
            SendToClient();
        } else {
            MMI_HILOGE("client socket error, wait for new connection");
        }
    }
    MMI_HILOGE("server socket error, socket is %{public}d", serverFd_);
    return;
}

void ContainerIpcMgr::SendToClient()
{
    while (clientFd_ >= 0) {
        std::lock_guard<std::mutex> guard(mux_);
        if (eventQueue_.empty()) {
            mux_.unlock();
            usleep(MICRO_SECOND_INTERVAL);
            continue;
        }
        size_t length = CalculateSequenceLength(eventQueue_.front());
        if (length == 0 || length > MAX_EVENT_DATA_LENGTH) {
            eventQueue_.pop();
            MMI_HILOGE("SequenceLength error, ignore this event");
            continue;
        }
        size_t unWritedLength = length + sizeof(size_t);
        void* buffer = malloc(unWritedLength);
        if (buffer == nullptr) {
            MMI_HILOGE("bad malloc occurs, please check memory");
            Stop();
        }
        *(size_t*)buffer = length;
        if (transformer(eventQueue_.front(), buffer)) {
            char* tmp = (char*)buffer;
            while (unWritedLength > 0) {
                ssize_t size = write(clientFd_, tmp, unWritedLength);
                if (size <= 0) {
                    MMI_HILOGE("socket write error, ret is %{public}zd", size);
                    free(buffer);
                    close(clientFd_);
                    clientFd_ = -1;
                    do {
                        eventQueue_.pop();
                    } while (!eventQueue_.empty());
                    return;
                } else {
                    unWritedLength -= size;
                    tmp += size;
                }
            }
            eventQueue_.pop();
            MMI_HILOGE("OHOS send event successfully!");
        } else {
            eventQueue_.pop();
            MMI_HILOGE("transformer error occurs, ignore this event");
        }
        free(buffer);
    }
    MMI_HILOGE("client socket error, socket is %{public}d", clientFd_);
    return;
}


void ContainerIpcMgr::Stop()
{
    CloseSocket();
    std::lock_guard<std::mutex> guard(mux_);
    while (!eventQueue_.empty()) {
        eventQueue_.pop();
    }
    return;
}

void ContainerIpcMgr::CloseSocket()
{
    if (clientFd_ >= 0) {
        close(clientFd_);
        clientFd_ = -1;
    }
    if (serverFd_ >= 0) {
        close(serverFd_);
        serverFd_ = -1;
    }
    return;
}

bool ContainerIpcMgr::transformer(const std::shared_ptr<InputEvent> inputEvent, void* buffer)
{
    // unimplemented and only used for testing hmos client now
    ((char*)buffer)[4] = 'a';
    ((char*)buffer)[5] = 'b';
    ((char*)buffer)[6] = 'c';
    ((char*)buffer)[7] = 'd';
    ((char*)buffer)[8] = 'e';
    ((char*)buffer)[9] = 'f';
    ((char*)buffer)[10] = 'g';
    return true;
}

int ContainerIpcMgr::CalculateSequenceLength(const std::shared_ptr<InputEvent> inputEvent)
{
    // unimplemented and only used for testing hmos client now
    return 7;
}

} // namespace MMI
} // namespace OHOS
