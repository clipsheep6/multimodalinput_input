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

#include "standardized_event_manager.h"

#include <sstream>

#include "define_multimodal.h"
#include "error_multimodal.h"
#include "net_packet.h"
#include "proto.h"
#include "util.h"

#include "input_event_data_transformation.h"
#include "multimodal_event_handler.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, MMI_LOG_DOMAIN, "StandardizedEventManager"
};
} // namespace

StandardizedEventManager::StandardizedEventManager() {}

StandardizedEventManager::~StandardizedEventManager() {}

void StandardizedEventManager::SetClientHandle(MMIClientPtr client)
{
    CALL_LOG_ENTER;
    client_ = client;
}

#ifdef OHOS_BUILD_ENABLE_KEYBOARD
int32_t StandardizedEventManager::SubscribeKeyEvent(
    const KeyEventInputSubscribeManager::SubscribeKeyEventInfo &subscribeInfo)
{
    CALL_LOG_ENTER;
    NetPacket pkt(MmiMessageId::SUBSCRIBE_KEY_EVENT);
    std::shared_ptr<KeyOption> keyOption = subscribeInfo.GetKeyOption();
    uint32_t preKeySize = keyOption->GetPreKeys().size();
    pkt << subscribeInfo.GetSubscribeId() << keyOption->GetFinalKey() << keyOption->IsFinalKeyDown()
    << keyOption->GetFinalKeyDownDuration() << preKeySize;

    std::set<int32_t> preKeys = keyOption->GetPreKeys();
    for (const auto &item : preKeys) {
        pkt << item;
    }
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write subscribe key event failed");
        return RET_ERR;
    }
    if (!SendMsg(pkt)) {
        MMI_HILOGE("Client failed to send message");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t StandardizedEventManager::UnsubscribeKeyEvent(int32_t subscribeId)
{
    CALL_LOG_ENTER;
    NetPacket pkt(MmiMessageId::UNSUBSCRIBE_KEY_EVENT);
    pkt << subscribeId;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write unsubscribe key event failed");
        return RET_ERR;
    }
    if (!SendMsg(pkt)) {
        MMI_HILOGE("Client failed to send message");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t StandardizedEventManager::InjectEvent(const std::shared_ptr<KeyEvent> key)
{
    CALL_LOG_ENTER;
    CHKPR(key, RET_ERR);
    key->UpdateId();
    if (key->GetKeyCode() < 0) {
        MMI_HILOGE("keyCode is invalid:%{public}u", key->GetKeyCode());
        return RET_ERR;
    }
    NetPacket pkt(MmiMessageId::INJECT_KEY_EVENT);
    int32_t errCode = InputEventDataTransformation::KeyEventToNetPacket(key, pkt);
    if (errCode != RET_OK) {
        MMI_HILOGE("Serialization is Failed, errCode:%{public}u", errCode);
        return RET_ERR;
    }
    if (!SendMsg(pkt)) {
        MMI_HILOGE("Send inject event Msg error");
        return RET_ERR;
    }
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_KEYBOARD

#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
int32_t StandardizedEventManager::InjectPointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    CALL_LOG_ENTER;
    CHKPR(pointerEvent, ERROR_NULL_POINTER);
    MMI_HILOGD("Inject pointer event:");
    std::stringstream sStream;
    sStream << *pointerEvent;
    std::string sLine;
    while (std::getline(sStream, sLine)) {
        MMI_HILOGD("%{public}s", sLine.c_str());
    }
    NetPacket pkt(MmiMessageId::INJECT_POINTER_EVENT);
    if (InputEventDataTransformation::Marshalling(pointerEvent, pkt) != RET_OK) {
        MMI_HILOGE("Marshalling pointer event failed");
        return RET_ERR;
    }
    if (!SendMsg(pkt)) {
        MMI_HILOGE("SendMsg failed");
        return RET_ERR;
    }
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH

#if defined(OHOS_BUILD_ENABLE_POINTER) && defined(OHOS_BUILD_ENABLE_POINTER_DRAWING)
int32_t StandardizedEventManager::MoveMouseEvent(int32_t offsetX, int32_t offsetY)
{
    CALL_LOG_ENTER;
    NetPacket pkt(MmiMessageId::MOVE_MOUSE);
    pkt << offsetX << offsetY;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write move mouse event failed");
        return RET_ERR;
    }
    if (!SendMsg(pkt)) {
        MMI_HILOGE("SendMsg failed");
        return RET_ERR;
    }
    return RET_OK;
}
#endif // OHOS_BUILD_ENABLE_POINTER && OHOS_BUILD_ENABLE_POINTER_DRAWING

int32_t StandardizedEventManager::GetDeviceIds(int32_t userData)
{
    NetPacket pkt(MmiMessageId::INPUT_DEVICE_IDS);
    pkt << userData;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write userData failed");
        return RET_ERR;
    }
    return SendMsg(pkt);
}

int32_t StandardizedEventManager::GetDevice(int32_t userData, int32_t deviceId)
{
    NetPacket pkt(MmiMessageId::INPUT_DEVICE);
    pkt << userData << deviceId;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write userData failed");
        return RET_ERR;
    }
    return SendMsg(pkt);
}

int32_t StandardizedEventManager::SupportKeys(int32_t userData, int32_t deviceId, std::vector<int32_t> keyCodes)
{
    NetPacket pkt(MmiMessageId::INPUT_DEVICE_KEYSTROKE_ABILITY);
    pkt << userData << deviceId << keyCodes;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write keyCodes failed");
        return RET_ERR;
    }
    return SendMsg(pkt);
}

int32_t StandardizedEventManager::GetKeyboardType(int32_t userData, int32_t deviceId) const
{
    NetPacket pkt(MmiMessageId::INPUT_DEVICE_KEYBOARD_TYPE);
    pkt << userData << deviceId;
    if (pkt.ChkRWError()) {
        MMI_HILOGE("Packet write userData failed");
        return PACKET_WRITE_FAIL;
    }
    return SendMsg(pkt);
}

int32_t StandardizedEventManager::RegisterInputDeviceMonitor()
{
    NetPacket pkt(MmiMessageId::ADD_INPUT_DEVICE_MONITOR);
    return SendMsg(pkt);
}

int32_t StandardizedEventManager::UnRegisterInputDeviceMonitor()
{
    NetPacket pkt(MmiMessageId::REMOVE_INPUT_DEVICE_MONITOR);
    return SendMsg(pkt);
}

bool StandardizedEventManager::SendMsg(NetPacket& pkt) const
{
    CHKPF(client_);
    return client_->SendMessage(pkt);
}
} // namespace MMI
} // namespace OHOS
