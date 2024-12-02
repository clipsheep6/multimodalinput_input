/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "libinput_adapter.h"

#include <cinttypes>
#include <climits>
#include <regex>

#include <dirent.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "define_multimodal.h"
#include "i_input_windows_manager.h"
#include "param_wrapper.h"
#include "util.h"
#include "input_device_manager.h"

#undef MMI_LOG_DOMAIN
#define MMI_LOG_DOMAIN MMI_LOG_SERVER
#undef MMI_LOG_TAG
#define MMI_LOG_TAG "LibinputAdapter"

namespace OHOS {
namespace MMI {
namespace {
constexpr int32_t WAIT_TIME_FOR_INPUT { 10 };
constexpr int32_t MAX_RETRY_COUNT { 5 };
constexpr int32_t MIN_RIGHT_BTN_AREA_PERCENT { 0 };
constexpr int32_t MAX_RIGHT_BTN_AREA_PERCENT { 100 };
constexpr int32_t INVALID_RIGHT_BTN_AREA { -1 };
#ifdef OHOS_BUILD_ENABLE_VKEYBOARD
constexpr int32_t VKEY_TP_SM_MSG_SIZE { 6 };
constexpr int32_t VKEY_TP_SM_MSG_TYPE_IDX { 0 };
constexpr int32_t VKEY_TP_SM_MSG_POINTER_ID_IDX { 1 };
constexpr int32_t VKEY_TP_SM_MSG_POS_X_IDX { 2 };
constexpr int32_t VKEY_TP_SM_MSG_POS_Y_IDX { 3 };
constexpr int32_t VKEY_TP_SM_MSG_SCALE_IDX { 4 };
constexpr int32_t VKEY_TP_SM_MSG_ANGLE_IDX { 5 };
constexpr int32_t VKEY_TP_GSE_TWO_FINGERS { 2 };
constexpr uint32_t VKEY_TP_LB_ID { 272 };
constexpr uint32_t VKEY_TP_RB_ID { 273 };
constexpr uint32_t VKEY_TP_SEAT_BTN_COUNT_NONE { 0 };
constexpr uint32_t VKEY_TP_SEAT_BTN_COUNT_ONE { 1 };
constexpr uint32_t VKEY_TP_AXES_ZERO { 0 };
constexpr uint32_t VKEY_TP_AXES_ONE { 1 };
constexpr double VTP_SCALE_AND_ANGLE_FACTOR { 1000.0 };
enum class VKeyboardTouchEventType : int32_t {
    TOUCH_DOWN = 0,
    TOUCH_UP = 1,
    TOUCH_MOVE = 2,
};
#endif // OHOS_BUILD_ENABLE_VKEYBOARD

void HiLogFunc(struct libinput* input, libinput_log_priority priority, const char* fmt, va_list args)
{
    CHKPV(input);
    CHKPV(fmt);
    char buffer[256] = {};
    if (vsnprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, fmt, args) == -1) {
        MMI_HILOGE("Call vsnprintf_s failed");
        va_end(args);
        return;
    }
    if (strstr(buffer, "LOG_LEVEL_I") != nullptr) {
        MMI_HILOGI("PrintLog_Info:%{public}s", buffer);
    } else if (strstr(buffer, "LOG_LEVEL_D") != nullptr) {
        MMI_HILOGD("PrintLog_Info:%{public}s", buffer);
    } else if (strstr(buffer, "LOG_LEVEL_E") != nullptr) {
        MMI_HILOGE("PrintLog_Info:%{public}s", buffer);
    } else {
        MMI_HILOGD("PrintLog_Info:%{public}s", buffer);
    }
    va_end(args);
}
} // namespace

int32_t LibinputAdapter::DeviceLedUpdate(struct libinput_device *device, int32_t funcKey, bool enable)
{
    CHKPR(device, RET_ERR);
    return libinput_set_led_state(device, funcKey, enable);
}

void LibinputAdapter::InitRightButtonAreaConfig()
{
    CHKPV(input_);

    int32_t height_percent = OHOS::system::GetIntParameter("const.multimodalinput.rightclick_y_percentage",
                                                           INVALID_RIGHT_BTN_AREA);
    if ((height_percent <= MIN_RIGHT_BTN_AREA_PERCENT) || (height_percent > MAX_RIGHT_BTN_AREA_PERCENT)) {
        MMI_HILOGE("Right button area height percent param is invalid");
        return;
    }

    int32_t width_percent = OHOS::system::GetIntParameter("const.multimodalinput.rightclick_x_percentage",
                                                          INVALID_RIGHT_BTN_AREA);
    if ((width_percent <= MIN_RIGHT_BTN_AREA_PERCENT) || (width_percent > MAX_RIGHT_BTN_AREA_PERCENT)) {
        MMI_HILOGE("Right button area width percent param is invalid");
        return;
    }

    auto status = libinput_config_rightbutton_area(input_, height_percent, width_percent);
    if (status != LIBINPUT_CONFIG_STATUS_SUCCESS) {
        MMI_HILOGE("Config the touchpad right button area failed");
    }
}

constexpr static libinput_interface LIBINPUT_INTERFACE = {
    .open_restricted = [](const char *path, int32_t flags, void *user_data)->int32_t {
        if (path == nullptr) {
            MMI_HILOGWK("Input device path is nullptr");
            return RET_ERR;
        }
        char realPath[PATH_MAX] = {};
        if (realpath(path, realPath) == nullptr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_FOR_INPUT));
            MMI_HILOGWK("The error path is %{public}s", path);
            return RET_ERR;
        }
        int32_t fd = 0;
        for (int32_t i = 0; i < MAX_RETRY_COUNT; i++) {
            fd = open(realPath, flags);
            if (fd >= 0) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_FOR_INPUT));
        }
        int32_t errNo = errno;
        std::regex re("(\\d+)");
        std::string str_path(path);
        std::smatch match;
        int32_t id;
        bool isPath = std::regex_search(str_path, match, re);
        if (!isPath) {
            id = -1;
        } else {
            id = std::stoi(match[0]);
        }
        MMI_HILOGWK("Libinput .open_restricted id:%{public}d, fd:%{public}d, errno:%{public}d",
            id, fd, errNo);
        return fd < 0 ? RET_ERR : fd;
    },
    .close_restricted = [](int32_t fd, void *user_data)
    {
        if (fd < 0) {
            return;
        }
        MMI_HILOGI("Libinput .close_restricted fd:%{public}d", fd);
        close(fd);
    },
};

