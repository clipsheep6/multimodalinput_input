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
#ifndef CALL_DINPUT_PROXY_H
#define CALL_DINPUT_PROXY_H

#include "i_call_dinput.h"
#include "iremote_proxy.h"
#include "iremote_object.h"

namespace OHOS {
namespace MMI {
class CallDinputProxy final : public IRemoteProxy<ICallDinput> {
public:
    explicit CallDinputProxy(const sptr<IRemoteObject> &impl);
    virtual ~CallDinputProxy() override;
    virtual int32_t HandlePrepareDinput(std::string deviceId, int32_t status) override;
    virtual int32_t HandleUnprepareDinput(std::string deviceId, int32_t status) override;
    virtual int32_t HandleStartDinput(std::string deviceId, uint32_t inputTypes, int32_t status) override;
    virtual int32_t HandleStopDinput(std::string deviceId, uint32_t inputTypes, int32_t status) override;
    virtual int32_t HandleRemoteInputAbility(const std::set<int32_t> remoteInputAbility) override;
private:
    static inline BrokerDelegator<CallDinputProxy> delegator_;
};
} // namespace MMI
} // namespace OHOS
#endif // CALL_DINPUT_PROXY_H
#endif
