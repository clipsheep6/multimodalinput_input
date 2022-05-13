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

#include "input_event_handler.h"

#include <cstdio>
#include <cstring>
#include <functional>
#include <vector>
#include <cinttypes>

#include <sys/stat.h>
#include <unistd.h>

#include "hitrace_meter.h"
#include "libinput.h"

#include "bytrace_adapter.h"
#include "i_key_command_manager.h"
#include "input_device_manager.h"
#include "libinput_adapter.h"
#include "mmi_func_callback.h"
#include "time_cost_chk.h"
#include "timer_manager.h"
#include "touch_transform_point_manager.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputEventHandler" };
} // namespace

InputEventHandler::InputEventHandler()
{
    notifyDeviceChange_ = nullptr;
}

InputEventHandler::~InputEventHandler() {}

void InputEventHandler::Init()
{
    BuildInputHandlerChain();
    MsgCallback funs[] = {
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_DEVICE_ADDED),
            MsgCallbackBind1(&InputEventHandler::OnEventDeviceAdded, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_DEVICE_REMOVED),
            MsgCallbackBind1(&InputEventHandler::OnEventDeviceRemoved, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_KEYBOARD_KEY),
            MsgCallbackBind1(&InputEventHandler::OnEventKey, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_POINTER_MOTION),
            MsgCallbackBind1(&InputEventHandler::OnEventPointer, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE),
            MsgCallbackBind1(&InputEventHandler::OnEventPointer, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_POINTER_BUTTON),
            MsgCallbackBind1(&InputEventHandler::OnEventPointer, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_POINTER_AXIS),
            MsgCallbackBind1(&InputEventHandler::OnEventPointer, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_TOUCH_DOWN),
            MsgCallbackBind1(&InputEventHandler::OnEventTouch, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_TOUCH_UP),
            MsgCallbackBind1(&InputEventHandler::OnEventTouch, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_TOUCH_MOTION),
            MsgCallbackBind1(&InputEventHandler::OnEventTouch, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_TOUCH_CANCEL),
            MsgCallbackBind1(&InputEventHandler::OnEventTouch, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_TOUCH_FRAME),
            MsgCallbackBind1(&InputEventHandler::OnEventTouch, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_TOUCHPAD_DOWN),
            MsgCallbackBind1(&InputEventHandler::OnEventTouchpad, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_TOUCHPAD_UP),
            MsgCallbackBind1(&InputEventHandler::OnEventTouchpad, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_TOUCHPAD_MOTION),
            MsgCallbackBind1(&InputEventHandler::OnEventTouchpad, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_TABLET_TOOL_AXIS),
            MsgCallbackBind1(&InputEventHandler::OnTabletToolEvent, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY),
            MsgCallbackBind1(&InputEventHandler::OnTabletToolEvent, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_TABLET_TOOL_TIP),
            MsgCallbackBind1(&InputEventHandler::OnTabletToolEvent, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN),
            MsgCallbackBind1(&InputEventHandler::OnEventGesture, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE),
            MsgCallbackBind1(&InputEventHandler::OnEventGesture, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_GESTURE_SWIPE_END),
            MsgCallbackBind1(&InputEventHandler::OnEventGesture, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_GESTURE_PINCH_BEGIN),
            MsgCallbackBind1(&InputEventHandler::OnEventGesture, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_GESTURE_PINCH_UPDATE),
            MsgCallbackBind1(&InputEventHandler::OnEventGesture, this)
        },
        {
            static_cast<MmiMessageId>(LIBINPUT_EVENT_GESTURE_PINCH_END),
            MsgCallbackBind1(&InputEventHandler::OnEventGesture, this)
        },
    };
    for (auto &item : funs) {
        if (!RegistrationEvent(item)) {
            MMI_HILOGW("Failed to register event errCode:%{public}d", EVENT_REG_FAIL);
            continue;
        }
    }
}

void InputEventHandler::OnEvent(void *event)
{
    CHKPV(event);
    auto *lpEvent = static_cast<libinput_event *>(event);
    CHKPV(lpEvent);
    if (initSysClock_ != 0 && lastSysClock_ == 0) {
        MMI_HILOGE("Event not handled. id:%{public}" PRId64 ",eventType:%{public}d,initSysClock:%{public}" PRId64,
                   idSeed_, eventType_, initSysClock_);
    }

    eventType_ = libinput_event_get_type(lpEvent);
    initSysClock_ = GetSysClockTime();
    lastSysClock_ = 0;
    idSeed_ += 1;
    const uint64_t maxUInt64 = (std::numeric_limits<uint64_t>::max)() - 1;
    if (idSeed_ >= maxUInt64) {
        MMI_HILOGE("Invaild value. id:%{public}" PRId64, idSeed_);
        idSeed_ = 1;
        return;
    }

    MMI_HILOGD("Event reporting. id:%{public}" PRId64 ",tid:%{public}" PRId64 ",eventType:%{public}d,"
               "initSysClock:%{public}" PRId64, idSeed_, GetThisThreadId(), eventType_, initSysClock_);

    OnEventHandler(lpEvent);
    lastSysClock_ = GetSysClockTime();
    int64_t lostTime = lastSysClock_ - initSysClock_;
    MMI_HILOGD("Event handling completed. id:%{public}" PRId64 ",lastSynClock:%{public}" PRId64
               ",lostTime:%{public}" PRId64, idSeed_, lastSysClock_, lostTime);
}

