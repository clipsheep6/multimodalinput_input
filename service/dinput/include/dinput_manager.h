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

#ifndef OHOS_DINPUT_MANAGER_H
#define OHOS_DINPUT_MANAGER_H

#include <string>
#include <vector>
#include "singleton.h"
//#include "uds_session.h"
#include "mmi_prepare_d_input_call_back_stub.h"
#include "mmi_start_d_input_call_back_stub.h"
#include "mmi_stop_d_input_call_back_stub.h"
#include "mmi_unprepare_d_input_call_back_stub.h"

namespace OHOS {
namespace MMI {

struct DMouseLocation{
    int32_t globalX;
    int32_t globalY;
    int32_t dx;
    int32_t dy;
    int32_t displayId;
    int32_t logicalDisplayWidth;
    int32_t logicalDisplayHeight;
    int32_t logicalDisplayTopLeftX;
    int32_t logicalDisplayTopLeftY;
};

class DInputManager : public DelayedSingleton<DInputManager> {
public:
    DInputManager();
    virtual ~DInputManager();
    void SetMouseLocation(DMouseLocation info);
    DMouseLocation& GetMouseLocation();
    std::vector<int32_t> GetInputVirtualDeviceIds();
    // int32_t PrepareRemoteInput(int32_t taskId ,const std::string& deviceId, SessionPtr session);
    // int32_t UnprepareRemoteInput(int32_t taskId ,const std::string& deviceId, SessionPtr session);
    // int32_t StartRemoteInput(int32_t taskId ,const std::string& deviceId, SessionPtr session);
    // int32_t StopRemoteInput(int32_t taskId ,const std::string& deviceId, SessionPtr session);
    std::string GetDeviceId();
    virtual int32_t PrepareRemoteInputToDinput(const std::string& deviceId, sptr<MultimodalPrepareDInputCallback> callback);
    virtual int32_t UnPrepareRemoteInputToDinput(const std::string& deviceId, sptr<MultimodalUnPrepareDInputCallback> callback);
    virtual int32_t StartRemoteInputToDinput(const std::string& deviceId, sptr<MultimodalStartDInputCallback> callback);
    virtual int32_t StopRemoteInputToDinput(const std::string& deviceId, sptr<MultimodalStopDInputCallback> callback);

private:
    int32_t fd_ = 0;
    //SessionPtr sess_ = nullptr;
    DMouseLocation mouseLocation = {};
    std::string deviceId_ = "";
};
}
}
#define DInputMgr OHOS::MMI::DInputManager::GetInstance()
#endif
