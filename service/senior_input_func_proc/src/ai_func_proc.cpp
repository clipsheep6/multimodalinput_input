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
#include "ai_func_proc.h"
#include <set>
#include "proto.h"

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AIFuncProc" };
    }
}

using namespace std;
using namespace OHOS::MMI;
int32_t OHOS::MMI::AIFuncProc::DeviceEventDispatchProcess(const RawInputEvent &event)
{
    if (CheckEventCode(event) == RET_ERR) {
        MMI_LOGE("aisensor event.code error. event.code: %{public}d", event.ev_code);
        return RET_ERR;
    }
    if (DeviceEventProcess(event) == RET_ERR) {
        MMI_LOGE("aisensor device event process fail");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t OHOS::MMI::AIFuncProc::CheckEventCode(const RawInputEvent& event)
{
    static const std::set<int16_t> g_aiSensorAllowProcCodes = {
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
        ID_MSG_ON_MEDIA_CONTROL,
        ID_MSG_ON_TELEPHONE_CONTROL
    };

    auto findNum = static_cast<ID_MSG>(event.ev_code);
    if (g_aiSensorAllowProcCodes.find(findNum) != g_aiSensorAllowProcCodes.end()) {
        return RET_OK;
    } else {
        return RET_ERR;
    }
}

int32_t OHOS::MMI::AIFuncProc::GetDevType()
{
    return static_cast<int32_t>(INPUT_DEVICE_CAP_AISENSOR);
}