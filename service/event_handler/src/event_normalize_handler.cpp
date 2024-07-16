/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "event_normalize_handler.h"

#include "display_manager.h"

#include "bytrace_adapter.h"
#ifdef OHOS_BUILD_ENABLE_CROWN
#include "crown_transform_processor.h"
#endif // OHOS_BUILD_ENABLE_CROWN
#include "define_multimodal.h"
#include "dfx_hisysevent.h"
#include "error_multimodal.h"
#include "event_log_helper.h"
#include "event_resample.h"
#ifdef OHOS_BUILD_ENABLE_FINGERPRINT
#include "fingerprint_event_processor.h"
#endif // OHOS_BUILD_ENABLE_FINGERPRINT
#include "gesture_handler.h"
#include "input_device_manager.h"
#include "input_event_handler.h"
#include "input_scene_board_judgement.h"
#include "key_auto_repeat.h"
#include "key_event_normalize.h"
#include "key_event_value_transformation.h"
#include "libinput_adapter.h"
#include "mmi_log.h"
#include "time_cost_chk.h"
#include "timer_manager.h"
#include "touch_event_normalize.h"
#include "touchpad_transform_processor.h"
#include "util_ex.h"

#undef MMI_LOG_DOMAIN
#define MMI_LOG_DOMAIN MMI_LOG_HANDLER
#undef MMI_LOG_TAG
#define MMI_LOG_TAG "EventNormalizeHandler"

