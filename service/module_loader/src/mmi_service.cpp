/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "mmi_service.h"

#include <cinttypes>
#include <csignal>

#include <sys/signalfd.h>
#ifdef OHOS_RSS_CLIENT
#include <unordered_map>
#endif

#include "event_dump.h"
#include "input_windows_manager.h"
#include "i_pointer_drawing_manager.h"
#include "mmi_log.h"
#include "multimodal_input_connect_def_parcel.h"
#ifdef OHOS_RSS_CLIENT
#include "res_sched_client.h"
#include "res_type.h"
#include "system_ability_definition.h"
#endif
#include "timer_manager.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MMIService" };
const std::string DEF_INPUT_SEAT = "seat0";
} // namespace

const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<MMIService>::GetInstance().get());

struct mmi_epoll_event {
    int32_t fd;
    EpollEventType event_type;
};

template<class ...Ts>
void CheckDefineOutput(const char* fmt, Ts... args)
{
    using namespace OHOS::MMI;
    CHKPV(fmt);
    char buf[MAX_PACKET_BUF_SIZE] = {};
    int32_t ret = snprintf_s(buf, MAX_PACKET_BUF_SIZE, MAX_PACKET_BUF_SIZE - 1, fmt, args...);
    if (ret == -1) {
        KMSG_LOGI("call snprintf_s fail.ret = %d", ret);
        return;
    }
    KMSG_LOGI("%s", buf);
    MMI_HILOGI("%{public}s", buf);
}

static void CheckDefine()
{
    CheckDefineOutput("ChkDefs:");
#ifdef OHOS_BUILD
    CheckDefineOutput("%-40s", "OHOS_BUILD");
#endif
#ifdef OHOS_BUILD_LIBINPUT
    CheckDefineOutput("%-40s", "OHOS_BUILD_LIBINPUT");
#endif
#ifdef OHOS_BUILD_HDF
    CheckDefineOutput("%-40s", "OHOS_BUILD_HDF");
#endif
#ifdef OHOS_BUILD_MMI_DEBUG
    CheckDefineOutput("%-40s", "OHOS_BUILD_MMI_DEBUG");
#endif
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    CheckDefineOutput("%-40s", "OHOS_BUILD_ENABLE_POINTER_DRAWING");
#endif
#ifdef OHOS_BUILD_ENABLE_INTERCEPTOR
    CheckDefineOutput("%-40s", "OHOS_BUILD_ENABLE_INTERCEPTOR");
#endif
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    CheckDefineOutput("%-40s", "OHOS_BUILD_ENABLE_KEYBOARD");
#endif
#ifdef OHOS_BUILD_ENABLE_POINTER
    CheckDefineOutput("%-40s", "OHOS_BUILD_ENABLE_POINTER");
#endif
#ifdef OHOS_BUILD_ENABLE_TOUCH
    CheckDefineOutput("%-40s", "OHOS_BUILD_ENABLE_TOUCH");
#endif
}

std::shared_ptr<IUdsServer> IUdsServer::GetInstance()
{
    // auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    // if (sm == nullptr) {
    //     MMI_HILOGE("get system ability manager fail");
    //     return false;
    // }
    // auto sa = sm->GetSystemAbility(IMultimodalInputConnect::MULTIMODAL_INPUT_CONNECT_SERVICE_ID);
    // if (sa == nullptr) {
    //     MMI_HILOGE("get sa fail");
    //     return false;
    // }

    // std::weak_ptr<MultimodalInputConnectManager> weakPtr = shared_from_this();
    // auto deathCallback = [weakPtr](const wptr<IRemoteObject> &object) {
    //     auto sharedPtr = weakPtr.lock();
    //     if (sharedPtr != nullptr) {
    //         sharedPtr->OnDeath();
    //     }
    // };

    // multimodalInputConnectRecipient_ = new (std::nothrow) MultimodalInputConnectDeathRecipient(deathCallback);
    // CHKPF(multimodalInputConnectRecipient_);
    // sa->AddDeathRecipient(multimodalInputConnectRecipient_);
    // multimodalInputConnectService_ = iface_cast<IMultimodalInputConnect>(sa);
    // if (multimodalInputConnectService_ == nullptr) {
    //     MMI_HILOGE("get multimodalinput service fail");
    //     return false;
    // }

    auto service = OHOS::DelayedSingleton<MMIService>::GetInstance();
    return service;
}

MMIService::MMIService() : SystemAbility(MULTIMODAL_INPUT_CONNECT_SERVICE_ID, true) {}

MMIService::~MMIService() {}

