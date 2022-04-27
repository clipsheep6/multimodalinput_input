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

#ifndef MULTIMODAL_INPUT_CONNECT_STUB_H
#define MULTIMODAL_INPUT_CONNECT_STUB_H

#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "iremote_stub.h"
#include "message_parcel.h"
#include "nocopyable.h"

#include "entrust_tasks.h"
#include "i_multimodal_input_connect.h"
#include "mmi_log.h"
#include "multimodal_input_connect_define.h"
#include "remote_msg_handler.h"
#include "uds_session.h"

namespace OHOS {
namespace MMI {
class MultimodalInputConnectStub : public IRemoteStub<IMultimodalInputConnect> {
public:
    MultimodalInputConnectStub() = default;
    DISALLOW_COPY_AND_MOVE(MultimodalInputConnectStub);
    ~MultimodalInputConnectStub() = default;

    virtual bool IsRunning() const = 0;
    virtual SessionPtr GetSessionByPid(int32_t pid) const = 0;
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
        MessageOption& options) override;

protected:
    int32_t StubSetPointerVisible(MessageParcel& data, MessageParcel& reply);
    EntrustTasks entrustTasks_;
    RemoteMsgHandler rMsgHandler_;

    bool CheckPermission();
};
} // namespace MMI
} // namespace OHOS
#endif // MULTIMODAL_INPUT_CONNECT_STUB_H