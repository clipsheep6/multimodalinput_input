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

#include "input_device_manger.h"

#include <iostream>

#include "joystick_manager_command.h"
#include "keyboard_manager_commond.h"
#include "mouse_manager_command.h"
#include "touch_manager_command.h"

namespace OHOS {
namespace MMI {
IInputCommand* InputDeviceManager::CreatDevice(int32_t type)
{
    switch (type) {
        case 'M': {
            return new MouseManagerCommand();
            break;
        }
        case 'K': {
            return new KeyboardManagerCommand();
            break;
        }
        case 'T': {
            return new TouchManagerCommand();
            break;
        }
        case 'J': {
            return new JoystickManagerCommand();
            break;
        }
        case '?': {
            manager.ShowUsage();
            return nullptr;
        }
        default: {
            std::cout << "invalid command" << std::endl;
            manager.ShowUsage();
            return nullptr;
        }
    }
}
} // namespace MMI
} // namespace OHOS
