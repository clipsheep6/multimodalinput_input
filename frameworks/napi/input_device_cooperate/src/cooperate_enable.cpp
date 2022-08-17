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

#include "constants.h"
#include "define_multimodal.h"
#include "input_manager.h"
#include "mmi_log.h"
#include "refbase.h"
#include "util_napi.h"

namespace OHOS::MMI::CooperateEnable {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "CooperateEnable" };
} // namespace

Context::Context(napi_env env) : BaseContext(env) { }

void Context::ParseParams(napi_value *params, size_t paramsCount)
{
    CALL_INFO_TRACE;
    if (paramsCount != 1 && paramsCount != 2) {
        THROWERR(env_, "cooperate enable: paramsCount error");
        parseOk_ = false;
        return;
    }
    if (!UtilNapi::TypeOf(env_, params[0], napi_boolean)) {
        THROWERR(env_, "cooperate enable: the first parameter is not boolean");
        parseOk_ = false;
        return;
    }
    CHKRV(env_, napi_get_value_bool(env_, params[0], &enable_), GET_BOOL);

    if (paramsCount == 1) {
        isPromise_ = true;
        parseOk_ = true;
        return;
    }
    if (paramsCount == 2) {
        if (!UtilNapi::TypeOf(env_, params[1], napi_function)) {
            THROWERR(env_, "cooperate enable: the second paramter is not function");
            parseOk_ = false;
            return;
        }
        isPromise_ = false;
        callback_ = params[1];
        parseOk_ = true;
    }
}

static void AsyncExecuteEnable(napi_env env, void *data)
{
    CALL_INFO_TRACE;
    auto context = static_cast<Context*>(data);
    CHKPV(context);
    if (!context->parseOk_) {
        context->errorCode_ = RET_ERR;
        return;
    }
    context->errorCode_ = InputMgr->EnableInputDeviceCooperate(context->enable_);
}

static void CompleteExecuteEnable(napi_env env, napi_status status, void *data)
{
    CALL_INFO_TRACE;
    auto context = static_cast<Context*>(data);
    CHKPV(context);
    if (context->errorCode_ == RET_OK) {
        napi_value undefined = nullptr;
        CHKRV(env, napi_get_undefined(env, &undefined), GET_UNDEFINED);
        napi_value argv[2] = { undefined, undefined };
        if (context->isPromise_) {
            napi_resolve_deferred(env, context->deferred_, argv[0]);
            return;
        }
        UtilNapi::CallFunction<Context>(env, context, 2, argv);
    } else {
        napi_value errorObject = UtilNapi::CreateErrorMessage(env, RET_ERR, "enable failed");
        if (context->isPromise_) {
            napi_reject_deferred(env, context->deferred_, errorObject);
            return;
        }
        UtilNapi::CallFunction<Context>(env, context, 1, &errorObject);
    }
}

napi_value Enable(napi_env env, napi_callback_info info)
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
        CHKRP(env, napi_get_undefined(env, &promise), GET_UNDEFINED);
        CHKRP(env, napi_create_reference(env, context->callback_, 1, &context->callbackRef_), CREATE_REFERENCE);
    }

    napi_value asyncName = nullptr;
    CHKRP(env, napi_create_string_utf8(env, "enable", NAPI_AUTO_LENGTH, &asyncName), CREATE_STRING);
    CHKRP(env, napi_create_async_work(env, nullptr, asyncName, AsyncExecuteEnable, CompleteExecuteEnable,
        static_cast<void*>(context), &context->asyncWork_), CREATE_ASYNC_WORK);
    CHKRP(env, napi_queue_async_work(env, context->asyncWork_), QUEUE_ASYNC_WORK);
    return promise;
}
} // namespace OHOS::MMI::CooperateEnable
