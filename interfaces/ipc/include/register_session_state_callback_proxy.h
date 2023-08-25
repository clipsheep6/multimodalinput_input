/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef REGISTER_SESSION_STATE_CALLBACK_PROXY_H
#define REGISTER_SESSION_STATE_CALLBACK_PROXY_H

#include "i_session_state_callback.h"

#include <string>

#include "iremote_proxy.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class RegisterSessionStateCallbackProxy : public IRemoteProxy<ISessionStateCallback> {
public:
    explicit RegisterSessionStateCallbackProxy(const sptr<IRemoteObject> &object);
    ~RegisterSessionStateCallbackProxy() override;

    void OnResult(const std::string &deviceId, const uint32_t status) override;

private:
    static inline BrokerDelegator<RegisterSessionStateCallbackProxy> delegator_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // REGISTER_SESSION_STATE_CALLBACK_PROXY_H
