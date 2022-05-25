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
#include "key_map_manager.h"
#include "key_autorepeat.h"
#include "mmi_func_callback.h"
#include "mouse_event_handler.h"
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
    idSeed_ += 1;
    const uint64_t maxUInt64 = (std::numeric_limits<uint64_t>::max)() - 1;
    if (idSeed_ >= maxUInt64) {
        MMI_HILOGE("value is flipped. id:%{public}" PRId64, idSeed_);
        idSeed_ = 1;
    }

    auto *lpEvent = static_cast<libinput_event *>(event);
    CHKPV(lpEvent);
    int32_t eventType = libinput_event_get_type(lpEvent);
    int64_t beginTime = GetSysClockTime();
    MMI_HILOGD("Event reporting. id:%{public}" PRId64 ",tid:%{public}" PRId64 ",eventType:%{public}d,"
               "beginTime:%{public}" PRId64, idSeed_, GetThisThreadId(), eventType, beginTime);

    OnEventHandler(lpEvent);
    int64_t endTime = GetSysClockTime();
    int64_t lostTime = endTime - beginTime;
    MMI_HILOGD("Event handling completed. id:%{public}" PRId64 ",endTime:%{public}" PRId64
               ",lostTime:%{public}" PRId64, idSeed_, endTime, lostTime);
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

UDSServer* InputEventHandler::GetUDSServer() const
{
    return udsServer_;
}

int32_t InputEventHandler::AddInputEventFilter(sptr<IEventFilter> filter)
{
    return eventDispatch_.AddInputEventFilter(filter);
}

int32_t InputEventHandler::OnEventDeviceAdded(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    auto device = libinput_event_get_device(event);
    CHKPR(device, ERROR_NULL_POINTER);
    InputDevMgr->OnInputDeviceAdded(device);
    KeyMapMgr->ParseDeviceConfigFile(device);
    KeyRepeat->AddDeviceConfig(device);
    return RET_OK;
}

int32_t InputEventHandler::OnEventDeviceRemoved(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    auto device = libinput_event_get_device(event);
    CHKPR(device, ERROR_NULL_POINTER);
    KeyMapMgr->RemoveKeyValue(device);
    KeyRepeat->RemoveDeviceConfig(device);
    InputDevMgr->OnInputDeviceRemoved(device);
    return RET_OK;
}

int32_t InputEventHandler::AddHandleTimer(int32_t timeout)
{
    CALL_LOG_ENTER;
    timerId_ = TimerMgr->AddTimer(timeout, 1, [this]() {
        auto ret = eventDispatch_.DispatchKeyEventPid(*(this->udsServer_), this->keyEvent_);
        if (ret != RET_OK) {
            MMI_HILOGE("KeyEvent dispatch failed. ret:%{public}d,errCode:%{public}d", ret, KEY_EVENT_DISP_FAIL);
        }
        int32_t triggertime = KeyRepeat->GetIntervalTime(keyEvent_->GetDeviceId());
        this->AddHandleTimer(triggertime);
    });
    return timerId_;
}

int32_t InputEventHandler::OnEventKey(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    CHKPR(udsServer_, ERROR_NULL_POINTER);
    if (keyEvent_ == nullptr) {
        keyEvent_ = KeyEvent::Create();
    }

    std::vector<int32_t> pressedKeys = keyEvent_->GetPressedKeys();
    int32_t lastPressedKey = -1;
    if (!pressedKeys.empty()) {
        lastPressedKey = pressedKeys.back();
        MMI_HILOGD("The last repeat button, keyCode:%{public}d", lastPressedKey);
    }
    auto packageResult = eventPackage_.PackageKeyEvent(event, keyEvent_);
    if (packageResult == MULTIDEVICE_SAME_EVENT_MARK) {
        MMI_HILOGD("The same event reported by multi_device should be discarded");
        return RET_OK;
    }
    if (packageResult != RET_OK) {
        MMI_HILOGE("KeyEvent package failed. ret:%{public}d,errCode:%{public}d", packageResult, KEY_EVENT_PKG_FAIL);
        return KEY_EVENT_PKG_FAIL;
    }

    BytraceAdapter::StartBytrace(keyEvent_);

    auto ret = eventDispatch_.DispatchKeyEventPid(*udsServer_, keyEvent_);
    if (ret != RET_OK) {
        MMI_HILOGE("KeyEvent dispatch failed. ret:%{public}d,errCode:%{public}d", ret, KEY_EVENT_DISP_FAIL);
        return KEY_EVENT_DISP_FAIL;
    }
    KeyRepeat->SelectAutoRepeat(keyEvent_, lastPressedKey);
    MMI_HILOGD("keyCode:%{public}d,action:%{public}d", keyEvent_->GetKeyCode(), keyEvent_->GetKeyAction());
    return RET_OK;
}

int32_t InputEventHandler::OnEventPointer(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    return OnMouseEventHandler(event);
}

int32_t InputEventHandler::OnEventTouchSecond(libinput_event *event)
{
    CALL_LOG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    auto type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_TOUCH_CANCEL || type == LIBINPUT_EVENT_TOUCH_FRAME) {
        MMI_HILOGD("This touch event is canceled type:%{public}d", type);
        return RET_OK;
    }
    auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_TOUCH);
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    eventDispatch_.HandlePointerEvent(pointerEvent);
    if (type == LIBINPUT_EVENT_TOUCH_UP) {
        pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
        MMI_HILOGD("This touch event is up remove this finger");
        if (pointerEvent->GetPointersIdList().empty()) {
            MMI_HILOGD("This touch event is final finger up remove this finger");
            pointerEvent->Reset();
        }
        return RET_OK;
    }
    return RET_OK;
}

