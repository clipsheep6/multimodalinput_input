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
#ifndef OHOS_MMI_ST_DATA_PROCESS_H
#define OHOS_MMI_ST_DATA_PROCESS_H

#include <iostream>
#include <vector>
#include <regex>
#include "singleton.h"
#include "libmmi_util.h"
#include <map>
#include "auto_test_multimodal.h"

struct ClientList {
    int32_t fd;
    int32_t surfaceId;
    int32_t abilityId;
};

namespace OHOS {
namespace MMI {
struct Libinput {
    std::string eventType;
    uint32_t keyCode;
    int32_t keyState;
    int32_t rawX;
    int32_t rawY;
    int32_t absoluteX;
    int32_t absoluteY;
};

struct Mapping {
    int32_t eventType;
};

struct Standard {
    int32_t reRventType;
    int32_t curRventType;
    uint32_t key;
    int32_t State;
    int32_t rawX;
    int32_t rawY;
    int32_t absoluteX;
    int32_t absoluteY;
};

struct ManagePkt {
    int32_t WMS;
    int32_t CALL_AMS;
    int32_t dis_System;
    int32_t dis_Client;
    int32_t dis_Camrea;
    int32_t sizeOfAppManager;
    std::vector<int32_t> window_List;
    std::vector<struct ClientList> appManage;
};

struct Dispatcher {
    std::string event_Type;
    int32_t source_Type;
    int32_t L3key;
    int32_t state;
    int32_t rawX;
    int32_t rawY;
    int32_t mixedKey;
    int32_t fd;
    int32_t surfaceId;
    int32_t abilityId;
    int32_t absoluteX;
    int32_t absoluteY;
    std::vector<float> axisValueList;
    int32_t slot;
    std::string deviceType;
    std::string deviceId;
};

struct Client_Pkt {
    std::string event_Type;
    int32_t key;
    int32_t state;
    int32_t rawX;
    int32_t rawY;
    std::string onFun;
    int32_t fd;
    int32_t surfaceId;
    int32_t abilityId;
    int32_t absoluteX;
    int32_t absoluteY;
    int32_t source_Type;
    int32_t slot;
    std::string deviceType;
    std::string deviceId;
};

class STDataProcess : public OHOS::Singleton<OHOS::MMI::STDataProcess> {
public:
    STDataProcess();
    virtual ~STDataProcess();