namespace OHOS {
namespace MMI {
namespace {
constexpr int32_t FINGER_NUM { 2 };
constexpr int32_t MT_TOOL_PALM { 2 };
const std::vector<int32_t> ALL_EVENT_TYPES = {
    static_cast<int32_t>(LIBINPUT_EVENT_DEVICE_ADDED),
    static_cast<int32_t>(LIBINPUT_EVENT_DEVICE_REMOVED),
    static_cast<int32_t>(LIBINPUT_EVENT_KEYBOARD_KEY),
    static_cast<int32_t>(LIBINPUT_EVENT_POINTER_MOTION),
    static_cast<int32_t>(LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE),
    static_cast<int32_t>(LIBINPUT_EVENT_POINTER_BUTTON),
    static_cast<int32_t>(LIBINPUT_EVENT_POINTER_BUTTON_TOUCHPAD),
    static_cast<int32_t>(LIBINPUT_EVENT_POINTER_AXIS),
    static_cast<int32_t>(LIBINPUT_EVENT_POINTER_TAP),
    static_cast<int32_t>(LIBINPUT_EVENT_POINTER_MOTION_TOUCHPAD),
    static_cast<int32_t>(LIBINPUT_EVENT_TOUCHPAD_DOWN),
    static_cast<int32_t>(LIBINPUT_EVENT_TOUCHPAD_UP),
    static_cast<int32_t>(LIBINPUT_EVENT_TOUCHPAD_MOTION),
    static_cast<int32_t>(LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN),
    static_cast<int32_t>(LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE),
    static_cast<int32_t>(LIBINPUT_EVENT_GESTURE_SWIPE_END),
    static_cast<int32_t>(LIBINPUT_EVENT_GESTURE_PINCH_BEGIN),
    static_cast<int32_t>(LIBINPUT_EVENT_GESTURE_PINCH_UPDATE),
    static_cast<int32_t>(LIBINPUT_EVENT_GESTURE_PINCH_END),
    static_cast<int32_t>(LIBINPUT_EVENT_TOUCH_DOWN),
    static_cast<int32_t>(LIBINPUT_EVENT_TOUCH_UP),
    static_cast<int32_t>(LIBINPUT_EVENT_TOUCH_MOTION),
    static_cast<int32_t>(LIBINPUT_EVENT_TABLET_TOOL_AXIS),
    static_cast<int32_t>(LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY),
    static_cast<int32_t>(LIBINPUT_EVENT_TABLET_TOOL_TIP),
    static_cast<int32_t>(LIBINPUT_EVENT_JOYSTICK_BUTTON),
    static_cast<int32_t>(LIBINPUT_EVENT_JOYSTICK_AXIS),
    static_cast<int32_t>(LIBINPUT_EVENT_SWITCH_TOGGLE)
};
const std::string EVENT_FILE_PATH = "/data/service/el1/public/multimodalinput/";
const std::string EVENT_FILE_NAME = "/data/service/el1/public/multimodalinput/multimodal_event.dmp";
const std::string EVENT_FILE_NAME_HISTORY = "/data/service/el1/public/multimodalinput/multimodal_event_history.dmp";
const int32_t FILE_MAX_SIZE = 100 * 1024 * 1024;
const int32_t EVENT_OUT_SIZE = 30;
const int32_t FUNC_EXE_OK = 0;
const int32_t STRING_WIDTH = 3;
}

std::queue<std::string> EventNormalizeHandler::eventQueue_;
std::list<std::string> EventNormalizeHandler::dumperEventList_;
std::mutex EventNormalizeHandler::queueMutex_;
std::condition_variable EventNormalizeHandler::queueCondition_;
bool EventNormalizeHandler::runningSignal_ = true;
std::string EventNormalizeHandler::eventString_;

void EventNormalizeHandler::HandleEvent(libinput_event* event, int64_t frameTime)
{
    CALL_DEBUG_ENTER;

    if (ProcessNullEvent(event, frameTime)) {
        return;
    }

    if (event == nullptr) {
        return;
    }
    DfxHisysevent::GetDispStartTime();
    auto type = libinput_event_get_type(event);
    TimeCostChk chk("HandleLibinputEvent", "overtime 1000(us)", MAX_INPUT_EVENT_TIME, type);
    if (type == LIBINPUT_EVENT_TOUCH_CANCEL || type == LIBINPUT_EVENT_TOUCH_FRAME) {
        MMI_HILOGD("This touch event is canceled type:%{public}d", type);
        return;
    }
    if ((type == LIBINPUT_EVENT_POINTER_TAP) &&
        (MULTI_FINGERTAP_HDR->GetMultiFingersState() == MulFingersTap::TRIPLETAP)) {
        MULTI_FINGERTAP_HDR->SetMULTI_FINGERTAP_HDRDefault();
        return;
    }
    if ((type < LIBINPUT_EVENT_TOUCHPAD_DOWN) || (type > LIBINPUT_EVENT_TOUCHPAD_MOTION)) {
        auto iter = std::find(ALL_EVENT_TYPES.begin(), ALL_EVENT_TYPES.end(), static_cast<int32_t>(type));
        if (iter != ALL_EVENT_TYPES.end()) {
            MULTI_FINGERTAP_HDR->SetMULTI_FINGERTAP_HDRDefault();
        }
    }
    BytraceAdapter::StartHandleInput(static_cast<int32_t>(type));
    InitEventString(static_cast<int32_t>(type));
    switch (type) {
        case LIBINPUT_EVENT_DEVICE_ADDED: {
            OnEventDeviceAdded(event);
            break;
        }
        case LIBINPUT_EVENT_DEVICE_REMOVED: {
            OnEventDeviceRemoved(event);
            break;
        }
        case LIBINPUT_EVENT_KEYBOARD_KEY: {
            HandleKeyboardEvent(event);
            DfxHisysevent::CalcKeyDispTimes();
            break;
        }
        case LIBINPUT_EVENT_POINTER_MOTION:
        case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
        case LIBINPUT_EVENT_POINTER_BUTTON:
        case LIBINPUT_EVENT_POINTER_BUTTON_TOUCHPAD:
        case LIBINPUT_EVENT_POINTER_AXIS:
        case LIBINPUT_EVENT_POINTER_TAP:
        case LIBINPUT_EVENT_POINTER_MOTION_TOUCHPAD: {
            HandleMouseEvent(event);
            DfxHisysevent::CalcPointerDispTimes();
            break;
        }
        case LIBINPUT_EVENT_TOUCHPAD_DOWN:
        case LIBINPUT_EVENT_TOUCHPAD_UP:
        case LIBINPUT_EVENT_TOUCHPAD_MOTION: {
            HandleTouchPadEvent(event);
            DfxHisysevent::CalcPointerDispTimes();
            break;
        }
        case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
        case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
        case LIBINPUT_EVENT_GESTURE_SWIPE_END:
        case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
        case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
        case LIBINPUT_EVENT_GESTURE_PINCH_END: {
            HandleGestureEvent(event);
            DfxHisysevent::CalcPointerDispTimes();
            break;
        }
        case LIBINPUT_EVENT_TOUCH_DOWN:
        case LIBINPUT_EVENT_TOUCH_UP:
        case LIBINPUT_EVENT_TOUCH_MOTION: {
            HandleTouchEvent(event, frameTime);
            DfxHisysevent::CalcPointerDispTimes();
            break;
        }
        case LIBINPUT_EVENT_TABLET_TOOL_AXIS:
        case LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY:
        case LIBINPUT_EVENT_TABLET_TOOL_TIP: {
            HandleTableToolEvent(event);
            break;
        }
        case LIBINPUT_EVENT_JOYSTICK_BUTTON:
        case LIBINPUT_EVENT_JOYSTICK_AXIS: {
            HandleJoystickEvent(event);
            DfxHisysevent::CalcPointerDispTimes();
            break;
        }
        case LIBINPUT_EVENT_SWITCH_TOGGLE: {
            HandleSwitchInputEvent(event);
            break;
        }
        default: {
            MMI_HILOGD("This device does not support :%d", type);
            break;
        }
    }
    eventString_ += "}";
    PushEventStr();
    BytraceAdapter::StopHandleInput();
    DfxHisysevent::ReportDispTimes();
}

bool EventNormalizeHandler::ProcessNullEvent(libinput_event *event, int64_t frameTime)
{
    std::shared_ptr<PointerEvent> pointerEvent = EventResampleHdr->GetPointerEvent();
    if ((event == nullptr) && (pointerEvent != nullptr) && MMISceneBoardJudgement::IsSceneBoardEnabled()
        && MMISceneBoardJudgement::IsResampleEnabled()) {
        int32_t sourceType = pointerEvent->GetSourceType();
        if (sourceType == PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
            HandleTouchEvent(event, frameTime);
        }
        return true;
    }
    return false;
}

int32_t EventNormalizeHandler::OnEventDeviceAdded(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    auto device = libinput_event_get_device(event);
    CHKPR(device, ERROR_NULL_POINTER);
    INPUT_DEV_MGR->OnInputDeviceAdded(device);
    KeyMapMgr->ParseDeviceConfigFile(device);
    KeyRepeat->AddDeviceConfig(device);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    KeyEventHdr->ResetKeyEvent(device);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    return RET_OK;
}

int32_t EventNormalizeHandler::OnEventDeviceRemoved(libinput_event *event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    auto device = libinput_event_get_device(event);
    CHKPR(device, ERROR_NULL_POINTER);
    KeyMapMgr->RemoveKeyValue(device);
    KeyRepeat->RemoveDeviceConfig(device);
    INPUT_DEV_MGR->OnInputDeviceRemoved(device);
    return RET_OK;
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
void EventNormalizeHandler::HandleKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPV(nextHandler_);
    DfxHisysevent::GetDispStartTime();
    CHKPV(keyEvent);
    EventLogHelper::PrintEventData(keyEvent, MMI_LOG_HEADER);
    UpdateKeyEventHandlerChain(keyEvent);
    if (keyEvent->IsRepeat()) {
        KeyRepeat->SelectAutoRepeat(keyEvent);
        keyEvent->SetRepeat(false);
    }
    DfxHisysevent::CalcKeyDispTimes();
    DfxHisysevent::ReportDispTimes();
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#ifdef OHOS_BUILD_ENABLE_POINTER
void EventNormalizeHandler::HandlePointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPV(nextHandler_);
    DfxHisysevent::GetDispStartTime();
    CHKPV(pointerEvent);
    if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_AXIS_END) {
        MMI_HILOGI("MouseEvent Normalization Results, PointerAction:%{public}d, PointerId:%{public}d,"
            "SourceType:%{public}d, ButtonId:%{public}d,"
            "VerticalAxisValue:%{public}lf, HorizontalAxisValue:%{public}lf",
            pointerEvent->GetPointerAction(), pointerEvent->GetPointerId(), pointerEvent->GetSourceType(),
            pointerEvent->GetButtonId(), pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
            pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL));
        PointerEvent::PointerItem item;
        if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item)) {
            MMI_HILOGE("Get pointer item failed. pointer:%{public}d", pointerEvent->GetPointerId());
            return;
        }
        if (!EventLogHelper::IsBetaVersion()) {
            MMI_HILOGI("MouseEvent Item Normalization Results, IsPressed:%{public}d, Pressure:%{public}f"
                       ", Device:%{public}d",
                static_cast<int32_t>(item.IsPressed()), item.GetPressure(), item.GetDeviceId());
        } else {
            MMI_HILOGI("MouseEvent Item Normalization Results, DownTime:%{public}" PRId64 ", IsPressed:%{public}d,"
                "DisplayX:%{public}d, DisplayY:%{public}d, WindowX:%{public}d, WindowY:%{public}d,"
                "Width:%{public}d, Height:%{public}d, Pressure:%{public}f, Device:%{public}d",
                item.GetDownTime(), static_cast<int32_t>(item.IsPressed()), item.GetDisplayX(), item.GetDisplayY(),
                item.GetWindowX(), item.GetWindowY(), item.GetWidth(), item.GetHeight(), item.GetPressure(),
                item.GetDeviceId());
        }
    }
    WIN_MGR->UpdateTargetPointer(pointerEvent);
    nextHandler_->HandlePointerEvent(pointerEvent);
    DfxHisysevent::CalcPointerDispTimes();
    DfxHisysevent::ReportDispTimes();
}
#endif // OHOS_BUILD_ENABLE_POINTER

