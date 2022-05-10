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
#ifndef UDS_CLIENT_H
#define UDS_CLIENT_H

#include <functional>
#include <future>
#include <thread>

#include "nocopyable.h"

#include "circle_stream_buffer.h"
#include "net_packet.h"
#include "uds_socket.h"

namespace OHOS {
namespace MMI {
class UDSClient;
using MsgClientFunCallback = std::function<void(const UDSClient&, NetPacket&)>;
class UDSClient : public UDSSocket {
public:
    UDSClient();
    DISALLOW_COPY_AND_MOVE(UDSClient);
    virtual ~UDSClient();

    virtual int32_t Socket() = 0;
    int32_t ConnectTo();

    bool SendMsg(const char *buf, size_t size) const;
    bool SendMsg(const NetPacket& pkt) const;
    void Stop();

    bool GetRunStatus() const
    {
        return isRunning_;
    }
    bool GetConnectedStatus() const
    {
        return isConnected_;
    }

protected:
    virtual bool IsFirstConnectFailExit()
    {
        return false;
    }
    virtual void OnConnected() {}
    virtual void OnDisconnected() {}
    virtual void OnThreadLoop() {}

    bool StartClient(MsgClientFunCallback fun, bool detachMode);
    void Disconnected(int32_t fd);

    void OnPacket(NetPacket& pkt);
    void OnRecvMsg(const char *buf, size_t size);
    void OnEvent(const struct epoll_event& ev);
    void OnThread();
    void SetToExit();

protected:
    std::thread t_;
    bool isThreadHadRun_ = false;
    bool isToExit_ = false;
    bool isRunning_ = false;
    bool isConnected_ = false;
    MsgClientFunCallback recvFun_;
    CircleStreamBuffer circBuf_;
};
} // namespace MMI
} // namespace OHOS

#endif // UDS_CLIENT_H