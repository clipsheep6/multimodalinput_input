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
#include "prepare_dinput_callback.h"
#include "singleton.h"
#include "start_dinput_callback.h"
#include "stop_dinput_callback.h"
#include "unprepare_dinput_callback.h"

namespace OHOS {
namespace MMI {

struct DMouseLocation {
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
    DInputManager() {}
    virtual ~DInputManager() {}
    void SetMouseLocation(DMouseLocation info);
    DMouseLocation& GetMouseLocation();
    std::vector<int32_t> GetInputVirtualDeviceIds();
    std::string GetDeviceId();
    virtual int32_t PrepareRemoteInput(const std::string& deviceId, sptr<PrepareDInputCallback> callback);
    virtual int32_t UnPrepareRemoteInput(const std::string& deviceId, sptr<UnprepareDInputCallback> callback);
    virtual int32_t StartRemoteInput(const std::string& deviceId, sptr<StartDInputCallback> callback);
    virtual int32_t StopRemoteInput(const std::string& deviceId, sptr<StopDInputCallback> callback);

private:
    int32_t fd_ = 0;
    DMouseLocation mouseLocation = {};
    std::string deviceId_ = "";
};
}
}
#define DInputMgr OHOS::MMI::DInputManager::GetInstance()
#endif