bool LibinputAdapter::Init(FunInputEvent funInputEvent)
{
    CALL_DEBUG_ENTER;
    CHKPF(funInputEvent);
    funInputEvent_ = funInputEvent;
    input_ = libinput_path_create_context(&LIBINPUT_INTERFACE, nullptr);
    CHKPF(input_);
    libinput_log_set_handler(input_, &HiLogFunc);
    fd_ = libinput_get_fd(input_);
    if (fd_ < 0) {
        libinput_unref(input_);
        fd_ = -1;
        MMI_HILOGE("The fd_ is less than 0");
        return false;
    }
    InitRightButtonAreaConfig();
    return hotplugDetector_.Init([this](std::string path) { OnDeviceAdded(std::move(path)); },
        [this](std::string path) { OnDeviceRemoved(std::move(path)); });
}

void LibinputAdapter::EventDispatch(int32_t fd)
{
    CALL_DEBUG_ENTER;
    if (fd == fd_) {
        MMI_HILOGD("Start to libinput_dispatch");
        if (libinput_dispatch(input_) != 0) {
            MMI_HILOGE("Failed to dispatch libinput");
            return;
        }
        OnEventHandler();
        MMI_HILOGD("End to OnEventHandler");
    } else if (fd == hotplugDetector_.GetFd()) {
        hotplugDetector_.OnEvent();
    } else {
        MMI_HILOGE("EventDispatch() called with unknown fd:%{public}d", fd);
    }
}

void LibinputAdapter::Stop()
{
    CALL_DEBUG_ENTER;
    hotplugDetector_.Stop();
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
    if (input_ != nullptr) {
        libinput_unref(input_);
        input_ = nullptr;
    }
}

void LibinputAdapter::ProcessPendingEvents()
{
    OnEventHandler();
}

void LibinputAdapter::InitVKeyboard(HandleTouchPoint handleTouchPoint,
    GetMessage getMessage,
    GetAllTouchMessage getAllTouchMessage,
    ClearTouchMessage clearTouchMessage,
    GetAllKeyMessage getAllKeyMessage,
    ClearKeyMessage clearKeyMessage)
{
    handleTouchPoint_ = handleTouchPoint;
    getMessage_ = getMessage;
    getAllTouchMessage_ = getAllTouchMessage;
    clearTouchMessage_ = clearTouchMessage;
    getAllKeyMessage_ = getAllKeyMessage;
    clearKeyMessage_ = clearKeyMessage;

    deviceId = -1;
}

void LibinputAdapter::InjectKeyEvent(libinput_event_touch* touch, int32_t keyCode,
                                     libinput_key_state state, int64_t frameTime)
{
#ifdef OHOS_BUILD_ENABLE_VKEYBOARD
    libinput_event_keyboard* key_event_pressed =
            libinput_create_keyboard_event(touch, keyCode, state);

    funInputEvent_((libinput_event*)key_event_pressed, frameTime);
    free(key_event_pressed);
#endif // OHOS_BUILD_ENABLE_VKEYBOARD
}

void LibinputAdapter::InjectCombinationKeyEvent(libinput_event_touch* touch, std::vector<int32_t>& toggleKeyCodes,
                                                int32_t triggerKeyCode, int64_t frameTime)
{
    for (auto& toggleCode: toggleKeyCodes) {
        InjectKeyEvent(touch, toggleCode, libinput_key_state::LIBINPUT_KEY_STATE_PRESSED, frameTime);
    }
    InjectKeyEvent(touch, triggerKeyCode, libinput_key_state::LIBINPUT_KEY_STATE_PRESSED, frameTime);
    InjectKeyEvent(touch, triggerKeyCode, libinput_key_state::LIBINPUT_KEY_STATE_RELEASED, frameTime);
    for (auto& toggleCode: toggleKeyCodes) {
        InjectKeyEvent(touch, toggleCode, libinput_key_state::LIBINPUT_KEY_STATE_RELEASED, frameTime);
    }
}

