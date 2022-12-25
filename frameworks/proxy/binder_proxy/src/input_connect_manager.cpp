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

#include "input_connect_manager.h"

#include <chrono>
#include <thread>

#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "mmi_log.h"
#include "input_connect_death_recipient.h"
#include "input_connect_define.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
std::shared_ptr<InputConnectManager> g_instance = nullptr;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputConnectManager" };
} // namespace

std::shared_ptr<InputConnectManager> InputConnectManager::GetInstance()
{
    static std::once_flag flag;
    std::call_once(flag, [&]() {
        g_instance.reset(new (std::nothrow) InputConnectManager());
    });

    CHKPP(g_instance);
    if (g_instance != nullptr) {
        g_instance->ConnectInputService();
    }
    return g_instance;
}

int32_t InputConnectManager::AllocSocketPair(const int32_t moduleType)
{
    CALL_DEBUG_ENTER;
    if (inputConnectService_ == nullptr) {
        MMI_HILOGE("Client has not connect server");
        return RET_ERR;
    }

    const std::string programName(GetProgramName());
    int32_t result = inputConnectService_->AllocSocketFd(programName, moduleType, socketFd_, tokenType_);
    if (result != RET_OK) {
        MMI_HILOGE("AllocSocketFd has error:%{public}d", result);
        return RET_ERR;
    }

    MMI_HILOGI("AllocSocketPair success. socketFd_:%{public}d tokenType_:%{public}d", socketFd_, tokenType_);
    return RET_OK;
}

int32_t InputConnectManager::GetClientSocketFdOfAllocedSocketPair() const
{
    CALL_DEBUG_ENTER;
    return socketFd_;
}

int32_t InputConnectManager::AddInputEventFilter(sptr<IEventFilter> filter, int32_t filterId, int32_t priority)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->AddInputEventFilter(filter, filterId, priority);
}

int32_t InputConnectManager::RemoveInputEventFilter(int32_t filterId)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->RemoveInputEventFilter(filterId);
}

int32_t InputConnectManager::SetPointerVisible(bool visible)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->SetPointerVisible(visible);
}

int32_t InputConnectManager::IsPointerVisible(bool &visible)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->IsPointerVisible(visible);
}

int32_t InputConnectManager::MarkProcessed(int32_t eventType, int32_t eventId)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->MarkProcessed(eventType, eventId);
}

int32_t InputConnectManager::SetPointerSpeed(int32_t speed)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->SetPointerSpeed(speed);
}

int32_t InputConnectManager::GetPointerSpeed(int32_t &speed)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->GetPointerSpeed(speed);
}

int32_t InputConnectManager::SetPointerStyle(int32_t windowId, int32_t pointerStyle)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->SetPointerStyle(windowId, pointerStyle);
}

int32_t InputConnectManager::GetPointerStyle(int32_t windowId, int32_t &pointerStyle)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->GetPointerStyle(windowId, pointerStyle);
}

int32_t InputConnectManager::RegisterDevListener()
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->RegisterDevListener();
}

int32_t InputConnectManager::UnregisterDevListener()
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->UnregisterDevListener();
}

int32_t InputConnectManager::SupportKeys(int32_t deviceId, std::vector<int32_t> &keys,
    std::vector<bool> &keystroke)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->SupportKeys(deviceId, keys, keystroke);
}

int32_t InputConnectManager::GetDeviceIds(std::vector<int32_t> &ids)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->GetDeviceIds(ids);
}

int32_t InputConnectManager::GetDevice(int32_t deviceId, std::shared_ptr<InputDevice> &inputDevice)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->GetDevice(deviceId, inputDevice);
}

int32_t InputConnectManager::GetKeyboardType(int32_t deviceId, int32_t &keyboardType)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->GetKeyboardType(deviceId, keyboardType);
}

int32_t InputConnectManager::AddInterceptorHandler(HandleEventType eventType,
    int32_t priority, uint32_t deviceTags)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->AddInterceptorHandler(eventType, priority, deviceTags);
}

int32_t InputConnectManager::RemoveInterceptorHandler(HandleEventType eventType,
    int32_t priority, uint32_t deviceTags)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->RemoveInterceptorHandler(eventType, priority, deviceTags);
}

int32_t InputConnectManager::AddMonitorHandler(HandleEventType eventType)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->AddMonitorHandler(eventType);
}

int32_t InputConnectManager::RemoveMonitorHandler(HandleEventType eventType)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->RemoveMonitorHandler(eventType);
}

int32_t InputConnectManager::MarkEventConsumed(int32_t eventId)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->MarkEventConsumed(eventId);
}

int32_t InputConnectManager::SubscribeKeyEvent(int32_t subscribeId, const std::shared_ptr<KeyOption> option)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->SubscribeKeyEvent(subscribeId, option);
}

int32_t InputConnectManager::UnsubscribeKeyEvent(int32_t subscribeId)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->UnsubscribeKeyEvent(subscribeId);
}

