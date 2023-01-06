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

#ifndef TOUCH_MANAGER_COMMAND_H
#define TOUCH_MANAGER_COMMAND_H

#include "i_input_command.h"

namespace OHOS {
namespace MMI {
class TouchManagerCommand : public IInputCommand {
public:
    TouchManagerCommand() = default;
    int32_t CommandHandle(int32_t argc, char *argv[], char* &optarg, int32_t &optind, int32_t &optionIndex);
    int32_t MoveCommand(int32_t &argc, char *argv[], char* &optarg, int32_t &optind);
    int32_t UpDownCommand(int32_t argc, char *argv[], int32_t c, char* &optarg, int32_t &optind);
    int32_t ClickCommand(int32_t argc, char *argv[], char* &optarg, int32_t &optind);
    int32_t IntervalCommand(char* &optarg);
    int32_t DragCommand(int32_t& argc, char *argv[], char* &optarg, int32_t &optind);

    void SetTouchEvent(int32_t px, int32_t py, int32_t action, PointerEvent::PointerItem &item,
        std::shared_ptr<PointerEvent>& pointerEvent, int64_t timeMs = -1);
    int32_t ParamMoveCommand(int32_t argc, char *argv[], char* &optarg, int32_t &optind);
    int32_t ParamDragCommand(int32_t& argc, char *argv[], char* &optarg, int32_t &optind, int32_t &pressTimems);

private:
    int32_t px1_ { 0 };
    int32_t py1_ { 0 };
    int32_t px2_ { 0 };
    int32_t py2_ { 0 };
    int32_t totalTimeMs_ { 0 };
    int32_t moveArgcSeven_ { 7 };
};
} // namespace MMI
} // namespace OHOS
#endif // TOUCH_MANAGER_COMMAND_H