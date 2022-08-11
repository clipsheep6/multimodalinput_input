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

#include "js_pointer_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JsPointerManager" };

enum class ReturnType {
    VOID,
    BOOL,
    NUMBER,
};
}

bool JsCommon::TypeOf(napi_env env, napi_value value, napi_valuetype type)
{
    napi_valuetype valueType = napi_undefined;
    CHKRF(env, napi_typeof(env, value, &valueType), TYPEOF);
    if (valueType != type) {
        return false;
    }
    return true;
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

void getResult(sptr<AsyncContext> asyncContext, size_t* argc, napi_value* argv)
{
    CALL_DEBUG_ENTER;
    napi_env env = asyncContext->env;
    *argc = 2;
    if (asyncContext->errorCode == RET_OK) {
        napi_get_undefined(env, &argv[0]);
    } else {
        napi_value returnResult;
        CHKRV(env, napi_create_int32(env, asyncContext->errorCode, &returnResult), CREATE_INT32);
        CHKRV(env, napi_create_object(env, &argv[0]), CREATE_OBJECT);
        CHKRV(env, napi_set_named_property(env, argv[0], "code", returnResult), SET_NAMED_PROPERTY);
    }

    ReturnType resultType;
    asyncContext->reserve >> resultType;
    if (resultType == ReturnType::BOOL) {
        bool temp;
        asyncContext->reserve >> temp;
        CHKRV(env, napi_get_boolean(env, temp, &argv[1]), CREATE_BOOL);
    } else if (resultType == ReturnType::NUMBER) {
        int32_t temp;
        asyncContext->reserve >> temp;
        CHKRV(env, napi_create_int32(env, temp, &argv[1]), CREATE_INT32);
    } else {
        *argc = 1;
        CHKRV(env, napi_get_undefined(env, &argv[0]), GET_UNDEFINED);
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
            /**
             * After the asynchronous task is created, the asyncCallbackInfo reference count is reduced
             * to 0 destructions, so you need to add 1 to the asyncCallbackInfo reference count when the
             * asynchronous task is created, and subtract 1 from the reference count after the naked
             * pointer is converted to a pointer when the asynchronous task is executed, the reference
             * count of the smart pointer is guaranteed to be 1.
             */
            asyncContext->DecStrongRef(nullptr);
            size_t argc = 2;
            napi_value argv[2];
            getResult(asyncContext, &argc, argv);
            if (asyncContext->deferred) {
                if (asyncContext->errorCode == RET_OK) {
                    CHKRV(env, napi_resolve_deferred(env, asyncContext->deferred, argv[argc - 1]), RESOLVE_DEFERRED);
                } else {
                    CHKRV(env, napi_reject_deferred(env, asyncContext->deferred, argv[argc - 1]), REJECT_DEFERRED);
                }
            } else {
                napi_value callback = nullptr;
                CHKRV(env, napi_get_reference_value(env, asyncContext->callback, &callback), GET_REFERENCE);
                napi_value callResult = nullptr;
                CHKRV(env, napi_call_function(env, nullptr, callback, argc, argv, &callResult), CALL_FUNCTION);
            }
        },
        asyncContext.GetRefPtr(), &asyncContext->work);
    if (status != napi_ok || napi_queue_async_work(env, asyncContext->work) != napi_ok) {
        MMI_HILOGE("Create async work failed");
        asyncContext->DecStrongRef(nullptr);
    }
}

napi_value JsPointerManager::SetPointerVisible(napi_env env, bool visible, napi_value handle)
{
    CALL_DEBUG_ENTER;
    sptr<AsyncContext> asyncContext = new (std::nothrow) AsyncContext(env);
    if (asyncContext == nullptr) {
        THROWERR(env, "create AsyncContext failed");
        return nullptr;
    }

    asyncContext->errorCode = InputManager::GetInstance()->SetPointerVisible(visible);
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

napi_value JsPointerManager::IsPointerVisible(napi_env env, napi_value handle)
{
    CALL_DEBUG_ENTER;
    sptr<AsyncContext> asyncContext = new (std::nothrow) AsyncContext(env);
    if (asyncContext == nullptr) {
        THROWERR(env, "create AsyncContext failed");
        return nullptr;
    }

    bool visible = InputManager::GetInstance()->IsPointerVisible();
    asyncContext->errorCode = ERR_OK;
    asyncContext->reserve << ReturnType::BOOL << visible;

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

napi_value JsPointerManager::SetPointerLocation(napi_env env, napi_value handle, int32_t x, int32_t y)
{
    CALL_DEBUG_ENTER;
    sptr<AsyncContext> asyncContext = new (std::nothrow) AsyncContext(env);
    if (asyncContext == nullptr) {
        THROWERR(env, "create AsyncContext failed");
        return nullptr;
    }
    asyncContext->errorCode = InputManager::GetInstance()->SetPointerLocation(x, y);
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

napi_value JsPointerManager::GetPointerSpeed(napi_env env, napi_value handle)
{
    CALL_DEBUG_ENTER;
    sptr<AsyncContext> asyncContext = new (std::nothrow) AsyncContext(env);
    if (asyncContext == nullptr) {
        THROWERR(env, "Create AsyncContext failed");
        return nullptr;
    }
    int32_t pointerSpeed = InputManager::GetInstance()->GetPointerSpeed();
    if (pointerSpeed != RET_ERR) {
        asyncContext->errorCode = ERR_OK;
        asyncContext->reserve << ReturnType::NUMBER << pointerSpeed;
    }
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
} // namespace MMI
} // namespace OHOS