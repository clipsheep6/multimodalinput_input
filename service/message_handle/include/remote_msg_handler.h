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
#include "iremote_object.h"

#include "msg_handler.h"
#include "i_multimodal_input_connect.h"
#include "uds_session.h"

namespace OHOS {
namespace MMI {
using RemoteMsgFun = std::function<void(MessageParcel &, MessageParcel &)>;
class RemoteMsgHandler : public MsgHandler<uint32_t, RemoteMsgFun> {
public:
    RemoteMsgHandler() = default;
    DISALLOW_COPY_AND_MOVE(RemoteMsgHandler);
    virtual ~RemoteMsgHandler() = default;

    void Init(IMultimodalInputConnect& multStub);
    void OnMsgHandler(int32_t taskId, int32_t uid, int32_t pid, uint64_t stid, uint32_t code,
        MessageParcel& data, MessageParcel& reply);

private:
    void OnAllocSocketFd(MessageParcel &data, MessageParcel &reply);
    void OnAddInputEventFilter(MessageParcel& data, MessageParcel& reply);
    void StubSetPointerVisible(MessageParcel& data, MessageParcel& reply);

private:
    int32_t lastClientPid_ = 0;
    int32_t lastClientUid_ = 0;
    SessionPtr lastSession_ = nullptr;
    IMultimodalInputConnect* multStub_ = nullptr;
};
} // namespace MMI
} // namespace OHOS
#endif // REMOTE_MSG_HANDLER_H