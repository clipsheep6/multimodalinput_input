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

#ifndef I_MULTIMODAL_INPUT_CONNECT_H
#define I_MULTIMODAL_INPUT_CONNECT_H

#include "iremote_broker.h"

#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
#include "i_call_dinput.h"
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
#include "i_event_filter.h"
#include "input_handler_type.h"
#include "key_event.h"
#include "key_option.h"
#include "pointer_event.h"

namespace OHOS {
namespace MMI {
using DeviceUniqId = std::tuple<int32_t, int32_t, int32_t, int32_t, int32_t, std::string>;
class IMultimodalInputConnect : public IRemoteBroker {
public:
    static constexpr int32_t INVALID_SOCKET_FD = -1;
    static constexpr int32_t MULTIMODAL_INPUT_CONNECT_SERVICE_ID = 3101;
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.multimodalinput.IConnectManager");

    virtual int32_t AllocSocketFd(const std::string &programName, const int32_t moduleType,
        int32_t &socketFd, int32_t &tokenType) = 0;
    virtual int32_t AddInputEventFilter(sptr<IEventFilter> filter) = 0;
    virtual int32_t SetPointerVisible(bool visible) = 0;
    virtual int32_t IsPointerVisible(bool &visible) = 0;
    virtual int32_t SetPointerSpeed(int32_t speed) = 0;
    virtual int32_t GetPointerSpeed(int32_t &speed) = 0;
    virtual int32_t SupportKeys(int32_t userData, int32_t deviceId, std::vector<int32_t> &keys) = 0;
    virtual int32_t GetDeviceIds(int32_t userData) = 0;
    virtual int32_t GetDevice(int32_t userData, int32_t id) = 0;
    virtual int32_t RegisterDevListener() = 0;
    virtual int32_t UnregisterDevListener() = 0;
    virtual int32_t GetKeyboardType(int32_t userData, int32_t deviceId) = 0;
    virtual int32_t AddInputHandler(InputHandlerType handlerType, HandleEventType eventType) = 0;
    virtual int32_t RemoveInputHandler(InputHandlerType handlerType, HandleEventType eventType) = 0;
    virtual int32_t MarkEventConsumed(int32_t eventId) = 0;
    virtual int32_t MoveMouseEvent(int32_t offsetX, int32_t offsetY) = 0;
    virtual int32_t InjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent) = 0;
    virtual int32_t SubscribeKeyEvent(int32_t subscribeId, const std::shared_ptr<KeyOption> option) = 0;
    virtual int32_t UnsubscribeKeyEvent(int32_t subscribeId) = 0;
    virtual int32_t InjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent) = 0;
    virtual int32_t SetAnrObserver() = 0;
    virtual int32_t SetPointerLocation(int32_t x, int32_t y) = 0;
    virtual int32_t SetInputDeviceSeatName(const std::string& seatName, DeviceUniqId& deviceUniqId) = 0;
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    virtual int32_t GetRemoteInputAbility(std::string deviceId, sptr<ICallDinput> ablitity) = 0;
    virtual int32_t PrepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput) = 0;
    virtual int32_t UnprepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput) = 0;
    virtual int32_t StartRemoteInput(const std::string& deviceId, uint32_t inputAbility,
        sptr<ICallDinput> prepareDinput) = 0;
    virtual int32_t StopRemoteInput(const std::string& deviceId, uint32_t inputAbility,
        sptr<ICallDinput> prepareDinput) = 0;
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
    enum {
        ALLOC_SOCKET_FD = 0,
        ADD_INPUT_EVENT_FILTER = 1,
        SET_POINTER_VISIBLE = 2,
        IS_POINTER_VISIBLE = 3,
        SUBSCRIBE_KEY_EVENT = 6,
        UNSUBSCRIBE_KEY_EVENT = 7,
        ADD_INPUT_HANDLER = 8,
        REMOVE_INPUT_HANDLER = 9,
        MARK_EVENT_CONSUMED = 10,
        MOVE_MOUSE = 11,
        INJECT_KEY_EVENT = 12,
        INJECT_POINTER_EVENT = 13,
        SET_ANR_OBSERVER = 14,
        SUPPORT_KEYS = 15,
        GET_DEVICE_IDS = 16,
        GET_DEVICE = 17,
        REGISTER_DEV_MONITOR = 18,
        UNREGISTER_DEV_MONITOR = 19,
        GET_KEYBOARD_TYPE = 20,
        SET_POINTER_SPEED = 21,
        GET_POINTER_SPEED = 22,
        GET_REMOTE_ABILITY = 30,
        PREPARE_DINPUT = 31,
        UNPREPARE_DINPUT = 32,
        START_DINPUT = 33,
        STOP_DINPUT = 34,
        SIMULATE_CROSS_LOCATION = 35,
        SET_INPUT_DEVICE_SEAT_NAME = 36
    };

    enum {
        CONNECT_MODULE_TYPE_MMI_CLIENT = 0,
    };
};
} // namespace MMI
} // namespace OHOS
#endif // I_MULTIMODAL_INPUT_CONNECT_H