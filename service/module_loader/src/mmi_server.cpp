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
#include "mmi_server.h"
#include "util.h"
#include "log.h"
#include "event_dump.h"

namespace OHOS::MMI {
	namespace {
		static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MMIServer" };
	}
}

#if defined(DEBUG_CODE_TEST) and defined(_DEBUG)

extern "C" void __gcov_flush(void);

void exported_gcov_flush()
{
    __gcov_flush();
}

#endif

static void CheckDefine()
{
    printf("ChkDefs: \n");
#ifdef OHOS_BUILD
    printf("\tOHOS_BUILD\n");
#endif
#ifdef OHOS_WESTEN_MODEL
    printf("\tOHOS_WESTEN_MODEL\n");
#endif
#ifdef OHOS_AUTO_TEST_FRAME
    printf("\tOHOS_AUTO_TEST_FRAME\n");
#endif
#ifdef OHOS_BUILD_LIBINPUT
    printf("\tOHOS_BUILD_LIBINPUT\n");
#endif
#ifdef OHOS_BUILD_HDF
    printf("\tOHOS_BUILD_HDF\n");
#endif
#ifdef OHOS_BUILD_AI
    printf("\tOHOS_BUILD_AI\n");
#endif
#ifdef DEBUG_CODE_TEST
    printf("\tDEBUG_CODE_TEST\n");
#endif
#ifdef OHOS_BUILD_MMI_DEBUG
    printf("\tOHOS_BUILD_MMI_DEBUG\n");
#endif
}

OHOS::MMI::MMIServer::MMIServer()
{
}

OHOS::MMI::MMIServer::~MMIServer()
{
    MMI_LOGT("enter");
}

int32_t OHOS::MMI::MMIServer::Start()
{
    CheckDefine();
#ifdef OHOS_BUILD_MMI_DEBUG
    // start log
    auto log = GetEnv("MI_LOG");
    if (log.empty()) {
        log = DEF_LOG_CONFIG;
    }
    printf("LogConfig: %s\n", log.c_str());
    if (!LogManager::GetInstance().IsRuning()) {
        if (!LogManager::GetInstance().Init(log.c_str())) {
            return LOG_CONFIG_FAIL;
        }
        if (!LogManager::GetInstance().Start()) {
            return LOG_START_FAIL;
        }
    }
#endif // OHOS_BUILD_MMI_DEBUG
    auto tid = GetThisThreadIdOfLL();
    MMI_LOGD("Main Thread tid:%{public}llu", tid);

    int32_t ret = RET_OK;
    ret = InitUds();
    CHKR((ret == RET_OK), ret, ret);

    ret = InitExpSoLibrary();
    CHKR((ret == RET_OK), ret, ret);

    MMI_LOGD("Screen_Manager Init");
    CHKR(WinMgr->Init(*this), WINDOWS_MSG_INIT_FAIL, WINDOWS_MSG_INIT_FAIL);

    MMI_LOGD("AppRegister Init");
    CHKR(AppRegs->Init(*this), APP_REG_INIT_FAIL, APP_REG_INIT_FAIL);

    MMI_LOGD("DeviceRegister Init");
    CHKR(DevRegister->Init(), DEV_REG_INIT_FAIL, DEV_REG_INIT_FAIL);

    MMI_LOGD("MsgHandler Init");
    CHKR(sMsgHandler_.Init(*this), SVR_MSG_HANDLER_INIT_FAIL, SVR_MSG_HANDLER_INIT_FAIL);
#ifdef  OHOS_BUILD_AI
    MMI_LOGD("SeniorInput Init");
    CHKR(seniorInput_.Init(*this), SENIOR_INPUT_DEV_INIT_FAIL, SENIOR_INPUT_DEV_INIT_FAIL);
    sMsgHandler_.SetSeniorInputHandle(seniorInput_);
#endif // OHOS_BUILD_AI

    MMIEventDump->Init(*this);
    ret = InitLibinput();
    CHKR((ret == RET_OK), ret, ret);
    SetRecvFun(std::bind(&ServerMsgHandler::OnMsgHandler, &sMsgHandler_, std::placeholders::_1, std::placeholders::_2));

    CHKR(StartServer(), LIBMMI_SVR_START_FAIL, LIBMMI_SVR_START_FAIL);

#ifdef DEBUG_CODE_TEST
    auto curTime = OHOS::MMI::GetMillisTime();
    auto consumeTime = curTime - GetMmiServerStartTime();
    MMI_LOGW("The server started successfully, the time consumed was %{public}lld Ms curTime:%{public}lld", consumeTime, curTime);
#endif
    return RET_OK;
}

