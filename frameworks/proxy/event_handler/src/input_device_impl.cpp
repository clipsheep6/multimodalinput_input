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

#include "error_multimodal.h"
#include "input_manager_impl.h"
#include "multimodal_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceImpl"};
} // namespace

InputDeviceImpl& InputDeviceImpl::GetInstance()
{
    static InputDeviceImpl instance;
    return instance;
}

void InputDeviceImpl::RegisterInputDeviceMonitor(std::function<void(std::string, int32_t)> listening)
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    auto eventHandler = InputMgrImpl->GetCurrentEventHandler();
    CHKPV(eventHandler);
    auto listen = std::make_pair(eventHandler, listening);
    devMonitor_ = listen;
    MMIEventHdl.RegisterInputDeviceMonitor();
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::UnRegisterInputDeviceMonitor()
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    MMIEventHdl.UnRegisterInputDeviceMonitor();
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::OnDevMonitorTask(DevMonitor devMonitor, std::string type, int32_t deviceId)
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    devMonitor.second(type, deviceId);
    MMI_HILOGD("report device changed task, event type:%{public}s", type.c_str());
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::OnDevMonitor(std::string type, int32_t deviceId)
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    std::lock_guard<std::mutex> guard(mtx_);
    if (!MMIEventHandler::PostTask(devMonitor_.first,
        std::bind(&InputDeviceImpl::OnDevMonitorTask, this, devMonitor_, type, deviceId))) {
        MMI_HILOGE("post task failed");
    }
    MMI_HILOGD("report device changed, event type:%{public}s", type.c_str());
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::GetInputDeviceIdsAsync(std::function<void(int32_t, std::vector<int32_t>&)> callback)
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    auto eventHandler = InputMgrImpl->GetCurrentEventHandler();
    CHKPV(eventHandler);
    InputDeviceData data;
    data.ids = std::make_pair(eventHandler, callback);
    if (userData_ == INT32_MAX) {
        MMI_HILOGE("userData exceeds the maximum");
        return;
    }
    inputDevices_[userData_] = data;
    MMIEventHdl.GetDeviceIds(userData_);
    ++userData_;
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::GetInputDeviceAsync(int32_t deviceId,
    std::function<void(int32_t, std::shared_ptr<InputDeviceInfo>)> callback)
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    auto eventHandler = InputMgrImpl->GetCurrentEventHandler();
    CHKPV(eventHandler);
    InputDeviceData data;
    data.inputDevice = std::make_pair(eventHandler, callback);
    if (userData_ == INT32_MAX) {
        MMI_HILOGE("userData exceeds the maximum");
        return;
    }
    inputDevices_[userData_] = data;
    MMIEventHdl.GetDevice(userData_, deviceId);
    ++userData_;
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::SupportKeys(int32_t deviceId, std::vector<int32_t> keyCodes,
    std::function<void(std::vector<bool>&)> callback)
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    std::lock_guard<std::mutex> guard(mtx_);
    auto eventHandler = InputMgrImpl->GetCurrentEventHandler();
    CHKPV(eventHandler);
    InputDeviceData data;
    data.keys = std::make_pair(eventHandler, callback);
    if (userData_ == INT32_MAX) {
        MMI_HILOGE("userData exceeds the maximum");
        return;
    }
    inputDevices_[userData_] = data;
    MMIEventHdl.SupportKeys(userData_, deviceId, keyCodes);
    ++userData_;
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::GetKeyboardTypeAsync(int32_t deviceId, std::function<void(int32_t, int32_t)> callback)
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    auto eventHandler = InputMgrImpl->GetCurrentEventHandler();
    CHKPV(eventHandler);
    InputDeviceData data;
    data.kbTypes = std::make_pair(eventHandler, callback);
    if (userData_ == INT32_MAX) {
        MMI_HILOGE("userData exceeds the maximum");
        return;
    }
    inputDevices_[userData_] = data;
    MMIEventHdl.GetKeyboardType(userData_, deviceId);
    ++userData_;
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::OnInputDeviceTask(InputDeviceImpl::DevInfo devInfo, int32_t userData,
    std::shared_ptr<InputDeviceInfo> devData)
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    CHK_PIDANDTID();
    CHKPV(devData);
    devInfo.second(userData, devData);
    MMI_HILOGD("report device info task, userData:%{public}d name:%{public}s",
        userData, devData->name.c_str());
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::OnInputDevice(int32_t userData, std::shared_ptr<InputDeviceInfo> devData)
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    CHK_PIDANDTID();
    std::lock_guard<std::mutex> guard(mtx_);
    auto iter = inputDevices_.find(userData);
    if (iter == inputDevices_.end()) {
        MMI_HILOGD("find userData failed");
        return;
    }
    if (iter->second.cppDev != nullptr) {
        CHKPV(devData);
        iter->second.cppDev(devData);
        MMI_HILOGD("innerkits interface");
        return;
    }
    auto devInfo = GetDeviceInfo(userData);
    CHKPV(devInfo);
    if (!MMIEventHandler::PostTask(devInfo->first,
        std::bind(&InputDeviceImpl::OnInputDeviceTask, this, *devInfo, userData, devData))) {
        MMI_HILOGE("post task failed");
    }
    MMI_HILOGD("report device info, userData:%{public}d name:%{public}s type:%{public}d",
        userData, devData->name.c_str(), devData->deviceType);
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::OnInputDeviceIdsTask(InputDeviceImpl::DevIds devIds, int32_t userData, std::vector<int32_t> ids)
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    CHK_PIDANDTID();
    devIds.second(userData, ids);
    MMI_HILOGD("report all device, userData:%{public}d devices:(%{public}s)",
        userData, IdsListToString(ids).c_str());
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::OnInputDeviceIds(int32_t userData, std::vector<int32_t> &ids)
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    CHK_PIDANDTID();
    std::lock_guard<std::mutex> guard(mtx_);
    auto iter = inputDevices_.find(userData);
    if (iter == inputDevices_.end()) {
        MMI_HILOGD("find userData failed");
        return;
    }
    if (iter->second.cppIds != nullptr) {
        iter->second.cppIds(ids);
        MMI_HILOGD("innerkits interface");
        return;
    }
    auto devIds = GetDeviceIds(userData);
    CHKPV(devIds);
    if (!MMIEventHandler::PostTask(devIds->first,
        std::bind(&InputDeviceImpl::OnInputDeviceIdsTask, this, *devIds, userData, ids))) {
        MMI_HILOGE("post task failed");
    }
    MMI_HILOGD("report all device, userData:%{public}d device:(%{public}s)",
        userData, IdsListToString(ids).c_str());
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::OnSupportKeysTask(InputDeviceImpl::DevKeys devKeys, int32_t userData,
    std::vector<bool> keystrokeAbility)
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    CHK_PIDANDTID();
    devKeys.second(keystrokeAbility);
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::OnSupportKeys(int32_t userData, const std::vector<bool> &keystrokeAbility)
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    CHK_PIDANDTID();
    std::lock_guard<std::mutex> guard(mtx_);
    auto iter = inputDevices_.find(userData);
    if (iter == inputDevices_.end()) {
        MMI_HILOGD("find userData failed");
        return;
    }
    auto devKeys = GetDeviceKeys(userData);
    CHKPV(devKeys);
    if (!MMIEventHandler::PostTask(devKeys->first,
        std::bind(&InputDeviceImpl::OnSupportKeysTask, this, *devKeys, userData, keystrokeAbility))) {
        MMI_HILOGE("post task failed");
    }
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::OnKeyboardTypeTask(InputDeviceImpl::DevKeyboardTypes kbTypes, int32_t userData,
    int32_t keyboardType)
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    CHK_PIDANDTID();
    kbTypes.second(userData, keyboardType);
    MMI_HILOGD("Keyboard type event callback userData:%{public}d keyboardType:(%{public}d)",
        userData, keyboardType);
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

