/*
 * @Descripttion:
 * @version:
 * @Author: sueRimn
 * @Date: 2022-02-11 15:09:02
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-02-25 16:59:25
 */
/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "js_register_module.h"
#include <cinttypes>
#include "input_manager.h"

namespace OHOS {
namespace MMI {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JSRegisterMoudle" };
constexpr uint32_t ARGV_FIRST = 0;
constexpr uint32_t ARGV_SECOND = 1;
constexpr uint32_t ARGV_THIRD = 2;
constexpr uint32_t ARGC_NUM_1 = 1;
constexpr uint32_t ARGC_NUM_2 = 2;
constexpr uint32_t ARGC_NUM_3 = 3;
constexpr uint32_t INIT_REF_COUNT = 1;
constexpr size_t PARAMETER_NUM = 1;
constexpr size_t MAX_STRING_LEN = 1024;

static napi_value HideMouse(napi_env env, napi_callback_info info)
{
    MMI_LOGI("HideMouse begin");
    napi_ref handlerRef {nullptr};
    JsRegisterModule::GetParameter(env, info, handlerRef);
    auto *instance = InputManager::GetInstance();
    int32_t ret = instance->HideMouse([](bool returnResult) {
    });
    napi_value result = nullptr;
    if (ret != 0) {
        return result;
    }
    napi_get_undefined(env, &result);
    MMI_LOGI("HideMouse end");
    return result;

}

static napi_value ShowMouse(napi_env env, napi_callback_info info)
{
    MMI_LOGI("ShowMouse begin");
    napi_ref handlerRef {nullptr};
    JsRegisterModule::GetParameter(env, info, handlerRef);
    auto *instance = InputManager::GetInstance();
    int32_t ret = instance->ShowMouse([](bool returnResult) {
    });
    napi_value result = nullptr;
    if (ret != 0) {
        return result;
    }
    napi_get_undefined(env, &result);
    MMI_LOGI("ShowMouse end");
    return result;
}

static napi_value PrepareRemoteInput(napi_env env, napi_callback_info info)
{
    MMI_LOGI("PrepareRemoteInput begin");
    std::string deviceId;
    napi_ref handlerRef {nullptr};
    JsRegisterModule::GetParameter(env, info, deviceId, handlerRef);
    CallbackInfo<int32_t>* cb = new CallbackInfo<int32_t>;
    cb->env = env;

    uv_loop_s* loop {nullptr};
    if (napi_get_uv_event_loop(env, &loop) != napi_ok) {
        napi_throw_error(env, nullptr, "PrepareRemoteInput: call to napi_get_uv_event_loop failed");
    }
    cb->loop = loop;
    cb->handleRef = handlerRef;
    uv_work_t* work = new uv_work_t;
    work->data = (void*)cb;
    auto *instance = InputManager::GetInstance();
    instance->PrepareRemoteInput(deviceId, [work](int32_t returnResult) {
        JsRegisterModule::HandleCallBack(work, returnResult);
    });
    MMI_LOGI("PrepareRemoteInput end");
    return nullptr;
}

static napi_value UnprepareRemoteInput(napi_env env, napi_callback_info info)
{
    MMI_LOGI("UnprepareRemoteInput begin");
    std::string deviceId;
    napi_ref handlerRef {nullptr};
    JsRegisterModule::GetParameter(env, info, deviceId, handlerRef);

    CallbackInfo<int32_t>* cb = new CallbackInfo<int32_t>;
    cb->env = env;

    uv_loop_s* loop {nullptr};
    if (napi_get_uv_event_loop(env, &loop) != napi_ok) {
        napi_throw_error(env, nullptr, "UnprepareRemoteInput: call to napi_get_uv_event_loop failed");
    }
    cb->loop = loop;
    cb->handleRef = handlerRef;
    uv_work_t* work = new uv_work_t;
    work->data = (void*)cb;
    auto *instance = InputManager::GetInstance();
    instance->UnprepareRemoteInput(deviceId, [work](int32_t returnResult) {
        JsRegisterModule::HandleCallBack(work, returnResult);
    });
    MMI_LOGI("UnprepareRemoteInput end");
    return nullptr;
}

static napi_value GetVirtualDeviceIdListAsync(napi_env env, napi_callback_info info)
{
    MMI_LOGI("begin");
    napi_ref handlerRef {nullptr};
    JsRegisterModule::GetParameter(env, info, handlerRef);
    CallbackInfo<std::vector<int32_t>>* cb = new CallbackInfo<std::vector<int32_t>>;
    cb->env = env;

    uv_loop_s* loop {nullptr};
    if (napi_get_uv_event_loop(env, &loop) != napi_ok) {
        napi_throw_error(env, nullptr, "call to napi_get_uv_event_loop failed");
    }
    cb->loop = loop;
    cb->handleRef = handlerRef;
    uv_work_t* work = new uv_work_t;
    work->data = (void*)cb;
    auto *instance = InputManager::GetInstance();
    MMI_LOGI("GetVirtualDeviceIdListAsync InputManager begin");
    instance->GetVirtualDeviceIdListAsync([work](std::vector<int32_t> returnResult) {
        auto callbackInfo = (CallbackInfo<std::vector<int32_t>>*)work->data;
        callbackInfo->returnResult = returnResult;
        uv_queue_work(callbackInfo->loop, work, [](uv_work_t *work) {}, [](uv_work_t *work, int32_t status) {
            MMI_LOGI("uv_queue_work begin");
            auto* cbInfo = (CallbackInfo<std::vector<int32_t>>*)work->data;
            napi_env env = cbInfo->env;
            napi_ref handleRef = cbInfo->handleRef;
            auto returnResult = cbInfo->returnResult;
            delete cbInfo;
            delete work;
            cbInfo = nullptr;
            work = nullptr;

            napi_value typeArr;
            napi_create_array(env, &typeArr);
            napi_value returnResult_;
            for (size_t i = 0; i < returnResult.size(); i++) {
                napi_create_int32(env, returnResult[i], &returnResult_);
                napi_set_element(env, typeArr, i, returnResult_);
            }
            JsRegisterModule::CallFunction(env, handleRef, typeArr);

            MMI_LOGI("uv_queue_work end");
        });
    });
    MMI_LOGI("GetVirtualDeviceIdListAsync end");
    return nullptr;
}


static napi_value GetVirtualDeviceAsync(napi_env env, napi_callback_info info)
{
    MMI_LOGI("begin");
    int32_t deviceId;
    napi_ref handlerRef {nullptr};
    JsRegisterModule::GetParameter(env, info, deviceId, handlerRef);
    auto* cb = new CallbackInfo<std::shared_ptr<InputDeviceImpl::InputDeviceInfo>>();
    cb->env = env;

    uv_loop_s* loop {nullptr};
    if (napi_get_uv_event_loop(env, &loop) != napi_ok) {
        napi_throw_error(env, nullptr, "call to napi_get_uv_event_loop failed");
    }
    cb->loop = loop;
    cb->handleRef = handlerRef;
    uv_work_t* work = new uv_work_t;
    work->data = (void*)cb;
    auto *ins = InputManager::GetInstance();
    ins->GetVirtualDeviceAsync(deviceId, [work](std::shared_ptr<InputDeviceImpl::InputDeviceInfo> returnResult) {
        auto callbackInfo = (CallbackInfo<std::shared_ptr<InputDeviceImpl::InputDeviceInfo>>*)work->data;
        callbackInfo->returnResult = returnResult;
        uv_queue_work(callbackInfo->loop, work, [](uv_work_t *work) {}, [](uv_work_t *work, int32_t status) {
            MMI_LOGI("uv_queue_work begin");
            auto* cbInfo = (CallbackInfo<std::shared_ptr<InputDeviceImpl::InputDeviceInfo>>*)work->data;
            napi_env env = cbInfo->env;
            napi_ref handleRef = cbInfo->handleRef;
            auto returnResult = cbInfo->returnResult;
            delete cbInfo;
            delete work;
            cbInfo = nullptr;
            work = nullptr;

            napi_value object;

            napi_create_object(env, &object);
            JsRegisterModule::SetValueInt32(env, "id", returnResult->id, object);
            JsRegisterModule::SetValueUtf8String(env, "name", returnResult->name, object);
            JsRegisterModule::SetValueInt32(env, "devcieType", returnResult->devcieType, object);
            JsRegisterModule::CallFunction(env, handleRef, object);
            MMI_LOGI("uv_queue_work end");
        });
    });
    MMI_LOGI("end");
    return nullptr;
}

static napi_value GetAllNodeDeviceInfo(napi_env env, napi_callback_info info)
{
    MMI_LOGI("begin");
    napi_ref handlerRef {nullptr};

    JsRegisterModule::GetParameter(env, info, handlerRef);
    CallbackInfo<std::vector<std::string>>* cb = new CallbackInfo<std::vector<std::string>>;
    cb->env = env;

    uv_loop_s* loop {nullptr};
    if (napi_get_uv_event_loop(env, &loop) != napi_ok) {
        napi_throw_error(env, nullptr, "call to napi_get_uv_event_loop failed");
    }
    cb->loop = loop;
    cb->handleRef = handlerRef;
    uv_work_t* work = new uv_work_t;
    work->data = (void*)cb;
    auto *instance = InputManager::GetInstance();
    instance->GetAllNodeDeviceInfo([work](std::vector<std::string> returnResult) {
        auto callbackInfo = (CallbackInfo<std::vector<std::string>>*)work->data;
        callbackInfo->returnResult = returnResult;
        uv_queue_work(callbackInfo->loop, work, [](uv_work_t *work) {}, [](uv_work_t *work, int32_t status) {
            MMI_LOGI("uv_queue_work begin");
            auto cbInfo = (CallbackInfo<std::vector<std::string>>*)work->data;
            napi_env env = cbInfo->env;
            napi_ref handleRef = cbInfo->handleRef;
            auto returnResult = cbInfo->returnResult;
            delete cbInfo;
            delete work;
            cbInfo = nullptr;
            work = nullptr;

            napi_value typeArr;
            napi_create_array(env, &typeArr);
            napi_value returnResult_;
            for (size_t i = 0; i < returnResult.size(); i++) {
                napi_create_string_utf8(env, returnResult[i].c_str(), NAPI_AUTO_LENGTH, &returnResult_);
                napi_set_element(env, typeArr, i, returnResult_);
            }
            JsRegisterModule::CallFunction(env, handleRef, typeArr);
            MMI_LOGI("uv_queue_work end");
        });
    });
    MMI_LOGI("end");
    return nullptr;
}

static napi_value GetMouseLocation(napi_env env, napi_callback_info info)
{
    MMI_LOGI("GetMouseLocation begin");
    napi_ref handlerRef {nullptr};
    JsRegisterModule::GetParameter(env, info, handlerRef);
    auto* cb = new CallbackInfo<std::shared_ptr<DMouseLocation>>();
    cb->env = env;

    uv_loop_s* loop {nullptr};
    if (napi_get_uv_event_loop(env, &loop) != napi_ok) {
        napi_throw_error(env, nullptr, "call to napi_get_uv_event_loop failed");
    }
    cb->loop = loop;
    cb->handleRef = handlerRef;
    uv_work_t* work = new uv_work_t;
    work->data = (void*)cb;
    auto *instance = InputManager::GetInstance();
    instance->GetMouseLocation([work](std::shared_ptr<DMouseLocation> returnResult) {
        auto callbackInfo = (CallbackInfo<std::shared_ptr<DMouseLocation>>*)work->data;
        callbackInfo->returnResult = returnResult;
        uv_queue_work(callbackInfo->loop, work, [](uv_work_t *work) {}, [](uv_work_t *work, int32_t status) {
            MMI_LOGI("uv_queue_work begin");
            auto* cbInfo = (CallbackInfo<std::shared_ptr<DMouseLocation>>*)work->data;
            napi_env env = cbInfo->env;
            napi_ref handleRef = cbInfo->handleRef;
            auto returnResult = cbInfo->returnResult;
            delete cbInfo;
            delete work;
            cbInfo = nullptr;
            work = nullptr;

            napi_value obj;
            napi_create_object(env, &obj);
            JsRegisterModule::SetValueInt32(env, "globleX", returnResult->globalX, obj);
            JsRegisterModule::SetValueInt32(env, "globleY", returnResult->globalY, obj);
            JsRegisterModule::SetValueInt32(env, "dx", returnResult->dx, obj);
            JsRegisterModule::SetValueInt32(env, "dy", returnResult->dy, obj);
            JsRegisterModule::SetValueInt32(env, "displayId", returnResult->displayId, obj);
            JsRegisterModule::SetValueInt32(env, "logicalDisplayWidth", returnResult->logicalDisplayWidth, obj);
            JsRegisterModule::SetValueInt32(env, "logicalDisplayHeight", returnResult->logicalDisplayHeight, obj);
            JsRegisterModule::SetValueInt32(env, "logicalDisplayTopLeftX", returnResult->logicalDisplayTopLeftX, obj);
            JsRegisterModule::SetValueInt32(env, "logicalDisplayTopLeftY", returnResult->logicalDisplayTopLeftY, obj);
            JsRegisterModule::CallFunction(env, handleRef, obj);
            MMI_LOGI("uv_queue_work end");
        });
    });
    MMI_LOGI("GetMouseLocation end");
    return nullptr;
}

static napi_value SubscribeKeyEvent(napi_env env, napi_callback_info info)
{
    MMI_LOGI("SubscribeKeyEvent begin");
    napi_ref handlerRef {nullptr};
    int32_t preKey;
    int32_t finalKey;
    JsRegisterModule::GetParameter(env, info, preKey, finalKey, handlerRef);
    CallbackInfo<int32_t>* cb = new CallbackInfo<int32_t>;
    cb->env = env;

    uv_loop_s* loop {nullptr};
    if (napi_get_uv_event_loop(env, &loop) != napi_ok) {
        napi_throw_error(env, nullptr, "SubscribeKeyEvent: call to napi_get_uv_event_loop failed");
    }
    cb->loop = loop;
    cb->handleRef = handlerRef;
    uv_work_t* work = new uv_work_t;
    work->data = (void*)cb;
    auto *instance = InputManager::GetInstance();
    std::shared_ptr<KeyOption> keyOption = std::make_shared<KeyOption>();
    std::set<int32_t> preKeys;
    preKeys.insert(preKey);
    keyOption->SetPreKeys(preKeys);
    keyOption->SetFinalKeyDown(true);
    keyOption->SetFinalKey(finalKey);
    keyOption->SetFinalKeyDownDuration(0);
    instance->SubscribeKeyEvent(keyOption, [work](std::shared_ptr<KeyEvent> returnResult) {
        JsRegisterModule::HandleCallBack(work, true);
    });
    MMI_LOGI("SubscribeKeyEvent end");
    return nullptr;
}

static napi_value StartRemoteInput(napi_env env, napi_callback_info info)
{
    MMI_LOGI("StartRemoteInput begin");
    std::string deviceId;
    napi_ref handlerRef {nullptr};
    JsRegisterModule::GetParameter(env, info, deviceId, handlerRef);
    CallbackInfo<int32_t>* cb = new CallbackInfo<int32_t>;
    cb->env = env;

    uv_loop_s* loop {nullptr};
    if (napi_get_uv_event_loop(env, &loop) != napi_ok) {
        napi_throw_error(env, nullptr, "StartRemoteInput: call to napi_get_uv_event_loop failed");
    }
    cb->loop = loop;
    cb->handleRef = handlerRef;
    uv_work_t* work = new uv_work_t;
    work->data = (void*)cb;
    auto *instance = InputManager::GetInstance();
    instance->StartRemoteInput(deviceId, [work](int32_t returnResult) {
        JsRegisterModule::HandleCallBack(work, returnResult);
    });
    MMI_LOGI("StartRemoteInput end");
    return nullptr;
}

static napi_value StopRemoteInput(napi_env env, napi_callback_info info)
{
    MMI_LOGI("StopRemoteInput begin");
    std::string deviceId;
    napi_ref handlerRef {nullptr};
    JsRegisterModule::GetParameter(env, info, deviceId, handlerRef);
    CallbackInfo<int32_t>* cb = new CallbackInfo<int32_t>;
    cb->env = env;

    uv_loop_s* loop {nullptr};
    if (napi_get_uv_event_loop(env, &loop) != napi_ok) {
        napi_throw_error(env, nullptr, "StopRemoteInput: call to napi_get_uv_event_loop failed");
    }
    cb->loop = loop;
    cb->handleRef = handlerRef;
    uv_work_t* work = new uv_work_t;
    work->data = (void*)cb;
    auto *instance = InputManager::GetInstance();
    instance->StopRemoteInput(deviceId, [work](int32_t returnResult) {
        JsRegisterModule::HandleCallBack(work, returnResult);
    });
    MMI_LOGI("StopRemoteInput end");
    return nullptr;
}

static napi_value SimulateCrossLocation(napi_env env, napi_callback_info info)
{
    MMI_LOGI("SimulateCrossLocation begin");
    int32_t mouseX;
    int32_t mouseY;
    napi_ref handlerRef {nullptr};
    JsRegisterModule::GetParameter(env, info, mouseX, mouseY, handlerRef);
    CallbackInfo<int32_t>* cb = new CallbackInfo<int32_t>;
    cb->env = env;

    uv_loop_s* loop {nullptr};
    if (napi_get_uv_event_loop(env, &loop) != napi_ok) {
        napi_throw_error(env, nullptr, "SimulateCrossLocation: call to napi_get_uv_event_loop failed");
    }
    cb->loop = loop;
    cb->handleRef = handlerRef;
    uv_work_t* work = new uv_work_t;
    work->data = (void*)cb;
    auto *instance = InputManager::GetInstance();
    instance->SimulateCrossLocation(mouseX, mouseY, [work](int32_t returnResult) {
        JsRegisterModule::HandleCallBack(work, returnResult);
    });
    MMI_LOGI("SimulateCrossLocation end");
    return nullptr;
}

void JsRegisterModule::CallFunction(napi_env env, napi_ref& handleRef, napi_value& object)
{
    napi_value handler;
    napi_get_reference_value(env, handleRef, &handler);
    napi_value result;
    if (handler != nullptr) {
        napi_call_function(env, nullptr, handler, PARAMETER_NUM, &object, &result);
    } else {
        MMI_LOGI("handler is nullptr");
    }
    uint32_t refCount {0};
    if (napi_reference_unref(env, handleRef, &refCount) != napi_ok) {
        napi_throw_error(env, nullptr, "uv_queue_work call to napi_reference_unref failed");
        return;
    }
}

void JsRegisterModule::SetValueInt32(const napi_env &env, const std::string &fieldStr, const int32_t intValue,
    napi_value &result)
{
    napi_value value = nullptr;
    napi_create_int32(env, intValue, &value);
    napi_set_named_property(env, result, fieldStr.c_str(), value);
}

void JsRegisterModule::SetValueUtf8String(const napi_env &env, const std::string &fieldStr, const std::string &str,
    napi_value &result)
{
    napi_value value = nullptr;
    napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, fieldStr.c_str(), value);
}

void JsRegisterModule::GetParameter(napi_env env, napi_callback_info info, std::string& first, napi_ref& second)
{
    MMI_LOGI("begin");
    size_t argc = ARGC_NUM_2;
    napi_value argv[ARGC_NUM_2];
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_get_cb_info failed");
    }
    if (argc != ARGC_NUM_2) {
        napi_throw_error(env, nullptr, "GetParameter: requires 2 parameter");
    }

