/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "injection_event_dispatch.h"

int32_t main(int32_t argc, const char* argv[])
{
    if (argc < OHOS::MMI::ARGV_VALID) {
        printf("Invaild Input Para, Plase Check the validity of the para! errCode:%d\n", OHOS::PARAM_INPUT_FAIL);
        return RET_ERR;
    }
    OHOS::MMI::InjectionEventDispatch injection;
    if (!(injection.StartLog())) {
        printf("log start error\n");
        return RET_ERR;
    }
    std::vector<std::string> argvs;
    for (int32_t i = 0; i < argc; i++) {
        argvs.push_back(argv[i]);
    }
    injection.Init();
    if (!(injection.VirifyArgvs(argc, argvs))) {
        printf("Invaild Input Para, Plase Check the validity of the para! errCode:%d\n", OHOS::PARAM_INPUT_FAIL);
        return RET_ERR;
    }
    injection.Run();

    return RET_OK;
}