int32_t MMIService::AddEpoll(EpollEventType type, int32_t fd)
{
    if (!(type >= EPOLL_EVENT_BEGIN && type < EPOLL_EVENT_END)) {
        MMI_HILOGE("Invalid param type");
        return RET_ERR;
    }
    if (fd < 0) {
        MMI_HILOGE("Invalid param fd_");
        return RET_ERR;
    }
    if (mmiFd_ < 0) {
        MMI_HILOGE("Invalid param mmiFd_");
        return RET_ERR;
    }
    auto eventData = static_cast<mmi_epoll_event*>(malloc(sizeof(mmi_epoll_event)));
    if (!eventData) {
        MMI_HILOGE("Malloc failed");
        return RET_ERR;
    }
    eventData->fd = fd;
    eventData->event_type = type;
    MMI_HILOGD("userdata:[fd:%{public}d,type:%{public}d]", eventData->fd, eventData->event_type);

    struct epoll_event ev = {};
    ev.events = EPOLLIN;
    ev.data.ptr = eventData;
    auto ret = EpollCtl(fd, EPOLL_CTL_ADD, ev, mmiFd_);
    if (ret < 0) {
        free(eventData);
        eventData = nullptr;
        ev.data.ptr = nullptr;
        return ret;
    }
    return RET_OK;
}

bool MMIService::InitLibinputService()
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_HDF
    MMI_HILOGD("HDF Init");
    hdfEventManager.SetupCallback();
#endif
    if (!(libinputAdapter_.Init(std::bind(&InputEventHandler::OnEvent, &southEventHandler_, std::placeholders::_1),
        DEF_INPUT_SEAT))) {
        MMI_HILOGE("libinput init, bind failed");
        return false;
    }
    auto inputFd = libinputAdapter_.GetInputFd();
    auto ret = AddEpoll(EPOLL_EVENT_INPUT, inputFd);
    if (ret <  0) {
        MMI_HILOGE("AddEpoll error ret: %{public}d", ret);
        EpollClose();
        return false;
    }
    MMI_HILOGD("AddEpoll, epollfd: %{public}d, fd: %{public}d", mmiFd_, inputFd);
    return true;
}

bool MMIService::InitService()
{
    CALL_LOG_ENTER;
    if (state_ != ServiceRunningState::STATE_NOT_START) {
        MMI_HILOGE("Service running status is not enabled");
        return false;
    }
    if (!(Publish(this))) {
        MMI_HILOGE("Service initialization failed");
        return false;
    }
    if (EpollCreat(MAX_EVENT_SIZE) < 0) {
        MMI_HILOGE("epoll create failed");
        return false;
    }
    auto ret = AddEpoll(EPOLL_EVENT_SOCKET, epollFd_);
    if (ret <  0) {
        MMI_HILOGE("AddEpoll error ret:%{public}d", ret);
        EpollClose();
        return false;
    }
    MMI_HILOGD("AddEpoll, epollfd:%{public}d,fd:%{public}d", mmiFd_, epollFd_);
    return true;
}

int32_t MMIService::Init()
{
    CALL_LOG_ENTER;
    CheckDefine();

    MMI_HILOGD("InputEventHandler Init");
    southEventHandler_.Init();

    MMI_HILOGD("ServerMsgHandler Init");
    sMsgHandler_.Init(&southEventHandler_);
    MMI_HILOGD("EventDump Init");
    MMIEventDump->Init();

    MMI_HILOGD("WindowsManager Init");
    if (!WinMgr->Init()) {
        MMI_HILOGE("Windows message init failed");
        return WINDOWS_MSG_INIT_FAIL;
    }
#ifdef OHOS_BUILD_ENABLE_POINTER
    MMI_HILOGD("PointerDrawingManager Init");
    if (!IPointerDrawingManager::GetInstance()->Init()) {
        MMI_HILOGE("Pointer draw init failed");
        return POINTER_DRAW_INIT_FAIL;
    }
#endif // OHOS_BUILD_ENABLE_POINTER
    MMI_HILOGD("Create epoll Init");
    mmiFd_ = EpollCreat(MAX_EVENT_SIZE);
    if (mmiFd_ < 0) {
        MMI_HILOGE("Epoll creat failed");
        return EPOLL_CREATE_FAIL;
    }
    MMI_HILOGD("InitService Init");
    if (!InitService()) {
        MMI_HILOGE("Saservice init failed");
        return SASERVICE_INIT_FAIL;
    }
    MMI_HILOGD("InitLibinputService Init");
    if (!InitLibinputService()) {
        MMI_HILOGE("Libinput init failed");
        return LIBINPUT_INIT_FAIL;
    }
    MMI_HILOGD("Server Message Handler Bind");
    SetRecvFun(std::bind(&ServerMsgHandler::OnMsgHandler, &sMsgHandler_, std::placeholders::_1, std::placeholders::_2));
    MMI_HILOGD("Init eave");
    return RET_OK;
}