#ifdef OHOS_BUILD_ENABLE_TOUCH
void EventNormalizeHandler::HandleTouchEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPV(nextHandler_);
    DfxHisysevent::GetDispStartTime();
    CHKPV(pointerEvent);
    WIN_MGR->UpdateTargetPointer(pointerEvent);
    nextHandler_->HandleTouchEvent(pointerEvent);
    DfxHisysevent::CalcPointerDispTimes();
    DfxHisysevent::ReportDispTimes();
}
#endif // OHOS_BUILD_ENABLE_TOUCH

int32_t EventNormalizeHandler::HandleKeyboardEvent(libinput_event* event)
{
#ifdef OHOS_BUILD_ENABLE_FINGERPRINT
    if (FingerprintEventHdr->IsFingerprintEvent(event)) {
        MMI_HILOGI("The current event is finger");
        return FingerprintEventHdr->HandleFingerprintEvent(event);
    }
#endif // OHOS_BUILD_ENABLE_FINGERPRINT
    CHKPR(nextHandler_, ERROR_UNSUPPORT);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    BytraceAdapter::StartPackageEvent("package keyEvent");
    auto keyEvent = KeyEventHdr->GetKeyEvent();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    CHKPR(event, ERROR_NULL_POINTER);
    std::vector<int32_t> pressedKeys = keyEvent->GetPressedKeys();
    int32_t lastPressedKey = -1;
    if (!pressedKeys.empty()) {
        lastPressedKey = pressedKeys.back();
        MMI_HILOGD("The last repeat button, keyCode:%{public}d", lastPressedKey);
    }
    auto packageResult = KeyEventHdr->Normalize(event, keyEvent);
    WIN_MGR->HandleKeyEventWindowId(keyEvent);
    eventString_ += ConvertKeyEventToStr(keyEvent);
    LogTracer lt(keyEvent->GetId(), keyEvent->GetEventType(), keyEvent->GetKeyAction());
    if (packageResult == MULTIDEVICE_SAME_EVENT_MARK) {
        MMI_HILOGD("The same event reported by multi_device should be discarded");
        BytraceAdapter::StopPackageEvent();
        return RET_OK;
    }
    if (packageResult != RET_OK) {
        MMI_HILOGE("KeyEvent package failed, ret:%{public}d, errCode:%{public}d", packageResult, KEY_EVENT_PKG_FAIL);
        BytraceAdapter::StopPackageEvent();
        return KEY_EVENT_PKG_FAIL;
    }
    BytraceAdapter::StopPackageEvent();
    BytraceAdapter::StartBytrace(keyEvent);
    EventLogHelper::PrintEventData(keyEvent, MMI_LOG_HEADER);
    auto device = INPUT_DEV_MGR->GetInputDevice(keyEvent->GetDeviceId());
    CHKPR(device, RET_ERR);
    MMI_HILOGI("InputTracking id:%{public}d event created by:%{public}s", keyEvent->GetId(), device->GetName().c_str());
    UpdateKeyEventHandlerChain(keyEvent);
    KeyRepeat->SelectAutoRepeat(keyEvent);
    MMI_HILOGD("keyCode:%{public}d, action:%{public}d", keyEvent->GetKeyCode(), keyEvent->GetKeyAction());
#else
    MMI_HILOGW("Keyboard device does not support");
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    return RET_OK;
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
void EventNormalizeHandler::UpdateKeyEventHandlerChain(const std::shared_ptr<KeyEvent> keyEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(keyEvent);
    int32_t currentShieldMode = KeyEventHdr->GetCurrentShieldMode();
    if (currentShieldMode == SHIELD_MODE::FACTORY_MODE) {
        MMI_HILOGI("The current mode is factory");
        auto eventDispatchHandler = InputHandler->GetEventDispatchHandler();
        CHKPV(eventDispatchHandler);
        eventDispatchHandler->HandleKeyEvent(keyEvent);
    } else {
        CHKPV(nextHandler_);
        nextHandler_->HandleKeyEvent(keyEvent);
    }
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

int32_t EventNormalizeHandler::HandleMouseEvent(libinput_event* event)
{
#ifdef OHOS_BUILD_ENABLE_FINGERPRINT
    if (FingerprintEventHdr->IsFingerprintEvent(event)) {
        return FingerprintEventHdr->HandleFingerprintEvent(event);
    }
#endif // OHOS_BUILD_ENABLE_FINGERPRINT
#ifdef OHOS_BUILD_ENABLE_CROWN
    if (CROWN_EVENT_HDR->IsCrownEvent(event)) {
        return CROWN_EVENT_HDR->NormalizeRotateEvent(event);
    }
#endif // OHOS_BUILD_ENABLE_CROWN
    CHKPR(nextHandler_, ERROR_UNSUPPORT);
#ifdef OHOS_BUILD_ENABLE_POINTER
    BytraceAdapter::StartPackageEvent("package mouseEvent");
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    const auto &keyEvent = KeyEventHdr->GetKeyEvent();
    CHKPR(keyEvent, ERROR_NULL_POINTER);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    TerminateRotate(event);
    TerminateAxis(event);
    if (MouseEventHdr->OnEvent(event) == RET_ERR) {
        MMI_HILOGE("OnEvent is failed");
        BytraceAdapter::StopPackageEvent();
        return RET_ERR;
    }
    auto pointerEvent = MouseEventHdr->GetPointerEvent();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    LogTracer lt(pointerEvent->GetId(), pointerEvent->GetEventType(), pointerEvent->GetPointerAction());
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    std::vector<int32_t> pressedKeys = keyEvent->GetPressedKeys();
    for (const int32_t& keyCode : pressedKeys) {
        MMI_HILOGI("Pressed keyCode:%{public}d", keyCode);
    }
    pointerEvent->SetPressedKeys(pressedKeys);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    BytraceAdapter::StopPackageEvent();
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    HandlePalmEvent(event, pointerEvent);
    if (SetOriginPointerId(pointerEvent) != RET_OK) {
        MMI_HILOGE("Failed to set origin pointerId");
        return RET_ERR;
    }
    eventString_ += ConvertPointerEventToStr(pointerEvent);
    nextHandler_->HandlePointerEvent(pointerEvent);
#else
    MMI_HILOGW("Pointer device does not support");
#endif // OHOS_BUILD_ENABLE_POINTER
    return RET_OK;
}

void EventNormalizeHandler::HandlePalmEvent(libinput_event* event, std::shared_ptr<PointerEvent> pointerEvent)
{
    auto touchpad = libinput_event_get_touchpad_event(event);
    if (touchpad == nullptr) {
        return;
    }
    int32_t toolType = libinput_event_touchpad_get_tool_type(touchpad);
    if (toolType == MT_TOOL_PALM) {
        MMI_HILOGD("ToolType is MT_TOOL_PALM");
        pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_CANCEL);
    }
}

int32_t EventNormalizeHandler::HandleTouchPadEvent(libinput_event* event)
{
    CHKPR(nextHandler_, ERROR_UNSUPPORT);
#ifdef OHOS_BUILD_ENABLE_POINTER
    CHKPR(event, ERROR_NULL_POINTER);
    auto touchpad = libinput_event_get_touchpad_event(event);
    CHKPR(touchpad, ERROR_NULL_POINTER);
    auto type = libinput_event_get_type(event);
    int32_t seatSlot = libinput_event_touchpad_get_seat_slot(touchpad);
    GestureIdentify(event);
    MULTI_FINGERTAP_HDR->HandleMulFingersTap(touchpad, type);
    auto pointerEvent = TOUCH_EVENT_HDR->OnLibInput(event, TouchEventNormalize::DeviceType::TOUCH_PAD);
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    LogTracer lt(pointerEvent->GetId(), pointerEvent->GetEventType(), pointerEvent->GetPointerAction());
    eventString_ += ConvertPointerEventToStr(pointerEvent);
    if (MULTI_FINGERTAP_HDR->GetMultiFingersState() == MulFingersTap::TRIPLETAP) {
        bool threeFingerSwitch = false;
        TOUCH_EVENT_HDR->GetTouchpadThreeFingersTapSwitch(threeFingerSwitch);
        if (!threeFingerSwitch) {
            return RET_OK;
        }
        nextHandler_->HandlePointerEvent(pointerEvent);
        MULTI_FINGERTAP_HDR->ClearPointerItems(pointerEvent);
    }
    buttonIds_.insert(seatSlot);
    if (buttonIds_.size() == FINGER_NUM &&
        (type == LIBINPUT_EVENT_TOUCHPAD_DOWN || type == LIBINPUT_EVENT_TOUCHPAD_UP)) {
        MMI_HILOGD("Handle mouse axis event");
        HandleMouseEvent(event);
    }
    if (type == LIBINPUT_EVENT_TOUCHPAD_UP) {
        pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
        MMI_HILOGD("This event is up remove this finger");
        if (pointerEvent->GetPointerIds().empty()) {
            MMI_HILOGD("This event is final up");
            pointerEvent->Reset();
        }
        buttonIds_.erase(seatSlot);
    }
    if (buttonIds_.empty()) {
        MULTI_FINGERTAP_HDR->SetMULTI_FINGERTAP_HDRDefault(false);
    }
    return RET_OK;
#else
    MMI_HILOGW("Pointer device does not support");
#endif // OHOS_BUILD_ENABLE_POINTER
    return RET_OK;
}

int32_t EventNormalizeHandler::GestureIdentify(libinput_event* event)
{
    CHKPR(event, ERROR_NULL_POINTER);
    auto touchpad = libinput_event_get_touchpad_event(event);
    CHKPR(touchpad, ERROR_NULL_POINTER);
    int32_t seatSlot = libinput_event_touchpad_get_seat_slot(touchpad);
    double logicalX = libinput_event_touchpad_get_x(touchpad);
    double logicalY = libinput_event_touchpad_get_y(touchpad);
    auto originType = libinput_event_get_type(event);
    auto device = libinput_event_get_device(event);
    CHKPR(device, RET_ERR);
    int32_t deviceId = INPUT_DEV_MGR->FindInputDeviceId(device);
    auto mouseEvent = MouseEventHdr->GetPointerEvent(deviceId);
    auto actionType = PointerEvent::POINTER_ACTION_UNKNOWN;
    if (mouseEvent == nullptr || mouseEvent->GetPressedButtons().empty()) {
        actionType = GESTURE_HANDLER->GestureIdentify(originType, seatSlot, logicalX, logicalY);
    }
    if (actionType == PointerEvent::POINTER_ACTION_UNKNOWN) {
        MMI_HILOGD("Gesture identify failed");
        return RET_ERR;
    }
    bool tpRotateSwitch = true;
    TOUCH_EVENT_HDR->GetTouchpadRotateSwitch(tpRotateSwitch);
    if (!tpRotateSwitch) {
        MMI_HILOGD("touchpad rotate switch is false");
        return RET_ERR;
    }
    auto rotateAngle = GESTURE_HANDLER->GetRotateAngle();
    CHKPR(nextHandler_, ERROR_UNSUPPORT);
#ifdef OHOS_BUILD_ENABLE_POINTER
    if (MouseEventHdr->NormalizeRotateEvent(event, actionType, rotateAngle) == RET_ERR) {
        MMI_HILOGE("OnEvent is failed");
        return RET_ERR;
    }
    auto pointerEvent = MouseEventHdr->GetPointerEvent();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    LogTracer lt(pointerEvent->GetId(), pointerEvent->GetEventType(), pointerEvent->GetPointerAction());
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    nextHandler_->HandlePointerEvent(pointerEvent);
    if (actionType == PointerEvent::POINTER_ACTION_ROTATE_END) {
        pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
    }
#else
    MMI_HILOGW("Pointer device does not support");
#endif // OHOS_BUILD_ENABLE_POINTER
    return RET_OK;
}

int32_t EventNormalizeHandler::HandleGestureEvent(libinput_event* event)
{
    CHKPR(nextHandler_, ERROR_UNSUPPORT);
#ifdef OHOS_BUILD_ENABLE_POINTER
    CHKPR(event, ERROR_NULL_POINTER);
    auto pointerEvent = TOUCH_EVENT_HDR->OnLibInput(event, TouchEventNormalize::DeviceType::TOUCH_PAD);
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    LogTracer lt(pointerEvent->GetId(), pointerEvent->GetEventType(), pointerEvent->GetPointerAction());
    eventString_ += ConvertPointerEventToStr(pointerEvent);
    nextHandler_->HandlePointerEvent(pointerEvent);
    auto type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_GESTURE_SWIPE_END || type == LIBINPUT_EVENT_GESTURE_PINCH_END) {
        pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
        MMI_HILOGD("This touch pad event is up remove this finger");
        if (pointerEvent->GetPointerIds().empty()) {
            MMI_HILOGD("This touch pad event is final finger up remove this finger");
            pointerEvent->Reset();
        }
    }
#else
    MMI_HILOGW("Pointer device does not support");
#endif // OHOS_BUILD_ENABLE_POINTER
    return RET_OK;
}

