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

#include "multimodal_standardized_event_manager.h"
#include <cinttypes>
#include "define_multimodal.h"
#include "error_multimodal.h"
#include "immi_token.h"
#include "input_event_data_transformation.h"
#include "multimodal_event_handler.h"
#include "net_packet.h"
#include "proto.h"

namespace OHOS {
namespace MMI {
    namespace {
        constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
            LOG_CORE, MMI_LOG_DOMAIN, "MultimodalStandardizedEventManager"
        };
    }

MultimodalStandardizedEventManager::MultimodalStandardizedEventManager() {}

MultimodalStandardizedEventManager::~MultimodalStandardizedEventManager() {}

void MultimodalStandardizedEventManager::SetClientHandle(MMIClientPtr client)
{
    MMI_LOGD("enter");
    client_ = client;
}

int32_t MultimodalStandardizedEventManager::RegisterStandardizedEventHandle(const sptr<IRemoteObject> token,
    int32_t windowId, StandEventPtr standardizedEventHandle)
{
    CHKR((token && standardizedEventHandle), PARAM_INPUT_INVALID, MMI_STANDARD_EVENT_INVALID_PARAMETER);
    auto messageId = standardizedEventHandle->GetType();
    CHKR(messageId > MmiMessageId::INVALID, VAL_NOT_EXP, MMI_STANDARD_EVENT_INVALID_PARAMETER);
    auto range = mapEvents_.equal_range(messageId);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second.eventCallBack == standardizedEventHandle) {
            MMI_LOGE("Duplicate registration information, registration failed. errCode:%{public}d",
                     MMI_STANDARD_EVENT_EXIST);
            return OHOS::MMI_STANDARD_EVENT_EXIST;
        }
    }
    MMI_LOGD("Register app event:typeId:%{public}d", messageId);
    std::string registerhandle;
    if (!MakeRegisterHandle(messageId, windowId, registerhandle)) {
        MMI_LOGE("Invalid registration parameter, errCode:%{public}d", MMI_STANDARD_EVENT_INVALID_PARAMETER);
        return OHOS::MMI_STANDARD_EVENT_INVALID_PARAMETER;
    }
    registerEvents_.insert(registerhandle);
    StandEventCallBack StandEventInfo = {};
    StandEventInfo.windowId = windowId;
    StandEventInfo.eventCallBack = standardizedEventHandle;
    mapEvents_.insert(std::make_pair(messageId, StandEventInfo));

    std::string bundlerName = "EmptyBundlerName";
    std::string appName = "EmptyAppName";
    auto abilityId = *reinterpret_cast<int32_t*>(token.GetRefPtr());

    OHOS::MMI::NetPacket pkt(MmiMessageId::REGISTER_MSG_HANDLER);
    pkt << messageId << abilityId << windowId << bundlerName << appName;
    SendMsg(pkt);
    return OHOS::MMI_STANDARD_EVENT_SUCCESS;
}