    napi_valuetype valueType {napi_undefined};
    if (napi_typeof(env, argv[ARGV_FIRST], &valueType) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_typeof failed");
    }
    if (valueType != napi_string) {
        napi_throw_error(env, nullptr, "GetParameter: The first parameter is not a string");
    }
    if (napi_typeof(env, argv[ARGV_SECOND], &valueType) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_typeof failed");
    }
    if (valueType != napi_function) {
        napi_throw_error(env, nullptr, "GetParameter: The second parameter is not a function");
    }
    char eventName[MAX_STRING_LEN] = { 0 };
    size_t typeLen = 0;
    napi_get_value_string_utf8(env, argv[ARGV_FIRST], eventName, MAX_STRING_LEN - 1, &typeLen);
    first = eventName;
    MMI_LOGI("GetParameter deviceId = %{public}s", eventName);

    MMI_LOGI("GetParameter deviceId = %{public}s", first.c_str());

    if (napi_create_reference(env, argv[ARGV_SECOND], INIT_REF_COUNT, &second) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_create_reference failed");
    }
}

void JsRegisterModule::GetParameter(napi_env env, napi_callback_info info, int32_t& first, napi_ref& second)
{
    MMI_LOGI("begin");
    size_t argc = ARGC_NUM_2;
    napi_value argv[ARGC_NUM_2];
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_get_cb_info failed");
    }
    if (argc != ARGC_NUM_2) {
        napi_throw_error(env, nullptr, "GetParameter: requires 2 parameter");
    }

    napi_valuetype valueType {napi_undefined};
    if (napi_typeof(env, argv[ARGV_FIRST], &valueType) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_typeof failed");
    }
    if (valueType != napi_number) {
        napi_throw_error(env, nullptr, "GetParameter: The first parameter is not a string");
    }
    if (napi_typeof(env, argv[ARGV_SECOND], &valueType) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_typeof failed");
    }
    if (valueType != napi_function) {
        napi_throw_error(env, nullptr, "GetParameter: The second parameter is not a function");
    }
    if (napi_get_value_int32(env, argv[ARGV_FIRST], &first) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_get_value_int32 failed");
    }
    MMI_LOGI("GetParameter(napi_create_reference) begin");
    if (napi_create_reference(env, argv[ARGV_SECOND], INIT_REF_COUNT, &second) != napi_ok) {
        napi_throw_error(env, nullptr, "PrepareRemoteInput: call to napi_create_reference failed");
    }
}