void MMIService::OnStart()
{
    CALL_LOG_ENTER;
    int sleepSeconds = 3;
    sleep(sleepSeconds);
    CHK_PIDANDTID();
    int32_t ret = Init();
    if (RET_OK != ret) {
        MMI_HILOGE("Init mmi_service failed");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    MMI_HILOGD("Started successfully");
    t_ = std::thread(std::bind(&MMIService::OnThread, this));
#ifdef OHOS_RSS_CLIENT
    AddSystemAbilityListener(RES_SCHED_SYS_ABILITY_ID);
#endif
    t_.join();
}

void MMIService::OnStop()
{
    CALL_LOG_ENTER;
    CHK_PIDANDTID();
    UdsStop();
    southEventHandler_.Clear();
    libinputAdapter_.Stop();
    state_ = ServiceRunningState::STATE_NOT_START;
#ifdef OHOS_RSS_CLIENT
    RemoveSystemAbilityListener(RES_SCHED_SYS_ABILITY_ID);
#endif
}

void MMIService::OnDump()
{
    CALL_LOG_ENTER;
    CHK_PIDANDTID();
    MMIEventDump->Dump();
}

void MMIService::OnConnected(SessionPtr s)
{
    CHKPV(s);
    int32_t fd = s->GetFd();
    MMI_HILOGI("fd:%{public}d", fd);
}

void MMIService::OnDisconnected(SessionPtr s)
{
    CHKPV(s);
    int32_t fd = s->GetFd();
    MMI_HILOGW("enter, session desc:%{public}s, fd: %{public}d", s->GetDescript().c_str(), fd);
#ifdef OHOS_BUILD_ENABLE_POINTER
    IPointerDrawingManager::GetInstance()->DeletePointerVisible(s->GetPid());
#endif // OHOS_BUILD_ENABLE_POINTER
}

int32_t MMIService::AllocSocketFd(const std::string &programName, const int32_t moduleType, int32_t &toReturnClientFd)
{
    CALL_LOG_ENTER;
    MMI_HILOGI("enter, programName:%{public}s,moduleType:%{public}d", programName.c_str(), moduleType);

    toReturnClientFd = INVALID_SOCKET_FD;
    int32_t serverFd = INVALID_SOCKET_FD;
    int32_t uid = GetCallingUid();
    int32_t pid = GetCallingPid();
    const int32_t ret = AddSocketPairInfo(programName, moduleType, uid, pid, serverFd, toReturnClientFd);
    if (ret != RET_OK) {
        MMI_HILOGE("call AddSocketPairInfo return %{public}d", ret);
        return RET_ERR;
    }

    MMI_HILOGIK("leave, programName:%{public}s,moduleType:%{public}d,alloc success",
        programName.c_str(), moduleType);

    return RET_OK;
}

int32_t MMIService::StubHandleAllocSocketFd(MessageParcel& data, MessageParcel& reply)
{
    sptr<ConnectReqParcel> req = data.ReadParcelable<ConnectReqParcel>();
    CHKPR(req, RET_ERR);
    MMI_HILOGIK("clientName:%{public}s,moduleId:%{public}d", req->data.clientName.c_str(), req->data.moduleId);

    int32_t clientFd = INVALID_SOCKET_FD;
    int32_t ret = AllocSocketFd(req->data.clientName, req->data.moduleId, clientFd);
    if (ret != RET_OK) {
        MMI_HILOGE("call AddSocketPairInfo return %{public}d", ret);
        reply.WriteInt32(RET_ERR);
        return RET_ERR;
    }

    MMI_HILOGI("call AllocSocketFd success");

    reply.WriteInt32(RET_OK);
    reply.WriteFileDescriptor(clientFd);

    MMI_HILOGI("send clientFd to client, clientFd = %{public}d", clientFd);
    close(clientFd);
    return RET_OK;
}

int32_t MMIService::AddInputEventFilter(sptr<IEventFilter> filter)
{
    return southEventHandler_.AddFilter(filter);
}

int32_t MMIService::SetPointerVisible(bool visible)
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_ENABLE_POINTER
    IPointerDrawingManager::GetInstance()->SetPointerVisible(GetCallingPid(), visible);
#endif // OHOS_BUILD_ENABLE_POINTER
    return RET_OK;
}

int32_t MMIService::IsPointerVisible(bool &visible)
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_ENABLE_POINTER
    visible = IPointerDrawingManager::GetInstance()->IsPointerVisible();
#endif
    return RET_OK;
}

int32_t MMIService::HandleNonConsumedTouchEvent(std::shared_ptr<PointerEvent> event)
{
    return southEventHandler_.HandleTouchEvent(event);
}