int32_t MultimodalStandardizedEventManager::UnregisterStandardizedEventHandle(const sptr<IRemoteObject> token,
    int32_t windowId, StandEventPtr standardizedEventHandle)
{
    CHKR((token && standardizedEventHandle), PARAM_INPUT_INVALID, MMI_STANDARD_EVENT_INVALID_PARAMETER);
    auto typeId = standardizedEventHandle->GetType();
    CHKR(typeId > MmiMessageId::INVALID, VAL_NOT_EXP, MMI_STANDARD_EVENT_INVALID_PARAMETER);

    std::string registerhandle;
    if (!MakeRegisterHandle(typeId, windowId, registerhandle)) {
        MMI_LOGE("Invalid unregistration parameter, typeId:%{public}d,windowId:%{public}d,errCode:%{public}d",
                 typeId, windowId, MMI_STANDARD_EVENT_INVALID_PARAMETER);
        return MMI_STANDARD_EVENT_INVALID_PARAMETER;
    }
    registerEvents_.erase(registerhandle);
    auto range = mapEvents_.equal_range(typeId);
    bool isHandleExist = false;
    for (StandEventMMaps::iterator it = range.first; it != range.second; ++it) {
        if (it->second.eventCallBack == standardizedEventHandle) {
            mapEvents_.erase(it);
            isHandleExist = true;
            break;
        }
    }
    if (!isHandleExist) {
        MMI_LOGE("Unregistration does not exist, Unregistration failed, typeId:%{public}d,windowId:%{public}d,"
                 "errCode:%{public}d", typeId, windowId, MMI_STANDARD_EVENT_NOT_EXIST);
        return MMI_STANDARD_EVENT_NOT_EXIST;
    }
    MMI_LOGD("Unregister app event:typeId:%{public}d", typeId);
    OHOS::MMI::NetPacket pkt(MmiMessageId::UNREGISTER_MSG_HANDLER);
    pkt << typeId;
    SendMsg(pkt);
    return OHOS::MMI_STANDARD_EVENT_SUCCESS;
}

