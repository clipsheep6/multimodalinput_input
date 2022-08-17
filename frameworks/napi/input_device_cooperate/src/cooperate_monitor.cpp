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

#include "cooperate_monitor.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <set>
#include <string_view>

#include <uv.h>

#include "constants.h"
#include "define_multimodal.h"
#include "input_manager.h"
#include "mmi_log.h"
#include "refbase.h"
#include "util_napi.h"

namespace OHOS::MMI::CooperateMonitor {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "CooperateMonitor" };
constexpr std::string_view COOPERATE = "cooperate";
std::set<std::string> types;
} // namespace

Context::Context(napi_env env) : BaseContext(env) { }

void Context::ParseParams(napi_value *params, size_t paramsCount)
{
    CALL_INFO_TRACE;
    if (paramsCount != 1 && paramsCount != 2) {
        THROWERR(env_, "cooperate monitor: paramsCount error");
        parseOk_ = false;
        return;
    }
    if (!UtilNapi::TypeOf(env_, params[0], napi_string)) {
        THROWERR(env_, "cooperate monitor: the first parameter is not string");
        parseOk_ = false;
        return;
    }
    char type[MAX_STRING_LEN] = { 0 };
    size_t length = 0;
    CHKRV(env_, napi_get_value_string_utf8(env_, params[0], type, sizeof(type), &length), GET_STRING);
    type_ = type;

    if (paramsCount == 2) {
        if (!UtilNapi::TypeOf(env_, params[1], napi_function)) {
            THROWERR(env_, "cooperate monitor: the second paramter is not function");
            parseOk_ = false;
            return;
        }
        isPromise_ = false;
        callback_ = params[1];
    }
    parseOk_ = true;
}

static void AsyncCallback(uv_work_t *work, int32_t status)
{
    CALL_INFO_TRACE;
    CHKPV(work);
    auto context = static_cast<Context*>(work->data);
    CHKPV(context);

    napi_value undefined = nullptr;
    CHKRV(context->env_, napi_get_undefined(context->env_, &undefined), GET_UNDEFINED);
    napi_value argv[2] = { undefined, undefined };

    napi_value object = nullptr;
    CHKRV(context->env_, napi_create_object(context->env_, &object), CREATE_OBJECT);

    napi_value deviceDescriptor = nullptr;
    CHKRV(context->env_, napi_create_string_utf8(context->env_, context->deviceDescriptor_.c_str(),
        NAPI_AUTO_LENGTH, &deviceDescriptor), CREATE_STRING);
    napi_value eventMsg = nullptr;
    CHKRV(context->env_, napi_create_int32(context->env_,
        static_cast<int32_t>(context->eventMsg_), &eventMsg), CREATE_INT32);

    CHKRV(context->env_, napi_set_named_property(context->env_, object, "deviceDescriptor", deviceDescriptor),
        SET_NAMED_PROPERTY);
    CHKRV(context->env_, napi_set_named_property(context->env_, object, "eventMsg", eventMsg), SET_NAMED_PROPERTY);
    argv[1] = object;
    UtilNapi::CallFunction<Context>(context->env_, context, 2, argv);
}

static void Callback(sptr<Context> context, std::string deviceDescriptor, CooperateMessages cooperateMessages)
{
    CALL_INFO_TRACE;
    uv_loop_s *loop = nullptr;
    CHKRV(context->env_, napi_get_uv_event_loop(context->env_, &loop), GET_UV_LOOP);
    uv_work_t *work = new(std::nothrow) uv_work_t;
    CHKPV(work);

    context->deviceDescriptor_ = deviceDescriptor;
    context->eventMsg_ = cooperateMessages;

    work->data = static_cast<void*>(context);
    int32_t result;
    result = uv_queue_work(loop, work, [](uv_work_t *work) {}, AsyncCallback);
    if(result != 0) {
        MMI_HILOGE("cooperate monitor uv_queue_work failed");
        delete work;
        work = nullptr;
    }
}

