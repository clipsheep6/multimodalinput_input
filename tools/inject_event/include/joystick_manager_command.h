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

#ifndef JOYSTICK_MANAGER_COMMAND_H
#define JOYSTICK_MANAGER_COMMAND_H

#include "i_input_command.h"

namespace OHOS {
namespace MMI {
class JoystickManagerCommand : public IInputCommand {
private:
    struct JoystickInfo {
        int32_t buttonId { -1 };
        int32_t absValue { -1 };
        int32_t taktTime { 0 };
        PointerEvent::AxisType absType;
    };
public:
    JoystickManagerCommand() = default;
    int32_t CommandHandle(int32_t argc, char *argv[], char* &optarg, int32_t &optind, int32_t &optionIndex);
    int32_t MoveCommand(char* &optarg, JoystickInfo &joyInfo);
    int32_t ButtonCommand(int32_t c, char* &optarg, JoystickInfo &joyInfo);

    int32_t SetJoystickEvent(int32_t argc, char *argv[], std::shared_ptr<PointerEvent> pointerEvent,
        JoystickInfo &joyInfo, int32_t &optind);
    int32_t ParmSetJoystickEvent(int32_t argc, char *argv[],
        std::shared_ptr<PointerEvent> pointerEvent, int32_t &optind);
private:
    std::vector<std::pair<int32_t, JoystickInfo>> state_;

    enum JoystickEvent {
        JOYSTICK_BUTTON_UP,
        JOYSTICK_BUTTON_PRESS,
        JOYSTICK_MOVE,
        JOYSTICK_CLICK,
        JOYSTICK_INTERVAL
    };
};
} // namespace MMI
} // namespace OHOS
#endif // JOYSTICK_MANAGER_COMMAND_H