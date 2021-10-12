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
#ifndef OHOS_UDS_CLIENT_H
#define OHOS_UDS_CLIENT_H

#include <thread>
#include <functional>
#include "uds_socket.h"
#include "net_packet.h"

namespace OHOS {
namespace MMI {
class UDSClient;
using MsgClientFunCallback = std::function<void(const UDSClient&, NetPacket&)>;
class UDSClient : public UDSSocket {
public:
    UDSClient();
    virtual ~UDSClient();

    int32_t ConnectTo(const std::string& path, bool isBind = false);

    bool SendMsg(const char *buf, size_t size) const;
    bool SendMsg(const NetPacket& pkt) const;
    void Stop();

    bool GetRunStatus() const
    {
        return isRun_;
    }
    bool GetConnectedStatus() const
    {
        return isConnected_;
    }

protected:
    virtual void OnConnected() {}
    virtual void OnDisconnected() {}

    bool StartClient(MsgClientFunCallback fun);
    void OnRecv(const char *buf, size_t size);
    void OnEvent(const epoll_event& ev, StreamBuffer& buf);
    void OnThread();

protected:
    std::thread t_;
    bool isRun_ = false;
    bool isConnected_ = false;
    bool isBind_ = true;
    MsgClientFunCallback recvFun_;
};
}
}

#endif
