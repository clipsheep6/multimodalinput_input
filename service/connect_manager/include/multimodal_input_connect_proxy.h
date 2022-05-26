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

#ifndef MULTIMODAL_INPUT_CONNECT_PROXY_H
#define MULTIMODAL_INPUT_CONNECT_PROXY_H

#include "iremote_object.h"
#include "iremote_proxy.h"
#include "nocopyable.h"
#include "singleton.h"
#include "system_ability.h"

#include "i_multimodal_input_connect.h"

namespace OHOS {
namespace MMI {
class MultimodalInputConnectProxy final : public IRemoteProxy<IMultimodalInputConnect> {
public:
    explicit MultimodalInputConnectProxy(const sptr<IRemoteObject> &impl);
    DISALLOW_COPY_AND_MOVE(MultimodalInputConnectProxy);
    virtual ~MultimodalInputConnectProxy() override;
    virtual int32_t AllocSocketFd(const std::string &programName, const int32_t moduleType, int32_t &socketFd) override;
    virtual int32_t AddInputEventFilter(sptr<IEventFilter> filter) override;
    virtual int32_t SetPointerVisible(bool visible) override;
    virtual int32_t IsPointerVisible(bool &visible) override;
#ifdef OHOS_BUILD_KEY_MOUSE
    virtual int32_t SetPointerLocation(int32_t x, int32_t y) override;
#endif
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    virtual int32_t GetRemoteInputAbility(std::string deviceId, sptr<ICallDinput> ablitity) override;
    virtual int32_t PrepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput) override;
    virtual int32_t UnprepareRemoteInput(const std::string& deviceId, sptr<ICallDinput> prepareDinput) override;
    virtual int32_t StartRemoteInput(const std::string& deviceId, uint32_t inputAbility, sptr<ICallDinput> prepareDinput) override;
    virtual int32_t StopRemoteInput(const std::string& deviceId, uint32_t inputAbility, sptr<ICallDinput> prepareDinput) override;

#endif // OHOS_DISTRIBUTED_INPUT_MODEL
private:
    static inline BrokerDelegator<MultimodalInputConnectProxy> delegator_;
};
} // namespace MMI
} // namespace OHOS
#endif // MULTIMODAL_INPUT_CONNECT_PROXY_H