void JsRegisterModule::GetParameter(napi_env env, napi_callback_info info,
    int32_t& first, int32_t& second, napi_ref& third)
{
    MMI_LOGI("begin");
    size_t argc = ARGC_NUM_3;
    napi_value argv[ARGC_NUM_3];
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_get_cb_info failed");
    }
    if (argc != ARGC_NUM_3) {
        napi_throw_error(env, nullptr, "GetParameter: requires 2 parameter");
    }

    napi_valuetype valueType {napi_undefined};
    if (napi_typeof(env, argv[ARGV_FIRST], &valueType) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_typeof failed");
    }
    if (valueType != napi_number) {
        napi_throw_error(env, nullptr, "GetParameter: The first parameter is not a string");
    }
    if (napi_typeof(env, argv[ARGV_SECOND], &valueType) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_typeof failed");
    }
    if (valueType != napi_number) {
        napi_throw_error(env, nullptr, "GetParameter: The second parameter is not a string");
    }

    if (napi_typeof(env, argv[ARGV_THIRD], &valueType) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_typeof failed");
    }
    if (valueType != napi_function) {
        napi_throw_error(env, nullptr, "GetParameter: The third parameter is not a function");
    }
    if (napi_get_value_int32(env, argv[ARGV_FIRST], &first) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_get_value_int32 failed");
    }
    if (napi_get_value_int32(env, argv[ARGV_SECOND], &second) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_get_value_int32 failed");
    }
    if (napi_create_reference(env, argv[ARGV_THIRD], INIT_REF_COUNT, &third) != napi_ok) {
        napi_throw_error(env, nullptr, "call to napi_create_reference failed");
    }
}

