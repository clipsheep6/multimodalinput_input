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

#include "multimodal_event_handler.h"
#include "input_manager_impl.h"
#include "immi_token.h"
#include "input_event.h"
#include "mmi_client.h"
#include "proto.h"

namespace OHOS {
namespace MMI {
namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "MultimodalEventHandler"};
}
void OnConnected(const OHOS::MMI::IfMMIClient& client)
{
#ifdef OHOS_WESTEN_MODEL
    int32_t winId = 0;
    int32_t abilityId = 0;
    std::string bundlerName = "EmptyBundlerName";
    std::string appName = "EmptyAppName";
    auto abilityInfoVec = MMIEventHdl.GetAbilityInfoVec();
    if (!abilityInfoVec.empty()) {
        winId = abilityInfoVec[0].windowId;
        abilityId = *reinterpret_cast<int32_t*>(abilityInfoVec[0].token.GetRefPtr());
    }
    OHOS::MMI::NetPacket pkt(MmiMessageId::REGISTER_APP_INFO);
    pkt << abilityId << winId << bundlerName << appName;
    client.SendMessage(pkt);

    for (auto& val : abilityInfoVec) {
        if (val.sync == REG_STATUS_SYNCED) {
            val.sync = REG_STATUS_NOT_SYNC;
            continue;
        }
        EventManager.RegisterStandardizedEventHandle(val.token, val.windowId, val.standardizedEventHandle);
    }
#else
    InputManagerImpl::GetInstance()->OnConnected();
#endif
}

MultimodalEventHandler::MultimodalEventHandler()
{
#ifdef OHOS_BUILD_MMI_DEBUG
    VerifyLogManagerRun();
#endif
}

int32_t MultimodalEventHandler::RegisterStandardizedEventHandle(const sptr<IRemoteObject> token,
    int32_t windowId, StandEventPtr standardizedEventHandle)
{
#ifdef OHOS_WESTEN_MODEL
    MMI_LOGD("begin");
    KMSG_LOGI("Register Standardized Event Handle start!");
    int32_t ret = OHOS::MMI_STANDARD_EVENT_SUCCESS;
    EventRegesterInfo regInfo = {};
    if (client_ && client_->GetCurrentConnectedStatus()) {
        regInfo.sync = REG_STATUS_SYNCED;
        ret = EventManager.RegisterStandardizedEventHandle(token, windowId, standardizedEventHandle);
    }
    regInfo.token = token;
    regInfo.windowId = windowId;
    regInfo.standardizedEventHandle = standardizedEventHandle;
    abilityInfoVec_.push_back(regInfo);

    if (!InitClient()) {
        MMI_LOGE("init client failed!");
        return OHOS::MMI_STANDARD_EVENT_INVALID_PARAM;
    }
    MMI_LOGD("end");
    return ret;
#else
    return RET_OK;
#endif
}

int32_t MultimodalEventHandler::UnregisterStandardizedEventHandle(const sptr<IRemoteObject> token,
    int32_t windowId, StandEventPtr standardizedEventHandle)
{
#ifdef OHOS_WESTEN_MODEL
    return EventManager.UnregisterStandardizedEventHandle(token, windowId, standardizedEventHandle);
#else
    return RET_OK;
#endif
}

int32_t MultimodalEventHandler::InjectEvent(const OHOS::KeyEvent& keyEvent)
{
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }
    return EventManager.InjectEvent(keyEvent);
}

int32_t MultimodalEventHandler::InjectEvent(const OHOS::MMI::KeyEvent& keyEvent)
{
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }
    return EventManager.InjectEvent(keyEvent);
}

int32_t MultimodalEventHandler::InjectEvent(const std::shared_ptr<OHOS::MMI::KeyEvent> keyEventPtr)
{
    CHKPR(keyEventPtr, ERROR_NULL_POINTER);
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }
    return EventManager.InjectEvent(keyEventPtr);
}

int32_t MultimodalEventHandler::GetMultimodeInputInfo()
{
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }
    return MMI_SERVICE_RUNNING;
}

std::vector<EventRegesterInfo>& MultimodalEventHandler::GetAbilityInfoVec()
{
    return abilityInfoVec_;
}

bool MultimodalEventHandler::InitClient()
{
    MMI_LOGD("enter");
    if (client_ != nullptr) {
        return true;
    }
    client_ = std::make_shared<MMIClient>();
    CHKPF(client_);
    cMsgHandler_ = std::make_shared<ClientMsgHandler>();
    CHKPF(cMsgHandler_);
    EventManager.SetClientHandle(client_);
    client_->RegisterConnectedFunction(&OnConnected);
    if (!(client_->Start(cMsgHandler_, true))) {
        MMI_LOGE("The client fails to start");
        return false;
    }
    MMI_LOGD("leave");
    return true;
}

MMIClientPtr MultimodalEventHandler::GetMMIClient()
{
    if (InitClient()) {
        return client_;
    }
    return nullptr;
}

int32_t MultimodalEventHandler::GetDeviceIds(int32_t taskId)
{
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }
    return EventManager.GetDeviceIds(taskId);
}

int32_t MultimodalEventHandler::GetDevice(int32_t taskId, int32_t deviceId)
{
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }
    return EventManager.GetDevice(taskId, deviceId);
}

int32_t MultimodalEventHandler::InjectPointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }
    return EventManager.InjectPointerEvent(pointerEvent);
}

