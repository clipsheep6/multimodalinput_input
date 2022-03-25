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
#include "mmi_client.h"
#include "multimodal_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceImpl"}; // namepace
}
InputDeviceImpl& InputDeviceImpl::GetInstance()
{
    static InputDeviceImpl instance;
    return instance;
}

void InputDeviceImpl::GetInputDeviceIdsAsync(int32_t userData,
    std::function<void(int32_t, std::vector<int32_t>)> callback)
{
    MMI_LOGD("begin");
    inputDevciceIds_[userData] = callback;
    MMIEventHdl.GetDeviceIds(userData);
    MMI_LOGD("end");
}

void InputDeviceImpl::GetInputDeviceAsync(int32_t userData, int32_t deviceId,
    std::function<void(int32_t, std::shared_ptr<InputDeviceInfo>)> callback)
{
    MMI_LOGD("begin");
    inputDevcices_[userData] = callback;
    MMIEventHdl.GetDevice(userData, deviceId);
    MMI_LOGD("end");
}

void InputDeviceImpl::OnInputDevice(int32_t userData, int32_t id, std::string name, int32_t deviceType)
{
    MMI_LOGD("begin");
    auto iter = inputDevcices_.find(userData);
    if (iter == inputDevcices_.end()) {
        MMI_LOGE("failed to find the callback function");
        return;
    }
    auto inputDeviceInfo = std::make_shared<InputDeviceInfo>(id, name, deviceType);
    iter->second(userData, inputDeviceInfo);
    MMI_LOGD("end");
}

void InputDeviceImpl::OnInputDeviceIds(int32_t userData, std::vector<int32_t> ids)
{
    MMI_LOGD("begin");
    auto iter = inputDevciceIds_.find(userData);
    if (iter == inputDevciceIds_.end()) {
        MMI_LOGE("failed to find the callback function");
        return;
    }
    iter->second(userData, ids);
    MMI_LOGD("end");
}

void InputDeviceImpl::GetVirtualDeviceIdsAsync(std::function<void(std::vector<int32_t>)> callback)
{
    MMI_LOGI("GetVirtualDeviceIdsAsync begin");
    std::lock_guard<std::mutex> guard(mtx_);
    virtualIdsRequests_.insert(std::pair<int32_t,
        std::function<void(std::vector<int32_t>)>>(virtualIdsTaskId_, callback));
    MMIEventHdl.GetVirtualDeviceIds(virtualIdsTaskId_);
    virtualIdsTaskId_++;
    MMI_LOGI("end");
}

void InputDeviceImpl::GetVirtualDeviceAsync(int32_t deviceId,
                                           std::function<void(std::shared_ptr<InputDeviceInfo>)> callback)
{
    MMI_LOGI("GetVirtualDeviceAsync begin");
    std::lock_guard<std::mutex> guard(mtx_);
    virtualDevciceRequests_.insert(std::pair<int32_t,
        std::function<void(std::shared_ptr<InputDeviceInfo>)>>(virtualDeviceTaskId_, callback));
    MMIEventHdl.GetVirtualDevice(virtualDeviceTaskId_, deviceId);
    virtualDeviceTaskId_++;
    MMI_LOGI("end");
}

void InputDeviceImpl::OnVirtualDevice(int32_t taskId, int32_t id, std::string name, int32_t deviceType)
{
    MMI_LOGI("OnVirtualDevice begin");
    auto inputDeviceInfo = std::make_shared<InputDeviceInfo>(id, name, deviceType);
    inputDeviceInfo->id = id;
    inputDeviceInfo->name = name;
    inputDeviceInfo->devcieType = deviceType;

    for (auto it = virtualDevciceRequests_.begin(); it != virtualDevciceRequests_.end(); it++) {
        if (it->first == taskId) {
            it->second(inputDeviceInfo);
            break;
        }
    }
    MMI_LOGI("end");
}

void InputDeviceImpl::OnVirtualDeviceIds(int32_t taskId, std::vector<int32_t> ids)
{
    MMI_LOGI("OnVirtualDeviceIds begin");
    for (auto it = virtualIdsRequests_.begin(); it != virtualIdsRequests_.end(); it++) {
        if (it->first == taskId) {
            it->second(ids);
            break;
        }
    }
    MMI_LOGI("end");
}

void InputDeviceImpl::GetAllNodeDeviceInfo(std::function<void(std::vector<std::string>)> callback)
{
    MMI_LOGI("GetAllNodeDeviceInfo begin");
    std::lock_guard<std::mutex> guard(mtx_);
    nodeDeviceInfoRequests_.insert(std::pair<int32_t,
        std::function<void(std::vector<std::string>)>>(nodeDeviceInfoTaskId_, callback));
    MMIEventHdl.GetAllNodeDeviceInfo(nodeDeviceInfoTaskId_);
    nodeDeviceInfoTaskId_++;
    MMI_LOGI("end");
}

void InputDeviceImpl::OnGetAllNodeDeviceInfo(int32_t taskId, std::vector<std::string> ids)
{
    MMI_LOGI("OnVirtualDeviceIds begin");
    for (auto it = nodeDeviceInfoRequests_.begin(); it != nodeDeviceInfoRequests_.end(); it++) {
        if (it->first == taskId) {
            it->second(ids);
            break;
        }
    }
    MMI_LOGI("end");
}

void InputDeviceImpl::ShowMouse(std::function<void(bool)> callback)
{
    MMI_LOGI("ShowMouse begin");
    std::lock_guard<std::mutex> guard(mtx_);
    showMouseRequests_.insert(std::pair<int32_t,
        std::function<void(bool)>>(showMouseTaskId_, callback));
    MMIEventHdl.ShowMouse(showMouseTaskId_);
    showMouseTaskId_++;
    MMI_LOGI("end");
}

void InputDeviceImpl::HideMouse(std::function<void(bool)> callback)
{
    MMI_LOGI("HideMouse begin");
    std::lock_guard<std::mutex> guard(mtx_);
    hideMouseRequests_.insert(std::pair<int32_t,
        std::function<void(bool)>>(hideMouseTaskId_, callback));
    MMIEventHdl.HideMouse(hideMouseTaskId_);
    hideMouseTaskId_++;
    MMI_LOGI("end");
}

void InputDeviceImpl::OnShowMouse(int32_t taskId)
{
    MMI_LOGI("ShowMouse begin");
    for (auto it = showMouseRequests_.begin(); it != showMouseRequests_.end(); it++) {
        if (it->first == taskId) {
            it->second(true);
            break;
        }
    }
    MMI_LOGI("end");
}

void InputDeviceImpl::OnHideMouse(int32_t taskId)
{
    MMI_LOGI("HideMouse begin");
    for (auto it = hideMouseRequests_.begin(); it != hideMouseRequests_.end(); it++) {
        if (it->first == taskId) {
            it->second(true);
            break;
        }
    }
    MMI_LOGI("end");
}

} // namespace MMI
} // namespace OHOS
