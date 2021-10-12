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
#include "auto_test_msg_handle.h"

#include "c_singleton.h"
#include "libmmi_util.h"
#include "proto.h"
#include "auto_test_data_process.h"

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AutoTestMsgHandle" };
    }
}

#ifdef OHOS_AUTO_TEST_FRAME

OHOS::MMI::AutoTestMsgHandle::AutoTestMsgHandle()
{
}

OHOS::MMI::AutoTestMsgHandle::~AutoTestMsgHandle()
{
}

bool OHOS::MMI::AutoTestMsgHandle::Init()
{
    MsgCallback funs[] = {
        {ID_MSG_AUTOTEST_FRAME_LIBPKT, std::bind(&AutoTestMsgHandle::OnTestLibinputPkt, this, std::placeholders::_1,
            std::placeholders::_2)},
        {ID_MSG_AUTOTEST_FRAME_MAPPKT, std::bind(&AutoTestMsgHandle::OnTestMappingPkt, this, std::placeholders::_1,
            std::placeholders::_2)},
        {ID_MSG_AUTOTEST_FRAME_STDPKT, std::bind(&AutoTestMsgHandle::OnTestStandardPkt, this, std::placeholders::_1,
            std::placeholders::_2)},
        {ID_MSG_AUTOTEST_FRAME_MAGPKT, std::bind(&AutoTestMsgHandle::OnTestManagePkt, this, std::placeholders::_1,
            std::placeholders::_2)},
        {ID_MSG_AUTOTEST_FRAME_DPCPKT, std::bind(&AutoTestMsgHandle::OnTestDispatcherPkt, this, std::placeholders::_1,
            std::placeholders::_2)},
        {ID_MSG_AUTOTEST_FRAME_CLTPKT, std::bind(&AutoTestMsgHandle::OnTestClientPkt, this, std::placeholders::_1,
            std::placeholders::_2)},
        {ID_MSG_AUTOTEST_FRAME_CLISTPKT, std::bind(&AutoTestMsgHandle::OnTestClientListPkt, this, std::placeholders::_1,
            std::placeholders::_2)},
        {ID_MSG_AUTOTEST_FRAME_KEYTYPEPKT, std::bind(&AutoTestMsgHandle::OnTestKeyTypePkt, this,
            std::placeholders::_1, std::placeholders::_2)},
        {ID_MSG_GETMMIINFO_ACK, std::bind(&AutoTestMsgHandle::GetMultimodeInputInfo, this, std::placeholders::_1,
            std::placeholders::_2)},
    };
    for (auto& it : funs) {
        CHKC(RegistrationEvent(it), EVENT_REG_FAIL);
    }
    return OHOS::MMI::ClientMsgHandler::Init();
}

int32_t OHOS::MMI::AutoTestMsgHandle::OnTestLibinputPkt(const UDSClient& client, NetPacket& pkt)
{
    AutoTestLibinputPkt libinputPkt;
    pkt >> libinputPkt;

    AUTOTEST_DATAPROCESS->LibinputPktDataProcess(libinputPkt);
    return RET_OK;
}

int32_t OHOS::MMI::AutoTestMsgHandle::OnTestMappingPkt(const UDSClient& client, NetPacket& pkt)
{
    int32_t sourceType;
    pkt >> sourceType;

    AUTOTEST_DATAPROCESS->MappingPktDataProcess(sourceType);
    return RET_OK;
}

int32_t OHOS::MMI::AutoTestMsgHandle::OnTestStandardPkt(const UDSClient& client, NetPacket& pkt)
{
    AutoTestStandardPkt standardPkt;
    pkt >> standardPkt;

    AUTOTEST_DATAPROCESS->StandardPktDataProcess(standardPkt);
    return RET_OK;
}

int32_t OHOS::MMI::AutoTestMsgHandle::OnTestManagePkt(const UDSClient& client, NetPacket& pkt)
{
    AutoTestManagePkt managePkt;
    std::vector<int32_t> windowList;
    std::vector<AutoTestClientListPkt> clientList;
    pkt >> managePkt;
    for (int32_t i = 0; i < managePkt.sizeOfWindowList; i++) {
        int32_t windowId;
        pkt >> windowId;
        windowList.push_back(windowId);
    }
    for (int32_t i = 0; i < managePkt.sizeOfAppManager; i++) {
        AutoTestClientListPkt clientListPkt;
        pkt >> clientListPkt;
        clientList.push_back(clientListPkt);
    }

    AUTOTEST_DATAPROCESS->ManagePktDataProcess(managePkt, windowList, clientList);
    return RET_OK;
}

int32_t OHOS::MMI::AutoTestMsgHandle::OnTestDispatcherPkt(const UDSClient& client, NetPacket& pkt)
{
    AutoTestDispatcherPkt dispatcherPkt;
    std::vector<float> autoTestJoystic;
    uint16_t joysticAxisNum;
    float standardValue;
    pkt >> joysticAxisNum >> dispatcherPkt;
    for (auto i = 0; i < joysticAxisNum; i++) {
        pkt >> standardValue;
        autoTestJoystic.push_back(standardValue);
    }
    AUTOTEST_DATAPROCESS->DispatcherPktDataProcess(dispatcherPkt, autoTestJoystic);
    return RET_OK;
}

// 该函数接收到的消息是，客户端接收到的数据包
// 其中，clientPkt结构体包含eventType、keyOfHos、rawX、rawY、callBakeName、socketFd、windowId、abilityId;
int32_t OHOS::MMI::AutoTestMsgHandle::OnTestClientPkt(const UDSClient& client, NetPacket& pkt)
{
    AutoTestClientPkt clientPkt;
    pkt >> clientPkt;

    AUTOTEST_DATAPROCESS->ClientPktDataProcess(clientPkt);
    return RET_OK;
}

// 该函数接收到的消息是，server端注册的应用信息列表
// 其中，AutoTestClientListPkt结构体包含socketfd、windowId、abilityId;
int32_t OHOS::MMI::AutoTestMsgHandle::OnTestClientListPkt(const UDSClient& client, NetPacket& pkt)
{
    uint32_t sizeOfList;
    std::vector<AutoTestClientListPkt> clientList;
    pkt >> sizeOfList;
    for (uint32_t i = 0; i < sizeOfList; i++) {
        AutoTestClientListPkt clientListPkt;
        pkt >> clientListPkt;
        clientList.push_back(clientListPkt);
    }

    AUTOTEST_DATAPROCESS->ClientListPktDataProcess(clientList);
    return RET_OK;
}

int32_t OHOS::MMI::AutoTestMsgHandle::OnTestKeyTypePkt(const UDSClient& client, NetPacket& pkt)
{
    AutoTestKeyTypePkt keyTypePkt;
    pkt >> keyTypePkt;

    AUTOTEST_DATAPROCESS->KeyTypePktDataProcess(keyTypePkt);
    return RET_OK;
}

int32_t OHOS::MMI::AutoTestMsgHandle::GetMultimodeInputInfo(const UDSClient& client, NetPacket& pkt)
{
    TagPackHead tagPackHeadAck;
    pkt >> tagPackHeadAck;
    std::cout << "GetMultimodeInputInfo: The client fd is " << tagPackHeadAck.sizeEvent[0] << std::endl;
    return RET_OK;
}
#endif