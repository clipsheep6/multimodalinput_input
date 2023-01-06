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

#ifndef I_INPUT_COMMAND_H
#define I_INPUT_COMMAND_H

#include <initializer_list>

#include <getopt.h>

#include "input_manager_command.h"
#include "pointer_event.h"

#define RETCASE(parcel, ret) \
    do { \
        if ((parcel) != RET_OK) { \
            return ret; \
        } \
    } while (0)

namespace OHOS {
namespace MMI {
inline constexpr int32_t SLEEPTIME = 20;
inline constexpr int32_t MOUSE_ID = 7;
inline constexpr int32_t JOYSTICK_BUTTON_ID = 25;
inline constexpr int32_t TWO_MORE_COMMAND = 2;
inline constexpr int32_t THREE_MORE_COMMAND = 3;
inline constexpr int32_t MAX_PRESSED_COUNT = 30;
inline constexpr int32_t ACTION_TIME = 3000;
inline constexpr int32_t BLOCK_TIME_MS = 10;
inline constexpr int64_t MIN_TAKTTIME_MS = 1;
inline constexpr int64_t MAX_TAKTTIME_MS = 15000;

class IInputCommand {
public:
    IInputCommand() = default;
    virtual int32_t CommandHandle(int32_t argc, char *argv[], char* &optarg, int32_t &optind, int32_t &optionIndex) = 0;
protected:
    template <typename T, typename ...Args>
    T MatchCommand(int32_t type, const T one, const T two, const Args& ...args)
    {
        if (type == static_cast<int32_t>('d')) {
            return one;
        } else if (type == static_cast<int32_t>('u')) {
            return two;
        } else {
            T temp {};
            (void)std::initializer_list<T>{ (temp = args, one)... };
            return temp;
        }
    }
public:
    InputManagerCommand manager;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_COMMAND_H