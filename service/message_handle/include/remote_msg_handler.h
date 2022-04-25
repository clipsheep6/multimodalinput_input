/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef REMOTE_MSG_HANDLER_H
#define REMOTE_MSG_HANDLER_H
#include "message_parcel.h"
#include "nocopyable.h"

#include "msg_handler.h"
#include "uds_server.h"

namespace OHOS {
namespace MMI {
typedef std::function<int32_t(SessionPtr sess, NetPacket& pkt)> RemoteMsgFun;
class RemoteMsgHandler : public MsgHandler<uint32_t, RemoteMsgHandler>{
public:
    RemoteMsgHandler() = default;
    DISALLOW_COPY_AND_MOVE(RemoteMsgHandler);
    virtual ~RemoteMsgHandler() = default;

    void Init(UDSServer& udsServer);
    void OnMsgHandler(int32_t uid, int32_t pid, uint32_t code, MessageParcel& data, MessageParcel& reply);

private:
    void OnAllocSocketFd(SessionPtr session, MessageParcel &data, MessageParcel &reply);
    void OnAddInputEventFilter(SessionPtr session, MessageParcel& data, MessageParcel& reply);

private:
    UDSServer* udsServer_ = nullptr;
    int32_t lastClientPid_ = 0;
    int32_t lastClientUid_ = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // REMOTE_MSG_HANDLER_H