int32_t EventNormalizeHandler::HandleTouchEvent(libinput_event* event, int64_t frameTime)
{
    CHKPR(nextHandler_, ERROR_UNSUPPORT);
#ifdef OHOS_BUILD_ENABLE_TOUCH
    BytraceAdapter::StartPackageEvent("package touchEvent");
    std::shared_ptr<PointerEvent> pointerEvent = nullptr;
    LogTracer lt;
    if (event != nullptr) {
        pointerEvent = TOUCH_EVENT_HDR->OnLibInput(event, TouchEventNormalize::DeviceType::TOUCH);
        CHKPR(pointerEvent, ERROR_NULL_POINTER);
        lt = LogTracer(pointerEvent->GetId(), pointerEvent->GetEventType(), pointerEvent->GetPointerAction());
    }
    if (MMISceneBoardJudgement::IsSceneBoardEnabled() && MMISceneBoardJudgement::IsResampleEnabled()) {
        ErrCode status = RET_OK;
        std::shared_ptr<PointerEvent> outputEvent = EventResampleHdr->OnEventConsume(pointerEvent, frameTime, status);
        CHKPR(outputEvent, RET_OK);
        MMI_HILOGD("Output event received, SourceType:%{public}d, PointerAction:%{public}d, status:%{public}d",
            outputEvent->GetSourceType(), outputEvent->GetPointerAction(), status);
        EndLogTraceId(pointerEvent->GetId());
        pointerEvent = outputEvent;
        lt = LogTracer(pointerEvent->GetId(), pointerEvent->GetEventType(), pointerEvent->GetPointerAction());
    }
    BytraceAdapter::StopPackageEvent();
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    if (KeyEventHdr != nullptr) {
        const auto &keyEvent = KeyEventHdr->GetKeyEvent();
        if (keyEvent != nullptr && pointerEvent != nullptr) {
            std::vector<int32_t> pressedKeys = keyEvent->GetPressedKeys();
            pointerEvent->SetPressedKeys(pressedKeys);
        }
    }
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    if (SetOriginPointerId(pointerEvent) != RET_OK) {
        MMI_HILOGE("Failed to set origin pointerId");
        return RET_ERR;
    }
    eventString_ += ConvertPointerEventToStr(pointerEvent);
    nextHandler_->HandleTouchEvent(pointerEvent);
    if ((pointerEvent != nullptr) && (event != nullptr)) {
        ResetTouchUpEvent(pointerEvent, event);
    }
#else
    MMI_HILOGW("Touchscreen device does not support");
#endif // OHOS_BUILD_ENABLE_TOUCH
    return RET_OK;
}

