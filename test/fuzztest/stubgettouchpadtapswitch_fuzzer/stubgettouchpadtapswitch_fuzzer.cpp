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

#include "stubgettouchpadtapswitch_fuzzer.h"

#include "mmi_log.h"
#include "mmi_service.h"
#include "multimodal_input_connect_stub.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "StubGetTouchpadTapSwitchFuzzTest"

namespace OHOS {
namespace MMI {
namespace OHOS {

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
    DelayedSingleton<MMIService>::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(MMI::MultimodalinputConnectInterfaceCode::GET_TP_TAP_SWITCH), datas, reply, option);
    return true;
}
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    OHOS::StubGetTouchpadTapSwitchFuzzTest(data, size);
    return 0;
}
} // namespace MMI
} // namespace OHOS