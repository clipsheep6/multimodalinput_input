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

#include "js_input_device_manager.h"
#include "napi_constants.h"

#include "input_device_impl.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JsInputDeviceManager" };
std::mutex mutex_;

enum class ReturnType {
    VOID,
    BOOL,
    NUMBER,
};
} // namespace
void JsInputDeviceManager::RegisterDevListener(napi_env env, const std::string &type, napi_value handle)
{
    CALL_DEBUG_ENTER;
    AddListener(env, type, handle);
}

void JsInputDeviceManager::UnregisterDevListener(napi_env env, const std::string &type, napi_value handle)
{
    CALL_DEBUG_ENTER;
    RemoveListener(env, type, handle);
}

napi_value JsInputDeviceManager::GetDeviceIds(napi_env env, napi_value handle)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    int32_t userData = InputDevImpl.GetUserData();
    napi_value ret = CreateCallbackInfo(env, handle, userData);
    auto callback = std::bind(EmitJsIds, userData, std::placeholders::_1);
    InputMgr->GetDeviceIds(callback);
    return ret;
}

napi_value JsInputDeviceManager::GetDevice(napi_env env, int32_t id, napi_value handle)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    int32_t userData = InputDevImpl.GetUserData();
    napi_value ret = CreateCallbackInfo(env, handle, userData);
    auto callback = std::bind(EmitJsDev, userData, std::placeholders::_1);
    InputMgr->GetDevice(id, callback);
    return ret;
}

napi_value JsInputDeviceManager::SupportKeys(napi_env env, int32_t id, std::vector<int32_t> &keyCodes,
    napi_value handle)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    int32_t userData = InputDevImpl.GetUserData();
    napi_value ret = CreateCallbackInfo(env, handle, userData);
    auto callback = std::bind(EmitSupportKeys, userData, std::placeholders::_1);
    InputMgr->SupportKeys(id, keyCodes, callback);
    return ret;
}

napi_value JsInputDeviceManager::GetKeyboardType(napi_env env, int32_t id, napi_value handle)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    int32_t userData = InputDevImpl.GetUserData();
    napi_value ret = CreateCallbackInfo(env, handle, userData);
    auto callback = std::bind(EmitJsKeyboardType, userData, std::placeholders::_1);
    InputMgr->GetKeyboardType(id, callback);
    return ret;
}

void getResult(sptr<AsyncContext> asyncContext, napi_value* results)
{
    CALL_DEBUG_ENTER;
    napi_env env = asyncContext->env;
    if (asyncContext->errorCode == RET_OK) {
        CHKRV(env, napi_get_undefined(env, &results[0]), GET_UNDEFINED);
    } else {
        CHKRV(env, napi_create_object(env, &results[0]), CREATE_OBJECT);
        napi_value errCode = nullptr;
        CHKRV(env, napi_create_int32(env, asyncContext->errorCode, &errCode), CREATE_INT32);
        CHKRV(env, napi_set_named_property(env, results[0], "code", errCode), SET_NAMED_PROPERTY);
    }

    ReturnType resultType;
    asyncContext->reserve >> resultType;
    if (resultType == ReturnType::BOOL) {
        bool temp;
        asyncContext->reserve >> temp;
        CHKRV(env, napi_get_boolean(env, temp, &results[1]), CREATE_BOOL);
    } else if (resultType == ReturnType::NUMBER) {
        int32_t temp;
        asyncContext->reserve >> temp;
        CHKRV(env, napi_create_int32(env, temp, &results[1]), CREATE_INT32);
    } else {
        CHKRV(env, napi_get_undefined(env, &results[1]), GET_UNDEFINED);
    }
}

