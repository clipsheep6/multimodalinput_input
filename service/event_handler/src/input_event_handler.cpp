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
#include "input_device_manager.h"
#include "i_key_command_manager.h"
#include "mmi_func_callback.h"
#include "libinput_adapter.h"
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
    inputEventHandler_ = BuildInputHandlerChain();
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
    return;
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

std::shared_ptr<IInputEventHandler> InputEventHandler::BuildInputHandlerChain()
{
    inputEventHandler_ = std::make_shared<IInputEventHandler>();
    CHKPP(inputEventHandler_);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    keyEventHandler_ = std::make_shared<KeyEventHandler>();
    CHKPP(keyEventHandler_);
    keyInterceptor_ = IInterceptorManagerGlobal::CreateInstance();
    auto keyCommandHandler = IKeyCommandManager::CreateInstance();
    keySubscriber_ = std::make_shared<KeyEventSubscriber>();
    keyMonitorHandler_ = std::make_shared<InputHandlerManagerGlobal>();
    auto keyDispatch = std::make_shared<EventDispatch>();
    keyEventHandler_->AddHandler(1, keyInterceptor_);
    keyEventHandler_->AddHandler(2, keyCommandHandler);
    keyEventHandler_->AddHandler(3, keySubscriber_);
    keyEventHandler_->AddHandler(4, keyMonitorHandler_);
    keyEventHandler_->AddHandler(5, keyDispatch);
    keyEventHandler_->AddFinish();
#endif
#ifdef OHOS_BUILD_ENABLE_POINTER
    pointerEventHandler_ = std::make_shared<PointerEventHandler>();
    pointerEventFilter_ = std::make_shared<EventFilterWrap>();
    pointerInterceptor_ = IInterceptorHandlerGlobal::CreateInstance();
    pointerMonitorHandler_ = std::make_shared<InputHandlerManagerGlobal>();
    auto pointerDispatch = std::make_shared<EventDispatch>();
    pointerEventHandler_->AddHandler(1, pointerEventFilter_);
    pointerEventHandler_->AddHandler(2, pointerInterceptor_);
    pointerEventHandler_->AddHandler(3, pointerMonitorHandler_);
    pointerEventHandler_->AddHandler(4, pointerDispatch);
    pointerEventHandler_->AddFinish();
#endif
#ifdef OHOS_BUILD_ENABLE_TOUCH
    touchEventHandler_ = std::make_shared<TouchEventHandler>();
    touchEventFilter_ = std::make_shared<EventFilterWrap>();
    touchInterceptor_ = IInterceptorHandlerGlobal::CreateInstance();
    touchIMonitorHandler_ = std::make_shared<InputHandlerManagerGlobal>();
    auto touchDispatch = std::make_shared<EventDispatch>();
    touchEventHandler_->AddHandler(1, touchEventFilter_);
    touchEventHandler_->AddHandler(2, touchInterceptor_);
    touchEventHandler_->AddHandler(3, touchIMonitorHandler_);
    touchEventHandler_->AddHandler(4, touchDispatch);
    touchEventHandler_->AddFinish();
#endif
    return inputEventHandler_;
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

std::shared_ptr<IInputEventHandler> InputEventHandler::GetInputHandlerChain() const
{
    return inputEventHandler_;
}

std::shared_ptr<KeyEventHandler> InputEventHandler::GetKeyEventHandler() const
{
    return keyEventHandler_;
}

std::shared_ptr<PointerEventHandler> InputEventHandler::GetPointerEventHandler() const
{
    return pointerEventHandler_;
}

std::shared_ptr<TouchEventHandler> InputEventHandler::GetTouchEventHandler() const
{
    return touchEventHandler_;
}

std::shared_ptr<IInterceptorManagerGlobal> InputEventHandler::GetKeyInterceptor() const
{
    return keyInterceptor_;
}

std::shared_ptr<KeyEventSubscriber> InputEventHandler::GetKeySubscriber() const
{
    return keySubscriber_;
}

std::shared_ptr<InputHandlerManagerGlobal> InputEventHandler::GetKeyInputHandlerMgr() const
{
    return keyMonitorHandler_;
}

std::shared_ptr<EventFilterWrap> InputEventHandler::GetPointerEventFilter() const
{
    return pointerEventFilter_;
}

std::shared_ptr<IInterceptorHandlerGlobal> InputEventHandler::GetPointerInterceptorMgr() const
{
    return pointerInterceptor_;
}

std::shared_ptr<InputHandlerManagerGlobal> InputEventHandler::GetPointerInputHandlerMgr() const
{
    return pointerMonitorHandler_;
}

std::shared_ptr<EventFilterWrap> InputEventHandler::GetTouchEventFilter() const
{
    return touchEventFilter_;
}

std::shared_ptr<IInterceptorHandlerGlobal> InputEventHandler::GetTouchInterceptorMgr() const
{
    return touchInterceptor_;
}

std::shared_ptr<InputHandlerManagerGlobal> InputEventHandler::GetTouchInputHandlerMgr() const
{
    return touchIMonitorHandler_;
}

int32_t InputEventHandler::AddInputEventFilter(sptr<IEventFilter> filter)
{
#ifdef OHOS_BUILD_ENABLE_POINTER
    do {
        CHKPB(pointerEventFilter_);
        pointerEventFilter_->AddInputEventFilter(filter);
    } while (0);
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    do {
        CHKPB(touchEventFilter_);
        touchEventFilter_->AddInputEventFilter(filter);
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
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    CHKPR(keyEventHandler_, ERROR_NULL_POINTER);
    keyEventHandler_->HandleEvent(event);
#else
    CHKPR(inputEventHandler_, ERROR_NULL_POINTER);
    inputEventHandler_->HandleKeyEvent(nullptr);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    return RET_OK;
}

int32_t InputEventHandler::OnEventPointer(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    if (keyEvent_ == nullptr) {
        keyEvent_ = KeyEvent::Create();
    }
#ifdef OHOS_BUILD_ENABLE_POINTER
    CHKPR(pointerEventHandler_, ERROR_NULL_POINTER);
    pointerEventHandler_->HandleEvent(event);
#else
    CHKPR(inputEventHandler_, ERROR_NULL_POINTER);
    inputEventHandler_->HandlePointerEvent(nullptr);
#endif // OHOS_BUILD_ENABLE_POINTER
    return RET_OK;
}

int32_t InputEventHandler::OnEventTouchpad(libinput_event *event)
{
    CALL_LOG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
#ifdef OHOS_BUILD_ENABLE_POINTER
    CHKPR(pointerEventHandler_, ERROR_NULL_POINTER);
    pointerEventHandler_->HandleEvent(event);
#else
    CHKPR(inputEventHandler_, ERROR_NULL_POINTER);
    inputEventHandler_->HandlePointerEvent(nullptr);
#endif // OHOS_BUILD_ENABLE_POINTER
    return RET_OK;
}

int32_t InputEventHandler::OnEventGesture(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
#ifdef OHOS_BUILD_ENABLE_POINTER
    CHKPR(pointerEventHandler_, ERROR_NULL_POINTER);
    int32_t ret = pointerEventHandler_->HandleEvent(event);
    if (ret != RET_OK) {
        MMI_HILOGE("Gesture event dispatch failed, errCode:%{public}d", GESTURE_EVENT_DISP_FAIL);
        return GESTURE_EVENT_DISP_FAIL;
    }
#else
    CHKPR(inputEventHandler_, ERROR_NULL_POINTER);
    inputEventHandler_->HandlePointerEvent(nullptr);
#endif // OHOS_BUILD_ENABLE_POINTER
    return RET_OK;
}

int32_t InputEventHandler::OnEventTouch(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    LibinputAdapter::LoginfoPackagingTool(event);
#ifdef OHOS_BUILD_ENABLE_TOUCH
    CHKPR(touchEventHandler_, ERROR_NULL_POINTER);
    touchEventHandler_->HandleEvent(event);
#else
    CHKPR(inputEventHandler_, ERROR_NULL_POINTER);
    inputEventHandler_->HandleTouchEvent(nullptr);
#endif // OHOS_BUILD_ENABLE_TOUCH
    return RET_OK;
}

int32_t InputEventHandler::OnTabletToolEvent(libinput_event *event)
{
    CALL_LOG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
#ifdef OHOS_BUILD_ENABLE_TOUCH
    CHKPR(touchEventHandler_, ERROR_NULL_POINTER);
    touchEventHandler_->HandleEvent(event);
#else
    CHKPR(inputEventHandler_, ERROR_NULL_POINTER);
    inputEventHandler_->HandleTouchEvent(nullptr);
#endif // OHOS_BUILD_ENABLE_TOUCH
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS