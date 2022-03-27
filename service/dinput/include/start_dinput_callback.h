/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef START_DINPUT_CALLBACK_H
#define START_DINPUT_CALLBACK_H

#include <string>
#include "iremote_proxy.h"
#include "start_d_input_call_back_stub.h"
#include "uds_session.h"

namespace OHOS {
namespace MMI {
class StartDInputCallback : public OHOS::DistributedHardware::DistributedInput::StartDInputCallbackStub {
public:
    StartDInputCallback() {}
    StartDInputCallback(const int32_t& taskId, SessionPtr& sess);
    virtual ~StartDInputCallback() {}
    void OnResult(const std::string& deviceId, const int32_t& status);
    int32_t taskId_;
    SessionPtr sess_;
};
}  // namespace MMI
}  // namespace OHOS

#endif  // START_DINPUT_CALLBACK_H
