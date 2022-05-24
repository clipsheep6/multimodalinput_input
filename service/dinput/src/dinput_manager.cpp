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

#include "define_multimodal.h"
#include "dinput_callback.h"
#include "dinput_manager.h"
#include "distributed_input_kit.h"
#include "input_device_manager.h"

namespace OHOS {
namespace MMI {
namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "DInputManager" };
}

void DInputManager::SetMouseLocation(DMouseLocation info)
{
    mouseLocation = info;
}

DMouseLocation& DInputManager::GetMouseLocation()
{
    return this->mouseLocation;
}

bool DInputManager::IsControllerSide(uint32_t inputAbility)
{
    CALL_LOG_ENTER;
    using namespace OHOS::DistributedHardware::DistributedInput;
    DInputServerType type = IsStartDistributedInput(inputAbility);
    MMI_HILOGD("DistributedInputKit::IsStartDistributedInput():%{public}d", type);
    if (DInputServerType::SINK_SERVER_TYPE != type) {
        return true;
    }
    return false;
}

bool DInputManager::IsDistributedInput(uint32_t inputAbility)
{
    CALL_LOG_ENTER;
    using namespace OHOS::DistributedHardware::DistributedInput;
    DInputServerType type = IsStartDistributedInput(inputAbility);
    MMI_HILOGD("DistributedInputKit::IsStartDistributedInput():%{public}d", type);
    if (DInputServerType::NULL_SERVER_TYPE != type) {
        return true;
    }
    return false;
}

bool DInputManager::CheckWhiteList(const std::shared_ptr<KeyEvent> key, bool &jumpIntercept)
{
    CALL_LOG_ENTER;
    jumpIntercept = false;
    std::string deviceId = "";
    DistributedHardware::DistributedInput::DInputServerType type = GetDInputServerType(KEYBOARD_ABILITY);
    if (DistributedHardware::DistributedInput::DInputServerType::SOURCE_SERVER_TYPE == type) {
        std::shared_ptr<InputDevice> inputDevice = InputDevMgr->GetRemoteInputDevice(key->GetDeviceId());
        if (inputDevice != nullptr) {
            deviceId = inputDevice->GetNetworkId();
            if (!IsNeedFilterOut(deviceId, key)) {
                return true;
            }
        }
    } else if (DistributedHardware::DistributedInput::DInputServerType::SINK_SERVER_TYPE == type) {
        if (!IsNeedFilterOut(deviceId, key)) {
            return true;
        }
        jumpIntercept = true;
    }
    return false;
}

OHOS::DistributedHardware::DistributedInput::DInputServerType DInputManager::GetDInputServerType(uint32_t inputAbility)
{
    CALL_LOG_ENTER;
    using namespace OHOS::DistributedHardware::DistributedInput;
    DInputServerType type = IsStartDistributedInput(inputAbility);
    MMI_HILOGD("DistributedInputKit::IsStartDistributedInput():%{public}d", type);
    return type;
}

OHOS::DistributedHardware::DistributedInput::DInputServerType DInputManager::IsStartDistributedInput(uint32_t inputAbility)
{
    MMI_HILOGD("enter: inputAbility = %{public}d", inputAbility);
    return OHOS::DistributedHardware::DistributedInput::DistributedInputKit::IsStartDistributedInput(inputAbility);
}

bool DInputManager::IsNeedFilterOut(const std::string deviceId, const std::shared_ptr<KeyEvent> key)
{
    CALL_LOG_ENTER;
    std::vector<OHOS::MMI::KeyEvent::KeyItem> pressedKeys = key->GetKeyItems();
    std::vector<int32_t> pressedKeysForDInput;
    pressedKeysForDInput.reserve(pressedKeys.size());
    for (int i = 0; i < pressedKeys.size(); i++) {
        pressedKeysForDInput.push_back(pressedKeys[i].GetKeyCode());
    }
    using namespace OHOS::DistributedHardware::DistributedInput;
    OHOS::DistributedHardware::DistributedInput::BusinessEvent businessEvent;
    businessEvent.keyCode = key->GetKeyCode();
    businessEvent.keyAction = key->GetKeyAction();
    businessEvent.pressedKeys = pressedKeysForDInput;
    for (const auto &item : businessEvent.pressedKeys) {
        MMI_HILOGD("pressedKeys :%{public}d", item);
    }

    MMI_HILOGD("istributedInputKit::IsNeedFilterOut deviceId:%{public}s, keyCode :%{public}d, keyAction :%{public}d",
        deviceId.c_str(), businessEvent.keyCode, businessEvent.keyAction);
    bool ret = IsNeedFilterOut(deviceId, businessEvent);
    MMI_HILOGD("istributedInputKit::IsNeedFilterOut:%{public}s", ret == true ? "true" : "false");
    return ret;
}

