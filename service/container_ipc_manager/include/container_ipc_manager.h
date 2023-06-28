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

#ifndef CONTAINER_IPC_MANAGER_H
#define CONTAINER_IPC_MANAGER_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>

#include "pointer_event.h"
#include "key_event.h"

namespace OHOS {
namespace MMI {

enum class ContainerType {
    IPC_NONE,
    IPC_HMOS,
    IPC_LINUX, //unsupported
};

class ContainerIpcMgr {
public:
    ContainerIpcMgr();
    ~ContainerIpcMgr();

    int32_t Init(ContainerType type);
    void Stop();
    int32_t SendKeyEvent(const std::shared_ptr<KeyEvent> keyEvent);
    int32_t SendPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent);

private:
    bool transformer(const std::shared_ptr<InputEvent> inputEvent, void* buffer);
    int CalculateSequenceLength(const std::shared_ptr<InputEvent> inputEvent);
    int32_t Start(const char path[]);
    void AcceptClient();
    void SendToClient();
    void CloseSocket();

private:
    std::mutex mux_;
    int serverFd_;
    int clientFd_;
    std::queue<std::shared_ptr<InputEvent>> eventQueue_;
};
} // namespace MMI
} // namespace OHOS
#endif // CONTAINER_IPC_MANAGER_H
