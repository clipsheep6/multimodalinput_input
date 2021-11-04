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

#include "st_client_helper.h"
#include "proto.h"
#include "error_multimodal.h"
#include "log.h"
#include "libmmi_st_client.h"
#include "multimodal_event_handler.h"
#include "st_msg_handler.h"
#include "st_helper.h"

namespace {
    using namespace OHOS::MMI;
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "StClientHelper" };
}

static bool connectIsOk = false;

static void OnConnected(const OHOS::MMI::IfMMIClient& client)
{
    MmiMessageId autoTestRegisterId = MmiMessageId::ST_MESSAGE_BEGIN;
    OHOS::MMI::NetPacket cktAutoTest(MmiMessageId::ST_MESSAGE_BEGIN);
    cktAutoTest << autoTestRegisterId;
    client.SendMessage(cktAutoTest);

    connectIsOk = true;
}

static void OnDisconnected(const OHOS::MMI::IfMMIClient& client)
{
    connectIsOk = false;
}

bool StClientHelper::StartStClient()
{
    MMI_LOGE("enter");
#ifdef OHOS_BUILD_MMI_DEBUG
    VerifyLogManagerRun();
#endif
    static auto stMsgHandler = std::make_shared<OHOS::MMI::STMsgHandler>();
    if (stMsgHandler == nullptr) {
        MMI_LOGE("stMsgHandler is nullptr");
        return false;
    }
    auto ret = stMsgHandler->Init();
    if (!ret) {
        MMI_LOGE("stMsgHandler is nullptr");
        return false;
    }

    client_ = std::make_shared<OHOS::MMI::STClient>();
    if (client_ == nullptr) {
        MMI_LOGE("client_ is nullptr");
        return false;
    }

    client_->RegisterConnectedFunction(&OnConnected);
    client_->RegisterDisconnectedFunction(&OnDisconnected);

    ret = client_->Start(stMsgHandler, true);
    if (!ret) {
        MMI_LOGE("client start fail. ret = %{public}d", ret);
        return false;
    }

    return true;
}

bool StClientHelper::StopStClient()
{
    MMI_LOGE("enter");

    GetSTHelper().KillClient();

#ifdef OHOS_BUILD_MMI_DEBUG
    OHOS::MMI::LogManager::GetInstance().Stop();

    if (OHOS::MMI::LogManager::GetInstance().ThreadIsEnd()) {
        MMI_LOGI("LogManager thread is end.");
    }
#endif
    return true;
}
