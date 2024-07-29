/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "multimodalInputConnectStub_fuzzer.h"

#include "mmi_log.h"
#include "mmi_service.h"
#include "multimodal_input_connect_stub.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "multimodalInputConnectStubFuzzTest"

namespace OHOS {
namespace MMI {
namespace OHOS {
bool StubUnsubscribeSwitchEventFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::UNSUBSCRIBE_SWITCH_EVENT), datas, reply, option);
    return true;
}

bool StubSetTouchpadRotateSwitchFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SET_TP_ROTATE_SWITCH), datas, reply, option);
    return true;
}

bool StubGetTouchpadRotateSwitchFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::GET_TP_ROTATE_SWITCH), datas, reply, option);
    return true;
}

bool StubGetKeyStateFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::GET_KEY_STATE), datas, reply, option);
    return true;
}

bool StubAuthorizeFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::NATIVE_AUTHORIZE), datas, reply, option);
    return true;
}

bool StubCancelInjectionFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::NATIVE_CANCEL_INJECTION), datas, reply, option);
    return true;
}

bool StubHasIrEmitterFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::NATIVE_INFRARED_OWN), datas, reply, option);
    return true;
}

bool StubGetInfraredFrequenciesFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::NATIVE_INFRARED_FREQUENCY), datas, reply, option);
    return true;
}

bool StubTransmitInfraredFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::NATIVE_CANCEL_TRANSMIT), datas, reply, option);
    return true;
}

bool StubSetPixelMapDataFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SET_PIXEL_MAP_DATA), datas, reply, option);
    return true;
}

bool StubSetCurrentUserFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SET_CURRENT_USERID), datas, reply, option);
    return true;
}

bool StubEnableHardwareCursorStatsFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::ENABLE_HARDWARE_CURSOR_STATS),
        datas, reply, option);
    return true;
}

bool StubGetHardwareCursorStatsFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::GET_HARDWARE_CURSOR_STATS), datas, reply, option);
    return true;
}

bool StubAddVirtualInputDeviceFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::ADD_VIRTUAL_INPUT_DEVICE), datas, reply, option);
    return true;
}

bool StubRemoveVirtualInputDeviceFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::REMOVE_VIRTUAL_INPUT_DEVICE), datas, reply, option);
    return true;
}

bool StubTransferBinderClientServiceFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::TRANSFER_BINDER_CLIENT_SERVICE),
        datas, reply, option);
    return true;
}

bool StubGetPointerSnapshotFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::GET_POINTER_SNAPSHOT), datas, reply, option);
    return true;
}

bool StubMarkEventConsumedFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::MARK_EVENT_CONSUMED), datas, reply, option);
    return true;
}

bool StubUnsubscribeKeyEventFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::UNSUBSCRIBE_KEY_EVENT), datas, reply, option);
    return true;
}

bool StubUnregisterInputDeviceMonitorFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::UNREGISTER_DEV_MONITOR), datas, reply, option);
    return true;
}

bool StubSupportKeysFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SUPPORT_KEYS), datas, reply, option);
    return true;
}

bool StubSubscribeSwitchEventFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SUBSCRIBE_SWITCH_EVENT), datas, reply, option);
    return true;
}

bool StubSubscribeKeyEventFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SUBSCRIBE_KEY_EVENT), datas, reply, option);
    return true;
}

bool StubSetTouchpadTapSwitchFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_TP_TAP_SWITCH), datas, reply, option);
    return true;
}

bool StubSetTouchpadSwipeSwitchFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_TP_SWIPE_SWITCH), datas, reply, option);
    return true;
}

bool StubSetTouchpadScrollSwitchFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_TP_SCROLL_SWITCH), datas, reply, option);
    return true;
}

bool StubSetTouchpadScrollDirectionFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SET_TP_SCROLL_DIRECT_SWITCH), datas, reply, option);
    return true;
}

bool StubSetTouchpadRightClickTypeFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_TP_RIGHT_CLICK_TYPE), datas, reply, option);
    return true;
}

bool StubSetTouchpadPointerSpeedFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_TP_POINTER_SPEED), datas, reply, option);
    return true;
}

bool StubSetTouchpadPinchSwitchFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_TP_PINCH_SWITCH), datas, reply, option);
    return true;
}

bool StubSetPointerVisibleFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SET_POINTER_VISIBLE), datas, reply, option);
    return true;
}

bool StubSetPointerStyleFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_POINTER_STYLE), datas, reply, option);
    return true;
}

bool StubSetPointerSpeedFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SET_POINTER_SPEED), datas, reply, option);
    return true;
}

bool StubSetPointerLocationFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_POINTER_LOCATION), datas, reply, option);
    return true;
}

bool StubSetMouseScrollRowsFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_MOUSE_SCROLL_ROWS), datas, reply, option);
        return true;
}

bool StubSetMousePrimaryButtonFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SET_MOUSE_PRIMARY_BUTTON), datas, reply, option);
    return true;
}

bool StubSetMouseIconFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_MOUSE_ICON), datas, reply, option);
    return true;
}

bool StubSetMouseCaptureModeFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_CAPTURE_MODE), datas, reply, option);
    DelayedSingleton<IInputWindowsManager>::DestroyInstance();
    return true;
}

bool StubSetKeyDownDurationFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_KEY_DOWN_DURATION), datas, reply, option);
    return true;
}

bool StubSetKeyboardRepeatRateFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SET_KEYBOARD_REPEAT_RATE), datas, reply, option);
    return true;
}

bool StubSetKeyboardRepeatDelayFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SET_KEYBOARD_REPEAT_DELAY), datas, reply, option);
    return true;
}

bool StubSetHoverScrollStateFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_HOVER_SCROLL_STATE), datas, reply, option);
    DelayedSingleton<IInputWindowsManager>::DestroyInstance();
    return true;
}

bool StubSetFunctionKeyStateFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_FUNCTION_KEY_STATE), datas, reply, option);
    return true;
}

bool StubSetDisplayBindFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_DISPLAY_BIND), datas, reply, option);
    return true;
}

bool StubSetAnrListenerFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::SET_ANR_OBSERVER), datas, reply, option);
    return true;
}

bool StubRemoveInputHandlerFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::REMOVE_INPUT_HANDLER), datas, reply, option);
    return true;
}

bool StubRemoveInputEventFilterFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::RMV_INPUT_EVENT_FILTER), datas, reply, option);
    return true;
}

bool StubRegisterInputDeviceMonitorFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::REGISTER_DEV_MONITOR), datas, reply, option);
    return true;
}

bool StubMoveMouseEventFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::MOVE_MOUSE), datas, reply, option);
    return true;
}

bool StubMarkProcessedFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::MARK_PROCESSED), datas, reply, option);
    return true;
}

bool StubIsPointerVisibleFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::IS_POINTER_VISIBLE), datas, reply, option);
    return true;
}

bool StubInjectPointerEventFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::INJECT_POINTER_EVENT), datas, reply, option);
    return true;
}

bool StubInjectKeyEventFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::INJECT_KEY_EVENT), datas, reply, option);
    return true;
}

bool StubGetWindowPidFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_WINDOW_PID), datas, reply, option);
    return true;
}

bool StubGetTouchpadTapSwitchFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_TP_TAP_SWITCH), datas, reply, option);
    return true;
}

bool StubGetTouchpadSwipeSwitchFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_TP_SWIPE_SWITCH), datas, reply, option);
    return true;
}

bool StubGetTouchpadScrollSwitchFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_TP_SCROLL_SWITCH), datas, reply, option);
    return true;
}

bool StubGetTouchpadScrollDirectionFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::GET_TP_SCROLL_DIRECT_SWITCH), datas, reply, option);
    return true;
}

bool StubGetTouchpadPointerSpeedFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_TP_POINTER_SPEED), datas, reply, option);
    return true;
}

bool StubGetTouchpadPinchSwitchFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_TP_PINCH_SWITCH), datas, reply, option);
    return true;
}

bool StubGetPointerStyleFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_POINTER_STYLE), datas, reply, option);
    return true;
}

bool StubGetPointerSpeedFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_POINTER_SPEED), datas, reply, option);
    return true;
}

bool StubGetMouseScrollRowsFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_MOUSE_SCROLL_ROWS), datas, reply, option);
    return true;
}

bool StubGetMousePrimaryButtonFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::GET_MOUSE_PRIMARY_BUTTON), datas, reply, option);
    return true;
}

bool StubGetKeyboardTypeFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::GET_KEYBOARD_TYPE), datas, reply, option);
    return true;
}

bool StubGetHoverScrollStateFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_HOVER_SCROLL_STATE), datas, reply, option);
    return true;
}

bool StubGetFunctionKeyStateFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_FUNCTION_KEY_STATE), datas, reply, option);
    return true;
}

bool StubGetDisplayBindInfoFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_DISPLAY_BIND_INFO), datas, reply, option);
    return true;
}

bool StubGetDeviceIdsFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::GET_DEVICE_IDS), datas, reply, option);
    return true;
}

bool StubEnableInputDeviceFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::ENABLE_INPUT_DEVICE), datas, reply, option);
    return true;
}

bool StubAppendExtraDataFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::APPEND_EXTRA_DATA), datas, reply, option);
    return true;
}

bool StubAddInputHandlerFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::ADD_INPUT_HANDLER), datas, reply, option);
    return true;
}

bool StubAddInputEventFilterFuzzTest(const uint8_t *data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::ADD_INPUT_EVENT_FILTER), datas, reply, option);
    return true;
}

bool SetShieldStatusFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_SHIELD_STATUS), datas, reply, option);
    return true;
}

bool SetPointerSizeFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_POINTER_SIZE), datas, reply, option);
    return true;
}

bool SetPointerColorFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_POINTER_COLOR), datas, reply, option);
    return true;
}

bool SetNapStatusFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_NAP_STATUS), datas, reply, option);
    return true;
}

bool SetMouseHotSpotFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_MOUSE_HOT_SPOT), datas, reply, option);
    return true;
}

bool SetCustomCursorFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::SET_CUSTOM_CURSOR), datas, reply, option);
    return true;
}

bool RemoveInputEventObserverFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::RMV_INPUT_EVENT_OBSERVER), datas, reply, option);
    return true;
}

bool NotifyNapOnlineFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::NOTIFY_NAP_ONLINE), datas, reply, option);
    return true;
}

bool CleanWindowStyleFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::CLEAN_WIDNOW_STYLE), datas, reply, option);
    return true;
}

bool EnableCombineKeyFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::ENABLE_COMBINE_KEY), datas, reply, option);
    return true;
}

bool GetAllNapstatusDataFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_ALL_NAPSTATUS_DATA), datas, reply, option);
    return true;
}

bool GetKeyBoardRepeatDelayFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::GET_KEYBOARD_REPEAT_DELAY), datas, reply, option);
    return true;
}

bool GetKeyBoardRepeatRateFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::GET_KEYBOARD_REPEAT_RATE), datas, reply, option);
    return true;
}

bool GetPointerColorFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_POINTER_COLOR), datas, reply, option);
    return true;
}

bool GetPointerSizeFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_POINTER_SIZE), datas, reply, option);
    return true;
}

bool GetShieldStatusFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_SHIELD_STATUS), datas, reply, option);
    return true;
}

bool GetTpRightClickTypeFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) || !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_TP_RIGHT_CLICK_TYPE), datas, reply, option);
    return true;
}

#ifdef OHOS_BUILD_ENABLE_ANCO
bool StubAncoAddChannelFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::ADD_ANCO_CHANNEL), datas, reply, option);
    return true;
}

bool StubAncoRemoveChannelFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_INTERFACE_TOKEN { u"ohos.multimodalinput.IConnectManager" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(FORMMGR_INTERFACE_TOKEN) ||
        !datas.WriteBuffer(data, size) || !datas.RewindRead(0)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    MMIService::GetInstance()->state_ = ServiceRunningState::STATE_RUNNING;
    MMIService::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MultimodalinputConnectInterfaceCode::REMOVE_ANCO_CHANNEL), datas, reply, option);
    return true;
}
#endif // OHOS_BUILD_ENABLE_ANCO
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    OHOS::StubUnsubscribeSwitchEventFuzzTest(data, size);
    OHOS::StubSetTouchpadRotateSwitchFuzzTest(data, size);
    OHOS::StubGetTouchpadRotateSwitchFuzzTest(data, size);
    OHOS::StubGetKeyStateFuzzTest(data, size);
    OHOS::StubAuthorizeFuzzTest(data, size);
    OHOS::StubCancelInjectionFuzzTest(data, size);
    OHOS::StubHasIrEmitterFuzzTest(data, size);
    OHOS::StubGetInfraredFrequenciesFuzzTest(data, size);
    OHOS::StubTransmitInfraredFuzzTest(data, size);
    OHOS::StubSetPixelMapDataFuzzTest(data, size);
    OHOS::StubSetCurrentUserFuzzTest(data, size);
    OHOS::StubEnableHardwareCursorStatsFuzzTest(data, size);
    OHOS::StubGetHardwareCursorStatsFuzzTest(data, size);
    OHOS::StubAddVirtualInputDeviceFuzzTest(data, size);
    OHOS::StubRemoveVirtualInputDeviceFuzzTest(data, size);
    OHOS::StubTransferBinderClientServiceFuzzTest(data, size);
    OHOS::StubGetPointerSnapshotFuzzTest(data, size);
    OHOS::StubMarkEventConsumedFuzzTest(data, size);
    OHOS::StubUnsubscribeKeyEventFuzzTest(data, size);
    OHOS::StubUnregisterInputDeviceMonitorFuzzTest(data, size);
    OHOS::StubSupportKeysFuzzTest(data, size);
    OHOS::StubSubscribeSwitchEventFuzzTest(data, size);
    OHOS::StubSubscribeKeyEventFuzzTest(data, size);
    OHOS::StubSetTouchpadTapSwitchFuzzTest(data, size);
    OHOS::StubSetTouchpadSwipeSwitchFuzzTest(data, size);
    OHOS::StubSetTouchpadScrollSwitchFuzzTest(data, size);
    OHOS::StubSetTouchpadScrollDirectionFuzzTest(data, size);
    OHOS::StubSetTouchpadRightClickTypeFuzzTest(data, size);
    OHOS::StubSetTouchpadPointerSpeedFuzzTest(data, size);
    OHOS::StubSetTouchpadPinchSwitchFuzzTest(data, size);
    OHOS::StubSetPointerVisibleFuzzTest(data, size);
    OHOS::StubSetPointerStyleFuzzTest(data, size);
    OHOS::StubSetPointerSpeedFuzzTest(data, size);
    OHOS::StubSetPointerLocationFuzzTest(data, size);
    OHOS::StubSetMouseScrollRowsFuzzTest(data, size);
    OHOS::StubSetMousePrimaryButtonFuzzTest(data, size);
    OHOS::StubSetMouseIconFuzzTest(data, size);
    OHOS::StubSetMouseCaptureModeFuzzTest(data, size);
    OHOS::StubSetKeyDownDurationFuzzTest(data, size);
    OHOS::StubSetKeyboardRepeatRateFuzzTest(data, size);
    OHOS::StubSetKeyboardRepeatDelayFuzzTest(data, size);
    OHOS::StubSetHoverScrollStateFuzzTest(data, size);
    OHOS::StubSetFunctionKeyStateFuzzTest(data, size);
    OHOS::StubSetDisplayBindFuzzTest(data, size);
    OHOS::StubSetAnrListenerFuzzTest(data, size);
    OHOS::StubRemoveInputHandlerFuzzTest(data, size);
    OHOS::StubRemoveInputEventFilterFuzzTest(data, size);
    OHOS::StubRegisterInputDeviceMonitorFuzzTest(data, size);
    OHOS::StubMoveMouseEventFuzzTest(data, size);
    OHOS::StubMarkProcessedFuzzTest(data, size);
    OHOS::StubIsPointerVisibleFuzzTest(data, size);
    OHOS::StubInjectPointerEventFuzzTest(data, size);
    OHOS::StubInjectKeyEventFuzzTest(data, size);
    OHOS::StubGetWindowPidFuzzTest(data, size);
    OHOS::StubGetTouchpadTapSwitchFuzzTest(data, size);
    OHOS::StubGetTouchpadSwipeSwitchFuzzTest(data, size);
    OHOS::StubGetTouchpadScrollSwitchFuzzTest(data, size);
    OHOS::StubGetTouchpadScrollDirectionFuzzTest(data, size);
    OHOS::StubGetTouchpadPointerSpeedFuzzTest(data, size);
    OHOS::StubGetTouchpadPinchSwitchFuzzTest(data, size);
    OHOS::StubGetPointerStyleFuzzTest(data, size);
    OHOS::StubGetPointerSpeedFuzzTest(data, size);
    OHOS::StubGetMouseScrollRowsFuzzTest(data, size);
    OHOS::StubGetMousePrimaryButtonFuzzTest(data, size);
    OHOS::StubGetKeyboardTypeFuzzTest(data, size);
    OHOS::StubGetHoverScrollStateFuzzTest(data, size);
    OHOS::StubGetFunctionKeyStateFuzzTest(data, size);
    OHOS::StubGetDisplayBindInfoFuzzTest(data, size);
    OHOS::StubGetDeviceIdsFuzzTest(data, size);
    OHOS::StubEnableInputDeviceFuzzTest(data, size);
    OHOS::StubAppendExtraDataFuzzTest(data, size);
    OHOS::StubAddInputHandlerFuzzTest(data, size);
    OHOS::StubAddInputEventFilterFuzzTest(data, size);
    OHOS::SetShieldStatusFuzzTest(data, size);
    OHOS::SetPointerSizeFuzzTest(data, size);
    OHOS::SetPointerColorFuzzTest(data, size);
    OHOS::SetNapStatusFuzzTest(data, size);
    OHOS::SetMouseHotSpotFuzzTest(data, size);
    OHOS::SetCustomCursorFuzzTest(data, size);
    OHOS::RemoveInputEventObserverFuzzTest(data, size);
    OHOS::NotifyNapOnlineFuzzTest(data, size);
    OHOS::CleanWindowStyleFuzzTest(data, size);
    OHOS::EnableCombineKeyFuzzTest(data, size);
    OHOS::GetAllNapstatusDataFuzzTest(data, size);
    OHOS::GetKeyBoardRepeatDelayFuzzTest(data, size);
    OHOS::GetKeyBoardRepeatRateFuzzTest(data, size);
    OHOS::GetPointerColorFuzzTest(data, size);
    OHOS::GetPointerSizeFuzzTest(data, size);
    OHOS::GetShieldStatusFuzzTest(data, size);
    OHOS::GetTpRightClickTypeFuzzTest(data, size);
#ifdef OHOS_BUILD_ENABLE_ANCO
    OHOS::StubAncoAddChannelFuzzTest(data, size);
    OHOS::StubAncoRemoveChannelFuzzTest(data, size);
#endif // OHOS_BUILD_ENABLE_ANCO
    return 0;
}
} // namespace MMI
} // namespace OHOS