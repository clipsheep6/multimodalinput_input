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

#include "event_dispatch.h"

#include <cinttypes>

#include "ability_manager_client.h"
#include "bytrace.h"
#include "input-event-codes.h"
#include "hisysevent.h"

#include "bytrace_adapter.h"
#include "error_multimodal.h"
#include "event_filter_wrap.h"
#include "input_event_data_transformation.h"
#include "input_event_handler.h"
#include "input_handler_manager_global.h"
#include "input_windows_manager.h"
#include "util.h"


namespace OHOS {
namespace MMI {
constexpr int64_t INPUT_UI_TIMEOUT_TIME = 5 * 1000000;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventDispatch" };
} // namespace

EventDispatch::EventDispatch() {}

EventDispatch::~EventDispatch() {}

void EventDispatch::OnEventTouchGetPointEventType(const EventTouch& touch,
                                                  const int32_t fingerCount,
                                                  POINT_EVENT_TYPE& pointEventType)
{
    if (fingerCount <= 0 || touch.time <= 0 || touch.seatSlot < 0 || touch.eventType < 0) {
        MMI_HILOGE("The in parameter is error, fingerCount:%{public}d, touch.time:%{public}" PRId64 ","
                   "touch.seatSlot:%{public}d, touch.eventType:%{public}d",
                   fingerCount, touch.time, touch.seatSlot, touch.eventType);
        return;
    }
    if (fingerCount == 1) {
        switch (touch.eventType) {
            case LIBINPUT_EVENT_TOUCH_DOWN: {
                pointEventType = PRIMARY_POINT_DOWN;
                break;
            }
            case LIBINPUT_EVENT_TOUCH_UP: {
                pointEventType = PRIMARY_POINT_UP;
                break;
            }
            case LIBINPUT_EVENT_TOUCH_MOTION: {
                pointEventType = POINT_MOVE;
                break;
            }
            default: {
                break;
            }
        }
    } else {
        switch (touch.eventType) {
            case LIBINPUT_EVENT_TOUCH_DOWN: {
                pointEventType = OTHER_POINT_DOWN;
                break;
            }
            case LIBINPUT_EVENT_TOUCH_UP: {
                pointEventType = OTHER_POINT_UP;
                break;
            }
            case LIBINPUT_EVENT_TOUCH_MOTION: {
                pointEventType = POINT_MOVE;
                break;
            }
            default: {
                break;
            }
        }
    }
}

int32_t EventDispatch::HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
#ifdef OHOS_BUILD_KEYBOARD
    CALL_LOG_ENTER;
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    auto fd = WinMgr->UpdateTarget(keyEvent);
    if (fd < 0) {
        MMI_HILOGE("Invalid fd, fd: %{public}d", fd);
        return RET_ERR;
    }
    MMI_HILOGD("event dispatcher of server:KeyEvent:KeyCode:%{public}d,"
               "ActionTime:%{public}" PRId64 ",Action:%{public}d,ActionStartTime:%{public}" PRId64 ","
               "EventType:%{public}d,Flag:%{public}u,"
               "KeyAction:%{public}d,Fd:%{public}d",
               keyEvent->GetKeyCode(), keyEvent->GetActionTime(), keyEvent->GetAction(),
               keyEvent->GetActionStartTime(),
               keyEvent->GetEventType(),
               keyEvent->GetFlag(), keyEvent->GetKeyAction(), fd);

    InputHandlerMgrGlobal->HandleEvent(keyEvent);
    auto udsServer = InputHandler->GetUDSServer();
    CHKPR(udsServer, ERROR_NULL_POINTER);
    auto session = udsServer->GetSession(fd);
    CHKPF(session);
    if (session->isANRProcess_) {
        MMI_HILOGD("is ANR process");
        return RET_OK;
    }

    auto currentTime = GetSysClockTime();
    if (TriggerANR(currentTime, session)) {
        session->isANRProcess_ = true;
        MMI_HILOGW("the key event does not report normally, triggering ANR");
         return RET_OK;
    }

