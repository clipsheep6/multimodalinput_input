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

#include "i_event_filter.h"
#ifdef OHOS_BUILD_KEY_MOUSE
#include "i_call_dinput.h"
#endif

namespace OHOS {
namespace MMI {
class IMultimodalInputConnect : public IRemoteBroker {
public:
    static constexpr int32_t INVALID_SOCKET_FD = -1;
    static constexpr int32_t MULTIMODAL_INPUT_CONNECT_SERVICE_ID = 3101;
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.multimodalinput.IConnectManager");

    virtual int32_t AllocSocketFd(const std::string &programName,
        const int32_t moduleType, int32_t &socketFd) = 0;
    virtual int32_t AddInputEventFilter(sptr<IEventFilter> filter) = 0;
    virtual int32_t SetPointerVisible(bool visible) = 0;
    virtual int32_t IsPointerVisible(bool &visible) = 0;
#ifdef OHOS_BUILD_KEY_MOUSE
    virtual int32_t SetPointerLocation(int32_t x, int32_t y) = 0;
    virtual int32_t GetRemoteInputAbility(std::string deviceId, sptr<ICallDinput> ablitity) = 0;
    virtual int32_t PrepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput) = 0;
    virtual int32_t UnprepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput) = 0;
    virtual int32_t StartRemoteInput(const std::string& deviceId, uint32_t inputAbility,
        sptr<ICallDinput> prepareDinput) = 0;
    virtual int32_t StopRemoteInput(const std::string& deviceId, uint32_t inputAbility,
        sptr<ICallDinput> prepareDinput) = 0;
#endif // OHOS_BUILD_KEY_MOUSE

    enum {
        ALLOC_SOCKET_FD = 0,
        ADD_INPUT_EVENT_FILTER = 1,
        SET_POINTER_VISIBLE = 2,
        IS_POINTER_VISIBLE = 3,
#ifdef OHOS_BUILD_KEY_MOUSE
        SIMULATE_CROSS_LOCATION = 5,
        START_REMOTE_INPUT = 6,
        STOP_REMOTE_INPUT = 7,
        GET_REMOTE_ABILITY = 8,
        PREPARE_DINPUT = 9,
        UNPREPARE_DINPUT = 10,
        START_DINPUT = 11,
        STOP_DINPUT = 12,
#endif
    };

    enum {
        CONNECT_MODULE_TYPE_MMI_CLIENT = 0,
    };
};
} // namespace MMI
} // namespace OHOS
#endif // I_MULTIMODAL_INPUT_CONNECT_H