void EventNormalizeHandler::ResetTouchUpEvent(std::shared_ptr<PointerEvent> pointerEvent,
    struct libinput_event *event)
{
    CHKPV(pointerEvent);
    CHKPV(event);
    auto type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_TOUCH_UP) {
        pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
        MMI_HILOGD("This touch event is up remove this finger");
        if (pointerEvent->GetPointerIds().empty()) {
            MMI_HILOGD("This touch event is final finger up remove this finger");
            pointerEvent->Reset();
        }
    }
}

int32_t EventNormalizeHandler::HandleTableToolEvent(libinput_event* event)
{
    CHKPR(nextHandler_, ERROR_UNSUPPORT);
#ifdef OHOS_BUILD_ENABLE_TOUCH
    CHKPR(event, ERROR_NULL_POINTER);
    BytraceAdapter::StartPackageEvent("package penEvent");
    auto pointerEvent = TOUCH_EVENT_HDR->OnLibInput(event, TouchEventNormalize::DeviceType::TABLET_TOOL);
    BytraceAdapter::StopPackageEvent();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    LogTracer lt(pointerEvent->GetId(), pointerEvent->GetEventType(), pointerEvent->GetPointerAction());
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    eventString_ += ConvertPointerEventToStr(pointerEvent);
    nextHandler_->HandleTouchEvent(pointerEvent);
    if (pointerEvent->GetPointerAction() == PointerEvent::POINTER_ACTION_UP) {
        pointerEvent->Reset();
    }
#else
    MMI_HILOGW("TableTool device does not support");
#endif // OHOS_BUILD_ENABLE_TOUCH
    return RET_OK;
}