bool DInputManager::IsNeedFilterOut(const std::string& deviceId,
    const OHOS::DistributedHardware::DistributedInput::BusinessEvent& businessEvent)
{
    return OHOS::DistributedHardware::DistributedInput::DistributedInputKit::IsNeedFilterOut(deviceId, businessEvent);
}

int32_t DInputManager::PrepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput)
{
    CALL_LOG_ENTER;
    sptr<PrepareDInputCallback> callback = new PrepareDInputCallback(prepareDinput);
    return OHOS::DistributedHardware::DistributedInput::DistributedInputKit::PrepareRemoteInput(deviceId, callback);
}

int32_t DInputManager::UnprepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput)
{
    CALL_LOG_ENTER;
    sptr<UnprepareDInputCallback> callback = new UnprepareDInputCallback(prepareDinput);
    return OHOS::DistributedHardware::DistributedInput::DistributedInputKit::UnprepareRemoteInput(deviceId, callback);
}

int32_t DInputManager::StartRemoteInput(const std::string& deviceId, uint32_t inputAbility, sptr<ICallDinput> prepareDinput)
{
    CALL_LOG_ENTER;
    sptr<StartDInputCallback> callback = new StartDInputCallback(prepareDinput);
    return OHOS::DistributedHardware::DistributedInput::DistributedInputKit::StartRemoteInput(deviceId, inputAbility, callback);
}

int32_t DInputManager::StopRemoteInput(const std::string& deviceId, uint32_t inputAbility, sptr<ICallDinput> prepareDinput)
{
    CALL_LOG_ENTER;
    sptr<StopDInputCallback> callback = new StopDInputCallback(prepareDinput);
    return OHOS::DistributedHardware::DistributedInput::DistributedInputKit::StopRemoteInput(deviceId, inputAbility, callback);
}

void DInputManager::OnStartRemoteInputSucess(const std::string& deviceId, const uint32_t& inputTypes)
{
   MMI_HILOGD("enter: inputTypes = %{public}d", inputTypes);
   int32_t diffBit = (inputTypes_ & FULL_ABILITY) ^ (inputTypes & FULL_ABILITY);
   MMI_HILOGD("diffBit = %{public}d", diffBit);
   if (diffBit == 0) {
       return;
   }
   inputTypes_ = (inputTypes_ & FULL_ABILITY) | (inputTypes & FULL_ABILITY);
   MMI_HILOGD("inputTypes_ = %{public}d", inputTypes_);
   int32_t addTypes = diffBit & (inputTypes & FULL_ABILITY);
   MMI_HILOGD("addTypes = %{public}d", addTypes);
   if (addTypes) {
       InputDevMgr->OnStartRemoteInputSucess(deviceId, addTypes);
   }
}

void DInputManager::OnStopRemoteInputSucess(const std::string& deviceId, const uint32_t& inputTypes)
{
    MMI_HILOGD("enter: inputTypes = %{public}d", inputTypes);
    int32_t removeTypes = (inputTypes_ & FULL_ABILITY) & (inputTypes & FULL_ABILITY);
    MMI_HILOGD("removeTypes = %{public}d", removeTypes);
    if (removeTypes == 0) {
       return;
    }
    inputTypes_ = inputTypes_ ^ removeTypes;
    MMI_HILOGD("inputTypes_ = %{public}d", inputTypes_);
    InputDevMgr->OnStopRemoteInputSucess(deviceId, removeTypes);
}
}
}