#ifdef OHOS_BUILD_ENABLE_VKEYBOARD
void LibinputAdapter::HandleVKeyTouchpadMessages(libinput_event_touch* touch)
{
    // Handle all track pad key messages
    std::vector<std::vector<int32_t>> keyMsgList;
    if (getAllKeyMessage_ != nullptr) {
        getAllKeyMessage_(keyMsgList);
    }
    if (clearKeyMessage_ != nullptr) {
        clearKeyMessage_();
    }
    OnVKeyTrackPadMessage(touch, keyMsgList);
    // Handle all track pad touch messages
    std::vector<std::vector<int32_t>> touchMsgList;
    if (getAllTouchMessage_ != nullptr) {
        getAllTouchMessage_(touchMsgList);
    }
    if (clearTouchMessage_ != nullptr) {
        clearTouchMessage_();
    }
    OnVKeyTrackPadMessage(touch, touchMsgList);
}

void LibinputAdapter::OnVKeyTrackPadMessage(libinput_event_touch* touch,
    const std::vector<std::vector<int32_t>>& msgList)
{
    for (auto msgItem : msgList) {
        if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
            MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
                static_cast<int32_t>(msgItem.size()));
            continue;
        }
        auto msgType = static_cast<VTPStateMachineMessageType>(msgItem[VKEY_TP_SM_MSG_TYPE_IDX]);
        switch (msgType) {
            case VTPStateMachineMessageType::POINTER_MOVE:
                if (!HandleVKeyTrackPadPointerMove(touch, msgItem)) {
                    MMI_HILOGE("Virtual TrackPad pointer move event cannot be handled");
                }
                break;
            case VTPStateMachineMessageType::LEFT_CLICK_DOWN:
                if (!HandleVKeyTrackPadLeftBtnDown(touch, msgItem)) {
                    MMI_HILOGE("Virtual TrackPad left button down event cannot be handled");
                }
                break;
            case VTPStateMachineMessageType::LEFT_CLICK_UP:
                if (!HandleVKeyTrackPadLeftBtnUp(touch, msgItem)) {
                    MMI_HILOGE("Virtual TrackPad left button up event cannot be handled");
                }
                break;
            case VTPStateMachineMessageType::RIGHT_CLICK_DOWN:
                if (!HandleVKeyTrackPadRightBtnDown(touch, msgItem)) {
                    MMI_HILOGE("Virtual TrackPad right button down event cannot be handled");
                }
                break;
            case VTPStateMachineMessageType::RIGHT_CLICK_UP:
                if (!HandleVKeyTrackPadRightBtnUp(touch, msgItem)) {
                    MMI_HILOGE("Virtual TrackPad right button up event cannot be handled");
                }
                break;
            default:
                OnVKeyTrackPadGestureMessage(touch, msgType, msgItem);
                break;
        }
    }
}