int32_t EventNormalizeHandler::HandleJoystickEvent(libinput_event* event)
{
    CHKPR(nextHandler_, ERROR_UNSUPPORT);
#ifdef OHOS_BUILD_ENABLE_JOYSTICK
    CHKPR(event, ERROR_NULL_POINTER);
    BytraceAdapter::StartPackageEvent("package joystickEvent");
    auto pointerEvent = TOUCH_EVENT_HDR->OnLibInput(event, TouchEventNormalize::DeviceType::JOYSTICK);
    BytraceAdapter::StopPackageEvent();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    BytraceAdapter::StartBytrace(pointerEvent, BytraceAdapter::TRACE_START);
    eventString_ += ConvertPointerEventToStr(pointerEvent);
    nextHandler_->HandlePointerEvent(pointerEvent);
#else
    MMI_HILOGW("Joystick device does not support");
#endif // OHOS_BUILD_ENABLE_JOYSTICK
    return RET_OK;
}

int32_t EventNormalizeHandler::HandleSwitchInputEvent(libinput_event* event)
{
    CHKPR(nextHandler_, ERROR_UNSUPPORT);
#ifdef OHOS_BUILD_ENABLE_SWITCH
    CHKPR(event, ERROR_NULL_POINTER);
    struct libinput_event_switch *swev = libinput_event_get_switch_event(event);
    CHKPR(swev, ERROR_NULL_POINTER);

    enum libinput_switch_state state = libinput_event_switch_get_switch_state(swev);
    enum libinput_switch sw = libinput_event_switch_get_switch(swev);
    MMI_HILOGI("libinput_event_switch type:%{public}d, state:%{public}d", sw, state);
    if (sw == LIBINPUT_SWITCH_PRIVACY && state == LIBINPUT_SWITCH_STATE_OFF) {
        MMI_HILOGD("Privacy switch event ignored");
        return RET_OK;
    }
    auto swEvent = std::make_unique<SwitchEvent>(static_cast<int32_t>(state));
    int32_t switchStatus = static_cast<int32_t>(sw);
    if (switchStatus) {
        RestoreTouchPadStatus();
    }
    swEvent->SetSwitchType(switchStatus);
    eventString_ += ConvertSwitchEventToStr(std::move(swEvent));
    nextHandler_->HandleSwitchEvent(std::move(swEvent));
#else
    MMI_HILOGW("Switch device does not support");
#endif // OHOS_BUILD_ENABLE_SWITCH
    return RET_OK;
}

int32_t EventNormalizeHandler::AddHandleTimer(int32_t timeout)
{
    CALL_DEBUG_ENTER;
    timerId_ = TimerMgr->AddTimer(timeout, 1, [this]() {
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
        auto keyEvent = KeyEventHdr->GetKeyEvent();
        CHKPV(keyEvent);
        UpdateKeyEventHandlerChain(keyEvent);
        int32_t triggerTime = KeyRepeat->GetIntervalTime(keyEvent->GetDeviceId());
        this->AddHandleTimer(triggerTime);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    });
    return timerId_;
}