void JsRegisterModule::GetParameter(napi_env env, napi_callback_info info, napi_ref& first)
{
    MMI_LOGI("begin");
    size_t argc = ARGC_NUM_1;
    napi_value argv[ARGC_NUM_1];
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_get_cb_info failed");
    }
    MMI_LOGI("napi_get_cb_info end");

    if (argc != ARGC_NUM_1) {
        napi_throw_error(env, nullptr, "GetParameter: requires 1 parameter");
    }

    napi_valuetype valueType {napi_undefined};
    if (napi_typeof(env, argv[ARGV_FIRST], &valueType) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_typeof failed");
    }
    if (valueType != napi_function) {
        napi_throw_error(env, nullptr, "GetParameter: The first parameter is not a function");
    }
    MMI_LOGI("napi_create_reference begin");
    if (napi_create_reference(env, argv[ARGV_FIRST], INIT_REF_COUNT, &first) != napi_ok) {
        napi_throw_error(env, nullptr, "GetParameter: call to napi_create_reference failed");
    }
}

void JsRegisterModule::HandleCallBack(uv_work_t* work, bool returnResult)
{
    MMI_LOGI("begin");
    auto callbackInfo = (CallbackInfo<bool>*)work->data;
    callbackInfo->returnResult = returnResult;
    uv_queue_work(callbackInfo->loop, work, [](uv_work_t *work) {}, [](uv_work_t *work, int32_t status) {
        MMI_LOGI("uv_queue_work begin");
        CallbackInfo<bool>* cbInfo = (CallbackInfo<bool>*)work->data;
        napi_env env = cbInfo->env;
        napi_ref handleRef = cbInfo->handleRef;
        auto returnResult = cbInfo->returnResult;
        delete cbInfo;
        delete work;
        cbInfo = nullptr;
        work = nullptr;

        napi_value returnResult_;
        if (napi_create_int32(env, returnResult, &returnResult_) != napi_ok) {
            napi_throw_error(env, nullptr, "uv_queue_work call to napi_create_int32 failed");
            return;
        }
        napi_value handler;
        if (napi_get_reference_value(env, handleRef, &handler) != napi_ok) {
            napi_throw_error(env, nullptr, "uv_queue_work call to napi_get_reference_value failed");
            return;
        }

        napi_value result;
        if (napi_call_function(env, nullptr, handler, PARAMETER_NUM, &returnResult_, &result) != napi_ok) {
            napi_throw_error(env, nullptr, "uv_queue_work call to napi_call_function failed");
            return;
        }
        uint32_t refCount {0};
        if (napi_reference_unref(env, handleRef, &refCount) != napi_ok) {
            napi_throw_error(env, nullptr, "uv_queue_work call to napi_reference_unref failed");
            return;
        }
        MMI_LOGI("uv_queue_work end");
        });
}

