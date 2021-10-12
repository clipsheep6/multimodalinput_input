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
#ifndef OHOS_UDS_SESSION_H
#define OHOS_UDS_SESSION_H

#include "net_packet.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <memory>

namespace OHOS {
namespace MMI {
class UDSSession;
using SessionPtr = std::shared_ptr<UDSSession>;
class UDSSession : public std::enable_shared_from_this<UDSSession> {
public:
    UDSSession(int32_t fd, sockaddr_un& addr);
    virtual ~UDSSession();

    bool SendMsg(const char *buf, size_t size) const;
    bool SendMsg(NetPacket& pkt) const;
    void Close();

    SessionPtr GetPtr()
    {
        return shared_from_this();
    }
    int32_t GetFd() const
    {
        return fd_;
    }
    const sockaddr_un *GetAddr() const
    {
        return &addr_;
    }

protected:
    int32_t fd_ = -1;
    sockaddr_un addr_ = {};
};
}
}
#endif // HOS_UDS_SESSION_H
