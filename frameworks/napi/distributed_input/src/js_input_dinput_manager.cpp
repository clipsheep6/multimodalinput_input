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

#include <map>
#include "js_input_dinput_manager.h"
#include "input_manager.h"
#include "util_napi.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JsInputDinputManager" };
const std::string CREATE_ARRAY = "napi_create_array";
const std::string CREATE_INT32 = "napi_create_int32";
const std::string CREATE_STRING = "napi_create_string";
const std::string CREATE_OBJECT = "napi_create_object";
const std::string SET_NAMED_PROPERTY = "napi_set_named_property";
const std::string SET_ELEMENT = "napi_set_element";
const std::string GET_REFERENCE = "napi_get_reference_value";
const std::string CALL_FUNCTION = "napi_call_function";
const std::string RESOLVE_DEFERRED = "napi_resolve_deferred";
const std::string REFERENCE_UNREF = "reference_unref";
std::mutex mutex_;
constexpr uint32_t EVDEV_UDEV_TAG_KEYBOARD = (1 << 1);
constexpr uint32_t EVDEV_UDEV_TAG_MOUSE = (1 << 2);
constexpr uint32_t EVDEV_UDEV_TAG_TOUCHPAD = (1 << 3);
// constexpr uint32_t EVDEV_UDEV_TAG_TOUCHSCREEN = (1 << 4);
std::map<int32_t, int32_t> deviceTypeMap = {
    {InputAbilityType::KEYBOARD, EVDEV_UDEV_TAG_KEYBOARD},
    {InputAbilityType::MOUSE, EVDEV_UDEV_TAG_MOUSE},
    {InputAbilityType::TOUCH_PAD, EVDEV_UDEV_TAG_TOUCHPAD},
};
}

napi_value JsInputDinputManager::PrepareRemoteInput(napi_env env, std::string& deviceId, napi_ref handle)
{
    auto* cb = CreateCallbackInfo<int32_t>(env, handle);
    if (cb != nullptr) {
        InputManager::GetInstance()->PrepareRemoteInput(deviceId, [cb](int32_t returnResult){
            cb->returnResult = returnResult;
            JsInputDinputManager::HandleCallBack(cb);
            });
        if (handle == nullptr) {
            return cb->promise;
        }
        
    }
    return nullptr;
}

napi_value JsInputDinputManager::UnprepareRemoteInput(napi_env env, std::string& deviceId, napi_ref handle)
{
    auto* cb = CreateCallbackInfo<int32_t>(env, handle);
    if (cb != nullptr) {
        InputManager::GetInstance()->UnprepareRemoteInput(deviceId, [cb](int32_t returnResult){
            cb->returnResult = returnResult;
            JsInputDinputManager::HandleCallBack(cb);
            });
        if (handle == nullptr) {
            return cb->promise;
        }
        
    }
    return nullptr;
}

napi_value JsInputDinputManager::StartRemoteInput(napi_env env, std::string& deviceId, std::vector<uint32_t> inputAbility, napi_ref handle)
{
    CALL_LOG_ENTER;
    auto* cb = CreateCallbackInfo<int32_t>(env, handle);
    uint32_t ability = ChangeToAbilityType(inputAbility);
    if (cb != nullptr) {
        InputManager::GetInstance()->StartRemoteInput(deviceId, ability, [cb](int32_t returnResult){
            cb->returnResult = returnResult;
            JsInputDinputManager::HandleCallBack(cb);
            });
        if (handle == nullptr) {
            return cb->promise;
        }
        
    }
    return nullptr;
}

napi_value JsInputDinputManager::StopRemoteInput(napi_env env, std::string& deviceId, std::vector<uint32_t> inputAbility, napi_ref handle)
{
    auto* cb = CreateCallbackInfo<int32_t>(env, handle);
    uint32_t ability = ChangeToAbilityType(inputAbility);
    if (cb != nullptr) {
        InputManager::GetInstance()->StopRemoteInput(deviceId, ability, [cb](int32_t returnResult){
            cb->returnResult = returnResult;
            JsInputDinputManager::HandleCallBack(cb);
            });
        if (handle == nullptr) {
            return cb->promise;
        }
        
    }
    return nullptr;
}

napi_value JsInputDinputManager::GetRemoteInputAbility(napi_env env, napi_ref handle, std::string deviceId)
{
    CALL_LOG_ENTER;
    auto* cb = CreateCallbackInfo<std::set<int32_t>>(env, handle);
    if (cb != nullptr) {
        InputManager::GetInstance()->GetRemoteInputAbility(deviceId, [cb](std::set<int32_t> returnResult){
            cb->returnResult = returnResult;
            JsInputDinputManager::HandleCallBack(cb);
        }); 
        if (handle == nullptr) {
            return cb->promise;
        }
    }
    return nullptr;
}

uint32_t JsInputDinputManager::ChangeToAbilityType(std::vector<uint32_t> abilities)
{
    uint32_t inputAbility = 0;
    for (auto item : abilities) {
        inputAbility = inputAbility | item;
    }
    return inputAbility;
}

void JsInputDinputManager::HandleCallBack(CallbackInfo<int32_t>* cb)
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    napi_value resourceName;
    napi_create_string_latin1(cb->env, "HandleCallBack", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        cb->env, nullptr, resourceName,
        [](napi_env env, void *data) {},
        [](napi_env env, napi_status status, void *data) {
            (void)status;
            auto *cbInfo = (CallbackInfo<int32_t>*)data;
            napi_value returnResult_;
            CHKRV(env, napi_create_int32(cbInfo->env, cbInfo->returnResult, &returnResult_), CREATE_INT32);
            napi_value resultObj[1] = {0};
            CHKRV(env, napi_create_object(cbInfo->env, &resultObj[0]), CREATE_OBJECT);
            CHKRV(env, napi_set_named_property(cbInfo->env, resultObj[0], "code", returnResult_), SET_NAMED_PROPERTY);
            if (cbInfo->promise != nullptr) {
                CallFunctionPromise(cbInfo->env, cbInfo->deferred, resultObj[0]);
            } else if (cbInfo->ref != nullptr) {
                CallFunctionAsync(cbInfo->env, cbInfo->ref, 1, &resultObj[0]);
            }
            MMI_HILOGI("uv_queue_work end");
            napi_delete_async_work(env, cbInfo->asyncWork);
            delete cbInfo;
        }, (void *)cb, &cb->asyncWork);
    napi_queue_async_work(cb->env, cb->asyncWork);
}

void JsInputDinputManager::HandleCallBack(CallbackInfo<std::set<int32_t>>* cb)
{
    CALL_LOG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    napi_value resourceName;
    napi_create_string_latin1(cb->env, "HandleCallBack", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        cb->env, nullptr, resourceName,
        [](napi_env env, void *data) {},
        [](napi_env env, napi_status status, void *data) {
            (void)status;
            auto *cbInfo = (CallbackInfo<std::set<int32_t>>*)data;
            napi_value resultObj[2] = {0};
            napi_get_undefined(cbInfo->env, &resultObj[0]);
            resultObj[1] = MakeInputAbilityObj(cbInfo->env, cbInfo->returnResult);
            CHKPV(resultObj);
            if (cbInfo->promise != nullptr) {
                CallFunctionPromise(cbInfo->env, cbInfo->deferred, resultObj[1]);
            } else if (cbInfo->ref != nullptr) {
                CallFunctionAsync(cbInfo->env, cbInfo->ref, 2, &resultObj[0]);
            }
            MMI_HILOGI("uv_queue_work end");
            napi_delete_async_work(env, cbInfo->asyncWork);
            delete cbInfo;
        }, (void *)cb, &cb->asyncWork);
    napi_queue_async_work(cb->env, cb->asyncWork);
}

napi_value JsInputDinputManager::MakeInputAbilityObj(napi_env env, std::set<int32_t> types)
{
    for (auto deviceType : types) {
        MMI_HILOGI("deviceType : %{public}d", deviceType);
    }
    napi_value returnResult;
    napi_value resultArry;
    int32_t i = 0;
    CHKRP(env, napi_create_array(env, &resultArry), CREATE_ARRAY);
    for (auto deviceType : types) {
        MMI_HILOGI("deviceType : %{public}d", deviceType);
        for (auto item : deviceTypeMap) {
            MMI_HILOGI("deviceType : %{public}d, item.second : %{public}d", deviceType, item.second);
            if (deviceType & item.second) {
                CHKRP(env, napi_create_int32(env, item.first, &returnResult), CREATE_INT32);
                CHKRP(env, napi_set_element(env, resultArry, i++, returnResult), SET_ELEMENT);
                break;
            }
        }
    }
    napi_value resultObj;
    CHKRP(env, napi_create_object(env, &resultObj), CREATE_OBJECT);
    CHKRP(env, napi_set_named_property(env, resultObj, "inputAbility", resultArry), SET_NAMED_PROPERTY);
    return resultObj;
}

void JsInputDinputManager::CallFunctionPromise(napi_env env, napi_deferred deferred, napi_value object)
{
    CALL_LOG_ENTER;
    CHKRV(env, napi_resolve_deferred(env, deferred, object), RESOLVE_DEFERRED);
}

void JsInputDinputManager::CallFunctionAsync(napi_env env, napi_ref handleRef, size_t count, napi_value* object)
{
    CALL_LOG_ENTER;
    napi_value handler;
    CHKRV(env, napi_get_reference_value(env, handleRef, &handler), GET_REFERENCE);
    napi_value result;
    if (handler != nullptr) {
        CHKRV(env, napi_call_function(env, nullptr, handler, count, object, &result), CALL_FUNCTION);
    } else {
        MMI_HILOGI("handler is nullptr");
    }
    uint32_t refCount {0};
    CHKRV(env, napi_reference_unref(env, handleRef, &refCount), REFERENCE_UNREF);
}
// void JsInputDinputManager::ResetEnv()
// {
//     env_ = nullptr;
// }
} // namespace MMI
} // namespace OHOS