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
 /**
 * @addtogroup multiInput
 * @{
 *
 * @brief Defines multiInput-related tests, including case data and functions for auto test,
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef OHOS_AUTO_TEST_MSG_H
#define OHOS_AUTO_TEST_MSG_H

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <thread>
#include <stdio.h>
#include "libmmi_util.h"
#include "c_singleton.h"

const int32_t PROCESS_SEEK_LENGTH = 255;
const int32_t FILE_FOUND_JURISDICTION = 777;

namespace OHOS {
    namespace MMI {
        class MultiInputCommon {
        public:
            MultiInputCommon();
            virtual ~MultiInputCommon();
            void ClientOne();
            void ClientTwo();
            void ClientThree();
            void ClientFour();
            void ClientFive();
            void ClientSix();
            void ClientSeven();
            void ClientEight();
            void WriteDeviceIni(const std::string &iniFilePath);
            void InJectionEvent(const std::string &eventvalue, const std::string &deviceMsg);
            void InitClient(const std::string &scene);
            void StartProcess(const std::string &scene);
            std::string GetProcessByName(const std::string &name);
            void SplitComplexity(const std::string &scene);
            void AssignmentResult(const std::string &scene);
            std::string valueServer;
            std::string valueClient;
        };
    }
}
#define AUTOTEST_MULTIINPUTCOMMON OHOS::MMI::CSingleton<OHOS::MMI::MultiInputCommon>::GetInstance()

#endif