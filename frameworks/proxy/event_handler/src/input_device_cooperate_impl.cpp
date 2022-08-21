/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "input_device_cooperate_impl.h"

#include "input_manager_impl.h"
#include "mmi_log.h"
#include "multimodal_event_handler.h"
#include "multimodal_input_connect_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceCooperateImpl"};
} // namespace

InputDeviceCooperateImpl &InputDeviceCooperateImpl::GetInstance()
{
    static InputDeviceCooperateImpl instance;
    return instance;
}

int32_t InputDeviceCooperateImpl::RegisterCooperateListener(InputDevCooperateListenerPtr listener)
{
    CALL_DEBUG_ENTER;
    CHKPR(listener, RET_ERR);
    for (const auto &item : devCooperateListener_) {
        if (item.second == listener) {
            MMI_HILOGW("The listener already exists");
            return RET_ERR;
        }
    }
    auto eventHandler = InputMgrImpl->GetCurrentEventHandler();
    CHKPR(eventHandler, RET_ERR);
    auto monitor = std::make_pair(eventHandler, listener);
    devCooperateListener_.push_back(monitor);
    if (!isListeningProcess_) {
        MMI_HILOGI("Start monitoring");
        isListeningProcess_ = true;
        return MultimodalInputConnMgr->RegisterCooperateListener();
    }
    return RET_OK;
}

int32_t InputDeviceCooperateImpl::UnregisterCooperateListener(InputDevCooperateListenerPtr listener)
{
    CALL_DEBUG_ENTER;
    if (listener == nullptr) {
        devCooperateListener_.clear();
        goto listenerLabel;
    }
    for (auto it = devCooperateListener_.begin(); it != devCooperateListener_.end(); ++it) {
        if (it->second == listener) {
            devCooperateListener_.erase(it);
            goto listenerLabel;
        }
    }

listenerLabel:
    if (isListeningProcess_ && devCooperateListener_.empty()) {
        isListeningProcess_ = false;
        return MultimodalInputConnMgr->UnregisterCooperateListener();
    }
    return RET_OK;
}

int32_t InputDeviceCooperateImpl::EnableInputDeviceCooperate(bool enabled, FuncCooperationMessage callback)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    auto eventHandler = InputMgrImpl->GetCurrentEventHandler();
    CHKPR(eventHandler, RET_ERR);
    CooperateEvent event;
    event.msg = std::make_pair(eventHandler, callback);
    if (userData_ == INT32_MAX) {
        MMI_HILOGE("userData exceeds the maximum");
        return RET_ERR;
    }
    devCooperateEvent_[userData_] = event;
    return MultimodalInputConnMgr->EnableInputDeviceCooperate(userData_++, enabled);
}

int32_t InputDeviceCooperateImpl::StartInputDeviceCooperate(const std::string &sinkDeviceId, int32_t srcInputDeviceId,
    FuncCooperationMessage callback)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    auto eventHandler = InputMgrImpl->GetCurrentEventHandler();
    CHKPR(eventHandler, RET_ERR);
    CooperateEvent event;
    event.msg = std::make_pair(eventHandler, callback);
    if (userData_ == INT32_MAX) {
        MMI_HILOGE("userData exceeds the maximum");
        return RET_ERR;
    }
    devCooperateEvent_[userData_] = event;
    return MultimodalInputConnMgr->StartInputDeviceCooperate(userData_++, sinkDeviceId, srcInputDeviceId);
}

int32_t InputDeviceCooperateImpl::StopDeviceCooperate(FuncCooperationMessage callback)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    auto eventHandler = InputMgrImpl->GetCurrentEventHandler();
    CHKPR(eventHandler, RET_ERR);
    CooperateEvent event;
    event.msg = std::make_pair(eventHandler, callback);
    if (userData_ == INT32_MAX) {
        MMI_HILOGE("userData exceeds the maximum");
        return RET_ERR;
    }
    devCooperateEvent_[userData_] = event;
    return MultimodalInputConnMgr->StopDeviceCooperate(userData_++);
}

int32_t InputDeviceCooperateImpl::GetInputDeviceCooperateState(
    const std::string &deviceId, FuncCooperateionState callback)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    auto eventHandler = InputMgrImpl->GetCurrentEventHandler();
    CHKPR(eventHandler, RET_ERR);
    CooperateEvent event;
    event.state = std::make_pair(eventHandler, callback);
    if (userData_ == INT32_MAX) {
        MMI_HILOGE("userData exceeds the maximum");
        return RET_ERR;
    }
    devCooperateEvent_[userData_] = event;
    return MultimodalInputConnMgr->GetInputDeviceCooperateState(userData_++, deviceId);
}

void InputDeviceCooperateImpl::OnDevCooperateListener(const std::string deviceId, CooperationMessage msg)
{
    CALL_DEBUG_ENTER;
    for (const auto &item : devCooperateListener_) {
        if (!MMIEventHandler::PostTask(item.first,
                std::bind(&InputDeviceCooperateImpl::OnDevCooperateListenerTask, this, item, deviceId, msg))) {
            MMI_HILOGE("Post task failed");
        }
    }
}

void InputDeviceCooperateImpl::OnCooprationMessage(int32_t userData, const std::string deviceId, CooperationMessage msg)
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    std::lock_guard<std::mutex> guard(mtx_);
    auto event = GetCooprateMessageEvent(userData);
    CHKPV(event);
    if (!MMIEventHandler::PostTask(event->first,
        std::bind(&InputDeviceCooperateImpl::OnCooperateMessageTask, this, *event, userData, deviceId, msg))) {
        MMI_HILOGE("Post task failed.");
    }
}

void InputDeviceCooperateImpl::OnCooperationState(int32_t userData, bool state)
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    std::lock_guard<std::mutex> guard(mtx_);
    auto event = GetCooprateStateEvent(userData);
    CHKPV(event);
    if (!MMIEventHandler::PostTask(event->first,
        std::bind(&InputDeviceCooperateImpl::OnCooperateStateTask, this, *event, userData, state))) {
        MMI_HILOGE("Post task failed.");
    }
}

int32_t InputDeviceCooperateImpl::GetUserData()
{
    return userData_;
}

const InputDeviceCooperateImpl::DevCooperationMsg *InputDeviceCooperateImpl::GetCooprateMessageEvent(
    int32_t userData) const
{
    auto iter = devCooperateEvent_.find(userData);
    return iter == devCooperateEvent_.end() ? nullptr : &iter->second.msg;
}

const InputDeviceCooperateImpl::DevCooperateionState *InputDeviceCooperateImpl::GetCooprateStateEvent(
    int32_t userData) const
{
    auto iter = devCooperateEvent_.find(userData);
    return iter == devCooperateEvent_.end() ? nullptr : &iter->second.state;
}

void InputDeviceCooperateImpl::OnDevCooperateListenerTask(const DevCooperateListener &devCooperateMonitor,
    const std::string &deviceId, CooperationMessage msg)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGI("Task listener device id is %{public}s", deviceId.c_str());
    devCooperateMonitor.second->OnCooperateMessage(deviceId, msg);
}

void InputDeviceCooperateImpl::OnCooperateMessageTask(const DevCooperationMsg &msgCooperation, int32_t userData,
    const std::string &deviceId, CooperationMessage msg)
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    msgCooperation.second(deviceId, msg);
    MMI_HILOGD("Cooperatinon message event callback userData:%{public}d deviceId:(%{public}s) msg:(%{public}d)",
        userData, deviceId.c_str(), msg);
}

void InputDeviceCooperateImpl::OnCooperateStateTask(const DevCooperateionState &stateCooperation,
    int32_t userData, bool state)
{
    CALL_DEBUG_ENTER;
    CHK_PID_AND_TID();
    stateCooperation.second(state);
    MMI_HILOGD("Cooperatinon state event callback userData:%{public}d state:(%{public}d)", userData, state);
}
} // namespace MMI
} // namespace OHOS