int32_t OHOS::MMI::MMIServer::InitExpSoLibrary()
{
    MMI_LOGD("Load Expansibility Operation");
    auto expConf = GetEnv("EXP_CONF");
    if (expConf.empty()) {
        expConf = DEF_EXP_CONFIG;
    }
    auto expSOPath = GetEnv("EXP_SOPATH");
    if (expSOPath.empty()) {
        expSOPath = DEF_EXP_SOPATH;
    }
    expOper_.LoadExteralLibrary(expConf.c_str(), expSOPath.c_str());
    return RET_OK;
}

int32_t OHOS::MMI::MMIServer::InitLibinput()
{
    MMI_LOGD("Libinput event handle init");
    CHKR(InputHandler->Init(*this), INPUT_EVENT_HANDLER_INIT_FAIL, INPUT_EVENT_HANDLER_INIT_FAIL);

#ifdef OHOS_BUILD_HDF
    MMI_LOGD("HDF Init");
    SetLibInputEventListener([](struct libinput_event* event) {
        InputHandler->OnEvent(event);
    });
    hdfEventManager.SetupCallback();
#else
    #ifdef OHOS_WESTEN_MODEL
        MMI_LOGD("InitLibinput WestonInit...");
        SetLibInputEventListener([](struct libinput_event* event) {
            InputHandler->OnEvent(event);
        });
    #else
        auto seatId = GetEnv("MI_SEATID");
        if (seatId.empty()) {
            seatId = DEF_SEAT_ID;
        }
        MMI_LOGD("MMIServer: seat:%{public}s", seatId.c_str());
        if (!input_.Init(std::bind(&InputEventHandler::OnEvent, InputHandler, std::placeholders::_1),
            seatId)) {
            MMI_LOGE("libinput Init Failed");
            return LIBINPUT_INIT_FAIL;
        }
        MMI_LOGD("libinput start");
        CHKR(input_.Start(), LIBINPUT_START_FAIL, LIBINPUT_START_FAIL);
    #endif
#endif
    return RET_OK;
}

void OHOS::MMI::MMIServer::OnTimer()
{
    InputHandler->OnCheckEventReport();
}

void OHOS::MMI::MMIServer::StopAll()
{
    MMI_LOGD("enter");
    Stop();
    RegEventHM->Clear();
    InputHandler->Clear();
#ifndef OHOS_WESTEN_MODEL
    input_.Stop();
#endif
    MMI_LOGD("leave");
}

void OHOS::MMI::MMIServer::OnConnected(SessionPtr s)
{
    CHK(s, NULL_POINTER);
    int32_t fd = s->GetFd();
    MMI_LOGI("MMIServer::_OnConnected fd:%{public}d", fd);
    AppRegs->RegisterConnectState(fd);
}

void OHOS::MMI::MMIServer::OnDisconnected(SessionPtr s)
{
    MMI_LOGW("MMIServer::OnDisconnected enter, addr:%{public}s fd:%{public}d", s->GetAddr()->sun_path, s->GetFd());
    CHK(s, NULL_POINTER);
    int32_t fd = s->GetFd();
    MMI_LOGI("MMIServer::_OnDisconnected fd:%{public}d", fd);

    auto appInfo = AppRegs->FindBySocketFd(fd);
    WinMgr->EraseSurfaceInfo(appInfo.windowId);
    AppRegs->UnregisterConnectState(fd);
#ifdef  OHOS_BUILD_AI
    seniorInput_.DeviceDisconnect(fd);
#endif // OHOS_BUILD_AI
}

int32_t OHOS::MMI::MMIServer::InitUds()
{
    int32_t ret = RET_OK;
    auto udsPath = GetEnv("UDS_PATH");
    if (udsPath.empty()) {
        udsPath = DEF_UDS_PATH;
    }

    MMI_LOGD("MMIServer::Start: udsPath:%{public}s", udsPath.c_str());
    if ((ret = Init(udsPath)) < 0) {
        MMI_LOGE("udsPath Init Failed");
        return ret;
    }
    char shellCmd[128] = {0};
    std::string shellPath = DEF_MMI_DATA_ROOT;
    (void)sprintf_s(shellCmd, sizeof(shellCmd), "chmod 777 %s -R", shellPath.c_str());
    system(shellCmd);
    return RET_OK;
}
