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

#include "cooperate_enable.h"

#include <cstddef>
#include <cstdint>

#include "constants.h"
#include "define_multimodal.h"
#include "input_manager.h"
#include "mmi_log.h"
#include "utils.h"
#include "util_napi.h"

namespace OHOS {
namespace MMI {
namespace CooperateEnable {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "cooperate_enable" };
} // namespace

static sptr<Context> ParseParams(napi_env env, napi_callback_info info)
{
    MMI_HILOGI("Cooperate enable: parse params");

    size_t argc = 1;
    napi_value argv[] = { nullptr };
    CHKRP(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);

    if (argc < 1 || argc > 1) {
        THROWERR(env, "Cooperate enable: parameter is not one or two");
        return nullptr;
    }

    if (!TypeOf(env, argv[0], napi_boolean)) {
        THROWERR(env, "Cooperate enable: the first parameter is not boolean");
        return nullptr;
    }

    sptr<Context> context = new Context();
    CHKRP(env, napi_get_value_bool(env, argv[0], &context->enable_), GET_BOOL);

    return context;
}

static void Execute(sptr<Context> context)
{
    MMI_HILOGI("Cooperate enable: Execute");

    int32_t result = InputMgr->EnableInputDeviceCooperate(context->enable_);
    if (result != RET_OK) {
        MMI_HILOGE("Cooperate enable: Execute error");
    }
}

napi_value Enable(napi_env env, napi_callback_info info)
{
    MMI_HILOGI("Cooperate enable");

    sptr<Context> context = ParseParams(env, info);
    if (context == nullptr) {
        MMI_HILOGE("Cooperate enable: context is nullptr");
        return nullptr;
    }

    Execute(context);
    return nullptr;
}
} // namespace  CooperateEnable
} // namespace MMI
} // namespace OHOS