#if 0 // TODO: BINDER 
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
#include <codecvt>
#include <locale>
#include <signal.h>
#include "libmmi_client.h"
#include "key_event_handler.h"
#include "multimodal_event_handler.h"
#include "common_event_handler.h"
#include "system_event_handler.h"
#include "telephone_event_handler.h"
#include "touch_event_handler.h"
#include "media_event_handler.h"

void OHOS::MMI::MMIClient::TestSocketPairClose()
{
    MMI_LOGE("enter");
    const int32_t ret = MultimodalInputConnectManager::GetInstance()->AllocSocketPair();
    if (ret != RET_OK) {
        MMI_LOGE("UDSSocket::Socket, call MultimodalInputConnectManager::AllocSocketPair return %{public}d", ret);
    }
    fd_ = MultimodalInputConnectManager::GetInstance()->GetClientSocketFdOfAllocedSocketPair();
    if (fd_ == IMultimodalInputConnect::INVALID_SOCKET_FD) {
        MMI_LOGE("UDSSocket::Socket, call MultimodalInputConnectManager::GetClientSocketFdOfAllocedSocketPair return invalid fd.");
    } else {
        MMI_LOGT("UDSSocket::Socket, call MultimodalInputConnectManager::GetClientSocketFdOfAllocedSocketPair return fd = %{public}d.", fd_);
    }

    auto f = [this](const char *title) {
        MMI_LOGI("%s, call write, fd = %d, enter\n", title, fd_);
        bool bToSendMessage = true;
        int i = 0;
        while (bToSendMessage) {
            scanf("%d", &bToSendMessage);
            if (bToSendMessage) {
                MMI_LOGI(">>> call write, fd = %d, begin\n", fd_);
                const int data = i++;
                const int ret = (int)write(fd_, &data, sizeof(data));
                if (ret < 0) {
                    int errnoSaved = errno;
                    MMI_LOGI("write, fd = %d, data = %d, errno = %d, message = %s\n", fd_, data, errnoSaved, strerror(errnoSaved));
                    continue;
                }
                MMI_LOGI(">>> call write, fd = %d, end\n", fd_);
            }            
        }
    };

    f("has alloced. ");

    shutdown(fd_, SHUT_RDWR);

    f("has shutdown. ");

    struct linger linger;
    linger.l_onoff = 1;
    linger.l_linger = 0;
    setsockopt(fd_, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

    f("has set sockopt. ");

    close(fd_);
    f("has closed");

    MMI_LOGE("leave");
}

static std::string Strftime(const std::string& format = "%F %T", time_t curTime = 0)
{
    if (curTime == 0) {
        curTime = time(nullptr);
    }
    auto t = localtime(&curTime);
    char longTime[LOG_MAX_TIME_LEN] = "";
    strftime(longTime, sizeof(longTime), format.c_str(), t);
    return longTime;
}

#define LOGLOG(fmt, ...) {\
    std::string __curtm = Strftime(); \
    MMI_LOGI("%s ", __curtm.c_str()); \
    MMI_LOGI(fmt, ##__VA_ARGS__); \
    MMI_LOGI("\n");}

using namespace OHOS;
using namespace OHOS::MMI;

class AppKeyEventHandle : public KeyEventHandler {
public:
    AppKeyEventHandle() {}
    ~AppKeyEventHandle() {}

    virtual bool OnKey(const KeyEvent& keylEvent) override
    {
        MMI_LOGT("AppKeyEventHandle::Onkey");
        return true;
    }
};

class AppTouchEventHandle : public TouchEventHandler {
public:
    AppTouchEventHandle() {}
    ~AppTouchEventHandle() {}

    virtual bool OnTouch(const TouchEvent& touchEvent) override
    {
        MMI_LOGT("AppTouchEventHandle::OnTouch");
        return true;
    }
};

class AppCommonEventHandle : public CommonEventHandler {
public:
    AppCommonEventHandle() {}
    ~AppCommonEventHandle() {}

    virtual bool OnShowMenu(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnShowMenu");
        return true;
    }

    virtual bool OnSend(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnSend");
        return true;
    }

    virtual bool OnCopy(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnCopy");
        return true;
    }

    virtual bool OnPaste(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnPaste");
        return true;
    }

    virtual bool OnCut(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnCut");
        return true;
    }

    virtual bool OnUndo(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnUndo");
        return true;
    }

    virtual bool OnRefresh(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnRefresh");
        return true;
    }

    virtual bool OnStartDrag(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnStartDrag");
        return true;
    }

    virtual bool OnCancel(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnCancel");
        return true;
    }

    virtual bool OnEnter(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnEnter");
        return true;
    }

    virtual bool OnPrevious(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnPrevious");
        return true;
    }

    virtual bool OnNext(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnNext");
        return true;
    }

    virtual bool OnBack(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnBack");
        return true;
    }

    virtual bool OnPrint(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppCommonEventHandle::OnPrint");
        return true;
    }
};

class AppMediaEventHandle : public MediaEventHandler {
public:
    AppMediaEventHandle() {}
    ~AppMediaEventHandle() {}

    virtual bool OnPlay(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppMediaEventHandle::OnPlay");
        return true;
    }

    virtual bool OnPause(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppMediaEventHandle::OnPause");
        return true;
    }

    virtual bool OnMediaControl(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppMediaEventHandle::OnMediaControl");
        return true;
    }
};

class AppSystemEventHandle : public SystemEventHandler {
public:
    AppSystemEventHandle() {}
    ~AppSystemEventHandle() {}

    virtual bool OnClosePage(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnClosePage");
        return true;
    }

    virtual bool OnLaunchVoiceAssistant(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnLaunchVoiceAssistant");
        return true;
    }

    virtual bool OnMute(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnMute");
        return true;
    }

    virtual bool OnScreenShot(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnScreenShot");
        return true;
    }

    virtual bool OnScreenSplit(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnScreenSplit");
        return true;
    }

    virtual bool OnStartScreenRecord(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnStartScreenRecord");
        return true;
    }

    virtual bool OnStopScreenRecord(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnStopScreenRecord");
        return true;
    }

    virtual bool OnGotoDesktop(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnGotoDesktop");
        return true;
    }

    virtual bool OnRecent(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnRecent");
        return true;
    }

    virtual bool OnShowNotification(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnShowNotification");
        return true;
    }

    virtual bool OnLockScreen(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnLockScreen");
        return true;
    }

    virtual bool OnSearch(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppSystemEventHandle::OnSearch");
        return true;
    }
};

class AppTelephoneEventHandle : public TelephoneEventHandler {
public:
    AppTelephoneEventHandle() {}
    ~AppTelephoneEventHandle() {}

    virtual bool OnAnswer(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppTelephoneEventHandle::OnAnswer");
        return true;
    }

    virtual bool OnRefuse(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppTelephoneEventHandle::OnRefuse");
        return true;
    }

    virtual bool OnHangup(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppTelephoneEventHandle::OnHangup");
        return true;
    }

    virtual bool OnTelephoneControl(const MultimodalEvent& event) override
    {
        MMI_LOGT("AppTelephoneEventHandle::OnTelephoneControl");
        return true;
    }
};

template<class T>
StandEventPtr CreateEvent()
{
    return StandEventPtr(new T());
}

class HosMmiClient {
private:
    sptr<IRemoteObject> remoteObject_;
    int32_t windowId_;
    std::map<std::string, StandEventPtr> handerMap_;
public:
    HosMmiClient()
    {
        windowId_ = getpid();
        std::string u8String = "hello world!\n";
        auto wsConvert = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{};
        auto u16String = wsConvert.from_bytes(u8String);
        remoteObject_ = TestIRemoteObject::Create(u16String);
    }
    ~HosMmiClient()
    {
        MMI_LOGI("in %s enter.\n", __func__);
        UnregisterStandardizedEventHandle();
        MMI_LOGI("in %s leave.\n", __func__);
    }

    void TestSocketPairClose()
    {
        MultimodalEventHandler::GetInstance()->TestSocketPairClose();
    }

    void RegisterStandardizedEventHandle()
    {
        MMI_LOGI("HosMmiClient RegisterStandardizedEventHandle enter.");

        MMI_LOGT("Register key event.");

        using namespace OHOS::MMI;

        auto appKey = CreateEvent<AppKeyEventHandle>();
        handerMap_[std::string("AppKeyEventHandle")] = appKey;
        MultimodalEventHandler::GetInstance()->RegisterStandardizedEventHandle(remoteObject_, windowId_, appKey);

        MMI_LOGT("Register touch event.");
        auto appTouch = CreateEvent<AppTouchEventHandle>();
        handerMap_[std::string("AppTouchEventHandle")] = appTouch;
        MMIEventHdl->RegisterStandardizedEventHandle(remoteObject_, windowId_, appTouch);

        MMI_LOGT("Register Common event.");
        auto appCommon = CreateEvent<AppCommonEventHandle>();
        handerMap_[std::string("AppCommonEventHandle")] = appCommon;
        MMIEventHdl->RegisterStandardizedEventHandle(remoteObject_, windowId_, appCommon);

        MMI_LOGT("Register Media event.");
        auto appMedia = CreateEvent<AppMediaEventHandle>();
        handerMap_[std::string("AppMediaEventHandle")] = appMedia;
        MMIEventHdl->RegisterStandardizedEventHandle(remoteObject_, windowId_, appMedia);

        MMI_LOGT("Register System event.");
        auto appSystem = CreateEvent<AppSystemEventHandle>();
        handerMap_[std::string("AppSystemEventHandle")] = appSystem;
        MMIEventHdl->RegisterStandardizedEventHandle(remoteObject_, windowId_, appSystem);

        MMI_LOGT("Register Telephone event.");
        auto appTelephone = CreateEvent<AppTelephoneEventHandle>();
        handerMap_[std::string("AppTelephoneEventHandle")] = appTelephone;
        MMIEventHdl->RegisterStandardizedEventHandle(remoteObject_, windowId_, appTelephone);
    }

    void UnregisterStandardizedEventHandle()
    {
        MMI_LOGI("HosMmiClient::UnregisterStandardizedEventHandle enter.");
        for (auto it = handerMap_.begin(); it != handerMap_.end();) {
            MMI_LOGT("UnregisterStandardizedEventHandle %{public}s", it->first.c_str());
            LOGLOG("UnregisterStandardizedEventHandle %s", it->first.c_str());
            MMIEventHdl->UnregisterStandardizedEventHandle(remoteObject_, windowId_, it->second);
            handerMap_.erase(it++);
        }
    }
};

static std::atomic_bool g_clientExit(false);

void RunClient()
{
    HosMmiClient client;
    client.RegisterStandardizedEventHandle();

    while (!g_clientExit) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LOGLOG("UnRegisterStandardizedEventHandle begin....\n");
    client.UnregisterStandardizedEventHandle();
    LOGLOG("UnRegisterStandardizedEventHandle end, wait 3000ms exit.\n");
}

void TestSocketPair()
{
    HosMmiClient client;
    client.TestSocketPairClose();
}

void ClientStopHandler(int signo)
{
    MMI_LOGI("oops! stop!!! %d\n", signo);
    g_clientExit = true;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, ClientStopHandler);

    auto funcIsContinue = [](const char *title) -> bool {
        bool bContinue;
        MMI_LOGI("%s\n", title);
        MMI_LOGI("Input option: [0 - yes, other any key - no]\n");
        scanf_s("%d", &bContinue);
        return !!bContinue;
    };

    // RunClient();
    
    for (int i = 0; i < 10; i++) {
        TestSocketPair();
        if (funcIsContinue("Will break?")) {
            break;
        }
    }

    if (funcIsContinue("Will exit client?\n")) {
        g_clientExit = true;
    }

    while (!g_clientExit) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LOGLOG("hosmmi-client exit.\n");

    return 0;
}
#endif