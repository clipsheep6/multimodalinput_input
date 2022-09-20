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

#include "mock_multimodal_input_connect_proxy.h"

#include "mmi_log.h"
#include "errors.h"
#include "message_parcel.h"
#include "message_option.h"
#include "mmi_log.h"
#include "multimodal_input_connect_def_parcel.h"
#include "multimodal_input_connect_define.h"
#include "string_ex.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MockMultimodalInputConnectProxy" };
static bool state_ = true;
} // namespace
void MultimodalInputConnectProxy::SetMIState(bool state)
{
    state_ = state;
}

int32_t MultimodalInputConnectProxy::RegisterCooperateListener()//a
{
    bool ret = state_;
    if(ret == false){
        return RET_ERR;
    }
    return RET_OK;
}


int32_t MultimodalInputConnectProxy::UnregisterCooperateListener()//a
{
    bool ret = state_;
    if(ret == false){
        return RET_ERR;
    }
    return RET_OK;
}


int32_t MultimodalInputConnectProxy::EnableInputDeviceCooperate(int32_t userData, bool enabled)//a
{
    bool ret = state_;
    if(ret == false){
        return RET_ERR;
    }
    MMI_HILOGI("EnableInputDeviceCooperate");
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::StartInputDeviceCooperate(int32_t userData, const std::string &sinkDeviceId,
    int32_t srcInputDeviceId)
{
    bool ret = state_;
    if(sinkDeviceId == "" || ret == false){
        return RET_ERR;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::StopDeviceCooperate(int32_t userData)
{
    bool ret = state_;
    if(ret == false){
        return RET_ERR;
    }
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
