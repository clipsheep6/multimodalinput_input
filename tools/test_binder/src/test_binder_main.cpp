/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <iostream>
#include <sstream>

#include "securec.h"

#include "hilog/log.h"
#include "mmi_log.h"
#include "time_cost_chk.h"
#include "error_multimodal.h"
#include "input_manager.h"
#include "util.h"

void Test(const char *msg)
{
    OHOS::MMI::TimeCostChk chk("test_binder_main", msg, 0, 0);
    std::vector<uint8_t> data;
    OHOS::MMI::InputManager::GetInstance()->TestOpenSSLRandBytes(data);
    std::ostringstream oss;
    for (const auto &i : data) {
        char buf[3] = {};
        sprintf_s(buf, 3, "%02x", i);
        buf[2] = '\0';
        oss << buf;
    }
    printf("cost time:%" PRId64 " us, data: %s\n", chk.GetElapsed_micro(), oss.str().c_str());
}

int32_t main(int32_t argc, char** argv)
{
    int times = atoi(argv[1]);
    for (int i = 0; i < times; ++i) {
        std::ostringstream oss;
        oss << "call[" << i << "] ";
        Test(oss.str().c_str());
    }
    
    return 0;
}