void AsyncCallbackWork(sptr<AsyncContext> asyncContext)
{
    CALL_DEBUG_ENTER;
    CHKPV(asyncContext);
    CHKPV(asyncContext->env);
    napi_env env = asyncContext->env;
    napi_value resource = nullptr;
    CHKRV(env, napi_create_string_utf8(env, "AsyncCallbackWork", NAPI_AUTO_LENGTH, &resource), CREATE_STRING_UTF8);
    asyncContext->IncStrongRef(nullptr);
    napi_status status = napi_create_async_work(env, nullptr, resource, [](napi_env env, void* data) {},
        [](napi_env env, napi_status status, void* data) {
            sptr<AsyncContext> asyncContext(static_cast<AsyncContext *>(data));
            asyncContext->DecStrongRef(nullptr);
            napi_value results[2] = { 0 };
            getResult(asyncContext, results);
            if (asyncContext->deferred) {
                if (asyncContext->errorCode == RET_OK) {
                    CHKRV(env, napi_resolve_deferred(env, asyncContext->deferred, results[1]), RESOLVE_DEFERRED);
                } else {
                    CHKRV(env, napi_reject_deferred(env, asyncContext->deferred, results[0]), REJECT_DEFERRED);
                }
            } else {
                napi_value callback = nullptr;
                CHKRV(env, napi_get_reference_value(env, asyncContext->callback, &callback), GET_REFERENCE);
                napi_value callResult = nullptr;
                CHKRV(env, napi_call_function(env, nullptr, callback, 2, results, &callResult), CALL_FUNCTION);
            }
        },
        asyncContext.GetRefPtr(), &asyncContext->work);
    if (status != napi_ok || napi_queue_async_work(env, asyncContext->work) != napi_ok) {
        MMI_HILOGE("Create async work failed");
        asyncContext->DecStrongRef(nullptr);
    }
}

AsyncContext::~AsyncContext()
{
    CALL_DEBUG_ENTER;
    if (work != nullptr) {
        CHKRV(env, napi_delete_async_work(env, work), DELETE_ASYNC_WORK);
    }
    if (callback != nullptr && env != nullptr) {
        CHKRV(env, napi_delete_reference(env, callback), DELETE_REFERENCE);
        env = nullptr;
    }
}

napi_value JsInputDeviceManager::GetFunctionKeyState(napi_env env, int32_t key, napi_value handle)
{
    CALL_DEBUG_ENTER;
    sptr<AsyncContext> asyncContext = new (std::nothrow) AsyncContext(env);
    if (asyncContext == nullptr) {
        THROWERR(env, "create AsyncContext failed");
        return nullptr;
    }

    bool state = InputManager::GetInstance()->GetFunctionKeyState(key);
    asyncContext->errorCode = ERR_OK;
    asyncContext->reserve << ReturnType::BOOL << state;

    napi_value promise = nullptr;
    if (handle != nullptr) {
        CHKRP(env, napi_create_reference(env, handle, 1, &asyncContext->callback), CREATE_REFERENCE);
        CHKRP(env, napi_get_undefined(env, &promise), GET_UNDEFINED);
    } else {
        CHKRP(env, napi_create_promise(env, &asyncContext->deferred, &promise), CREATE_PROMISE);
    }
    AsyncCallbackWork(asyncContext);
    return promise;
}

napi_value JsInputDeviceManager::SetFunctionKeyState(napi_env env, int32_t key, bool state, napi_value handle)
{
    CALL_DEBUG_ENTER;
    sptr<AsyncContext> asyncContext = new (std::nothrow) AsyncContext(env);
    if (asyncContext == nullptr) {
        THROWERR(env, "create AsyncContext failed");
        return nullptr;
    }

    asyncContext->errorCode = InputManager::GetInstance()->SetFunctionKeyState(key, state);
    asyncContext->reserve << ReturnType::VOID;

    napi_value promise = nullptr;
    if (handle != nullptr) {
        CHKRP(env, napi_create_reference(env, handle, 1, &asyncContext->callback), CREATE_REFERENCE);
        CHKRP(env, napi_get_undefined(env, &promise), GET_UNDEFINED);
    } else {
        CHKRP(env, napi_create_promise(env, &asyncContext->deferred, &promise), CREATE_PROMISE);
    }
    AsyncCallbackWork(asyncContext);
    return promise;
}

void JsInputDeviceManager::ResetEnv()
{
    CALL_DEBUG_ENTER;
    JsEventTarget::ResetEnv();
}
} // namespace MMI
} // namespace OHOS