int32_t EventNormalizeHandler::SetOriginPointerId(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    int32_t pointerId = pointerEvent->GetPointerId();
    PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerId, pointerItem)) {
        MMI_HILOGE("Can't find pointer item, pointer:%{public}d", pointerId);
        return RET_ERR;
    }
    pointerItem.SetOriginPointerId(pointerItem.GetPointerId());
    pointerEvent->UpdatePointerItem(pointerId, pointerItem);
    MMI_HILOGD("pointerId:%{public}d, originPointerId:%{public}d",
        pointerId, pointerItem.GetPointerId());
    return RET_OK;
}

void EventNormalizeHandler::RestoreTouchPadStatus()
{
    CALL_INFO_TRACE;
    auto ids = INPUT_DEV_MGR->GetTouchPadIds();
    for (auto id : ids) {
        MMI_HILOGI("Restore touchpad, deviceId:%{public}d", id);
        auto mouseEvent = TOUCH_EVENT_HDR->GetPointerEvent(id);
        if (mouseEvent != nullptr) {
            mouseEvent->Reset();
        }
        mouseEvent = MouseEventHdr->GetPointerEvent(id);
        if (mouseEvent != nullptr) {
            mouseEvent->Reset();
        }
    }
    buttonIds_.clear();
}

void EventNormalizeHandler::TerminateRotate(libinput_event* event)
{
    CALL_DEBUG_ENTER;
    if (!GESTURE_HANDLER->GetRotateStatus()) {
        return;
    }
    auto type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_POINTER_BUTTON_TOUCHPAD) {
        MMI_HILOGI("Terminate rotate event");
        auto rotateAngle = GESTURE_HANDLER->GetRotateAngle();
        int32_t actionType = PointerEvent::POINTER_ACTION_ROTATE_END;
        if (MouseEventHdr->NormalizeRotateEvent(event, actionType, rotateAngle) == RET_ERR) {
            MMI_HILOGE("OnEvent is failed");
            return;
        }
        auto pointerEvent = MouseEventHdr->GetPointerEvent();
        CHKPV(pointerEvent);
        LogTracer lt(pointerEvent->GetId(), pointerEvent->GetEventType(), pointerEvent->GetPointerAction());
        nextHandler_->HandlePointerEvent(pointerEvent);
        pointerEvent->RemovePointerItem(pointerEvent->GetPointerId());
        GESTURE_HANDLER->InitRotateGesture();
    }
}

void EventNormalizeHandler::TerminateAxis(libinput_event* event)
{
    CALL_DEBUG_ENTER;
    auto type = libinput_event_get_type(event);
    if (type == LIBINPUT_EVENT_POINTER_BUTTON_TOUCHPAD) {
        bool result = MouseEventHdr->CheckAndPackageAxisEvent(event);
        if (!result) {
            return;
        }
        MMI_HILOGI("Terminate axis event");
        auto pointerEvent = MouseEventHdr->GetPointerEvent();
        CHKPV(pointerEvent);
        LogTracer lt(pointerEvent->GetId(), pointerEvent->GetEventType(), pointerEvent->GetPointerAction());
        nextHandler_->HandlePointerEvent(pointerEvent);
    }
}

void EventNormalizeHandler::InitEventString(int32_t eventType)
{
    auto nowTime = std::chrono::system_clock::now();
    std::time_t timeT = std::chrono::system_clock::to_time_t(nowTime);
    auto milsecsCount = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime.time_since_epoch()).count();
    std::string handleTime = ConvertTimeToStr(static_cast<int64_t>(timeT));
    int32_t milsecI = milsecsCount % 1000;
    std::stringstream strStream;
    strStream << std::left << std::setw(STRING_WIDTH) << milsecI;
    std::string milsecStr(strStream.str());
    handleTime += "." + milsecStr;
    eventString_ = "{" + handleTime + ",eventType:" + std::to_string(eventType);
}

std::string EventNormalizeHandler::ConvertKeyEventToStr(const std::shared_ptr<KeyEvent> keyEvent)
{
    int64_t actionTime = keyEvent->GetActionTime();
    int32_t deviceId = keyEvent->GetDeviceId();
    int32_t keyCode = keyEvent->GetKeyCode();
    int32_t keyAction = keyEvent->GetKeyAction();
    std::vector<KeyEvent::KeyItem> keyItems = keyEvent->GetKeyItems();
    std::string eventStr = ",actionTime:" + std::to_string(actionTime);
    eventStr += ",deviceId:" + std::to_string(deviceId);
    eventStr += ",keyCode:" + std::to_string(keyCode);
    eventStr += ",keyAction:" + std::to_string(keyAction);
    eventStr += ",keyItems:[";
    for (size_t i = 0; i < keyItems.size(); i++) {
        int32_t pressed = keyItems[i].IsPressed() ? 1 : 0;
        int32_t deviceId = keyItems[i].GetDeviceId();
        int32_t keyCode = keyItems[i].GetKeyCode();
        int64_t downTime = keyItems[i].GetDownTime();
        uint32_t unicode = keyItems[i].GetUnicode();
        eventStr += "{pressed:" + std::to_string(pressed);
        eventStr += ",deviceId:" + std::to_string(deviceId);
        eventStr += ",keyCode:" + std::to_string(keyCode);
        eventStr += ",downTime:" + std::to_string(downTime);
        eventStr += ",unicode:" + std::to_string(unicode) + "}";
        if (i != keyItems.size() - 1) {
            eventStr += ",";
        }
    }
    eventStr += "]";
    return eventStr;
}