    NetPacket pkt(MmiMessageId::ON_KEYEVENT);
    InputEventDataTransformation::KeyEventToNetPacket(keyEvent, pkt);
    BytraceAdapter::StartBytrace(keyEvent, BytraceAdapter::KEY_DISPATCH_EVENT);
    pkt << fd;
    if (!udsServer->SendMsg(fd, pkt)) {
        MMI_HILOGE("Sending structure of EventKeyboard failed! errCode:%{public}d", MSG_SEND_FAIL);
        return MSG_SEND_FAIL;
    }
    session->AddEvent(keyEvent->GetId(), currentTime);
#endif
     return RET_OK;
}

int32_t EventDispatch::HandlePointerEvent(std::shared_ptr<PointerEvent> pointEvent)
{
#ifdef OHOS_BUILD_MOUSE
    CALL_LOG_ENTER;
    CHKPR(pointEvent, ERROR_NULL_POINTER);
    return DispatchPointerEvent(pointEvent);
#else
    return RET_OK;
#endif
}

int32_t EventDispatch::HandleTouchEvent(std::shared_ptr<PointerEvent> pointEvent)
{
#ifdef OHOS_BUILD_TOUCH
    CALL_LOG_ENTER;
    CHKPR(pointEvent, ERROR_NULL_POINTER);
    return DispatchPointerEvent(pointEvent);
#else
    return RET_OK;
#endif
}

int32_t EventDispatch::DispatchPointerEvent(std::shared_ptr<PointerEvent> pointEvent)
{
    CHKPR(pointEvent, ERROR_NULL_POINTER);
    auto fd = WinMgr->UpdateTargetPointer(pointEvent);
    if (fd < 0) {
        MMI_HILOGE("The fd less than 0, fd: %{public}d", fd);
        return RET_ERR;
    }
    NetPacket pkt(MmiMessageId::ON_POINTER_EVENT);
    InputEventDataTransformation::Marshalling(pointEvent, pkt);
    BytraceAdapter::StartBytrace(pointEvent, BytraceAdapter::TRACE_STOP);
    auto udsServer = InputHandler->GetUDSServer();
    if (udsServer == nullptr) {
        MMI_HILOGE("UdsServer is a nullptr");
        return RET_ERR;
    }

    auto session = udsServer->GetSession(fd);
    CHKPF(session);
    if (session->isANRProcess_) {
        MMI_HILOGD("is ANR process");
        return RET_OK;
    }

    auto currentTime = GetSysClockTime();
    if (TriggerANR(currentTime, session)) {
        session->isANRProcess_ = true;
        MMI_HILOGW("the pointer event does not report normally, triggering ANR");
        return RET_OK;
    }

    if (!udsServer->SendMsg(fd, pkt)) {
        MMI_HILOGE("Sending structure of EventTouch failed! errCode:%{public}d", MSG_SEND_FAIL);
        return RET_ERR;
    }
    session->AddEvent(pointEvent->GetId(), currentTime);
    return RET_OK;
}

int32_t EventDispatch::AddInputEventFilter(sptr<IEventFilter> filter)
{
    return EventFilterWraper->AddInputEventFilter(filter);
}

bool EventDispatch::TriggerANR(int64_t time, SessionPtr sess)
{
    CALL_LOG_ENTER;
    int64_t earlist;
    if (sess->IsEventQueueEmpty()) {
        earlist = time;
    } else {
        earlist = sess->GetEarlistEventTime();
    }

    if (time < (earlist + INPUT_UI_TIMEOUT_TIME)) {
        sess->isANRProcess_ = false;
        MMI_HILOGD("the event reports normally");
        return false;
    }

    int32_t ret = OHOS::HiviewDFX::HiSysEvent::Write(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTI_MODAL_INPUT,
        "APPLICATION_BLOCK_INPUT",
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "PID", sess->GetPid(),
        "UID", sess->GetUid(),
        "PACKAGE_NAME", "",
        "PROCESS_NAME", "",
        "MSG", "User input does not respond");
    if (ret != 0) {
        MMI_HILOGE("HiviewDFX Write failed, HiviewDFX errCode: %{public}d", ret);
    }

    ret = OHOS::AAFwk::AbilityManagerClient::GetInstance()->SendANRProcessID(sess->GetPid());
    if (ret != 0) {
        MMI_HILOGE("AAFwk SendANRProcessID failed, AAFwk errCode: %{public}d", ret);
    }
    return true;
}
} // namespace MMI
} // namespace OHOS