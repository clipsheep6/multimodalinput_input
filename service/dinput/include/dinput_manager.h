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
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL

#ifndef OHOS_DINPUT_MANAGER_H
#define OHOS_DINPUT_MANAGER_H

#include <string>
#include <vector>
#include "constants_dinput.h"
#include "i_call_dinput.h"
#include "key_event.h"
#include "singleton.h"


namespace OHOS {
namespace MMI {
struct DMouseLocation {
    int32_t globalX;
    int32_t globalY;
    int32_t dx;
    int32_t dy;
    int32_t displayId;
};
const uint32_t MOUSE_ABILITY = 1;
const int32_t KEYBOARD_ABILITY = 2;
const uint32_t TOUCHPAD_ABILITY = 4;
const uint32_t FULL_ABILITY = 7;
class DInputManager : public DelayedSingleton<DInputManager> {
public:
    DInputManager() {}
    ~DInputManager() {}
    void SetMouseLocation(DMouseLocation info);
    DMouseLocation& GetMouseLocation();
    bool IsControllerSide(uint32_t inputAbility);
    bool IsDistributedInput(uint32_t inputAbility);
    bool IsNeedFilterOut(const std::string deviceId, const std::shared_ptr<KeyEvent> key);
    virtual bool IsNeedFilterOut(const std::string& deviceId,
        const OHOS::DistributedHardware::DistributedInput::BusinessEvent& businessEvent);
    virtual OHOS::DistributedHardware::DistributedInput::DInputServerType IsStartDistributedInput(uint32_t inputAbility);
    bool CheckWhiteList(const std::shared_ptr<KeyEvent> key, bool &jumpIntercept);
    OHOS::DistributedHardware::DistributedInput::DInputServerType GetDInputServerType(uint32_t inputAbility);

    int32_t PrepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput);
    int32_t UnprepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput);
    int32_t StartRemoteInput(const std::string& deviceId, uint32_t inputAbility, sptr<ICallDinput> prepareDinput);
    int32_t StopRemoteInput(const std::string& deviceId, uint32_t inputAbility, sptr<ICallDinput> prepareDinput);
    void OnStartRemoteInputSucess(const std::string& deviceId, const uint32_t& inputTypes);
    void OnStopRemoteInputSucess(const std::string& deviceId, const uint32_t& inputTypes);
private:
    DMouseLocation mouseLocation = {};
    int32_t inputTypes_ = 0;
};
}
}
#define DInputMgr OHOS::MMI::DInputManager::GetInstance()
#endif
#endif