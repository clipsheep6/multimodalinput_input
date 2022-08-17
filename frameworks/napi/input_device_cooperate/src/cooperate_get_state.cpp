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

#include "cooperate_get_state.h"

#include <cstddef>
#include <functional>

#include "constants.h"
#include "define_multimodal.h"
#include "input_manager.h"
#include "mmi_log.h"
#include "utils.h"
#include "util_napi.h"

namespace OHOS {
namespace MMI {
namespace CooperateGetState {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "cooperate_get_state" };
} // namespace

static sptr<Context> ParseParams(napi_env env, napi_callback_info info)
{
    MMI_HILOGI("Cooperate getState: parse params");

    size_t argc = 2;
    napi_value argv[] = { nullptr, nullptr };
    CHKRP(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);

    if (argc < 1 || argc > 2) {
        THROWERR(env, "Cooperate getState: parameter is not one or two");
        return nullptr;
    }

    if (!TypeOf(env, argv[0], napi_string)) {
        THROWERR(env, "Cooperate getState: the first parameter is not string");
        return nullptr;
    }

    sptr<Context> context = new Context();
    if (argc == 2) {
        if (!TypeOf(env, argv[1], napi_function)) {
            THROWERR(env, "Cooperate getState: the second paramter is not function");
            delete context;
            context = nullptr;
            return nullptr;
        }
        context->isPromise_ = false;
        context->callback_ = argv[1];
    } else {
        context->isPromise_ = true;
    }

    char deviceId[MAX_STRING_LEN] = {0};
    size_t length = 0;
    CHKRP(env, napi_get_value_string_utf8(env, argv[0], deviceId, sizeof(deviceId), &length), GET_STRING);
    context->deviceId_ = deviceId;

    context->env_ = env;

    return context;
}

static void Callback(sptr<Context> context, bool state)
{
    MMI_HILOGI("Cooperate getState callback");

    napi_value object = nullptr;
    CHKRV(context->env_, napi_create_object(context->env_, &object), CREATE_OBJECT);

    napi_value jsstate = nullptr;
    CHKRV(context->env_, napi_get_boolean(context->env_, state, &jsstate), GET_BOOLEAN);

    CHKRV(context->env_, napi_set_named_property(context->env_, object, "state", jsstate), SET_NAMED_PROPERTY);

    if (context->isPromise_) {
        if (context->errorCode_ == SUCCESS) {
            CHKRV(context->env_, napi_resolve_deferred(context->env_, context->deferred_, object), RESOLVE_DEFERRED);
        } else {
            CHKRV(context->env_, napi_reject_deferred(context->env_, context->deferred_, object), REJECT_DEFERRED);
        }
    } else {
        napi_value callback = nullptr;
        CHKRV(context->env_, napi_get_reference_value(context->env_, context->callbackRef_, &callback),
            GET_REFERENCE_VALUE);
        napi_value result = nullptr;
        CHKRV(context->env_, napi_call_function(context->env_, nullptr, callback, 1, &object, &result), CALL_FUNCTION);
    }

    delete context;
    context = nullptr;
}

static void Execute(sptr<Context> context)
{
    MMI_HILOGI("Cooperate getState execute");

    auto callback = std::bind(Callback, context, std::placeholders::_1);
    int32_t result = InputMgr->GetInputDeviceCooperateState(context->deviceId_, callback);
    if (result != RET_OK) {
        MMI_HILOGE("cooperate execute error");
        context->errorCode_ = FAILED;
    } else {
        context->errorCode_ = SUCCESS;
    }
}

napi_value GetState(napi_env env, napi_callback_info info)
{
    MMI_HILOGI("Cooperate getState");

    sptr<Context> context = ParseParams(env, info);
    if (context == nullptr) {
        MMI_HILOGE("Cooperate getState: context is nullptr");
        return nullptr;
    }

    napi_value promise = nullptr;
    if (context->isPromise_) {
        CHKRP(env, napi_create_promise(env, &context->deferred_, &promise), CREATE_PROMISE);
    } else {
        CHKRP(env, napi_create_reference(env, context->callback_, 1, &context->callbackRef_), CREATE_REFERENCE);
        CHKRP(env, napi_get_undefined(env, &promise), GET_UNDEFINED);
    }

    Execute(context);

    return promise;
}
} // namespace CooperateGetState
} // namespace MMI
} // namespace OHOS