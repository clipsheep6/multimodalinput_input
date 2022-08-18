/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "cooperate_stop.h"

#include <cstdint>

#include "define_multimodal.h"
#include "input_manager.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace CooperateStop {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "cooperate_stop" };
} // namespace

static void Execute()
{
    MMI_HILOGI("Cooperate stop execute");

    int32_t result = InputMgr->StopDeviceCooperate();
    if (result != RET_OK) {
        MMI_HILOGE("Cooperate stop execute error");
    }
}

napi_value Stop(napi_env env, napi_callback_info info)
{
    MMI_HILOGI("Cooperate stop");

    Execute();
    return nullptr;
}
} // namespace CooperateStop
} // namespace MMI
} // namespace OHOS