std::string EventNormalizeHandler::ConvertPointerEventToStr(const std::shared_ptr<PointerEvent> pointerEvent)
{
    int64_t actionTime = pointerEvent->GetActionTime();
    int32_t deviceId = pointerEvent->GetDeviceId();
    int32_t pointerId = pointerEvent->GetPointerId();
    std::list<PointerEvent::PointerItem> pointers = pointerEvent->GetAllPointerItems();
    std::set<int32_t> pressedButtons = pointerEvent->GetPressedButtons();
    int32_t sourceType = pointerEvent->GetSourceType();
    int32_t pointerAction = pointerEvent->GetPointerAction();
    int32_t buttonId = pointerEvent->GetButtonId();
    std::string eventStr = ",actionTime:" + std::to_string(actionTime);
    eventStr += ",deviceId:" + std::to_string(deviceId);
    eventStr += ",pointerId:" + std::to_string(pointerId);
    eventStr += ",sourceType:" + std::to_string(sourceType);
    eventStr += ",pointerAction:" + std::to_string(pointerAction);
    eventStr += ",buttonId:" + std::to_string(buttonId);
    eventStr += ",pointers:[";
    size_t pointerSize = 0;
    for (auto it = pointers.begin(); it != pointers.end(); it++) {
        pointerSize++;
        int32_t displayX = (*it).GetDisplayX();
        int32_t displayY = (*it).GetDisplayY();
        int32_t windowX = (*it).GetWindowX();
        int32_t windowY = (*it).GetWindowY();
        int32_t targetWindowId = (*it).GetTargetWindowId();
        int32_t longAxis = (*it).GetLongAxis();
        int32_t shortAxis = (*it).GetShortAxis();
        eventStr += "{displayX:" + std::to_string(displayX);
        eventStr += ",displayY:" + std::to_string(displayY);
        eventStr += ",windowX:" + std::to_string(windowX);
        eventStr += ",windowY:" + std::to_string(windowY);
        eventStr += ",targetWindowId:" + std::to_string(targetWindowId);
        eventStr += ",longAxis:" + std::to_string(longAxis);
        eventStr += ",shortAxis:" + std::to_string(shortAxis) + "}";
        if (pointerSize != pointers.size()) {
            eventStr += ",";
        }
    }
    eventStr += "],pressedButtons:[";
    size_t buttonsSize = 0;
    for (auto it = pressedButtons.begin(); it != pressedButtons.end(); it++) {
        buttonsSize++;
        eventStr += std::to_string(*it);
        if (buttonsSize != pressedButtons.size()) {
            eventStr += ",";
        }
    }
    eventStr += "]";
    return eventStr;
}

std::string EventNormalizeHandler::ConvertSwitchEventToStr(const std::shared_ptr<SwitchEvent> switchEvent)
{
    int64_t actionTime = switchEvent->GetActionTime();
    int32_t deviceId = switchEvent->GetDeviceId();
    int32_t switchValue = switchEvent->GetSwitchValue();
    int32_t updateSwitchMask = switchEvent->GetSwitchMask();
    int32_t switchType = switchEvent->GetSwitchType();
    std::string eventStr = ",actionTime:" + std::to_string(actionTime);
    eventStr += ",deviceId:" + std::to_string(deviceId);
    eventStr += ",switchValue:" + std::to_string(switchValue);
    eventStr += ",updateSwitchMask:" + std::to_string(updateSwitchMask);
    eventStr += ",switchType:" + std::to_string(switchType);
    return eventStr;
}

std::string EventNormalizeHandler::ConvertTimeToStr(int64_t timestamp)
{
    std::string timeStr = std::to_string(timestamp);
    std::time_t timeT = timestamp;
    std::tm tmInfo;
    localtime_r(&timeT, &tmInfo);
    char buffer[32] = {0};
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tmInfo) > 0) {
        timeStr = buffer;
    }
    return timeStr;
}

void EventNormalizeHandler::PushEventStr()
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    dumperEventList_.push_back(eventString_);
    if (dumperEventList_.size() > EVENT_OUT_SIZE) {
        dumperEventList_.pop_front();
    }
    eventQueue_.push(eventString_);
    queueCondition_.notify_all();
}

std::string EventNormalizeHandler::PopEventStr()
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    if (eventQueue_.empty()) {
        queueCondition_.wait(lock, []() { return !eventQueue_.empty(); });
    }
    std::string eventStr = eventQueue_.front();
    eventQueue_.pop();
    return eventStr;
}

void EventNormalizeHandler::WriteEventFile()
{
    while (runningSignal_) {
        std::string eventStr = PopEventStr();
        struct stat statbuf;
        int fileSize = 0;
        if (stat(EVENT_FILE_NAME.c_str(), &statbuf) == FUNC_EXE_OK) {
            fileSize = statbuf.st_size;
        }
        if (fileSize >= FILE_MAX_SIZE) {
            if (access(EVENT_FILE_NAME_HISTORY.c_str(), F_OK) == FUNC_EXE_OK &&
                remove(EVENT_FILE_NAME_HISTORY.c_str()) != FUNC_EXE_OK) {
                MMI_HILOGE("remove %{public}s failed", EVENT_FILE_NAME_HISTORY.c_str());
            }
            if (rename(EVENT_FILE_NAME.c_str(), EVENT_FILE_NAME_HISTORY.c_str()) != FUNC_EXE_OK) {
                MMI_HILOGE("rename %{public}s failed", EVENT_FILE_NAME.c_str());
            }
        }
        std::ofstream file(EVENT_FILE_NAME, std::ios::app);
        if (file.is_open()) {
            file << eventStr << std::endl;
            file.close();
        } else {
            MMI_HILOGE("open %{public}s failed", EVENT_FILE_NAME.c_str());
        }
    }
}

void EventNormalizeHandler::Dump(int32_t fd, const std::vector<std::string> &args)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    for (auto it = dumperEventList_.begin(); it != dumperEventList_.end(); it++) {
        mprintf(fd, (*it).c_str());
    }
    mprintf(fd, "more event please view .dmp file at %s", EVENT_FILE_PATH.c_str());
}
} // namespace MMI
} // namespace OHOS