int32_t MultimodalEventHandler::AddKeyEventFIlter(int32_t id, std::string name, Authority authority)
{
    if (authority < NO_AUTHORITY || authority > HIGH_AUTHORITY) {
        MMI_LOGD("the input authority is incorrect");
        return RET_ERR;
    }
    OHOS::MMI::NetPacket pkt(MmiMessageId::ADD_KEY_EVENT_INTERCEPTOR);
    MMI_LOGD("client add a key event filter");
    pkt << id << name << authority;
    client_->SendMessage(pkt);
    return RET_OK;
}

int32_t MultimodalEventHandler::RemoveKeyEventFIlter(int32_t id)
{
    OHOS::MMI::NetPacket pkt(MmiMessageId::REMOVE_KEY_EVENT_INTERCEPTOR);
    MMI_LOGD("client remove a key event filter");
    pkt << id;
    client_->SendMessage(pkt);
    return RET_OK;
}

int32_t MultimodalEventHandler::AddTouchEventFilter(int32_t id, std::string name, Authority authority)
{
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }
    MMI_LOGD("client add a touch event filter");
    OHOS::MMI::NetPacket pkt(MmiMessageId::ADD_TOUCH_EVENT_INTERCEPTOR);
    int32_t ret = OHOS::MMI_STANDARD_EVENT_SUCCESS;
    pkt << id << name << authority;
    client_->SendMessage(pkt);
    return ret;
}

int32_t MultimodalEventHandler::RemoveTouchEventFilter(int32_t id)
{
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }
    MMI_LOGD("client remove a touch event filter");
    OHOS::MMI::NetPacket pkt(MmiMessageId::REMOVE_TOUCH_EVENT_INTERCEPTOR);
    int32_t ret = OHOS::MMI_STANDARD_EVENT_SUCCESS;
    pkt << id;
    client_->SendMessage(pkt);
    return ret;
}

int32_t MultimodalEventHandler::AddEventInterceptor(int32_t id, std::string name, Authority authority)
{
    if (authority < NO_AUTHORITY || authority > HIGH_AUTHORITY) {
        MMI_LOGD("the input authority is incorrect");
        return RET_ERR;
    }
    OHOS::MMI::NetPacket pkt(MmiMessageId::ADD_POINTER_INTERCEPTOR);
    MMI_LOGD("client add a pointer event interceptor");
    pkt << id << name << authority;
    client_->SendMessage(pkt);
    return RET_OK;
}

int32_t MultimodalEventHandler::RemoveEventInterceptor(int32_t id)
{
    OHOS::MMI::NetPacket pkt(MmiMessageId::REMOVE_POINTER_INTERCEPTOR);
    MMI_LOGD("client remove a pointer event interceptor");
    pkt << id;
    client_->SendMessage(pkt);
    return RET_OK;
}

int32_t MultimodalEventHandler::AddInterceptor(int32_t sourceType, int32_t id)
{
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }

    OHOS::MMI::NetPacket pkt(MmiMessageId::ADD_EVENT_INTERCEPTOR);
    pkt << sourceType << id;
    client_->SendMessage(pkt);
    MMI_LOGD("client add a touchpad event interceptor");
    return RET_OK;
}


int32_t MultimodalEventHandler::RemoveInterceptor(int32_t id)
{
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }

    OHOS::MMI::NetPacket pkt(MmiMessageId::REMOVE_EVENT_INTERCEPTOR);
    pkt << id;
    client_->SendMessage(pkt);
    MMI_LOGD("client remove a touchpad event interceptor");
    return RET_OK;
}

int32_t MultimodalEventHandler::AddInputEventMontior(int32_t keyEventType)
{
    MMI_LOGD("enter");
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }
    NetPacket pkt(MmiMessageId::ADD_INPUT_EVENT_MONITOR);
    pkt << keyEventType;
    client_->SendMessage(pkt);
    return RET_OK;
}

void MultimodalEventHandler::RemoveInputEventMontior(int32_t keyEventType)
{
    MMI_LOGD("enter");
    if (!InitClient()) {
        return;
    }
    NetPacket pkt(MmiMessageId::REMOVE_INPUT_EVENT_MONITOR);
    pkt << keyEventType;
    client_->SendMessage(pkt);
}

void MultimodalEventHandler::RemoveInputEventTouchpadMontior(int32_t pointerEventType)
{
    MMI_LOGD("enter");
    if (!InitClient()) {
        return;
    }
    NetPacket pkt(MmiMessageId::REMOVE_INPUT_EVENT_TOUCHPAD_MONITOR);
    pkt << OHOS::MMI::InputEvent::EVENT_TYPE_POINTER;
    client_->SendMessage(pkt);
}

int32_t MultimodalEventHandler::AddInputEventTouchpadMontior(int32_t pointerEventType)
{
    MMI_LOGD("enter");
    if (!InitClient()) {
        return MMI_SERVICE_INVALID;
    }
    NetPacket pkt(MmiMessageId::ADD_INPUT_EVENT_TOUCHPAD_MONITOR);
    pkt << OHOS::MMI::InputEvent::EVENT_TYPE_POINTER;
    MMI_LOGE("send msg before");
    bool isSuc = client_->SendMessage(pkt);
    if (isSuc)
        MMI_LOGD("sendAdd msg Success");
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
