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
#include "test_aux_tool_client.h"
#include "util.h"
#include "proto.h"
#include "injection_event_dispatch.h"

#if defined(DEBUG_CODE_TEST) and defined(_DEBUG)

extern "C" void __gcov_flush(void);
void exported_gcov_flush()
{
    __gcov_flush();
}
#endif

using namespace std;
using namespace OHOS::MMI;

namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MMITestAuxTool" };
}

int32_t MMITestAuxTool::ExecuteAllCommand()
{
    struct timeval time;
    RawInputEvent rawEvent = {};

    for (uint32_t item = 0; item < AI_CODE_MAX; item++) {
        gettimeofday(&time, 0);
        rawEvent.stamp = static_cast<uint32_t>(time.tv_usec);
        rawEvent.ev_type = static_cast<uint32_t>(INPUT_DEVICE_CAP_AI_SENSOR);
        rawEvent.ev_code = GetAiSensorAllowProcCodes(item);
        rawEvent.ev_value = item;
        NetPacket cktAi(ID_MSG_SENIOR_INPUT_FUNC);
        cktAi << rawEvent;
        SendMsg(cktAi);
    }
    return RET_OK;
}

bool MMITestAuxTool::Start()
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
    CHKF(cMsgHandler_.Init(), MSG_HANDLER_INIT_FAIL);

    auto callback = std::bind(&TestAuxToolMsgHandler::OnMsgHandler, &cMsgHandler_, std::placeholders::_1,
                              std::placeholders::_2);
    CHKF(StartClient(callback), START_CLI_FAIL);

    return true;
}

void MMITestAuxTool::OnDisconnected()
{
    MMI_LOGD("Disconnected from server... fd:%{public}d", GetFd());
}

void MMITestAuxTool::SetFunction(const ConnnectFun &fun)
{
    if (fun == nullptr) {
        return;
    }
    fun_ = fun;
}

void MMITestAuxTool::OnConnected()
{
    if (fun_ != nullptr) {
        fun_();
    }

    MMI_LOGD("Connection to server succeeded... fd:%{public}d", GetFd());
}

uint32_t OHOS::MMI::MMITestAuxTool::GetAiSensorAllowProcCodes(uint32_t item) const
{
    static const vector<ID_MSG> aiSensorAllowProcCodes {
    ID_MSG_ON_SHOW_MENU,
    ID_MSG_ON_SEND,
    ID_MSG_ON_COPY,
    ID_MSG_ON_PASTE,
    ID_MSG_ON_CUT,
    ID_MSG_ON_UNDO,
    ID_MSG_ON_REFRESH,
    ID_MSG_ON_CANCEL,
    ID_MSG_ON_ENTER,
    ID_MSG_ON_PREVIOUS,
    ID_MSG_ON_NEXT,
    ID_MSG_ON_BACK,
    ID_MSG_ON_PRINT,
    ID_MSG_ON_PLAY,
    ID_MSG_ON_PAUSE,
    ID_MSG_ON_SCREEN_SHOT,
    ID_MSG_ON_SCREEN_SPLIT,
    ID_MSG_ON_START_SCREEN_RECORD,
    ID_MSG_ON_STOP_SCREEN_RECORD,
    ID_MSG_ON_GOTO_DESKTOP,
    ID_MSG_ON_RECENT,
    ID_MSG_ON_SHOW_NOTIFICATION,
    ID_MSG_ON_LOCK_SCREEN,
    ID_MSG_ON_SEARCH,
    ID_MSG_ON_CLOSEPAGE,
    ID_MSG_ON_LAUNCH_VOICE_ASSISTANT,
    ID_MSG_ON_MUTE,
    ID_MSG_ON_ANSWER,
    ID_MSG_ON_REFUSE,
    ID_MSG_ON_HANG_UP,
    ID_MSG_ON_START_DRAG,
    ID_MSG_ON_TELEPHONE_CONTROL,
    ID_MSG_ON_TELEPHONE_CONTROL
    };

    return static_cast<uint32_t>(aiSensorAllowProcCodes[item]);
}
