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

#include "power_shutdown_callback_stub.h"

#include <message_parcel.h>

#include "power_common.h"
#include "power_shutdown_callback_proxy.h"
#include "xcollie.h"

namespace OHOS {
namespace PowerMgr {
int PowerShutdownCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    POWER_HILOGD(MODULE_SERVICE, "PowerShutdownCallbackStub::OnRemoteRequest, cmd = %d, flags= %d",
        code, option.GetFlags());
    const int DFX_DELAY_MS = 10000;
    int id = HiviewDFX::XCollie::GetInstance().SetTimer("PowerShutdownCallback", DFX_DELAY_MS, nullptr, nullptr,
        HiviewDFX::XCOLLIE_FLAG_NOOP);
    std::u16string descripter = PowerShutdownCallbackStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        POWER_HILOGE(MODULE_SERVICE, "PowerMgrStub::OnRemoteRequest failed, descriptor is not matched!");
        return E_GET_POWER_SERVICE_FAILED;
    }

    int ret = ERR_OK;
    switch (code) {
        case static_cast<uint32_t>(IShutdownCallback::POWER_SHUTDOWN_CHANGED): {
            ret = OnPowerShutdownCallbackStub();
        }
        default:
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    HiviewDFX::XCollie::GetInstance().CancelTimer(id);
    return ERR_OK;
}

int32_t PowerShutdownCallbackStub::OnPowerShutdownCallbackStub()
{
    ShutdownCallback();
    return ERR_OK;
}
} // namespace PowerMgr
} // namespace OHOS