int32_t MultimodalStandardizedEventManager::SubscribeKeyEvent(
    const KeyEventInputSubscribeManager::SubscribeKeyEventInfo &subscribeInfo)
{
    MMI_LOGD("Enter");
    OHOS::MMI::NetPacket pkt(MmiMessageId::SUBSCRIBE_KEY_EVENT);
    std::shared_ptr<OHOS::MMI::KeyOption> keyOption = subscribeInfo.GetKeyOption();
    uint32_t preKeySize = keyOption->GetPreKeys().size();
    pkt << subscribeInfo.GetSubscribeId() << keyOption->GetFinalKey() << keyOption->IsFinalKeyDown()
    << keyOption->GetFinalKeyDownDuration() << preKeySize;

    std::vector<int32_t> preKeys = keyOption->GetPreKeys();
    for (const auto &item : preKeys) {
        pkt << item;
    }
    if (MMIEventHdl.GetMMIClient() == nullptr) {
        MMI_LOGE("client init failed");
        return RET_ERR;
    }
    if (!SendMsg(pkt)) {
        MMI_LOGE("Client failed to send message");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t MultimodalStandardizedEventManager::UnSubscribeKeyEvent(int32_t subscribeId)
{
    MMI_LOGD("Enter");
    OHOS::MMI::NetPacket pkt(MmiMessageId::UNSUBSCRIBE_KEY_EVENT);
    pkt << subscribeId;
    if (MMIEventHdl.GetMMIClient() == nullptr) {
        MMI_LOGE("client init failed");
        return RET_ERR;
    }
    if (!SendMsg(pkt)) {
        MMI_LOGE("Client failed to send message");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnKey(const OHOS::KeyEvent& event)
{
    MMI_LOGD("enter");
#ifdef DEBUG_CODE_TEST
    if (event.GetDeviceUdevTags() == DEVICE_TYPE_VIRTUAL_KEYBOARD) {
        MMI_LOGD("Inject, keyCode:%{public}d,action:%{public}d,revPid:%{public}d",
            event.GetKeyCode(), event.IsKeyDown(), GetPid());
    }
#endif
    auto range = mapEvents_.equal_range(MmiMessageId::KEY_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.windowId == event.GetWindowID() && i->second.eventCallBack->OnKey(event) == false) {
            MMI_LOGW("OnKey Event consumption failed. errCode:%{public}d", EVENT_CONSUM_FAIL);
            break;
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnTouch(const TouchEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::TOUCH_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.windowId == event.GetWindowID() && i->second.eventCallBack->OnTouch(event) == false) {
            MMI_LOGW("OnTouch Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
            break;
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnShowMenu(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnShowMenu(event) == false) {
            MMI_LOGW("OnShowMenu Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnSend(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnSend(event) == false) {
            MMI_LOGW("OnSend Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnCopy(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnCopy(event) == false) {
            MMI_LOGW("OnCopy Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnPaste(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnPaste(event) == false) {
            MMI_LOGW("OnPaste Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnCut(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnCut(event) == false) {
            MMI_LOGW("OnCut Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnUndo(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnUndo(event) == false) {
            MMI_LOGW("OnUndo Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnRefresh(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnRefresh(event) == false) {
            MMI_LOGW("OnRefresh Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnStartDrag(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnStartDrag(event) == false) {
            MMI_LOGW("OnStartDrag Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnCancel(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnCancel(event) == false) {
            MMI_LOGW("OnCancel Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnEnter(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnEnter(event) == false) {
            MMI_LOGW("OnEnter Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnPrevious(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnPrevious(event) == false) {
            MMI_LOGW("OnPrevious Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnNext(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnNext(event) == false) {
            MMI_LOGW("OnNext Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnBack(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnBack(event) == false) {
            MMI_LOGW("OnBack Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnPrint(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::COMMON_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnPrint(event) == false) {
            MMI_LOGW("OnPrint Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnPlay(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::MEDIA_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnPlay(event) == false) {
            MMI_LOGW("OnPlay Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnPause(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::MEDIA_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnPause(event) == false) {
            MMI_LOGW("OnPause Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnMediaControl(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::MEDIA_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnMediaControl(event) == false) {
            MMI_LOGW("OnMediaControl Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnScreenShot(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnScreenShot(event) == false) {
            MMI_LOGW("OnScreenShot Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnScreenSplit(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnScreenSplit(event) == false) {
            MMI_LOGW("OnScreenSplit Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnStartScreenRecord(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnStartScreenRecord(event) == false) {
            MMI_LOGW("OnStartScreenRecord Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnStopScreenRecord(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnStopScreenRecord(event) == false) {
            MMI_LOGW("OnStopScreenRecord Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnGotoDesktop(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnGotoDesktop(event) == false) {
            MMI_LOGW("OnGotoDesktop Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnRecent(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnRecent(event) == false) {
            MMI_LOGW("OnRecent Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnShowNotification(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnShowNotification(event) == false) {
            MMI_LOGW("OnShowNotification Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnLockScreen(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnLockScreen(event) == false) {
            MMI_LOGW("OnLockScreen Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnSearch(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnSearch(event) == false) {
            MMI_LOGW("OnSearch Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnClosePage(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnClosePage(event) == false) {
            MMI_LOGW("OnClosePage Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnLaunchVoiceAssistant(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnLaunchVoiceAssistant(event) == false) {
            MMI_LOGW("OnLaunchVoiceAssistant Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnMute(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::SYSTEM_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnMute(event) == false) {
            MMI_LOGW("OnMute Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnAnswer(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::TELEPHONE_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnAnswer(event) == false) {
            MMI_LOGW("OnAnswer Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnRefuse(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::TELEPHONE_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnRefuse(event) == false) {
            MMI_LOGW("OnRefuse Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnHangup(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::TELEPHONE_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnHangup(event) == false) {
            MMI_LOGW("OnHangup Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t OHOS::MMI::MultimodalStandardizedEventManager::OnTelephoneControl(const MultimodalEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::TELEPHONE_EVENT_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnTelephoneControl(event) == false) {
            MMI_LOGW("OnTelephoneControl Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t MultimodalStandardizedEventManager::OnDeviceAdd(const DeviceEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::DEVICE_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnDeviceAdd(event) == false) {
            MMI_LOGW("OnDeviceAdd Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

int32_t MultimodalStandardizedEventManager::OnDeviceRemove(const DeviceEvent& event)
{
    MMI_LOGD("enter");
    auto range = mapEvents_.equal_range(MmiMessageId::DEVICE_BEGIN);
    for (auto i = range.first; i != range.second; i++) {
        if (i->second.eventCallBack->OnDeviceRemove(event) == false) {
            MMI_LOGW("OnDeviceRemove Event consumption failed, errCode:%{public}d", EVENT_CONSUM_FAIL);
        }
    }
    return RET_OK;
}

const std::set<std::string> *MultimodalStandardizedEventManager::GetRegisterEvent()
{
    return &registerEvents_;
}

void MultimodalStandardizedEventManager::ClearAll()
{
    mapEvents_.clear();
    registerEvents_.clear();
}

int32_t MultimodalStandardizedEventManager::InjectionVirtual(bool isPressed, int32_t keyCode,
                                                             int32_t keyDownDuration, int32_t maxKeyCode)
{
    VirtualKey virtualevent;
    virtualevent.isPressed = isPressed;
    virtualevent.keyCode = keyCode;
    virtualevent.keyDownDuration = keyDownDuration;
    OHOS::MMI::NetPacket pkt(MmiMessageId::ON_VIRTUAL_KEY);
    pkt << virtualevent;
    return SendMsg(pkt);
}

int32_t MultimodalStandardizedEventManager::InjectEvent(const OHOS::KeyEvent& key)
{
    VirtualKey virtualevent;
    if (key.GetKeyDownDuration() < 0) {
        MMI_LOGE("keyDownDuration is invalid");
        return false;
    }
    if (key.GetKeyCode() < 0) {
        MMI_LOGE("keyCode is invalid");
        return false;
    }
    virtualevent.isPressed = key.IsKeyDown();
    virtualevent.keyCode = key.GetKeyCode();
    virtualevent.keyDownDuration = key.GetKeyDownDuration();
    virtualevent.isIntercepted = key.IsIntercepted();
    NetPacket pkt(MmiMessageId::INJECT_KEY_EVENT);
    pkt << virtualevent;
    return SendMsg(pkt);
}

int32_t MultimodalStandardizedEventManager::InjectEvent(const KeyEvent& key)
{
    VirtualKey virtualevent;
    if (key.GetKeyCode() < 0) {
        MMI_LOGE("keyCode is invalid");
            return false;
    }
    virtualevent.isPressed = (key.GetKeyAction() == KeyEvent::KEY_ACTION_DOWN);
    virtualevent.keyCode = key.GetKeyCode();
    virtualevent.keyDownDuration = 0;
    virtualevent.isIntercepted = false;
    NetPacket pkt(MmiMessageId::INJECT_KEY_EVENT);
    pkt << virtualevent;
    return SendMsg(pkt);
}

int32_t MultimodalStandardizedEventManager::InjectEvent(const std::shared_ptr<KeyEvent> key)
{
    MMI_LOGD("begin");
    CHKR(key, ERROR_NULL_POINTER, RET_ERR);
    key->UpdateId();
    if (key->GetKeyCode() < 0) {
        MMI_LOGE("keyCode is invalid:%{public}u", key->GetKeyCode());
        return RET_ERR;
    }
    NetPacket pkt(MmiMessageId::NEW_INJECT_KEY_EVENT);
    int32_t errCode = InputEventDataTransformation::KeyEventToNetPacket(key, pkt);
    if (errCode != RET_OK) {
        MMI_LOGE("Serialization is Failed, errCode:%{public}u", errCode);
        return RET_ERR;
    }
    if (!SendMsg(pkt)) {
        MMI_LOGE("Send inject event Msg error");
        return RET_ERR;
    }
    MMI_LOGD("leave");
    return RET_OK;
}

int32_t MultimodalStandardizedEventManager::InjectPointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    MMI_LOGD("enter");
    CHKPR(pointerEvent, RET_ERR);
    std::vector<int32_t> pointerIds { pointerEvent->GetPointersIdList() };
    MMI_LOGD("Pointer event dispatcher of client:eventType:%{public}s,actionTime:%{public}" PRId64 ","
             "action:%{public}d,actionStartTime:%{public}" PRId64 ","
             "flag:%{public}u,pointerAction:%{public}s,sourceType:%{public}s,"
             "VerticalAxisValue:%{public}f,HorizontalAxisValue:%{public}f,pointerCount:%{public}zu",
             pointerEvent->DumpEventType(), pointerEvent->GetActionTime(),
             pointerEvent->GetAction(), pointerEvent->GetActionStartTime(),
             pointerEvent->GetFlag(), pointerEvent->DumpPointerAction(),
             pointerEvent->DumpSourceType(),
             pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
             pointerEvent->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL),
             pointerIds.size());

    for (const auto &pointerId : pointerIds) {
        OHOS::MMI::PointerEvent::PointerItem item;
        CHKR(pointerEvent->GetPointerItem(pointerId, item), PARAM_INPUT_FAIL, RET_ERR);

        MMI_LOGD("DownTime:%{public}" PRId64 ",isPressed:%{public}s,"
                "globalX:%{public}d,globalY:%{public}d,localX:%{public}d,localY:%{public}d,"
                "width:%{public}d,height:%{public}d,pressure:%{public}d",
                 item.GetDownTime(), (item.IsPressed() ? "true" : "false"),
                 item.GetGlobalX(), item.GetGlobalY(), item.GetLocalX(), item.GetLocalY(),
                 item.GetWidth(), item.GetHeight(), item.GetPressure());
    }
    std::vector<int32_t> pressedKeys = pointerEvent->GetPressedKeys();
    for (auto &keyCode : pressedKeys) {
        MMI_LOGI("Pressed keyCode:%{public}d", keyCode);
    }
    OHOS::MMI::NetPacket pkt(MmiMessageId::INJECT_POINTER_EVENT);
    CHKR((RET_OK == InputEventDataTransformation::Marshalling(pointerEvent, pkt)),
        STREAM_BUF_WRITE_FAIL, RET_ERR);
    MMI_LOGD("leave");
    CHKR(SendMsg(pkt), MSG_SEND_FAIL, RET_ERR);
    return RET_OK;
}

int32_t MultimodalStandardizedEventManager::GetDeviceIds(int32_t userData)
{
    OHOS::MMI::NetPacket pkt(MmiMessageId::INPUT_DEVICE_IDS);
    pkt << userData;
    return SendMsg(pkt);
}

int32_t MultimodalStandardizedEventManager::GetDevice(int32_t userData, int32_t deviceId)
{
    OHOS::MMI::NetPacket pkt(MmiMessageId::INPUT_DEVICE);
    pkt << userData << deviceId;
    return SendMsg(pkt);
}

bool MultimodalStandardizedEventManager::SendMsg(NetPacket& pkt) const
{
    CHKPF(client_);
    return client_->SendMessage(pkt);
}

bool MultimodalStandardizedEventManager::MakeRegisterHandle(MmiMessageId typeId, int32_t windowId,
                                                            std::string& rhandle)
{
    rhandle = std::to_string(windowId) + ",";
    switch (typeId) {
        case MmiMessageId::COMMON_EVENT_BEGIN:
            rhandle += "commoneventhandle";
            break;
        case MmiMessageId::KEY_EVENT_BEGIN:
            rhandle += "keyeventhandle";
            break;
        case MmiMessageId::MEDIA_EVENT_BEGIN:
            rhandle += "mediaeventhandle";
            break;
        case MmiMessageId::SYSTEM_EVENT_BEGIN:
            rhandle += "systemeventhandle";
            break;
        case MmiMessageId::TELEPHONE_EVENT_BEGIN:
            rhandle += "telephoneeventhandle";
            break;
        case MmiMessageId::TOUCH_EVENT_BEGIN:
            rhandle += "toucheventhandle";
            break;
        case MmiMessageId::DEVICE_BEGIN:
            rhandle += "devicehandle";
            break;
        default:
            MMI_LOGW("Unknown message type, type:%{public}d", typeId);
            return false;
    }
    return true;
}
} // namespace MMI
} // namespace OHOS