void InputDeviceImpl::OnKeyboardType(int32_t userData, int32_t keyboardType)
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    CHK_PIDANDTID();
    std::lock_guard<std::mutex> guard(mtx_);
    if (auto iter = inputDevices_.find(userData); iter == inputDevices_.end()) {
        MMI_HILOGD("find userData failed");
        return;
    }
    auto devKbTypes = GetKeyboardTypes(userData);
    CHKPV(devKbTypes);
    if (!MMIEventHandler::PostTask(devKbTypes->first,
        std::bind(&InputDeviceImpl::OnKeyboardTypeTask, this, *devKbTypes, userData, keyboardType))) {
        MMI_HILOGE("post task failed");
    }
    MMI_HILOGD("Keyboard type event userData:%{public}d keyboardType:%{public}d",
        userData, keyboardType);
#else
    MMI_HILOGW("device manager dose not support");
#endif
}

const InputDeviceImpl::DevInfo* InputDeviceImpl::GetDeviceInfo(int32_t userData) const
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    auto iter = inputDevices_.find(userData);
    if (iter == inputDevices_.end()) {
        return nullptr;
    }
    return &iter->second.inputDevice;
#else
    MMI_HILOGW("device manager dose not support");
    return nullptr;
#endif
}

const InputDeviceImpl::DevIds* InputDeviceImpl::GetDeviceIds(int32_t userData) const
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    auto iter = inputDevices_.find(userData);
    if (iter == inputDevices_.end()) {
        return nullptr;
    }
    return &iter->second.ids;
#else
    MMI_HILOGW("device manager dose not support");
    return nullptr;
#endif
}

const InputDeviceImpl::DevKeys* InputDeviceImpl::GetDeviceKeys(int32_t userData) const
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    auto iter = inputDevices_.find(userData);
    if (iter == inputDevices_.end()) {
        return nullptr;
    }
    return &iter->second.keys;
#else
    MMI_HILOGW("device manager dose not support");
    return nullptr;
#endif
}

const InputDeviceImpl::DevKeyboardTypes* InputDeviceImpl::GetKeyboardTypes(int32_t userData) const
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    auto iter = inputDevices_.find(userData);
    return iter == inputDevices_.end()? nullptr : &iter->second.kbTypes;
#else
    MMI_HILOGW("device manager dose not support");
    return nullptr;
#endif
}

int32_t InputDeviceImpl::GetUserData()
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    return userData_;
#else
    MMI_HILOGW("device manager dose not support");
    return ERROR_UNSUPPORT;
#endif
}
} // namespace MMI
} // namespace OHOS