int32_t InputEventHandler::OnEventHandler(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    auto type = libinput_event_get_type(event);
    TimeCostChk chk("InputEventHandler::OnEventHandler", "overtime 1000(us)", MAX_INPUT_EVENT_TIME, type);
    auto callback = GetMsgCallback(static_cast<MmiMessageId>(type));
    if (callback == nullptr) {
        MMI_HILOGE("Unknown event type:%{public}d,errCode:%{public}d", type, UNKNOWN_EVENT);
        return UNKNOWN_EVENT;
    }
    auto ret = (*callback)(event);
    if (ret != 0) {
        MMI_HILOGE("Event handling failed. type:%{public}d,ret:%{public}d,errCode:%{public}d",
                   type, ret, EVENT_CONSUM_FAIL);
        return ret;
    }
    return ret;
}

void InputEventHandler::BuildInputHandlerChain()
{
    keyEventHandler_ = std::make_shared<IInputEventHandler>();
    pointerEventHandler_ = std::make_shared<IInputEventHandler>();
    touchEventHandler_ = std::make_shared<IInputEventHandler>();
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    keyEventHandler_->AddConstructHandler<KeyEventHandler>(1);
    keyEventHandler_->AddInstanceHandler<IInterceptorHandlerGlobal>(1);
    keyEventHandler_->AddInstanceHandler<IKeyCommandManager>(1);
    keyEventHandler_->AddConstructHandler<KeyEventSubscriber>(1);
    keyEventHandler_->AddConstructHandler<InputHandlerManagerGlobal>(1);
    keyEventHandler_->AddConstructHandler<EventDispatch>(1);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#ifdef OHOS_BUILD_ENABLE_POINTER
    pointerEventHandler_->AddConstructHandler<PointerEventHandler>(1);
    pointerEventHandler_->AddConstructHandler<EventFilterWrap>(1);
    pointerEventHandler_->AddInstanceHandler<IInterceptorHandlerGlobal>(1);
    pointerEventHandler_->AddConstructHandler<InputHandlerManagerGlobal>(1);
    pointerEventHandler_->AddConstructHandler<EventDispatch>(1);
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    touchEventHandler_->AddConstructHandler<TouchEventHandler>(1);
    touchEventHandler_->AddConstructHandler<EventFilterWrap>(1);
    touchEventHandler_->AddInstanceHandler<IInterceptorHandlerGlobal>(1);
    touchEventHandler_->AddConstructHandler<InputHandlerManagerGlobal>(1);
    touchEventHandler_->AddConstructHandler<EventDispatch>(1);
#endif // OHOS_BUILD_ENABLE_TOUCH
}

int32_t InputEventHandler::HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPR(keyEventHandler_, ERROR_NULL_POINTER);
    return keyEventHandler_->HandleKeyEvent(keyEvent);
}

int32_t InputEventHandler::HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPR(pointerEventHandler_, ERROR_NULL_POINTER);
    return pointerEventHandler_->HandlePointerEvent(pointerEvent);
}

int32_t InputEventHandler::HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPR(touchEventHandler_, ERROR_NULL_POINTER);
    return touchEventHandler_->HandleTouchEvent(pointerEvent);
}

int32_t InputEventHandler::AddKeyInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPR(keyEventHandler_, ERROR_NULL_POINTER);
    return keyEventHandler_->AddInterceptor(handlerId, handlerType, session);
}

int32_t InputEventHandler::AddPointerInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPR(pointerEventHandler_, ERROR_NULL_POINTER);
    return pointerEventHandler_->AddInterceptor(handlerId, handlerType, session);
}

void InputEventHandler::RemoveKeyInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPV(keyEventHandler_);
    keyEventHandler_->RemoveInterceptor(handlerId, handlerType, session);
}

void InputEventHandler::RemovePointerInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPV(pointerEventHandler_);
    pointerEventHandler_->RemoveInterceptor(handlerId, handlerType, session);
}

int32_t InputEventHandler::AddTouchInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPR(touchEventHandler_, ERROR_NULL_POINTER);
    return touchEventHandler_->AddInterceptor(handlerId, handlerType, session);
}

void InputEventHandler::RemoveTouchInterceptor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPV(touchEventHandler_);
    touchEventHandler_->RemoveInterceptor(handlerId, handlerType, session);
}

void InputEventHandler::AddKeyMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPV(keyEventHandler_);
    keyEventHandler_->AddMonitor(handlerId, handlerType, session); 
}

void InputEventHandler::RemoveKeyMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPV(keyEventHandler_);
    keyEventHandler_->RemoveMonitor(handlerId, handlerType, session);
}

void InputEventHandler::AddPointerMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPV(pointerEventHandler_);
    pointerEventHandler_->AddMonitor(handlerId, handlerType, session);
}

void InputEventHandler::RemovePointerMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPV(pointerEventHandler_);
    pointerEventHandler_->RemoveMonitor(handlerId, handlerType, session);
}

void InputEventHandler::AddTouchMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPV(touchEventHandler_);
    touchEventHandler_->AddMonitor(handlerId, handlerType, session);
}

void InputEventHandler::RemoveTouchMonitor(int32_t handlerId, InputHandlerType handlerType, SessionPtr session)
{
    CHKPV(touchEventHandler_);
    touchEventHandler_->RemoveMonitor(handlerId, handlerType, session);
}
void InputEventHandler::TouchMonitorHandlerMarkConsumed(int32_t monitorId, int32_t eventId, SessionPtr sess)
{
    CHKPV(touchEventHandler_);
    touchEventHandler_->TouchMonitorHandlerMarkConsumed(monitorId, eventId, sess);
}

int32_t InputEventHandler::AddSubscriber(SessionPtr sess, int32_t subscribeId,
    const std::shared_ptr<KeyOption> keyOption)
{
    CHKPR(keyEventHandler_, ERROR_NULL_POINTER);
    return keyEventHandler_->AddSubscriber(sess, subscribeId, keyOption);
}

int32_t InputEventHandler::RemoveSubscriber(SessionPtr sess, int32_t subscribeId)
{
    CHKPR(keyEventHandler_, ERROR_NULL_POINTER);
    return keyEventHandler_->RemoveSubscriber(sess, subscribeId);
}

void InputEventHandler::OnCheckEventReport()
{
    if (initSysClock_ == 0 || lastSysClock_ != 0) {
        return;
    }
    constexpr int64_t MAX_DID_TIME = 1000 * 1000 * 3;
    auto curSysClock = GetSysClockTime();
    auto lostTime = curSysClock - initSysClock_;
    if (lostTime < MAX_DID_TIME) {
        return;
    }
    MMI_HILOGE("Event not responding. id:%{public}" PRId64 ",eventType:%{public}d,initSysClock:%{public}" PRId64 ","
               "lostTime:%{public}" PRId64, idSeed_, eventType_, initSysClock_, lostTime);
}

std::shared_ptr<KeyEvent> InputEventHandler::GetKeyEvent() const
{
    return keyEvent_;
}

int32_t InputEventHandler::AddFilter(sptr<IEventFilter> filter)
{
#ifdef OHOS_BUILD_ENABLE_POINTER
    do {
        CHKPB(pointerEventHandler_);
        pointerEventHandler_->AddFilter(filter);
    } while (0);
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    do {
        CHKPB(touchEventHandler_);
        touchEventHandler_->AddFilter(filter);
    } while (0);
#endif // OHOS_BUILD_ENABLE_TOUCH
    return RET_OK;
}

int32_t InputEventHandler::OnEventDeviceAdded(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    auto device = libinput_event_get_device(event);
    InputDevMgr->OnInputDeviceAdded(device);
    return RET_OK;
}
int32_t InputEventHandler::OnEventDeviceRemoved(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    auto device = libinput_event_get_device(event);
    InputDevMgr->OnInputDeviceRemoved(device);
    return RET_OK;
}

int32_t InputEventHandler::OnEventKey(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    if (keyEvent_ == nullptr) {
        keyEvent_ = KeyEvent::Create();
    }
    CHKPR(keyEventHandler_, ERROR_NULL_POINTER);
    keyEventHandler_->HandleLibinputEvent(event);
    return RET_OK;
}

int32_t InputEventHandler::OnEventPointer(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    if (keyEvent_ == nullptr) {
        keyEvent_ = KeyEvent::Create();
    }
    CHKPR(pointerEventHandler_, ERROR_NULL_POINTER);
    pointerEventHandler_->HandleLibinputEvent(event);
    return RET_OK;
}

int32_t InputEventHandler::OnEventTouchpad(libinput_event *event)
{
    CALL_LOG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    CHKPR(pointerEventHandler_, ERROR_NULL_POINTER);
    pointerEventHandler_->HandleLibinputEvent(event);
    return RET_OK;
}

int32_t InputEventHandler::OnEventGesture(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    CHKPR(pointerEventHandler_, ERROR_NULL_POINTER);
    int32_t ret = pointerEventHandler_->HandleLibinputEvent(event);
    if (ret != RET_OK) {
        MMI_HILOGE("Gesture event dispatch failed, errCode:%{public}d", GESTURE_EVENT_DISP_FAIL);
        return GESTURE_EVENT_DISP_FAIL;
    }
    return RET_OK;
}

int32_t InputEventHandler::OnEventTouch(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    LibinputAdapter::LoginfoPackagingTool(event);
    CHKPR(touchEventHandler_, ERROR_NULL_POINTER);
    touchEventHandler_->HandleLibinputEvent(event);
    return RET_OK;
}

int32_t InputEventHandler::OnTabletToolEvent(libinput_event *event)
{
    CALL_LOG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    CHKPR(touchEventHandler_, ERROR_NULL_POINTER);
    touchEventHandler_->HandleLibinputEvent(event);
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS