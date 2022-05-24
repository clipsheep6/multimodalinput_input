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
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
#ifndef I_CALL_DINPUT_H
#define I_CALL_DINPUT_H

#include <set>

#include "iremote_broker.h"

namespace OHOS {
namespace MMI {
class ICallDinput : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.multimodalinput.ICallDinput");
    virtual bool HandlePrepareDinput(std::string deviceId, int32_t status) = 0;
    virtual bool HandleUnprepareDinput(std::string deviceId, int32_t status) = 0;
    virtual bool HandleStartDinput(std::string deviceId, uint32_t inputTypes, int32_t status) = 0;
    virtual bool HandleStopDinput(std::string deviceId, uint32_t inputTypes, int32_t status) = 0;
    virtual bool HandleRemoteInputAbility(const std::set<int32_t> remoteInputAbility) = 0;
    enum class OPERATOR_TYPE {
        HANDLE_PREPARE_DINPUT = 0,
        HANDLE_UNPREPARE_DINPUT = 1,
        HANDLE_START_DINPUT = 2,
        HANDLE_STOP_DINPUT = 3,
        HANDLE_REMOTE_INPUT_ABILITY = 4,
    };
};
} // namespace MMI
} // namespace OHOS

#endif // I_CALL_DINPUT_H
#endif