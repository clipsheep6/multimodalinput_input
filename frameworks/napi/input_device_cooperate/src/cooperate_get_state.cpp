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
#include "refbase.h"
#include "util_napi.h"

namespace OHOS::MMI::CooperateGetState {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "CooperateGetState" };
} // namespace

Context::Context(napi_env env) : BaseContext(env) { }

void Context::ParseParams(napi_value *params, size_t paramsCount)
{
    CALL_INFO_TRACE;
    if (paramsCount != 1 && paramsCount != 2) {
        THROWERR(env_, "cooperate get state: paramsCount error");
        parseOk_ = false;
        return;
    }
    if (!UtilNapi::TypeOf(env_, params[0], napi_string)) {
        THROWERR(env_, "cooperate get state: the first parameter is not string");
        parseOk_ = false;
        return;
    }
    char deviceDescriptor[MAX_STRING_LEN] = { 0 };
    size_t length = 0;
    CHKRV(env_, napi_get_value_string_utf8(env_, params[0], deviceDescriptor,
        sizeof(deviceDescriptor), &length), GET_STRING);
    deviceDescriptor_ = deviceDescriptor;

    if (paramsCount == 1) {
        isPromise_ = true;
        parseOk_ = true;
        return;
    }
    if (paramsCount == 2) {
        if (!UtilNapi::TypeOf(env_, params[1], napi_function)) {
            THROWERR(env_, "cooperate get state: the second paramter is not function");
            parseOk_ = false;
            return;
        }
        isPromise_ = false;
        parseOk_ = true;
        callback_ = params[1];
    }
}

static void CallbackGetState(sptr<Context> context, bool state)
{
    CALL_INFO_TRACE;
    napi_value undefined = nullptr;
    CHKRV(context->env_, napi_get_undefined(context->env_, &undefined), GET_UNDEFINED);
    napi_value argv[2] = { undefined, undefined };

    napi_value object = nullptr;
    CHKRV(context->env_, napi_create_object(context->env_, &object), CREATE_OBJECT);
    napi_value jsstate = nullptr;
    CHKRV(context->env_, napi_get_boolean(context->env_, state, &jsstate), GET_BOOLEAN);
    CHKRV(context->env_, napi_set_named_property(context->env_, object, "state", jsstate), SET_NAMED_PROPERTY);
    argv[1] = object;

    if (context->isPromise_) {
        CHKRV(context->env_, napi_resolve_deferred(context->env_, context->deferred_, object), RESOLVE_DEFERRED);
        return;
    }
    UtilNapi::CallFunction<Context>(context->env_, context, 2, argv);
}

static void ExecuteGetState(sptr<Context> context)
{
    CALL_INFO_TRACE;
    CHKPV(context);
    if (context->parseOk_) {
        auto callback = std::bind(CallbackGetState, context, std::placeholders::_1);
        context->errorCode_ = InputMgr->GetInputDeviceCooperateState(context->deviceDescriptor_, callback);
    }
    if (!context->parseOk_ || context->errorCode_ != RET_OK) {
        napi_value errorObject = UtilNapi::CreateErrorMessage(context->env_, RET_ERR, "get state failed");
        if (context->isPromise_) {
            napi_resolve_deferred(context->env_, context->deferred_, errorObject);
            return;
        }
        UtilNapi::CallFunction<Context>(context->env_, context, 1, &errorObject);
    }
}

napi_value GetState(napi_env env, napi_callback_info info)
{
    CALL_INFO_TRACE;
    size_t argc = 2;
    napi_value argv[2] = {};
    CHKRP(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);

    sptr<Context> context = new(std::nothrow) Context(env);
    CHKPP(context);
    context->ParseParams(argv, argc);
    MMI_HILOGI("params parse OK? %{public}d", context->parseOk_);
    if (!context->parseOk_) {
        return nullptr;
    }

    napi_value promise = nullptr;
    if (context->isPromise_) {
        CHKRP(env, napi_create_promise(env, &context->deferred_, &promise), CREATE_PROMISE);
    } else {
        CHKRP(env, napi_create_reference(env, context->callback_, 1, &context->callbackRef_), CREATE_REFERENCE);
        CHKRP(env, napi_get_undefined(env, &promise), GET_UNDEFINED);
    }

    ExecuteGetState(context);
    return promise;
}
} // namespace OHOS::MMI::CooperateGetState
