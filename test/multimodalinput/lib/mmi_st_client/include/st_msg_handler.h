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
#ifndef OHOS_MMI_ST_MSG_HANDLER_H
#define OHOS_MMI_ST_MSG_HANDLER_H

#include "if_client_msg_handler.h"
#include "msg_handler.h"
#include "uds_client.h"
#include "config_multimodal.h"
#include "multimodal_event.h"

namespace OHOS {
namespace MMI {
typedef std::function<int32_t(const UDSClient&, NetPacket&)> ClientMsgFun;
class STMsgHandler : public MsgHandler<ClientMsgFun>, public IfClientMsgHandler, public std::enable_shared_from_this<IfClientMsgHandler> {
public:
    STMsgHandler() = default;
    virtual ~STMsgHandler() = default;
    virtual bool Init() override;
    void OnMsgHandler(const UDSClient& client, NetPacket& pkt) override;

    int32_t OnTestLibinputPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestMappingPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestStandardPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestManagePkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestDispatcherPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestClientPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestApiOnKeyClientPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestApiRegisterClientPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestApiDeviceClientPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestApiMouseClientPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestApiTouchClientPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestApiTouchFingerClientPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestApiTouchCoordinateClientPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestApiTouchTwyParClientPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestApiTouchTwyParFinClientPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestApiStylusClientPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestClientListPkt(const UDSClient& client, NetPacket& pkt);
    int32_t OnTestKeyTypePkt(const UDSClient& client, NetPacket& pkt);
};
}
}
#endif // OHOS_MMI_ST_MSG_HANDLER_H