    void LibinputPktDataProcess(const AutoTestLibinputPkt& libinputPkt);
    void MappingPktDataProcess(int32_t sourceType);
    void StandardPktDataProcess(const AutoTestStandardPkt& standardPkt);
    void ManagePktDataProcess(const AutoTestManagePkt& managePkt, std::vector<int32_t>& windowList, 
    std::vector<AutoTestClientListPkt>& clientList, AutoTestCoordinate coordinate);
    void DispatcherPktDataProcess(const AutoTestDispatcherPkt& dispatcherPkt, std::vector<float> standardValue);
    void ClientPktDataProcess(const AutoTestClientPkt& clientPkt);
    void ClientListPktDataProcess(std::vector<AutoTestClientListPkt>& clientListPkt);
    void KeyTypePktDataProcess(const AutoTestKeyTypePkt& keyTypePkt);
    struct ClientList GetClientMsg(int surfaceId);
	std::string GetWindowsList();
	struct AutoTestCoordinate GetCoordinate();
    std::string GetLibinputValue();
	std::string GetLibinputValue_touchScreen();
    std::string GetmappingResult();
    std::string GetStandardResult();
    std::string GetActualServerResult();
    std::string GetActualServerResult1();
    std::string GetActualClientResult();
	std::string GetActualServerResult_touchScreen();
	std::string GetActualClientResult_longClick();
    std::string GetActualClientResult1();
    std::string GetDispatcherStr();
    std::string MixedKeyServer();
    std::string MixedKeyClient();
    std::vector<float> GetAxisStrList();
    std::string GetInputValue(const std::string &event_type, const std::string &l3Key, const std::string &state,
                              const std::string &rawX, const std::string &rawY);
    std::string GetInputValue1(const std::string &event_type, const std::string &l3Key,
                                                  const std::string &state, const std::string &rawX,
                                                  const std::string &rawY, const std::string &source_Type,
                                                  const std::string &slot);
    void ModifyVariable(std::string &eventType, std::string &l3key, std::string &state, std::string &rawX,
                        std::string &rawY);
    void ModifyVariable1(std::string &eventType, std::string &l3Key, std::string &state,
                                            std::string &rawX, std::string &rawY,
                                            std::string &source_Type, std::string &slot);
	std::string GetActualInput();
    int GetFocusId();
    void ClearAll();

// API Use Data
    void ApiOnKeyClientPktDataProcess(const OnKeyDataToApi& clientPkt);
    void ApiRegisterClientPktDataProcess(const RegisterDataToApi& clientPkt);
    void ApiDeviceClientPktDataProcess(const DeviceDataToApi& clientPkt);
    void ApiMouseClientPktDataProcess(const MouseDataToApi& clientPkt);
    void ApiTouchClientPktDataProcess(const ThirteenParamTouchToApi& clientPkt);
    void ApiTouchFingerClientPktDataProcess(const TouchFingersToApi& clientPkt);
    void ApiTouchCoordinateClientPktDataProcess(const TouchCoordinateToApi& clientPkt);
    void ApiTouchTwyParClientPktDataProcess(const TwentyParamTouchToApi& clientPkt);
    void ApiTouchTwyParFinClientPktDataProcess(const TwyParTouchFinToApi& clientPkt);
    void ApiTouchStylusClientPktDataProcess(const StylusToApi& clientPkt);
    OnKeyDataToApi GetOnKeyData();
    RegisterDataToApi GetRegisterData();
    DeviceDataToApi GetDeviceData();
    MouseDataToApi GetMouseData();
    CoordinateToApi GetMouseCoordinateData();
    ThirteenParamTouchToApi GetTouchData();
    TouchFingersToApi GetTouchFingerData();
    TouchCoordinateToApi GetTouchCoordinateData();
    TwentyParamTouchToApi GetTouchTwyParData();
    TwyParTouchFinToApi GetTouchTwyParFinData();
    StylusToApi GetStylusData();

private:
	std::map < int, struct ClientList > clientMap_;
	std::string windowList = "";
	struct AutoTestCoordinate autoTestCoordinate = {};
    struct Libinput libinputValue_;
    struct Mapping mapping_ = {};
    struct Standard standardValue_ = {};
    struct ManagePkt manageValue_ = {};
	std::vector<struct Libinput> libinputList;
    std::vector<struct Dispatcher> dispatcherList_;
    std::vector<float> axisValueList_;
    std::vector<struct Client_Pkt> clientPktList_;
    std::vector<std::string> mixedKeyServer_;
    std::vector<std::string> mixedKeyClient_;
    std::string deviceType_;
    std::string deviceId_;

// API Use Data
    struct OnKeyDataToApi onKeyData_ = {};
    struct RegisterDataToApi registerData_ = {};
    struct DeviceDataToApi deviceData_ = {};
    struct MouseDataToApi mouseData_ = {};
    struct CoordinateToApi coordinateData_ = {};
    struct ThirteenParamTouchToApi touchData_ {};
    struct TouchFingersToApi touchFingerData_ {};
    struct TouchCoordinateToApi touchCoordinateData_ {};
    struct TwentyParamTouchToApi touchTwyParData_ {};
    struct TwyParTouchFinToApi touchTwyParFinData_ {};
    struct StylusToApi stylusData_ {};
};
} // namespace MMI
} // namespace OHOS

// 单例接口
inline OHOS::MMI::STDataProcess& GetDataProcess()
{
    return OHOS::MMI::STDataProcess::GetInstance();
}
#endif // OHOS_MMI_ST_DATA_PROCESS_H