int32_t MMIService::HandleTimerPointerEvent(std::shared_ptr<PointerEvent> event)
{
    return southEventHandler_.HandlePointerEvent(event);
}

std::shared_ptr<KeyEvent> MMIService::GetKeyEvent() const
{
    return southEventHandler_.GetKeyEvent();    
}

#ifdef OHOS_RSS_CLIENT
void MMIService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    if (systemAbilityId == RES_SCHED_SYS_ABILITY_ID) {
        int sleepSeconds = 1;
        sleep(sleepSeconds);
        uint64_t tid = tid_.load();
        std::unordered_map<std::string, std::string> payload;
        payload["uid"] = std::to_string(getuid());
        payload["pid"] = std::to_string(getpid());
        ResourceSchedule::ResSchedClient::GetInstance().ReportData(
            ResourceSchedule::ResType::RES_TYPE_REPORT_MMI_PROCESS, tid, payload);
    }
}
#endif

void MMIService::OnTimer()
{
    southEventHandler_.OnCheckEventReport();
    TimerMgr->ProcessTimers();
}

void MMIService::OnThread()
{
    SetThreadName(std::string("mmi_service"));
    uint64_t tid = GetThisThreadId();
    MMI_HILOGI("Main worker thread start. tid:%{public}" PRId64 "", tid);
#ifdef OHOS_RSS_CLIENT
    tid_.store(tid);
#endif

    int32_t count = 0;
    constexpr int32_t timeOut = 1;
    struct epoll_event ev[MAX_EVENT_SIZE] = {};
    while (state_ == ServiceRunningState::STATE_RUNNING) {
        count = EpollWait(ev[0], MAX_EVENT_SIZE, timeOut, mmiFd_);
        for (int32_t i = 0; i < count && state_ == ServiceRunningState::STATE_RUNNING; i++) {
            auto mmiEd = reinterpret_cast<mmi_epoll_event*>(ev[i].data.ptr);
            CHKPC(mmiEd);
            if (mmiEd->event_type == EPOLL_EVENT_INPUT) {
                libinputAdapter_.EventDispatch(ev[i]);
            } else if (mmiEd->event_type == EPOLL_EVENT_SOCKET) {
                OnEpollEvent(ev[i]);
            } else if (mmiEd->event_type == EPOLL_EVENT_SIGNAL) {
                OnSignalEvent(mmiEd->fd);
            } else {
                MMI_HILOGW("unknown epoll event type:%{public}d", mmiEd->event_type);
            }
        }
        if (state_ != ServiceRunningState::STATE_RUNNING) {
            break;
        }
        OnTimer();
    }
    MMI_HILOGI("Main worker thread stop. tid:%{public}" PRId64 "", tid);
}

bool MMIService::InitSignalHandler()
{
    CALL_LOG_ENTER;
    sigset_t mask = {0};
    int32_t retCode = sigfillset(&mask);
    if (retCode < 0) {
        MMI_HILOGE("fill signal set failed:%{public}d", errno);
        return false;
    }

    retCode = sigprocmask(SIG_SETMASK, &mask, nullptr);
    if (retCode < 0) {
        MMI_HILOGE("sigprocmask failed:%{public}d", errno);
        return false;
    }

    int32_t fdSignal = signalfd(-1, &mask, SFD_NONBLOCK|SFD_CLOEXEC);
    if (fdSignal < 0) {
        MMI_HILOGE("signal fd failed:%{public}d", errno);
        return false;
    }

    retCode = AddEpoll(EPOLL_EVENT_SIGNAL, fdSignal);
    if (retCode < 0) {
        MMI_HILOGE("AddEpoll signalFd failed:%{public}d", retCode);
        close(fdSignal);
        return false;
    }
    return true;
}

void MMIService::OnSignalEvent(int32_t signalFd)
{
    CALL_LOG_ENTER;
    signalfd_siginfo sigInfo;
    int32_t size = ::read(signalFd, &sigInfo, sizeof(signalfd_siginfo));
    if (size != static_cast<int32_t>(sizeof(signalfd_siginfo))) {
        MMI_HILOGE("read signal info faild, invalid size:%{public}d,errno:%{public}d", size, errno);
        return;
    }
    int32_t signo = static_cast<int32_t>(sigInfo.ssi_signo);
    MMI_HILOGD("receive signal:%{public}d", signo);
    switch (signo) {
        case SIGINT:
        case SIGQUIT:
        case SIGILL:
        case SIGABRT:
        case SIGBUS:
        case SIGFPE:
        case SIGKILL:
        case SIGSEGV:
        case SIGTERM: {
            state_ = ServiceRunningState::STATE_EXIT;
            break;
        }
        default: {
            break;
        }
    }
}
} // namespace MMI
} // namespace OHOS
