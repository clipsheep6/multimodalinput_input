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

#ifndef ADD_WHITE_LIST_INFOS_CALL_BACK_STUB_H
#define ADD_WHITE_LIST_INFOS_CALL_BACK_STUB_H

#include <string>
#include "i_add_white_list_infos_call_back.h"
#include "iremote_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class AddWhiteListInfosCallbackStub : public IRemoteStub<IAddWhiteListInfosCallback> {
public:
    AddWhiteListInfosCallbackStub();
    virtual ~AddWhiteListInfosCallbackStub() override;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    DISALLOW_COPY_AND_MOVE(AddWhiteListInfosCallbackStub);
};
}  // namespace DistributedHardware
}  // namespace DistributedInput
}  // namespace OHOS

#endif  // ADD_WHITE_LIST_INFOS_CALL_BACK_STUB_H