static void ExecuteOn(sptr<Context> context)
{
    CALL_INFO_TRACE;
    if (!context->parseOk_ || context->type_ != COOPERATE) {
        return;
    }
    auto type = types.find(context->type_);
    if (type != types.end()) {
        MMI_HILOGI("cooperate monitor execute on is already exit");
        context->errorCode_ = RET_OK;
    } else {
        types.insert(context->type_);
        auto callback = std::bind(Callback, context, std::placeholders::_1, std::placeholders::_2);
        context->errorCode_ = InputMgr->RegisterCooperateListener(callback);
    }
}

static void AsyncExecuteOff(napi_env env, void *data)
{
    CALL_INFO_TRACE;
    auto context = static_cast<Context*>(data);
    CHKPV(context);
    if (!context->parseOk_) {
        context->errorCode_ = RET_ERR;
        return;
    }
    if (context->type_ != COOPERATE) {
        MMI_HILOGE("cooperate monitor execute off type error");
        context->errorCode_ = RET_ERR;
        return;
    }
    types.erase(context->type_);
    if (!types.empty()) {
        context->errorCode_ = RET_ERR;
        return;
    }
    context->errorCode_ = InputMgr->UnregisterCooperateListener();
}

static void CompleteExecuteOff(napi_env env, napi_status status, void *data)
{
    CALL_INFO_TRACE;
    auto context = static_cast<Context*>(data);
    CHKPV(context);

    if (context->errorCode_ == RET_OK) {
        napi_value undefined = nullptr;
        CHKRV(env, napi_get_undefined(env, &undefined), GET_UNDEFINED);
        napi_value argv[2] = { undefined, undefined };
        UtilNapi::CallFunction<Context>(env, context, 2, argv);
    } else {
        napi_value errorObject = UtilNapi::CreateErrorMessage(env, RET_ERR, "monitor off failed");
        UtilNapi::CallFunction<Context>(env, context, 1, &errorObject);
    }
}

static void ExecuteOff(sptr<Context> context)
{
    CALL_INFO_TRACE;
    napi_value asyncName = nullptr;
    CHKRV(context->env_, napi_create_string_utf8(context->env_, "off", NAPI_AUTO_LENGTH, &asyncName), CREATE_STRING);
    CHKRV(context->env_, napi_create_async_work(context->env_, nullptr, asyncName,
        AsyncExecuteOff, CompleteExecuteOff, static_cast<void*>(context), &context->asyncWork_), CREATE_ASYNC_WORK);
    CHKRV(context->env_, napi_queue_async_work(context->env_, context->asyncWork_), QUEUE_ASYNC_WORK);
}

napi_value On(napi_env env, napi_callback_info info)
{
    CALL_INFO_TRACE;
    size_t argc = 0;
    napi_value argv[] = { nullptr };
    CHKRP(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);

    sptr<Context> context = new(std::nothrow) Context(env);
    CHKPP(context);
    context->ParseParams(argv, argc);
    MMI_HILOGI("params parse OK? %{public}d", context->parseOk_);
    CHKRP(env, napi_create_reference(env, context->callback_, 1, &context->callbackRef_), CREATE_REFERENCE);
    ExecuteOn(context);

    napi_value undefined = nullptr;
    CHKRP(env, napi_get_undefined(env, &undefined), GET_UNDEFINED);
    return undefined;
}

napi_value Off(napi_env env, napi_callback_info info)
{
    CALL_INFO_TRACE;
    size_t argc = 2;
    napi_value argv[2] = {};
    CHKRP(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);

    sptr<Context> context = new(std::nothrow) Context(env);
    CHKPP(context);
    context->ParseParams(argv, argc);
    MMI_HILOGI("params parse OK? %{public}d", context->parseOk_);
    CHKRP(env, napi_create_reference(env, context->callback_, 1, &context->callbackRef_), CREATE_REFERENCE);
    if (!context->parseOk_) {
        return nullptr;
    }

    if (context->callback_ != nullptr) {
        CHKRP(env, napi_create_reference(env, context->callback_, 1, &context->callbackRef_), CREATE_REFERENCE);
    }
    ExecuteOff(context);

    napi_value undefined = nullptr;
    CHKRP(env, napi_get_undefined(env, &undefined), GET_UNDEFINED);
    return undefined;
}
} // namespace OHOS::MMI::CooperateMonitor
