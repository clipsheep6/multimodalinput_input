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

#include "cooperate_start.h"

#include <cstddef>

#include "constants.h"
#include "define_multimodal.h"
#include "input_manager.h"
#include "mmi_log.h"
#include "utils.h"
#include "util_napi.h"

namespace OHOS {
namespace MMI {
namespace CooperateStart {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "cooperate_start" };
constexpr size_t ARGC_SIZE_MIN = 1;
constexpr size_t ARGC_SIZE_MAX = 2;
} // namespace

static sptr<Context> ParseParams(napi_env env, napi_callback_info info)
{
    MMI_HILOGI("Cooperate start: parse params");

    size_t argc = 2;
    napi_value argv[] = { nullptr, nullptr };
    CHKRP(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);

    if (argc < ARGC_SIZE_MIN || argc > ARGC_SIZE_MAX) {
        THROWERR(env, "Cooperate start: parameter is not one or two or three");
        return nullptr;
    }

    if (!TypeOf(env, argv[0], napi_string) || !TypeOf(env, argv[1], napi_number)) {
        THROWERR(env, "Cooperate start: the first parameter is not string, the second parameter is not number");
        return nullptr;
    }

    sptr<Context> context = new Context();

    char sinkDeviceId[MAX_STRING_LEN] = {};
    size_t length = 0;
    CHKRP(env, napi_get_value_string_utf8(env, argv[0], sinkDeviceId, sizeof(sinkDeviceId), &length), GET_STRING);
    context->sinkDeviceId_ = sinkDeviceId;

    CHKRP(env, napi_get_value_int32(env, argv[1], &context->srcInputDeviceId_), GET_INT32);

    return context;
}

static void Execute(sptr<Context> context)
{
    MMI_HILOGI("Cooperate start execute");

    int32_t result = InputMgr->StartInputDeviceCooperate(context->sinkDeviceId_, context->srcInputDeviceId_);
    if (result != RET_OK) {
        MMI_HILOGE("Cooperate start execute error");
    }
<<<<<<< HEAD
=======

>>>>>>> bd6697fa0b77718b443ac279fd0bd8d36af5ef9f
}

napi_value Start(napi_env env, napi_callback_info info)
{
    MMI_HILOGI("Cooperate start");

    sptr<Context> context = ParseParams(env, info);
    if (context == nullptr) {
        MMI_HILOGE("Cooperate start: context is nullptr");
        return nullptr;
    }

    Execute(context);
    return nullptr;
}
} // namespace CooperateStart
} // namespace MMI
} // namespace OHOS