int32_t InputEventHandler::OnEventTouchPadSecond(libinput_event *event)
{
    CALL_LOG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);

    auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_TOUCH_PAD);
    CHKPR(pointerEvent, RET_ERR);
    eventDispatch_.HandlePointerEvent(pointerEvent);
    auto type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_TOUCHPAD_UP) {
        pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
        MMI_HILOGD("This touch pad event is up remove this finger");
        if (pointerEvent->GetPointersIdList().empty()) {
            MMI_HILOGD("This touch pad event is final finger up remove this finger");
            pointerEvent->Reset();
        }
        return RET_OK;
    }
    return RET_OK;
}

int32_t InputEventHandler::OnEventTouch(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    LibinputAdapter::LoginfoPackagingTool(event);
    return OnEventTouchSecond(event);
}

int32_t InputEventHandler::OnEventTouchpad(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    OnEventTouchPadSecond(event);
    return RET_OK;
}

int32_t InputEventHandler::OnTabletToolEvent(libinput_event *event)
{
    CALL_LOG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    LibinputAdapter::LoginfoPackagingTool(event);
    auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_TABLET_TOOL);
    CHKPR(pointerEvent, RET_ERR);
    eventDispatch_.HandlePointerEvent(pointerEvent);
    if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_UP) {
        pointerEvent->Reset();
    }
    return RET_OK;
}

int32_t InputEventHandler::OnGestureEvent(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    auto pointerEvent = TouchTransformPointManger->OnLibInput(event, INPUT_DEVICE_CAP_GESTURE);
    CHKPR(pointerEvent, GESTURE_EVENT_PKG_FAIL);
    MMI_HILOGD("GestrueEvent package, eventType:%{public}d,actionTime:%{public}" PRId64 ","
               "action:%{public}d,actionStartTime:%{public}" PRId64 ","
               "pointerAction:%{public}d,sourceType:%{public}d,"
               "PinchAxisValue:%{public}.2f",
               pointerEvent->GetEventType(), pointerEvent->GetActionTime(),
               pointerEvent->GetAction(), pointerEvent->GetActionStartTime(),
               pointerEvent->GetPointerAction(), pointerEvent->GetSourceType(),
               pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_PINCH));

    PointerEvent::PointerItem item;
    pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item);
    MMI_HILOGD("Item:DownTime:%{public}" PRId64 ",IsPressed:%{public}s,"
               "GlobalX:%{public}d,GlobalY:%{public}d,LocalX:%{public}d,LocalY:%{public}d,"
               "Width:%{public}d,Height:%{public}d",
               item.GetDownTime(), (item.IsPressed() ? "true" : "false"),
               item.GetGlobalX(), item.GetGlobalY(), item.GetLocalX(), item.GetLocalY(),
               item.GetWidth(), item.GetHeight());

    int32_t ret = eventDispatch_.HandlePointerEvent(pointerEvent);
    if (ret != RET_OK) {
        MMI_HILOGE("Gesture event dispatch failed, errCode:%{public}d", GESTURE_EVENT_DISP_FAIL);
        return GESTURE_EVENT_DISP_FAIL;
    }
    return RET_OK;
}

int32_t InputEventHandler::OnEventGesture(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    OnGestureEvent(event);
    return RET_OK;
}

int32_t InputEventHandler::OnMouseEventHandler(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);

    auto ret = MouseEventHdr->Normalize(event);
    if (ret != RET_OK) {
        MMI_HILOGE("Normalize faild");
        return RET_ERR;
    }

    auto pointerEvent = MouseEventHdr->GetPointerEvent();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);

    if (keyEvent_ == nullptr) {
        keyEvent_ = KeyEvent::Create();
    }
    CHKPR(keyEvent_, ERROR_NULL_POINTER);
    std::vector<int32_t> pressedKeys = keyEvent_->GetPressedKeys();
    for (const int32_t& keyCode : pressedKeys) {
        MMI_HILOGI("Pressed keyCode:%{public}d", keyCode);
    }
    pointerEvent->SetPressedKeys(pressedKeys);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    eventDispatch_.HandlePointerEvent(pointerEvent);
    return RET_OK;
}

int32_t InputEventHandler::OnMouseEventEndTimerHandler(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    MMI_HILOGI("MouseEvent Normalization Results, PointerAction:%{public}d,PointerId:%{public}d,"
               "SourceType:%{public}d,ButtonId:%{public}d,"
               "VerticalAxisValue:%{public}lf,HorizontalAxisValue:%{public}lf",
               pointerEvent->GetPointerAction(), pointerEvent->GetPointerId(), pointerEvent->GetSourceType(),
               pointerEvent->GetButtonId(), pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
               pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL));
    PointerEvent::PointerItem item;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item)) {
        MMI_HILOGE("Get pointer item failed, pointer:%{public}d", pointerEvent->GetPointerId());
        return RET_ERR;
    }
    MMI_HILOGI("MouseEvent Item Normalization Results, DownTime:%{public}" PRId64 ",IsPressed:%{public}d,"
               "GlobalX:%{public}d,GlobalY:%{public}d,LocalX:%{public}d,LocalY:%{public}d,"
               "Width:%{public}d,Height:%{public}d,Pressure:%{public}f,Device:%{public}d",
               item.GetDownTime(), static_cast<int32_t>(item.IsPressed()), item.GetGlobalX(), item.GetGlobalY(),
               item.GetLocalX(), item.GetLocalY(), item.GetWidth(), item.GetHeight(), item.GetPressure(),
               item.GetDeviceId());

    eventDispatch_.HandlePointerEvent(pointerEvent);
    return RET_OK;
}

bool InputEventHandler::SendMsg(const int32_t fd, NetPacket& pkt) const
{
    CHKPF(udsServer_);
    return udsServer_->SendMsg(fd, pkt);
}
} // namespace MMI
} // namespace OHOS
