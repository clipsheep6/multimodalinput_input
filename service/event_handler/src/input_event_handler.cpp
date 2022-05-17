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
    udsServer_ = nullptr;
    notifyDeviceChange_ = nullptr;
}

InputEventHandler::~InputEventHandler() {}

void InputEventHandler::Init(UDSServer& udsServer)
{
    udsServer_ = &udsServer;
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
    keyEventHandler_ = std::make_shared<KeyEventHandler>();
    pointerEventHandler_ = std::make_shared<PointerEventHandler>();
    touchEventHandler_ = std::make_shared<TouchEventHandler>();
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    CHKPV(keyEventHandler_);
    keyInterceptorHandler_  = IInterceptorHandlerGlobal::CreateInstance();
    CHKPV(keyInterceptorHandler_);
    keyEventHandler_->SetNext(keyInterceptorHandler_);
    auto keyCommandHandler_ = IKeyCommandManager::GetInstance();
    CHKPV(keyCommandHandler_);
    keyInterceptorHandler_->SetNext(keyCommandHandler_);
    keySubscriberHandler_ = std::make_shared<KeyEventSubscriber>();
    CHKPV(keySubscriberHandler_);
    keyCommandHandler_->SetNext(keySubscriberHandler_);
    keyMonitorHandler_ = std::make_shared<InputHandlerManagerGlobal>();
    CHKPV(keyMonitorHandler_);
    keySubscriberHandler_->SetNext(keyMonitorHandler_);
    auto keyDispatchHandler = std::make_shared<EventDispatch>();
    CHKPV(keyDispatchHandler);
    keyMonitorHandler_->SetNext(keyDispatchHandler);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#ifdef OHOS_BUILD_ENABLE_POINTER
    CHKPV(pointerEventHandler_);
    pointerFilterHandler_  = std::make_shared<EventFilterWrap>();
    CHKPV(pointerFilterHandler_);
    pointerEventHandler_->SetNext(pointerFilterHandler_);
    pointerInterceptorHandler_  = IInterceptorHandlerGlobal::CreateInstance();
    CHKPV(pointerInterceptorHandler_);
    pointerFilterHandler_->SetNext(pointerInterceptorHandler_);
    pointerMonitorHandler_ = std::make_shared<InputHandlerManagerGlobal>();
    CHKPV(pointerMonitorHandler_);
    pointerInterceptorHandler_->SetNext(pointerMonitorHandler_);
    auto pointerDispatchHandler = std::make_shared<EventDispatch>();
    CHKPV(pointerDispatchHandler);
    pointerMonitorHandler_->SetNext(pointerDispatchHandler);
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    CHKPV(touchEventHandler_);
    touchFilterHandler_  = std::make_shared<EventFilterWrap>();
    CHKPV(touchFilterHandler_);
    touchEventHandler_->SetNext(touchFilterHandler_);
    touchInterceptorHandler_  = IInterceptorHandlerGlobal::CreateInstance();
    CHKPV(touchInterceptorHandler_);
    touchFilterHandler_->SetNext(touchInterceptorHandler_);
    touchMonitorHandler_ = std::make_shared<InputHandlerManagerGlobal>();
    CHKPV(touchMonitorHandler_);
    touchInterceptorHandler_->SetNext(touchMonitorHandler_);
    auto touchDispatchHandler = std::make_shared<EventDispatch>();
    CHKPV(touchDispatchHandler);
    touchMonitorHandler_->SetNext(touchDispatchHandler);
#endif // OHOS_BUILD_ENABLE_TOUCH
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

UDSServer* InputEventHandler::GetUDSServer() const
{
    return udsServer_;
}
std::shared_ptr<KeyEvent> InputEventHandler::GetKeyEvent() const
{
    return keyEvent_;
}

std::shared_ptr<IInputEventHandler> InputEventHandler::GetKeyEventHandler() const
{
    return keyEventHandler_;
}

std::shared_ptr<IInputEventHandler> InputEventHandler::GetPointerEventHandler() const
{
    return pointerEventHandler_;
}

std::shared_ptr<IInputEventHandler> InputEventHandler::GetTouchEventHandler() const
{
    return touchEventHandler_;
}

std::shared_ptr<IInterceptorHandlerGlobal> InputEventHandler::GetKeyInterceptorHandler() const
{
    return keyInterceptorHandler_;
}

std::shared_ptr<InputHandlerManagerGlobal> InputEventHandler::GetKeyMonitorHandler() const
{
    return keyMonitorHandler_;
}


std::shared_ptr<KeyEventSubscriber> InputEventHandler::GetKeySubscriberHandler() const
{
    return keySubscriberHandler_;
}

std::shared_ptr<EventFilterWrap> InputEventHandler::GetPointerFilterHandler() const
{
    return pointerFilterHandler_;
}

std::shared_ptr<InputHandlerManagerGlobal> InputEventHandler::GetPointerMonitorHandler() const
{
    return pointerMonitorHandler_;
}


std::shared_ptr<IInterceptorHandlerGlobal> InputEventHandler::GetPointerInterceptorHandler() const
{
    return pointerInterceptorHandler_;
}

std::shared_ptr<EventFilterWrap> InputEventHandler::GetTouchFilterHandler() const
{
    return touchFilterHandler_;
}


std::shared_ptr<IInterceptorHandlerGlobal> InputEventHandler::GetTouchInterceptorHandler() const
{
    return touchInterceptorHandler_;
}

std::shared_ptr<InputHandlerManagerGlobal> InputEventHandler::GetTouchMonitorHandler() const
{
    return touchMonitorHandler_;
}

int32_t InputEventHandler::AddInputEventFilter(sptr<IEventFilter> filter)
{
#ifdef OHOS_BUILD_ENABLE_POINTER
    do {
        CHKPB(pointerFilterHandler_);
        pointerFilterHandler_->AddInputEventFilter(filter);
    } while (0);
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    do {
        CHKPB(touchFilterHandler_);
        touchFilterHandler_->AddInputEventFilter(filter);
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