int32_t InputConnectManager::MoveMouseEvent(int32_t offsetX, int32_t offsetY)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->MoveMouseEvent(offsetX, offsetY);
}

int32_t InputConnectManager::InjectKeyEvent(const std::shared_ptr<KeyEvent> event)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->InjectKeyEvent(event);
}

int32_t InputConnectManager::InjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->InjectPointerEvent(pointerEvent);
}

int32_t InputConnectManager::SetAnrObserver()
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->SetAnrObserver();
}

int32_t InputConnectManager::RegisterCooperateListener()
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->RegisterCooperateListener();
}

int32_t InputConnectManager::UnregisterCooperateListener()
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->UnregisterCooperateListener();
}

int32_t InputConnectManager::EnableInputDeviceCooperate(int32_t userData, bool enabled)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->EnableInputDeviceCooperate(userData, enabled);
}

int32_t InputConnectManager::StartInputDeviceCooperate(int32_t userData,
    const std::string &sinkDeviceId, int32_t srcInputDeviceId)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->StartInputDeviceCooperate(userData, sinkDeviceId, srcInputDeviceId);
}

int32_t InputConnectManager::StopDeviceCooperate(int32_t userData)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->StopDeviceCooperate(userData);
}

int32_t InputConnectManager::GetInputDeviceCooperateState(int32_t userData, const std::string &deviceId)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->GetInputDeviceCooperateState(userData, deviceId);
}

int32_t InputConnectManager::SetInputDevice(const std::string& dhid, const std::string& screenId)
{
    CHKPR(inputConnectService_, RET_ERR);
    return inputConnectService_->SetInputDevice(dhid, screenId);
}

int32_t InputConnectManager::GetFunctionKeyState(int32_t funcKey, bool &state)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->GetFunctionKeyState(funcKey, state);
}

int32_t InputConnectManager::SetFunctionKeyState(int32_t funcKey, bool enable)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->SetFunctionKeyState(funcKey, enable);
}

int32_t InputConnectManager::SetPointerLocation(int32_t x, int32_t y)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->SetPointerLocation(x, y);
}

int32_t InputConnectManager::SetMouseCaptureMode(int32_t windowId, bool isCaptureMode)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->SetMouseCaptureMode(windowId, isCaptureMode);
}

int32_t InputConnectManager::GetDisplayBindInfo(DisplayBindInfos &infos)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->GetDisplayBindInfo(infos);
}

int32_t InputConnectManager::SetDisplayBind(int32_t deviceId, int32_t displayId, std::string &msg)
{
    CHKPR(inputConnectService_, INVALID_HANDLER_ID);
    return inputConnectService_->SetDisplayBind(deviceId, displayId, msg);    
}

bool InputConnectManager::ConnectInputService()
{
    CALL_DEBUG_ENTER;
    if (inputConnectService_ != nullptr) {
        return true;
    }
    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        MMI_HILOGE("Get system ability manager failed");
        return false;
    }
    auto sa = sm->GetSystemAbility(IInputConnect::MULTIMODAL_INPUT_CONNECT_SERVICE_ID);
    if (sa == nullptr) {
        MMI_HILOGE("Get sa failed");
        return false;
    }

    std::weak_ptr<InputConnectManager> weakPtr = shared_from_this();
    auto deathCallback = [weakPtr](const wptr<IRemoteObject> &object) {
        auto sharedPtr = weakPtr.lock();
        if (sharedPtr != nullptr) {
            sharedPtr->OnDeath();
        }
    };

    inputConnectRecipient_ = new (std::nothrow) InputConnectDeathRecipient(deathCallback);
    CHKPF(inputConnectRecipient_);
    sa->AddDeathRecipient(inputConnectRecipient_);
    inputConnectService_ = iface_cast<IInputConnect>(sa);
    if (inputConnectService_ == nullptr) {
        MMI_HILOGE("Get multimodalinput service failed");
        return false;
    }
    MMI_HILOGI("Get multimodalinput service successful");
    return true;
}

void InputConnectManager::OnDeath()
{
    CALL_DEBUG_ENTER;
    Clean();
    NotifyDeath();
}

void InputConnectManager::Clean()
{
    CALL_DEBUG_ENTER;
    if (inputConnectService_ != nullptr) {
        inputConnectService_.clear();
        inputConnectService_ = nullptr;
    }

    if (inputConnectRecipient_ != nullptr) {
        inputConnectRecipient_.clear();
        inputConnectRecipient_ = nullptr;
    }
}

void InputConnectManager::NotifyDeath()
{
    CALL_DEBUG_ENTER;

    int32_t retryCount = 50;
    do {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (ConnectInputService()) {
            MMI_HILOGD("Connect multimodalinput service successful");
            return;
        }
    } while (--retryCount > 0);
}
} // namespace MMI
} // namespace OHOS
