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
#ifndef HDI_INJECT_H
#define HDI_INJECT_H
#ifdef OHOS_BUILD_HDF

#include <list>
#include <vector>

#include "input_type.h"

#include "input_manager.h"
#include "uds_server.h"

namespace OHOS {
namespace MMI {
class HdiInject final {
    DECLARE_DELAYED_SINGLETON(HdiInject);

enum HdiInfoType {
    GET_STATUS_INFO = 1001,
    SET_HOT_PLUGS = 1002,
    SET_EVENT_INJECT = 1003,
    GET_DEVICE_INFO = 1004,
    SHOW_DEVICE_INFO = 1005,
    REPLY_STATUS_INFO = 2001,
};

enum HdiDeviceStatus {
    HDI_DEVICE_ADD_STATUS = 0,
    HDI_DEVICE_REMOVE_STATUS = 1,
};

struct DeviceInformation {
    bool status { false };
    int32_t devIndex { 0 };
    int32_t devType { 0 };
    int16_t fd { 0 };
    char chipName[32];
};
public:
    DISALLOW_COPY_AND_MOVE(HdiInject);
    bool Init(UDSServer &sess);
    void StartHdiserver();
    void ShowAllDeviceInfo();
    void InitDeviceInfo();
    int32_t GetDeviceCount();
    bool SyncDeviceHotStatus();
    bool ReportHotPlugEvent();
    void OnInitHdiServerStatus();
    int32_t GetDevTypeIndex(int32_t devIndex);
    int32_t GetDevIndexType(int32_t devType);
    int32_t ManageHdfInject(const MMI::SessionPtr sess, MMI::NetPacket &pkt);
    void OnSetHotPlugs(uint32_t devIndex, uint32_t devSatatus);
    int32_t OnSetEventInject(const RawInputEvent& allEvent, int32_t devType);
    bool SetDeviceHotStatus(int32_t devIndex, int32_t status);
    int32_t ScanInputDevice(uint32_t arrLen, DevDesc *staArr);
    bool ReportHotPlugEvent(uint32_t devIndex, uint32_t status);
public:
    InputHostCb hotPlugcallback_;
    InputEventCb eventcallback_;
private:
    bool initStatus_ { false };
    HotPlugEvent** event_ { nullptr };
    MMI::UDSServer* udsServerPtr_ { nullptr };
    std::vector<DeviceInformation> deviceArray_ = {};
};

#define MMIHdiInject ::OHOS::DelayedSingleton<HdiInject>::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // OHOS_BUILD_HDF
#endif // HDI_INJECT_H