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
#include "libmmi_client.h"
#include "util.h"
#include "log.h"
#include "proto.h"
#include "multimodal_event_handler.h"

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MMIClient" };
    }
}

#if defined(DEBUG_CODE_TEST) and defined(_DEBUG)

extern "C" void __gcov_flush(void);

void exported_gcov_flush()
{
    __gcov_flush();
}

#endif

OHOS::MMI::MMIClient::MMIClient()
{
}

OHOS::MMI::MMIClient::~MMIClient()
{
    MMI_LOGD("enter");
}

bool OHOS::MMI::MMIClient::SendMessage(const NetPacket &pkt) const
{
    return SendMsg(pkt);
}

bool OHOS::MMI::MMIClient::GetCurrentConnectedStatus() const
{
    return GetConnectedStatus();
}

bool OHOS::MMI::MMIClient::Start(IClientMsgHandlerPtr msgHdl)
{
    auto udsPath = GetEnv("UDS_PATH");
    if (udsPath.empty()) {
        udsPath = DEF_UDS_PATH;
    }
    auto log = GetEnv("MI_LOG");
    if (log.empty()) {
        log = DEF_LOG_CONFIG;
    }
    connPath_ = udsPath;
#ifdef OHOS_BUILD_MMI_DEBUG
    // start log
    if (!LogManager::GetInstance().Init(log.c_str())) {
        printf("Failed to get log instance initialization string\n");
        return false;
    }
    if (!LogManager::GetInstance().Start()) {
        printf("Failed to start log instance\n");
        return false;
    }
#endif // OHOS_BUILD_MMI_DEBUG
    EventManager.SetClientHandle(GetPtr());
    CHKF(msgHdl->Init(), MSG_HANDLER_INIT_FAIL);
    auto msgHdlImp = static_cast<ClientMsgHandler *>(msgHdl.get());
    CHKF(msgHdlImp, MSG_HANDLER_INIT_FAIL)
    auto callback = std::bind(&ClientMsgHandler::OnMsgHandler, msgHdlImp, std::placeholders::_1, std::placeholders::_2);
    CHKF(StartClient(callback), START_CLI_FAIL);

    return true;
}

void OHOS::MMI::MMIClient::RegisterConnectedFunction(ConnectCallback fun)
{
    funConnected_ = fun;
}

void OHOS::MMI::MMIClient::RegisterDisconnectedFunction(ConnectCallback fun)
{
    funDisconnected_ = fun;
}

void OHOS::MMI::MMIClient::VirtualKeyIn(RawInputEvent virtualKeyEvent)
{
    NetPacket ckt(ID_MSG_ON_VIRTUALKEY);
    ckt << virtualKeyEvent;
    SendMsg(ckt);
}

void OHOS::MMI::MMIClient::ReplyMessageToServer(int32_t idMsg, uint64_t time, uint64_t serverStartTime,
    uint64_t clientEndTime, int32_t fd) const
{
    NetPacket ckt(ID_MSG_CHECKREPLYMESSAGE);
    ckt << idMsg << time << fd << serverStartTime << clientEndTime;
    SendMsg(ckt);
}

#ifdef OHOS_AUTO_TEST_FRAME
void OHOS::MMI::MMIClient::AutoTestReplyClientPktToServer(const AutoTestClientPkt &autoTestClientPkt)
{
    NetPacket ckt(ID_MSG_AUTOTEST_FRAME_REPLYPKT);
    ckt << autoTestClientPkt;
    SendMsg(ckt);
}
#endif  // OHOS_AUTO_TEST_FRAME

void OHOS::MMI::MMIClient::SdkGetMultimodeInputInfo()
{
    TagPackHead tagPackHead = {ID_MSG_GETMMIINFO, {0}};
    NetPacket ckt(ID_MSG_GETMMIINFO);
    ckt << tagPackHead;
    SendMsg(ckt);
}

void OHOS::MMI::MMIClient::OnDisconnected()
{
    MMI_LOGD("Disconnected from server... fd:%{public}d", GetFd());
    if (funDisconnected_) {
        funDisconnected_(*this);
    }
    isConnected_ = false;
    EventManager.ClearAll();
}

void OHOS::MMI::MMIClient::OnConnected()
{
    MMI_LOGD("Connection to server succeeded... fd:%{public}d", GetFd());
    if (funConnected_) {
        funConnected_(*this);
    }
    isConnected_ = true;
}