void JsRegisterModule::HandleCallBack(uv_work_t* work, int32_t returnResult)
{
    MMI_LOGI("begin");
    auto callbackInfo = (CallbackInfo<int32_t>*)work->data;
    callbackInfo->returnResult = returnResult;
    uv_queue_work(callbackInfo->loop, work, [](uv_work_t *work) {}, [](uv_work_t *work, int32_t status) {
        MMI_LOGI("uv_queue_work begin");
        CallbackInfo<int32_t>* cbInfo = (CallbackInfo<int32_t>*)work->data;
        napi_env env = cbInfo->env;
        napi_ref handleRef = cbInfo->handleRef;
        auto returnResult = cbInfo->returnResult;
        delete cbInfo;
        delete work;
        cbInfo = nullptr;
        work = nullptr;

        napi_value returnResult_;
        if (napi_create_int32(env, returnResult, &returnResult_) != napi_ok) {
            napi_throw_error(env, nullptr, "uv_queue_work call to napi_create_int32 failed");
            return;
        }
        napi_value handler;
        if (napi_get_reference_value(env, handleRef, &handler) != napi_ok) {
            napi_throw_error(env, nullptr, "uv_queue_work call to napi_get_reference_value failed");
            return;
        }

        napi_value result;
        if (napi_call_function(env, nullptr, handler, PARAMETER_NUM, &returnResult_, &result) != napi_ok) {
            napi_throw_error(env, nullptr, "uv_queue_work call to napi_call_function failed");
            return;
        }
        uint32_t refCount {0};
        if (napi_reference_unref(env, handleRef, &refCount) != napi_ok) {
            napi_throw_error(env, nullptr, "uv_queue_work call to napi_reference_unref failed");
            return;
        }
        MMI_LOGI("uv_queue_work end");
        });
}

