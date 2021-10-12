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
#ifndef OHOS_AUTO_TEST_DATA_PROCESS_H
#define OHOS_AUTO_TEST_DATA_PROCESS_H
#include "libmmi_util.h"

#ifdef OHOS_AUTO_TEST_FRAME
#include <iostream>
#include <vector>

namespace OHOS {
    namespace MMI {
        class AutoTestDataProcess {
        public:
            AutoTestDataProcess();
            virtual ~AutoTestDataProcess();

            void LibinputPktDataProcess(const AutoTestLibinputPkt& libinputPkt);
            void MappingPktDataProcess(int32_t sourceType);
            void StandardPktDataProcess(const AutoTestStandardPkt& standardPkt);
            void ManagePktDataProcess(const AutoTestManagePkt& managePkt,
                std::vector<int32_t>& windowList, std::vector<AutoTestClientListPkt>& clientList);
            void DispatcherPktDataProcess(const AutoTestDispatcherPkt& dispatcherPkt, std::vector<float> standardValue);
            void ClientPktDataProcess(const AutoTestClientPkt& clientPkt);
            void ClientListPktDataProcess(std::vector<AutoTestClientListPkt>& clientListPkt);
            void KeyTypePktDataProcess(const AutoTestKeyTypePkt& keyTypePkt);

        protected:
        };
    }
}

// 单例接口
#define AUTOTEST_DATAPROCESS OHOS::MMI::CSingleton<OHOS::MMI::AutoTestDataProcess>::GetInstance()
#endif
#endif