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
#include "utils.h"
#include "util_napi.h"

namespace OHOS {
namespace MMI {
namespace CooperateMonitor {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "cooperate_monitor" };
constexpr std::string_view COOPERATE = "cooperate";

sptr<Context> context = nullptr;
std::set<std::string> types;
} // namespace

static sptr<Context> Parseparams(napi_env env, napi_callback_info info)
{
    MMI_HILOGI("Cooperate monitor: parse params start");

    size_t argc = 2;
    napi_value argv[] = { nullptr, nullptr };
    CHKRP(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);

    if (argc < 1 || argc > 2) {
        THROWERR(env, "Cooperate monitor: parameter is not one or two");
        return nullptr;
    }

    if (!TypeOf(env, argv[0], napi_string)) {
        THROWERR(env, "Cooperate monitor: the first parameter is not string");
        return nullptr;
    }

    if (context == nullptr) {
        context = new Context();
    }

    if (argc == 2) {
        if (!TypeOf(env, argv[1], napi_function)) {
            THROWERR(env, "Cooperate monitor: the second parameter is not function");
            if (types.empty()) {
                delete context;
                context = nullptr;
            }
            return nullptr;
        }
        context->callback_ = argv[1];
    } else {
        context->callback_ = nullptr;
    }

    char type[MAX_STRING_LEN] = {0};
    size_t length = 0;
    CHKRP(env, napi_get_value_string_utf8(env, argv[0], type, sizeof(type), &length), GET_STRING);
    context->type_ = type;
    context->env_ = env;

    return context;
}

static void AsyncCallback(uv_work_t *work, int32_t status)
{
    MMI_HILOGI("Cooperate monitor AsyncCallback");

    CHKPV(work);
    auto context_ = static_cast<Context*>(work->data);
    CHKPV(context_);

    napi_value object = nullptr;
    CHKRV(context_->env_, napi_create_object(context->env_, &object), CREATE_OBJECT);

    napi_value deviceId = nullptr;
    CHKRV(context_->env_, napi_create_string_utf8(context_->env_, context_->deviceId_.c_str(),
        NAPI_AUTO_LENGTH, &deviceId), CREATE_STRING);
    napi_value eventMsg = nullptr;
    CHKRV(context_->env_, napi_create_int32(context_->env_,
        static_cast<int32_t>(context->eventMsg_), &eventMsg), CREATE_INT32);
    MMI_HILOGI("eventMsg_:%{public}d", context->eventMsg_);
    CHKRV(context_->env_, napi_set_named_property(context_->env_, object, "deviceId", deviceId), SET_NAMED_PROPERTY);
    CHKRV(context_->env_, napi_set_named_property(context_->env_, object, "eventMsg", eventMsg), SET_NAMED_PROPERTY);

    napi_value callback = nullptr;
    CHKRV(context_->env_, napi_get_reference_value(context_->env_,
        context_->callbackRef_, &callback), GET_REFERENCE_VALUE);
    napi_value result = nullptr;
    CHKRV(context_->env_, napi_call_function(context_->env_, nullptr, callback, 1, &object, &result), CALL_FUNCTION);
}

static void Callback(std::string deviceId, CooperateMessages cooperateMessages)
{
    MMI_HILOGI("Cooperate monitor callback cooperateMessages: %{public}d", cooperateMessages);
    uv_loop_s *loop = nullptr;
    CHKRV(context->env_, napi_get_uv_event_loop(context->env_, &loop), GET_UV_LOOP);
    uv_work_t *work = new(std::nothrow) uv_work_t;
    CHKPV(work);

    context->deviceId_ = deviceId;
    context->eventMsg_ = cooperateMessages;

    work->data = static_cast<void*>(context);
    int32_t result;
    result = uv_queue_work(loop, work, [](uv_work_t *work) {}, AsyncCallback);
    if(result != 0) {
        MMI_HILOGE("Cooperate monitor uv_queue_work failed");
        delete work;
        work = nullptr;
    }
}

static void ExecuteOn()
{
    MMI_HILOGI("Cooperate monitor execute on");

    if (context->type_ != COOPERATE) {
        MMI_HILOGE("Cooperate monitor execute on type error");
        return;
    }

    auto type = types.find(context->type_);
    if (type != types.end()) {
        MMI_HILOGI("Cooperate monitor execute on is already exit");
        return;
    }

    types.insert(context->type_);

    auto callback = std::bind(Callback, std::placeholders::_1, std::placeholders::_2);
    int32_t result = InputMgr->RegisterCooperateListener(callback);
    if (result != RET_OK) {
        MMI_HILOGE("Cooperate monitor execute on error");
    }
}

static void AsyncExecuteOff(napi_env env, void *data)
{
    MMI_HILOGI("Cooperate monitor AsyncExecuteOff");

    int32_t result = InputMgr->UnregisterCooperateListener();
    if (result != RET_OK) {
        MMI_HILOGE("Cooperate monitor execute off error");
    }
}

static void CompleteExecuteOff(napi_env env, napi_status status, void *data)
{
    MMI_HILOGI("Cooperate monitor complateExecuteOff");

    auto context_ = static_cast<Context*>(data);
    if (context_->callback_ == nullptr) {
        return;
    }

    napi_value callback = nullptr;
    CHKRV(context_->env_, napi_get_reference_value(context_->env_, context_->callbackRef_, &callback),
        GET_REFERENCE_VALUE);
    napi_value argv[] = {nullptr};
    napi_value undefined = nullptr;
    CHKRV(context_->env_, napi_get_undefined(context_->env_, &undefined), GET_UNDEFINED);
    CHKRV(context_->env_, napi_call_function(context_->env_, undefined, callback, 1, argv, nullptr),
        CALL_FUNCTION);

    delete context_;
    context_ = nullptr;
}

static void ExecuteOff()
{
    MMI_HILOGI("Cooperate monitor execute off");

    if (context->type_ != COOPERATE) {
        MMI_HILOGE("Cooperate monitor execute off type error");
        return;
    }

    types.erase(context->type_);

    if (!types.empty()) {
        return;
    }

    napi_value asyncName = nullptr;
    CHKRV(context->env_, napi_create_string_utf8(context->env_, "off", NAPI_AUTO_LENGTH, &asyncName), CREATE_STRING);
    CHKRV(context->env_, napi_create_async_work(context->env_, nullptr, asyncName,
        AsyncExecuteOff, CompleteExecuteOff, static_cast<void*>(context), &context->asyncWork_), CREATE_ASYNC_WORK);
    CHKRV(context->env_, napi_queue_async_work(context->env_, context->asyncWork_), QUEUE_ASYNC_WORK);
}

napi_value On(napi_env env, napi_callback_info info)
{
    MMI_HILOGI("Cooperate monitor on");

    auto context = Parseparams(env, info);
    if (context == nullptr) {
        MMI_HILOGE("Cooperate monitor on: context is nullptr");
        return nullptr;
    }

    CHKRP(env, napi_create_reference(env, context->callback_, 1, &context->callbackRef_), CREATE_REFERENCE);
    ExecuteOn();

    return nullptr;
}

napi_value Off(napi_env env, napi_callback_info info)
{
    MMI_HILOGI("Cooperate monitor off");

    auto context = Parseparams(env, info);
    if (context == nullptr) {
        MMI_HILOGE("Cooperate monitor off: context is nullptr");
        return nullptr;
    }

    if (context->callback_ != nullptr) {
        CHKRP(env, napi_create_reference(env, context->callback_, 1, &context->callbackRef_), CREATE_REFERENCE);
    }
    ExecuteOff();

    return nullptr;
}
} // namespace CooperateMonitor
} // namespace MMI
} // namespace OHOS