/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "joystick_manager_command.h"

#include <iostream>

#include "string_ex.h"

#include "error_multimodal.h"
#include "define_multimodal.h"
#include "input_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JoystickManagerCommand" };
struct option joystickSensorOptions[] = {
    {"move", required_argument, NULL, 'm'},
    {"down", required_argument, NULL, 'd'},
    {"up", required_argument, NULL, 'u'},
    {"click", required_argument, NULL, 'c'},
    {"interval", required_argument, NULL, 'i'},
    {NULL, 0, NULL, 0}
};
} // namespace

int32_t JoystickManagerCommand::CommandHandle(int32_t argc, char *argv[], char* &optarg,
    int32_t &optind, int32_t &optionIndex)
{
    JoystickManagerCommand::JoystickInfo joyInfo;
    int32_t c = 0;
    while ((c = getopt_long(argc, argv, "m:d:u:c:i:", joystickSensorOptions, &optionIndex)) != -1) {
        switch (c) {
            case 'm': {
                RETCASE(JoystickManagerCommand::MoveCommand(optarg, joyInfo), EVENT_REG_FAIL);
                break;
            }
            case 'd':
            case 'u':
            case 'c': {
                RETCASE(JoystickManagerCommand::ButtonCommand(c, optarg, joyInfo), EVENT_REG_FAIL);
                break;
            }
            case 'i': {
                if (!StrToInt(optarg, joyInfo.taktTime)) {
                    std::cout << "Invalid command to interval time" << std::endl;
                    return EVENT_REG_FAIL;
                }
                state_.push_back(std::pair<int32_t, JoystickInfo>(JOYSTICK_INTERVAL, joyInfo));
                break;
            }
            default: {
                std::cout << "Invalid options" << std::endl;
                manager.ShowUsage();
                return EVENT_REG_FAIL;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
    }
    auto pointerEvent = PointerEvent::Create();
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    RETCASE(JoystickManagerCommand::SetJoystickEvent(argc, argv, pointerEvent, joyInfo, optind), EVENT_REG_FAIL);
    return RET_OK;
}

int32_t JoystickManagerCommand::MoveCommand(char* &optarg, JoystickInfo &joyInfo)
{
    std::string arg(optarg);
    std::string::size_type pos = arg.find('=');
    if (pos == std::string::npos) {
        std::cout << "Parameter format is error" << std::endl;
        return EVENT_REG_FAIL;
    }
    std::string absAction = arg.substr(0, pos);
    if (absAction == "x") {
        joyInfo.absType = PointerEvent::AxisType::AXIS_TYPE_ABS_X;
    } else if (absAction == "y") {
        joyInfo.absType = PointerEvent::AxisType::AXIS_TYPE_ABS_Y;
    } else if (absAction == "z") {
        joyInfo.absType = PointerEvent::AxisType::AXIS_TYPE_ABS_Z;
    } else if (absAction == "rz") {
        joyInfo.absType = PointerEvent::AxisType::AXIS_TYPE_ABS_RZ;
    } else if (absAction == "gas") {
        joyInfo.absType = PointerEvent::AxisType::AXIS_TYPE_ABS_GAS;
    } else if (absAction == "brake") {
        joyInfo.absType = PointerEvent::AxisType::AXIS_TYPE_ABS_BRAKE;
    } else if (absAction == "hat0x") {
        joyInfo.absType = PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0X;
    } else if (absAction == "hat0y") {
        joyInfo.absType = PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0Y;
    } else if (absAction == "throttle") {
        joyInfo.absType = PointerEvent::AxisType::AXIS_TYPE_ABS_THROTTLE;
    } else {
        std::cout << "Invalid abstype" << std::endl;
        return RET_ERR;
    }
    if (!StrToInt(arg.substr(pos + 1), joyInfo.absValue)) {
        std::cout << "Invalid parameter to move absValue" << std::endl;
        return EVENT_REG_FAIL;
    }
    state_.push_back(std::pair<int32_t, JoystickInfo>(JOYSTICK_MOVE, joyInfo));
    return RET_OK;
}

int32_t JoystickManagerCommand::ButtonCommand(int32_t c, char* &optarg, JoystickInfo &joyInfo)
{
    if (c != static_cast<int32_t>('d') &&  c != static_cast<int32_t>('u') && c != static_cast<int32_t>('c')) {
        return EVENT_REG_FAIL;
    }
    auto typeMatch = MatchCommand(c, "press", "raise", "click");
    if (!StrToInt(optarg, joyInfo.buttonId)) {
        std::cout << "Invalid "<< typeMatch <<" button command" << std::endl;
        return EVENT_REG_FAIL;
    }
    if (joyInfo.buttonId > JOYSTICK_BUTTON_ID) {
        std::cout << typeMatch <<" button value is greater than the max value" << std::endl;
        return EVENT_REG_FAIL;
    }
    int32_t match = MatchCommand(c, JOYSTICK_BUTTON_PRESS, JOYSTICK_BUTTON_UP, JOYSTICK_CLICK);
    state_.push_back(std::pair<int32_t, JoystickInfo>(match, joyInfo));
    return RET_OK;
}

int32_t JoystickManagerCommand::SetJoystickEvent(int32_t argc, char *argv[],
    std::shared_ptr<PointerEvent> pointerEvent, JoystickInfo &joyInfo, int32_t &optind)
{
    RETCASE(JoystickManagerCommand::ParmSetJoystickEvent(argc, argv, pointerEvent, optind), EVENT_REG_FAIL);
    auto setEvent = [pointerEvent](int32_t action, int32_t buttonId) {
        pointerEvent->SetPointerAction(action);
        pointerEvent->SetButtonId(buttonId);
        pointerEvent->SetButtonPressed(buttonId);
    };
    for (const auto &it : state_) {
        if (it.first == JOYSTICK_BUTTON_PRESS) {
            std::cout << "Press down " << it.second.buttonId <<std::endl;
            setEvent(PointerEvent::POINTER_ACTION_BUTTON_DOWN, it.second.buttonId);
        } else if (it.first == JOYSTICK_BUTTON_UP) {
            std::cout << "Lift up button " << it.second.buttonId << std::endl;
            setEvent(PointerEvent::POINTER_ACTION_BUTTON_UP, it.second.buttonId);
        } else if (it.first == JOYSTICK_MOVE) {
            pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_AXIS_UPDATE);
            pointerEvent->SetAxisValue(it.second.absType, it.second.absValue);
        } else if (it.first == JOYSTICK_CLICK) {
            std::cout << "Click " << it.second.buttonId << std::endl;
            pointerEvent->SetPointerId(0);
            setEvent(PointerEvent::POINTER_ACTION_BUTTON_DOWN, it.second.buttonId);
            pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_JOYSTICK);
            InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
            setEvent(PointerEvent::POINTER_ACTION_BUTTON_UP, it.second.buttonId);
        } else if (it.first == JOYSTICK_INTERVAL) {
            if ((MIN_TAKTTIME_MS > joyInfo.taktTime) || (MAX_TAKTTIME_MS < joyInfo.taktTime)) {
                std::cout << "TaktTime is out of range" << std::endl;
                return EVENT_REG_FAIL;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(joyInfo.taktTime));
            continue;
        }
        pointerEvent->SetPointerId(0);
        pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_JOYSTICK);
        InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    }
    return RET_OK;
}

int32_t JoystickManagerCommand::ParmSetJoystickEvent(int32_t argc, char *argv[],
    std::shared_ptr<PointerEvent> pointerEvent, int32_t &optind)
{
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    if (optind < argc) {
        std::cout << "non-option argv elements: ";
        while (optind < argc) {
            std::cout << argv[optind++] << "\t";
        }
        std::cout << std::endl;
        return EVENT_REG_FAIL;
    }
    if (state_.empty()) {
        std::cout << "Injection failed" << std::endl;
        return EVENT_REG_FAIL;
    }
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS