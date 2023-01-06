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

#ifndef KEYBOARD_MANAGER_COMMAND_H
#define KEYBOARD_MANAGER_COMMAND_H

#include <vector>

#include "i_input_command.h"

namespace OHOS {
namespace MMI {
class KeyboardManagerCommand : public IInputCommand {
public:
    KeyboardManagerCommand() = default;
    int32_t CommandHandle(int32_t argc, char *argv[], char* &optarg, int32_t &optind, int32_t &optionIndex);
    int32_t DownCommand(char* &optarg, int32_t &optind);
    int32_t UpCommand(char* &optarg, int32_t &optind);
    int32_t LongPressCommand(int32_t argc, char *argv[], char* &optarg, int32_t &optind);
    int32_t IntervalCommand(char* &optarg, int32_t &optind);

    int32_t ParamLongCommand(int32_t& argc, char *argv[], char* &optarg, int32_t &optind, int32_t &pressTimeMs);

private:
    std::vector<int32_t> downKey_;
    int32_t keyCode_ { 0 };
    int32_t isCombinationKey_ { 0 };
    int64_t time_ { -1 };
};
} // namespace MMI
} // namespace OHOS
#endif // KEYBOARD_MANAGER_COMMAND_H