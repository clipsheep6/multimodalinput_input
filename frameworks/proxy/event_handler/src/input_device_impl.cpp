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

#include "input_device_impl.h"

#include <algorithm>

#include "mmi_log.h"
#include "multimodal_event_handler.h"
#include "multimodal_input_connect_manager.h"
#include "napi_constants.h"
#include "net_packet.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceImpl" };
} // namespace

InputDeviceImpl& InputDeviceImpl::GetInstance()
{
    static InputDeviceImpl instance;
    return instance;
}

int32_t InputDeviceImpl::RegisterDevListener(const std::string &type, InputDevListenerPtr listener)
{
    CALL_DEBUG_ENTER;
    CHKPR(listener, ERROR_NULL_POINTER);
    if (type != CHANGED_TYPE) {
        MMI_HILOGE("Failed to register, listener event must be \"change\"");
        return RET_ERR;
    }
    auto iter = devListener_.find(CHANGED_TYPE);
    if (iter == devListener_.end()) {
        MMI_HILOGE("Find change failed");
        return RET_ERR;
    }
    auto &listeners = iter->second;

    if (!isListeningProcess_) {
        MMI_HILOGI("Start monitoring");
        isListeningProcess_ = true;
        int32_t ret = MultimodalInputConnMgr->RegisterDevListener();
        if (ret != RET_OK) {
            MMI_HILOGE("Failed to register");
            return ret;
        }
    }
    if (std::all_of(listeners.cbegin(), listeners.cend(),
                    [listener](InputDevListenerPtr tListener) {
                        return (tListener != listener);
                    })) {
        MMI_HILOGI("Add device listener");
        listeners.push_back(listener);
    } else {
        MMI_HILOGW("The listener already exists");
    }
    return RET_OK;
}

int32_t InputDeviceImpl::UnregisterDevListener(const std::string &type, InputDevListenerPtr listener)
{
    CALL_DEBUG_ENTER;
    if (type != CHANGED_TYPE) {
        MMI_HILOGE("Failed to cancel registration, listener event must be \"change\"");
        return RET_ERR;
    }
    auto iter = devListener_.find(CHANGED_TYPE);
    if (iter == devListener_.end()) {
        MMI_HILOGE("Find change failed");
        return RET_ERR;
    }
    if (listener == nullptr) {
        iter->second.clear();
        goto listenerLabel;
    }
    for (auto it = iter->second.begin(); it != iter->second.end(); ++it) {
        if (*it == listener) {
            iter->second.erase(it);
            goto listenerLabel;
        }
    }

listenerLabel:
    if (isListeningProcess_ && iter->second.empty()) {
        isListeningProcess_ = false;
        return MultimodalInputConnMgr->UnregisterDevListener();
    }
    return RET_OK;
}

void InputDeviceImpl::OnDevListener(int32_t deviceId, const std::string &type)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    auto iter = devListener_.find("change");
    if (iter == devListener_.end()) {
        MMI_HILOGE("Find change failed");
        return;
    }
    for (const auto &item : iter->second) {
        MMI_HILOGI("Report device change task, event type:%{public}s", type.c_str());
        if (type == "add") {
            item->OnDeviceAdded(deviceId, type);
            continue;
        }
        item->OnDeviceRemoved(deviceId, type);
    }
}

int32_t InputDeviceImpl::GetInputDeviceIds(FunInputDevIds callback)
{
    CALL_DEBUG_ENTER;
    CHKPR(callback, RET_ERR);
    std::vector<int32_t> ids;
    int32_t ret = MultimodalInputConnMgr->GetDeviceIds(ids);
    if (ret != RET_OK) {
        MMI_HILOGE("GetInputDeviceIds failed");
        return RET_ERR;
    }
    callback(ids);
    return ret;
}

int32_t InputDeviceImpl::GetInputDevice(int32_t deviceId, FunInputDevInfo callback)
{
    CALL_DEBUG_ENTER;
    CHKPR(callback, RET_ERR);
    std::shared_ptr<InputDevice> inputDevice = std::make_shared<InputDevice>();
    int32_t ret = MultimodalInputConnMgr->GetDevice(deviceId, inputDevice);
    if (ret != RET_OK) {
        MMI_HILOGE("GetDevice failed");
        return RET_ERR;
    }
    callback(inputDevice);
    return RET_OK;
}

int32_t InputDeviceImpl::SupportKeys(int32_t deviceId, std::vector<int32_t> keyCodes, FunInputDevKeys callback)
{
    CALL_DEBUG_ENTER;
    CHKPR(callback, RET_ERR);
    std::vector<bool> keystroke;
    int32_t ret = MultimodalInputConnMgr->SupportKeys(deviceId, keyCodes, keystroke);
    if (ret != RET_OK) {
        MMI_HILOGE("SupportKeys failed");
        return RET_ERR;
    }
    callback(keystroke);
    return RET_OK;
}

int32_t InputDeviceImpl::GetKeyboardType(int32_t deviceId, FunKeyboardTypes callback)
{
    CALL_DEBUG_ENTER;
    CHKPR(callback, RET_ERR);
    int32_t keyboardType = 0;
    int32_t ret = MultimodalInputConnMgr->GetKeyboardType(deviceId, keyboardType);
    if (ret != RET_OK) {
        MMI_HILOGE("GetKeyboardType failed");
        return RET_ERR;
    }
    callback(keyboardType);
    return RET_OK;
}

int32_t InputDeviceImpl::GetUserData()
{
    return userData_;
}
} // namespace MMI
} // namespace OHOS
