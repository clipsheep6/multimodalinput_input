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
#ifndef OHOS_AUTO_TEST_MSG_HANDLE_H
#define OHOS_AUTO_TEST_MSG_HANDLE_H

#include "client_msg_handler.h"

#ifdef OHOS_AUTO_TEST_FRAME
namespace OHOS {
    namespace MMI {
        class AutoTestMsgHandle : public ClientMsgHandler {
        public:
            AutoTestMsgHandle();
            virtual ~AutoTestMsgHandle();
            virtual bool Init() override;

            virtual int32_t OnTestLibinputPkt(const UDSClient& client, NetPacket& pkt);
            virtual int32_t OnTestMappingPkt(const UDSClient& client, NetPacket& pkt);
            virtual int32_t OnTestStandardPkt(const UDSClient& client, NetPacket& pkt);
            virtual int32_t OnTestManagePkt(const UDSClient& client, NetPacket& pkt);
            virtual int32_t OnTestDispatcherPkt(const UDSClient& client, NetPacket& pkt);
            virtual int32_t OnTestClientPkt(const UDSClient& client, NetPacket& pkt);
            virtual int32_t OnTestClientListPkt(const UDSClient& client, NetPacket& pkt);
            virtual int32_t OnTestKeyTypePkt(const UDSClient& client, NetPacket& pkt);
            virtual int32_t GetMultimodeInputInfo(const UDSClient& client, NetPacket& pkt);
        };
    }
}
#endif
#endif