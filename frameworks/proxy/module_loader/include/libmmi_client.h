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
#ifndef OHOS_LIBMMI_CLIENT_H
#define OHOS_LIBMMI_CLIENT_H

#include "client_msg_handler.h"
#include "if_mmi_client.h"

namespace OHOS::MMI {
class MMIClient : public UDSClient, public IMMIClient, public std::enable_shared_from_this<IMMIClient> {
public:
    MMIClient();
    virtual ~MMIClient() override;

    virtual bool SendMessage(const NetPacket& pkt) const override;
    virtual bool GetCurrentConnectedStatus() const override;

    bool Start(IClientMsgHandlerPtr msgHdl) override;
    void RegisterConnectedFunction(ConnectCallback fun) override;
    void RegisterDisconnectedFunction(ConnectCallback fun) override;
    void VirtualKeyIn(struct RawInputEvent virtualKeyEvent);
    void ReplyMessageToServer(int32_t idMsg, uint64_t time, uint64_t serverStartTime, uint64_t clientEndTime, 
                              int32_t fd) const;
#ifdef OHOS_AUTO_TEST_FRAME
    void AutoTestReplyClientPktToServer(const AutoTestClientPkt& autoTestClientPkt);
#endif  // OHOS_AUTO_TEST_FRAME
    void SdkGetMultimodeInputInfo();
    MMIClientPtr GetPtr()
    {
        return shared_from_this();
    }

protected:
    virtual void OnConnected() override;
    virtual void OnDisconnected() override;

protected:
    ConnectCallback funConnected_;
    ConnectCallback funDisconnected_;
};
}
#endif