void LibinputAdapter::OnVKeyTrackPadGestureMessage(libinput_event_touch* touch,
    VTPStateMachineMessageType msgType, const std::vector<int32_t>& msgItem)
{
    switch (msgType) {
        case VTPStateMachineMessageType::SCROLL_BEGIN:
            if (!HandleVKeyTrackPadScrollBegin(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad scroll begin event cannot be handled");
            }
            break;
        case VTPStateMachineMessageType::SCROLL_UPDATE:
            if (!HandleVKeyTrackPadScrollUpdate(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad scroll update event cannot be handled");
            }
            break;
        case VTPStateMachineMessageType::SCROLL_END:
            if (!HandleVKeyTrackPadScrollEnd(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad scroll end event cannot be handled");
            }
            break;
        case VTPStateMachineMessageType::PINCH_BEGIN:
            if (!HandleVKeyTrackPadPinchBegin(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad pinch begin event cannot be handled");
            }
            break;
        case VTPStateMachineMessageType::PINCH_UPDATE:
            if (!HandleVKeyTrackPadPinchUpdate(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad pinch update event cannot be handled");
            }
            break;
        case VTPStateMachineMessageType::PINCH_END:
            if (!HandleVKeyTrackPadPinchEnd(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad pinch end event cannot be handled");
            }
            break;
        default:
            OnVKeyTrackPadGestureTwoMessage(touch, msgType, msgItem);
            break;
    }
}

void LibinputAdapter::OnVKeyTrackPadGestureTwoMessage(libinput_event_touch* touch,
    VTPStateMachineMessageType msgType, const std::vector<int32_t>& msgItem)
{
    switch (msgType) {
        case VTPStateMachineMessageType::PAN_BEGIN:
            if (!HandleVKeyTrackPadPanBegin(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad pan begin event cannot be handled");
            }
            break;
        case VTPStateMachineMessageType::PAN_UPDATE:
            if (!HandleVKeyTrackPadPanUpdate(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad pan update event cannot be handled");
            }
            break;
        case VTPStateMachineMessageType::PAN_END:
            if (!HandleVKeyTrackPadPanEnd(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad pan end event cannot be handled");
            }
            break;
        case VTPStateMachineMessageType::ROT_BEGIN:
            if (!HandleVKeyTrackPadRotateBegin(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad rotate begin event cannot be handled");
            }
            break;
        case VTPStateMachineMessageType::ROT_UPDATE:
            if (!HandleVKeyTrackPadRotateUpdate(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad rotate update event cannot be handled");
            }
            break;
        case VTPStateMachineMessageType::ROT_END:
            if (!HandleVKeyTrackPadRotateEnd(touch, msgItem)) {
                MMI_HILOGE("Virtual TrackPad rotate update event cannot be handled");
            }
            break;
        default:
            break;
    }
}

bool LibinputAdapter::HandleVKeyTrackPadPointerMove(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    event_pointer pEvent;
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_MOTION_TOUCHPAD;
    pEvent.delta_raw_x = msgPPosX;
    pEvent.delta_raw_y = msgPPosY;
    libinput_event_pointer* lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    
    free(lpEvent);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadLeftBtnDown(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    event_pointer pEvent;
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_BUTTON_TOUCHPAD;
    pEvent.button = VKEY_TP_LB_ID;
    pEvent.seat_button_count = VKEY_TP_SEAT_BTN_COUNT_ONE;
    pEvent.state = libinput_button_state::LIBINPUT_BUTTON_STATE_PRESSED;
    libinput_event_pointer* lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadLeftBtnUp(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    event_pointer pEvent;
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_BUTTON_TOUCHPAD;
    pEvent.button = VKEY_TP_LB_ID;
    pEvent.seat_button_count = VKEY_TP_SEAT_BTN_COUNT_NONE;
    pEvent.state = libinput_button_state::LIBINPUT_BUTTON_STATE_RELEASED;
    libinput_event_pointer* lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadRightBtnDown(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    event_pointer pEvent;
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_BUTTON;
    pEvent.button = VKEY_TP_RB_ID;
    pEvent.seat_button_count = VKEY_TP_SEAT_BTN_COUNT_ONE;
    pEvent.state = libinput_button_state::LIBINPUT_BUTTON_STATE_PRESSED;
    libinput_event_pointer* lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadRightBtnUp(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    event_pointer pEvent;
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_BUTTON;
    pEvent.button = VKEY_TP_RB_ID;
    pEvent.seat_button_count = VKEY_TP_SEAT_BTN_COUNT_NONE;
    pEvent.state = libinput_button_state::LIBINPUT_BUTTON_STATE_RELEASED;
    libinput_event_pointer* lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadScrollBegin(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    MMI_HILOGI("VKey TrackPad Scroll Begin");
    event_pointer pEvent;
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_SCROLL_FINGER_BEGIN;
    pEvent.delta_x = msgPPosX;
    pEvent.delta_y = msgPPosY;
    pEvent.delta_raw_x = msgPPosX;
    pEvent.delta_raw_y = msgPPosY;
    pEvent.axes = VKEY_TP_AXES_ZERO;
    libinput_event_pointer* lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadScrollUpdate(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    MMI_HILOGI("VKey TrackPad Scroll Update");
    event_pointer pEvent;
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_AXIS;
    pEvent.delta_x = msgPPosX;
    pEvent.delta_y = msgPPosY;
    pEvent.delta_raw_x = msgPPosX;
    pEvent.delta_raw_y = msgPPosY;
    pEvent.source = libinput_pointer_axis_source::LIBINPUT_POINTER_AXIS_SOURCE_FINGER;
    pEvent.axes = VKEY_TP_AXES_ONE;
    libinput_event_pointer* lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_SCROLL_FINGER;
    lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadScrollEnd(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    MMI_HILOGI("VKey TrackPad Scroll End");
    event_pointer pEvent;
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_SCROLL_FINGER_END;
    pEvent.delta_x = msgPPosX;
    pEvent.delta_y = msgPPosY;
    pEvent.delta_raw_x = msgPPosX;
    pEvent.delta_raw_y = msgPPosY;
    pEvent.axes = VKEY_TP_AXES_ZERO;
    libinput_event_pointer* lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadPinchBegin(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    int32_t msgPScale = msgItem[VKEY_TP_SM_MSG_SCALE_IDX];
    double scaleToDouble = static_cast<double>(msgPScale) / VTP_SCALE_AND_ANGLE_FACTOR;
    int32_t msgPAngle = msgItem[VKEY_TP_SM_MSG_ANGLE_IDX];
    double angleToDouble = static_cast<double>(msgPAngle) / VTP_SCALE_AND_ANGLE_FACTOR;
    MMI_HILOGI("VKey TrackPad Pinch Begin scale: %{public}f, angle: %{public}f",
        scaleToDouble, angleToDouble);
    event_gesture gEvent;
    gEvent.event_type = libinput_event_type::LIBINPUT_EVENT_GESTURE_PINCH_BEGIN;
    gEvent.finger_count = VKEY_TP_GSE_TWO_FINGERS;
    gEvent.cancelled = 0;
    gEvent.delta_x = msgPPosX;
    gEvent.delta_y = msgPPosY;
    gEvent.delta_unaccel_x = msgPPosX;
    gEvent.delta_unaccel_y = msgPPosY;
    sloted_coords_info slotInfo;
    gEvent.solt_touches = slotInfo;
    gEvent.scale = scaleToDouble;
    gEvent.angle = angleToDouble;
    libinput_event_gesture* lgEvent = libinput_create_gesture_event(touch, gEvent);
    PrintVKeyTPGestureLog(gEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lgEvent, frameTime);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadPinchUpdate(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    int32_t msgPScale = msgItem[VKEY_TP_SM_MSG_SCALE_IDX];
    double scaleToDouble = static_cast<double>(msgPScale) / VTP_SCALE_AND_ANGLE_FACTOR;
    int32_t msgPAngle = msgItem[VKEY_TP_SM_MSG_ANGLE_IDX];
    double angleToDouble = static_cast<double>(msgPAngle) / VTP_SCALE_AND_ANGLE_FACTOR;
    MMI_HILOGI("VKey TrackPad Pinch Update scale: %{public}f, angle: %{public}f",
        scaleToDouble, angleToDouble);
    event_gesture gEvent;
    gEvent.event_type = libinput_event_type::LIBINPUT_EVENT_GESTURE_PINCH_UPDATE;
    gEvent.finger_count = VKEY_TP_GSE_TWO_FINGERS;
    gEvent.cancelled = 0;
    gEvent.delta_x = msgPPosX;
    gEvent.delta_y = msgPPosY;
    gEvent.delta_unaccel_x = msgPPosX;
    gEvent.delta_unaccel_y = msgPPosY;
    sloted_coords_info slotInfo;
    gEvent.solt_touches = slotInfo;
    gEvent.scale = scaleToDouble;
    gEvent.angle = angleToDouble;
    libinput_event_gesture* lgEvent = libinput_create_gesture_event(touch, gEvent);
    PrintVKeyTPGestureLog(gEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lgEvent, frameTime);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadPinchEnd(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    int32_t msgPScale = msgItem[VKEY_TP_SM_MSG_SCALE_IDX];
    double scaleToDouble = static_cast<double>(msgPScale) / VTP_SCALE_AND_ANGLE_FACTOR;
    int32_t msgPAngle = msgItem[VKEY_TP_SM_MSG_ANGLE_IDX];
    double angleToDouble = static_cast<double>(msgPAngle) / VTP_SCALE_AND_ANGLE_FACTOR;
    MMI_HILOGI("VKey TrackPad Pinch End scale: %{public}f, angle: %{public}f",
        scaleToDouble, angleToDouble);
    event_gesture gEvent;
    gEvent.event_type = libinput_event_type::LIBINPUT_EVENT_GESTURE_PINCH_END;
    gEvent.finger_count = VKEY_TP_GSE_TWO_FINGERS;
    gEvent.cancelled = 0;
    gEvent.delta_x = msgPPosX;
    gEvent.delta_y = msgPPosY;
    gEvent.delta_unaccel_x = msgPPosX;
    gEvent.delta_unaccel_y = msgPPosY;
    sloted_coords_info slotInfo;
    gEvent.solt_touches = slotInfo;
    gEvent.scale = scaleToDouble;
    gEvent.angle = angleToDouble;
    libinput_event_gesture* lgEvent = libinput_create_gesture_event(touch, gEvent);
    PrintVKeyTPGestureLog(gEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lgEvent, frameTime);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadPanBegin(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    MMI_HILOGI("VKey TrackPad Pan Begin");
    event_pointer pEvent;
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_SCROLL_FINGER_BEGIN;
    pEvent.delta_x = msgPPosX;
    pEvent.delta_y = msgPPosY;
    pEvent.delta_raw_x = msgPPosX;
    pEvent.delta_raw_y = msgPPosY;
    pEvent.axes = VKEY_TP_AXES_ZERO;
    libinput_event_pointer* lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadPanUpdate(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    MMI_HILOGI("VKey TrackPad Pan Update");
    event_pointer pEvent;
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_AXIS;
    pEvent.delta_x = msgPPosX;
    pEvent.delta_y = msgPPosY;
    pEvent.delta_raw_x = msgPPosX;
    pEvent.delta_raw_y = msgPPosY;
    pEvent.source = libinput_pointer_axis_source::LIBINPUT_POINTER_AXIS_SOURCE_FINGER;
    pEvent.axes = VKEY_TP_AXES_ONE;
    libinput_event_pointer* lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_SCROLL_FINGER;
    lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadPanEnd(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    MMI_HILOGI("VKey TrackPad Pan End");
    event_pointer pEvent;
    pEvent.event_type = libinput_event_type::LIBINPUT_EVENT_POINTER_SCROLL_FINGER_END;
    pEvent.delta_x = msgPPosX;
    pEvent.delta_y = msgPPosY;
    pEvent.delta_raw_x = msgPPosX;
    pEvent.delta_raw_y = msgPPosY;
    pEvent.axes = VKEY_TP_AXES_ZERO;
    libinput_event_pointer* lpEvent = libinput_create_pointer_event(touch, pEvent);
    PrintVKeyTPPointerLog(pEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lpEvent, frameTime);
    free(lpEvent);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadRotateBegin(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    int32_t msgPScale = msgItem[VKEY_TP_SM_MSG_SCALE_IDX];
    double scaleToDouble = static_cast<double>(msgPScale) / VTP_SCALE_AND_ANGLE_FACTOR;
    int32_t msgPAngle = msgItem[VKEY_TP_SM_MSG_ANGLE_IDX];
    double angleToDouble = static_cast<double>(msgPAngle) / VTP_SCALE_AND_ANGLE_FACTOR;
    MMI_HILOGI("VKey TrackPad Rotate Begin scale: %{public}f, angle: %{public}f",
        scaleToDouble, angleToDouble);
    event_gesture gEvent;
    gEvent.event_type = libinput_event_type::LIBINPUT_EVENT_GESTURE_PINCH_BEGIN;
    gEvent.finger_count = VKEY_TP_GSE_TWO_FINGERS;
    gEvent.cancelled = 0;
    gEvent.delta_x = msgPPosX;
    gEvent.delta_y = msgPPosY;
    gEvent.delta_unaccel_x = msgPPosX;
    gEvent.delta_unaccel_y = msgPPosY;
    sloted_coords_info slotInfo;
    gEvent.solt_touches = slotInfo;
    gEvent.scale = scaleToDouble;
    gEvent.angle = angleToDouble;
    libinput_event_gesture* lgEvent = libinput_create_gesture_event(touch, gEvent);
    PrintVKeyTPGestureLog(gEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lgEvent, frameTime);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadRotateUpdate(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    int32_t msgPScale = msgItem[VKEY_TP_SM_MSG_SCALE_IDX];
    double scaleToDouble = static_cast<double>(msgPScale) / VTP_SCALE_AND_ANGLE_FACTOR;
    int32_t msgPAngle = msgItem[VKEY_TP_SM_MSG_ANGLE_IDX];
    double angleToDouble = static_cast<double>(msgPAngle) / VTP_SCALE_AND_ANGLE_FACTOR;
    MMI_HILOGI("VKey TrackPad Rotate Update scale: %{public}f, angle: %{public}f",
        scaleToDouble, angleToDouble);
    event_gesture gEvent;
    gEvent.event_type = libinput_event_type::LIBINPUT_EVENT_GESTURE_PINCH_UPDATE;
    gEvent.finger_count = VKEY_TP_GSE_TWO_FINGERS;
    gEvent.cancelled = 0;
    gEvent.delta_x = msgPPosX;
    gEvent.delta_y = msgPPosY;
    gEvent.delta_unaccel_x = msgPPosX;
    gEvent.delta_unaccel_y = msgPPosY;
    sloted_coords_info slotInfo;
    gEvent.solt_touches = slotInfo;
    gEvent.scale = scaleToDouble;
    gEvent.angle = angleToDouble;
    libinput_event_gesture* lgEvent = libinput_create_gesture_event(touch, gEvent);
    PrintVKeyTPGestureLog(gEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lgEvent, frameTime);
    return true;
}

bool LibinputAdapter::HandleVKeyTrackPadRotateEnd(libinput_event_touch* touch,
    const std::vector<int32_t>& msgItem)
{
    if (msgItem.size() < VKEY_TP_SM_MSG_SIZE) {
        MMI_HILOGE("Virtual TrackPad state machine message size: %{public}d is not correct",
            static_cast<int32_t>(msgItem.size()));
        return false;
    }
    int32_t msgPId = msgItem[VKEY_TP_SM_MSG_POINTER_ID_IDX];
    int32_t msgPPosX = msgItem[VKEY_TP_SM_MSG_POS_X_IDX];
    int32_t msgPPosY = msgItem[VKEY_TP_SM_MSG_POS_Y_IDX];
    int32_t msgPScale = msgItem[VKEY_TP_SM_MSG_SCALE_IDX];
    double scaleToDouble = static_cast<double>(msgPScale) / VTP_SCALE_AND_ANGLE_FACTOR;
    int32_t msgPAngle = msgItem[VKEY_TP_SM_MSG_ANGLE_IDX];
    double angleToDouble = static_cast<double>(msgPAngle) / VTP_SCALE_AND_ANGLE_FACTOR;
    MMI_HILOGI("VKey TrackPad Rotate End scale: %{public}f, angle: %{public}f",
        scaleToDouble, angleToDouble);
    event_gesture gEvent;
    gEvent.event_type = libinput_event_type::LIBINPUT_EVENT_GESTURE_PINCH_END;
    gEvent.finger_count = VKEY_TP_GSE_TWO_FINGERS;
    gEvent.cancelled = 0;
    gEvent.delta_x = msgPPosX;
    gEvent.delta_y = msgPPosY;
    gEvent.delta_unaccel_x = msgPPosX;
    gEvent.delta_unaccel_y = msgPPosY;
    sloted_coords_info slotInfo;
    gEvent.solt_touches = slotInfo;
    gEvent.scale = scaleToDouble;
    gEvent.angle = angleToDouble;
    libinput_event_gesture* lgEvent = libinput_create_gesture_event(touch, gEvent);
    PrintVKeyTPGestureLog(gEvent);
    int64_t frameTime = GetSysClockTime();
    funInputEvent_((libinput_event*)lgEvent, frameTime);
    return true;
}

int32_t LibinputAdapter::ConvertToTouchEventType(
    libinput_event_type eventType)
{
    if (eventType == LIBINPUT_EVENT_TOUCH_DOWN) {
        return static_cast<int32_t>(VKeyboardTouchEventType::TOUCH_DOWN);
    } else if (eventType == LIBINPUT_EVENT_TOUCH_UP) {
        return static_cast<int32_t>(VKeyboardTouchEventType::TOUCH_UP);
    } else {
        return static_cast<int32_t>(VKeyboardTouchEventType::TOUCH_MOVE);
    }
}

void LibinputAdapter::PrintVKeyTPPointerLog(event_pointer &pEvent)
{
    MMI_HILOGD("######## pointerEvent");
    MMI_HILOGD("######## event type: %{public}d, delta.x: %{public}d, delta.y: %{public}d",
        static_cast<int32_t>(pEvent.event_type), static_cast<int32_t>(pEvent.delta_x),
        static_cast<int32_t>(pEvent.delta_y));
    MMI_HILOGD("######## delta_raw.x: %{public}d, delta_raw.y: %{public}d",
        static_cast<int32_t>(pEvent.delta_raw_x), static_cast<int32_t>(pEvent.delta_raw_y));
    MMI_HILOGD("######## absolute.x: %{public}d, absolute.y: %{public}d, source: %{public}d",
        static_cast<int32_t>(pEvent.absolute_x), static_cast<int32_t>(pEvent.absolute_y),
        static_cast<int32_t>(pEvent.source));
    MMI_HILOGD("######## axes: %{public}d, button: %{public}d, seat button count: %{public}d",
        static_cast<int32_t>(pEvent.axes), static_cast<int32_t>(pEvent.button),
        static_cast<int32_t>(pEvent.seat_button_count));
    MMI_HILOGD("######## state: %{public}d, discrete.x: %{public}d, discrete.y: %{public}d",
        static_cast<int32_t>(pEvent.state), static_cast<int32_t>(pEvent.discrete_x),
        static_cast<int32_t>(pEvent.discrete_y));
    MMI_HILOGD("######## v120.x: %{public}d, v120.y: %{public}d",
        static_cast<int32_t>(pEvent.v120_x), static_cast<int32_t>(pEvent.v120_y));
}

void LibinputAdapter::PrintVKeyTPGestureLog(event_gesture &gEvent)
{
    MMI_HILOGD("######## gestureEvent");
    MMI_HILOGD("######## event_type: %{public}d, finger_count: %{public}d, cancelled: %{public}d",
        static_cast<int32_t>(gEvent.event_type), static_cast<int32_t>(gEvent.finger_count),
        static_cast<int32_t>(gEvent.cancelled));
    MMI_HILOGD("######## delta_x: %{public}f, delta_y: %{public}f",
        static_cast<double>(gEvent.delta_x), static_cast<double>(gEvent.delta_y));
    MMI_HILOGD("######## delta_unaccel_x: %{public}f, delta_unaccel_y: %{public}f",
        static_cast<double>(gEvent.delta_unaccel_x), static_cast<double>(gEvent.delta_unaccel_y));
    MMI_HILOGD("######## solt_touches active_count: %{public}d",
        static_cast<int32_t>(gEvent.solt_touches.active_count));
    MMI_HILOGD("######## scale: %{public}f, angle: %{public}f",
        static_cast<double>(gEvent.scale), static_cast<double>(gEvent.angle));
}
#endif // OHOS_BUILD_ENABLE_VKEYBOARD

void LibinputAdapter::OnEventHandler()
{
    CALL_DEBUG_ENTER;
    CHKPV(funInputEvent_);
    libinput_event *event = nullptr;
    int64_t frameTime = GetSysClockTime();
    while ((event = libinput_get_event(input_))) {
#ifdef OHOS_BUILD_ENABLE_VKEYBOARD
        libinput_event_type eventType = libinput_event_get_type(event);
        if (eventType == LIBINPUT_EVENT_TOUCH_DOWN
            || eventType == LIBINPUT_EVENT_TOUCH_UP
            || eventType == LIBINPUT_EVENT_TOUCH_MOTION
            ) {
            libinput_event_touch* touch = libinput_event_get_touch_event(event);
            if (deviceId == -1) {
                // initialize touch device ID.
                libinput_device* device = libinput_event_get_device(event);
                deviceId = INPUT_DEV_MGR->FindInputDeviceId(device);
            }

            EventTouch touchInfo;
            int32_t logicalDisplayId = -1;
            double x = 0.0;
            double y = 0.0;
            int32_t touchId = libinput_event_touch_get_slot(touch);
            int32_t touchEventType = ConvertToTouchEventType(eventType);

            // touch up event has no coordinates information, skip coordinate calculation.
            if (eventType != LIBINPUT_EVENT_TOUCH_UP) {
                if (!WIN_MGR->TouchPointToDisplayPoint(deviceId, touch, touchInfo, logicalDisplayId)) {
                    MMI_HILOGE("Map touch point to display point failed");
                } else {
                    x = touchInfo.point.x;
                    y = touchInfo.point.y;

                    touchPoints_[touchId] = std::pair<double, double>(x, y);
                }
            } else {
                auto pos = touchPoints_.find(touchId);
                if (pos != touchPoints_.end()) {
                    x = (pos->second).first;
                    y = (pos->second).second;
                    touchPoints_.erase(pos);
                }
            }

            MMI_HILOGD("touch event. deviceId: %{private}d, touchId: %{private}d, x: %{private}d, y: %{private}d, \
type: %{private}d",
                deviceId,
                touchId,
                static_cast<int32_t>(x),
                static_cast<int32_t>(y),
                static_cast<int32_t>(eventType));

            if (handleTouchPoint_ != nullptr && handleTouchPoint_(x, y, touchId, touchEventType) == 0) {
                MMI_HILOGD("inside vkeyboard area");

                while (true) {
                    int toggleCodeFirst(-1);
                    int toggleCodeSecond(-1);
                    int keyCode(-1);
                    VKeyboardMessageType type = (VKeyboardMessageType)getMessage_(toggleCodeFirst, toggleCodeSecond,
                        keyCode);
                    MMI_HILOGD("get message type: %{private}d", static_cast<int32_t>(type));
                    if (type == VNoMessage) {
                        break;
                    }

                    switch (type) {
                        case VKeyboardMessageType::VKeyPressed: {
                            MMI_HILOGD("press key: %{private}d", keyCode);
                            InjectKeyEvent(touch, keyCode, libinput_key_state::LIBINPUT_KEY_STATE_PRESSED, frameTime);
                            InjectKeyEvent(touch, keyCode, libinput_key_state::LIBINPUT_KEY_STATE_RELEASED, frameTime);
                            break;
                        }
                        case VKeyboardMessageType::VCombinationKeyPressed: {
                            MMI_HILOGD("combination key. triger: %{private}d, toggle: %{private}d + %{private}d",
                                keyCode, toggleCodeFirst, toggleCodeSecond);
                            std::vector<int32_t> toggleKeyCodes;
                            if (toggleCodeFirst >= 0) {
                                toggleKeyCodes.push_back(toggleCodeFirst);
                            }
                            if (toggleCodeSecond >= 0) {
                                toggleKeyCodes.push_back(toggleCodeSecond);
                            }
                            InjectCombinationKeyEvent(touch, toggleKeyCodes, keyCode, frameTime);
                            break;
                        }
                        case VKeyboardMessageType::VStartLongPressControl: {
                            MMI_HILOGD("long press start: %{private}d", keyCode);
                            InjectKeyEvent(touch, keyCode, libinput_key_state::LIBINPUT_KEY_STATE_PRESSED, frameTime);
                            break;
                        }
                        case VKeyboardMessageType::VStopLongPressControl: {
                            MMI_HILOGD("long press stop: %{private}d", keyCode);
                            InjectKeyEvent(touch, keyCode, libinput_key_state::LIBINPUT_KEY_STATE_RELEASED, frameTime);
                            break;
                        }
                        default: break;
                    }
                }
                HandleVKeyTouchpadMessages(touch);
                libinput_event_destroy(event);
            } else {
                funInputEvent_(event, frameTime);
                libinput_event_destroy(event);
            }
        } else {
            funInputEvent_(event, frameTime);
            libinput_event_destroy(event);
        }
#else // OHOS_BUILD_ENABLE_VKEYBOARD
        funInputEvent_(event, frameTime);
        libinput_event_destroy(event);
#endif // OHOS_BUILD_ENABLE_VKEYBOARD
    }
    if (event == nullptr) {
        funInputEvent_(nullptr, 0);
    }
}

void LibinputAdapter::ReloadDevice()
{
    CALL_DEBUG_ENTER;
    CHKPV(input_);
    libinput_suspend(input_);
    libinput_resume(input_);
}

void LibinputAdapter::OnDeviceAdded(std::string path)
{
    std::regex re("(\\d+)");
    std::string str_path(path);
    std::smatch match;
    int32_t id;
    bool isPath = std::regex_search(str_path, match, re);
    if (!isPath) {
        id = -1;
    } else {
        id = std::stoi(match[0]);
    }
    MMI_HILOGI("OnDeviceAdded id:%{public}d", id);
    auto pos = devices_.find(path);
    if (pos != devices_.end()) {
        MMI_HILOGD("Path is found");
        return;
    }
    libinput_device* device = libinput_path_add_device(input_, path.c_str());
    if (device != nullptr) {
        devices_[std::move(path)] = libinput_device_ref(device);
        // Libinput doesn't signal device adding event in path mode. Process manually.
        OnEventHandler();
    }
}

void LibinputAdapter::OnDeviceRemoved(std::string path)
{
    std::regex re("(\\d+)");
    std::string str_path(path);
    std::smatch match;
    int32_t id;
    bool isPath = std::regex_search(str_path, match, re);
    if (!isPath) {
        id = -1;
    } else {
        id = std::stoi(match[0]);
    }
    MMI_HILOGI("OnDeviceRemoved id:%{public}d", id);
    auto pos = devices_.find(path);
    if (pos != devices_.end()) {
        libinput_path_remove_device(pos->second);
        libinput_device_unref(pos->second);
        devices_.erase(pos);
        // Libinput doesn't signal device removing event in path mode. Process manually.
        OnEventHandler();
    }
}
} // namespace MMI
} // namespace OHOS