EXTERN_C_START
static napi_value MmiInputDInputInit(napi_env env, napi_value exports)
{
    MMI_LOGI("MmiInputDInputInit: enter");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("hideMouse", HideMouse),
        DECLARE_NAPI_FUNCTION("prepareRemoteInput", PrepareRemoteInput),
        DECLARE_NAPI_FUNCTION("unprepareRemoteInput", UnprepareRemoteInput),
        DECLARE_NAPI_FUNCTION("showMouse", ShowMouse),
        DECLARE_NAPI_FUNCTION("getVirtualDeviceAsync", GetVirtualDeviceAsync),
        DECLARE_NAPI_FUNCTION("getVirtualDeviceIdListAsync", GetVirtualDeviceIdListAsync),
        DECLARE_NAPI_FUNCTION("getAllNodeDeviceInfo", GetAllNodeDeviceInfo),
        DECLARE_NAPI_FUNCTION("getMouseLocation", GetMouseLocation),
        DECLARE_NAPI_FUNCTION("startRemoteInput", StartRemoteInput),
        DECLARE_NAPI_FUNCTION("stopRemoteInput", StopRemoteInput),
        DECLARE_NAPI_FUNCTION("subscribeKeyEvent", SubscribeKeyEvent),
        DECLARE_NAPI_FUNCTION("simulateCrossLocation", SimulateCrossLocation)

    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    MMI_LOGI("MmiInputDInputInit: success");
    return exports;
}
EXTERN_C_END

static napi_module mmiInputDInputModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = MmiInputDInputInit,
    .nm_modname = "inputDInput",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&mmiInputDInputModule);
}
}
}
