/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "input_manager_command.h"
#ifdef OHOS_BUILD_ENABLE_EVENT_RECORDER
#include "input_replay_command.h"
#endif // OHOS_BUILD_ENABLE_EVENT_RECORDER

int32_t main(int32_t argc, char** argv)
{
#ifdef OHOS_BUILD_ENABLE_EVENT_RECORDER
    if (argc > 1 && (std::string(argv[1]) == "replay" || std::string(argv[1]) == "record")) {
        return OHOS::MMI::InputReplayCommand::HandleRecordReplayCommand(argc, argv);
    }
#endif // OHOS_BUILD_ENABLE_EVENT_RECORDER
    OHOS::MMI::InputManagerCommand command;
    return command.ParseCommand(argc, argv);
}