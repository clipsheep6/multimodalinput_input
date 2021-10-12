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
#include "auto_test_data_process.h"
#include "libmmi_util.h"

namespace OHOS::MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AutoTestDataProcess" };
    }
}

#ifdef OHOS_AUTO_TEST_FRAME
OHOS::MMI::AutoTestDataProcess::AutoTestDataProcess()
{
}

OHOS::MMI::AutoTestDataProcess::~AutoTestDataProcess()
{
}

void OHOS::MMI::AutoTestDataProcess::LibinputPktDataProcess(const AutoTestLibinputPkt& libinputPkt)
{
    MMI_LOGI("1.LibinputPkt: eventType= %{public}s, keyCode= %{public}d, keyState= %{public}d, rawX= %{public}lf, "
             "rawY= %{public}lf, absoluteX= %{public}lf, absoluteY= %{public}lf \n",
             libinputPkt.eventType, libinputPkt.keyCode, libinputPkt.keyState, libinputPkt.rawX, libinputPkt.rawY,
             libinputPkt.absoluteX, libinputPkt.absoluteY);
}

void OHOS::MMI::AutoTestDataProcess::MappingPktDataProcess(int32_t sourceType)
{
    MMI_LOGI(" 2.MappingPkt: sourceType= %{public}d \n", sourceType);
}

void OHOS::MMI::AutoTestDataProcess::StandardPktDataProcess(const AutoTestStandardPkt& standardPkt)
{
    MMI_LOGI("3.StandardPkt: reRventType= %{public}d, curRventType= %{public}d, keyCode= %{public}d, "
             "keyState= %{public}d, rawX= %{public}lf, rawY= %{public}lf, absoluteX= %{public}lf, "
             "absoluteY= %{public}lf \n",
             standardPkt.reRventType, standardPkt.curRventType, standardPkt.keyCode, standardPkt.keyState,
             standardPkt.rawX, standardPkt.rawY, standardPkt.absoluteX, standardPkt.absoluteY);
}

void OHOS::MMI::AutoTestDataProcess::ManagePktDataProcess(const AutoTestManagePkt& managePkt,
    std::vector<int32_t>& windowList, std::vector<AutoTestClientListPkt>& clientList)
{
    MMI_LOGI(" 4.ManagePkt: sizeOfWindowList= %{public}d, focusId= %{public}d, windowId= %{public}d, sizeOfAppManager= %{public}d \n",
        managePkt.sizeOfWindowList, managePkt.focusId, managePkt.windowId, managePkt.sizeOfAppManager);
    for (auto iter = windowList.cbegin(); iter != windowList.cend(); iter++) {
        MMI_LOGI(" 4.ManagePkt-2: windowId= %{public}d \n", *iter);
    }
    for (auto iter = clientList.cbegin(); iter != clientList.cend(); iter++) {
        MMI_LOGI("4.ManagePkt-3: clientList.fd= %{public}d, clientList.windowId= %{public}d,"
                 "clientList.abilityId= %{public}d \n", iter->socketFd, iter->windowId, iter->abilityId);
    }
}

void OHOS::MMI::AutoTestDataProcess::DispatcherPktDataProcess(const AutoTestDispatcherPkt& dispatcherPkt,
                                                              std::vector<float> standardValue)
{
    for (auto iter = standardValue.cbegin(); iter != standardValue.cend(); iter++) {
        MMI_LOGI(" 5-1.JoystickAxisStandardValue: standardValue= %{public}f \n", *iter);
    }
    MMI_LOGI("5.DispatcherPkt: eventType= %{public}s, sourceType= %{public}d, keyOfHos= %{public}d, "
             "keyState= %{public}d, rawX= %{public}lf, rawY= %{public}lf, mixedKey= %{public}d, "
             "socketFd= %{public}d, windowId= %{public}d, abilityId= %{public}d, absoluteX= %{public}lf, "
             "absoluteY= %{public}lf, deviceType = %{public}d, inputDeviceType = %{public}d slot= %{public}d \n",
             dispatcherPkt.eventType, dispatcherPkt.sourceType, static_cast<int32_t>(dispatcherPkt.keyOfHos),
             dispatcherPkt.keyState, dispatcherPkt.rawX, dispatcherPkt.rawY, dispatcherPkt.mixedKey,
             dispatcherPkt.socketFd, dispatcherPkt.windowId, dispatcherPkt.abilityId, dispatcherPkt.absoluteX,
             dispatcherPkt.absoluteY, dispatcherPkt.deviceType, dispatcherPkt.inputDeviceId, dispatcherPkt.slot);
}

void OHOS::MMI::AutoTestDataProcess::ClientPktDataProcess(const AutoTestClientPkt& clientPkt)
{
    MMI_LOGI("6.ClientPkt: eventType= %{public}s, keyOfHos= %{public}d, keyState= %{public}d, rawX= %{public}lf, "
             "rawY= %{public}lf, callBakeName= %{public}s, socketFd= %{public}d, windowId= %{public}d, "
             "abilityId= %{public}d, absoluteX= %{public}lf, absoluteY= %{public}lf, "
             "deviceType = %{public}d, inputDeviceType = %{public}d, sourceType = %{public}d, slot= %{public}d \n",
             clientPkt.eventType, static_cast<int32_t>(clientPkt.keyOfHos), clientPkt.keyState, clientPkt.rawX,
             clientPkt.rawY, clientPkt.callBakeName, clientPkt.socketFd, clientPkt.windowId, clientPkt.abilityId,
             clientPkt.absoluteX, clientPkt.absoluteY, clientPkt.deviceType, clientPkt.inputDeviceId,
             clientPkt.sourceType, clientPkt.slot);
}

void OHOS::MMI::AutoTestDataProcess::ClientListPktDataProcess(std::vector<AutoTestClientListPkt>& clientListPkt)
{
    for (auto iter = clientListPkt.cbegin(); iter != clientListPkt.cend(); iter++) {
        MMI_LOGI("7.ClientListPkt: clientList.fd= %{public}d, clientList.windowId= %{public}d, "
                 "clientList.abilityId= %{public}d \n", iter->socketFd, iter->windowId, iter->abilityId);
    }
}

void OHOS::MMI::AutoTestDataProcess::KeyTypePktDataProcess(const AutoTestKeyTypePkt& keyTypePkt)
{
    MMI_LOGI("4-1.KeyTypePkt: disSystem= %{public}d, disClient= %{public}d, disCamrea= %{public}d \n",
             keyTypePkt.disSystem, keyTypePkt.disClient, keyTypePkt.disCamrea);
}
#endif