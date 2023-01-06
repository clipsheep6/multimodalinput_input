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

#ifndef MOUSE_MANAGER_COMMAND_H
#define MOUSE_MANAGER_COMMAND_H

#include "i_input_command.h"

namespace OHOS {
namespace MMI {
class MouseManagerCommand : public IInputCommand {
public:
    MouseManagerCommand() = default;

    struct Param {
        int32_t px1 = 0;
        int32_t py1 = 0;
        int32_t px2 = 0;
        int32_t py2 = 0;
        int32_t totalTimeMs = 1000;
        bool foundTraceOption = false;
    };

    int32_t CommandHandle(int32_t argc, char *argv[], char* &optarg, int32_t &optind, int32_t &optionIndex);
    int32_t MoveCommand(int32_t argc, char *argv[], char* &optarg, int32_t &optind);
    int32_t ClickCommand(char* &optarg);
    int32_t DoubleClickCommand(int32_t argc, char *argv[], char* &optarg, int32_t &optind);
    int32_t UpDownCommand(int32_t c, char* &optarg, int32_t &optind);
    int32_t ScrollCommand(char* &optarg);
    int32_t DragCommand(int32_t &argc, char *argv[], char* &optarg, int32_t &optind);
    int32_t IntervalCommand(char* &optarg);

    int32_t ParamDoubleClickCommand(int32_t &argc, char *argv[], char* &optarg,
        int32_t &optind, int32_t pressTimeMs, int32_t clickIntervalTimeMs);
    bool IsTraceOption(const std::string &opt1);
    bool TraceMode(int32_t argCount, char *argvOffset[]);
    void SetMoveEvent(int32_t px, int32_t py, PointerEvent::PointerItem &item,
        std::shared_ptr<PointerEvent>& pointerEvent, int64_t timeMs = -1);
    int32_t NoTraceMode(int32_t &argc, char *argv[], char* &optarg, int32_t &optind);
    int32_t NoTraceModeParam(int32_t &argc, char *argv[], char* &optarg, int32_t &optind, Param &param);
    int32_t ParamDragParam(int32_t &argc, char *argv[], char* &optarg, int32_t &optind, Param &param);

private:
    int32_t px_ { 0 };
    int32_t py_ { 0 };
    int32_t buttonId_ { -1 };
    int32_t scrollValue_ { -1 };
};
} // namespace MMI
} // namespace OHOS
#endif // MOUSE_MANAGER_COMMAND_H