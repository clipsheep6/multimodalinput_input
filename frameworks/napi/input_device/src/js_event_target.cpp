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

#include "js_event_target.h"

#include <map>

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JsEventTarget" };
constexpr uint32_t EVDEV_UDEV_TAG_KEYBOARD = (1 << 1);
constexpr uint32_t EVDEV_UDEV_TAG_MOUSE = (1 << 2);
constexpr uint32_t EVDEV_UDEV_TAG_TOUCHPAD = (1 << 3);
constexpr uint32_t EVDEV_UDEV_TAG_TOUCHSCREEN = (1 << 4);
constexpr uint32_t EVDEV_UDEV_TAG_JOYSTICK = (1 << 6);
constexpr uint32_t EVDEV_UDEV_TAG_TRACKBALL = (1 << 10);

JsEventTarget::DeviceType g_deviceType[] = {
    {"keyboard", EVDEV_UDEV_TAG_KEYBOARD},
    {"mouse", EVDEV_UDEV_TAG_MOUSE},
    {"touchpad", EVDEV_UDEV_TAG_TOUCHPAD},
    {"touchscreen", EVDEV_UDEV_TAG_TOUCHSCREEN},
    {"joystick", EVDEV_UDEV_TAG_JOYSTICK},
    {"trackball", EVDEV_UDEV_TAG_TRACKBALL},
};
} // namespace

napi_env JsEventTarget::env_ = nullptr;
static std::map<int32_t, JsUtil::CallbackInfo*> callback_ {};
int32_t JsEventTarget::userData_ = 0;

void JsEventTarget::CallIdsAsyncWork(napi_env env, napi_status status, void* data)
{
    CALL_LOG_ENTER;
    CHKPV(data);
    JsUtil::CallbackInfo cbTemp(env);
    JsUtil jsUtil;
    jsUtil.GetCallbackInfo(data, cbTemp);

    napi_value arr = nullptr;
    CHKRV(env, napi_create_array(env, &arr), "napi_create_array");

    uint32_t index = 0;
    napi_value value = nullptr;
    for (const auto &item : cbTemp.data.ids) {
        CHKRV(env, napi_create_int32(env, item, &value), "napi_create_int32");
        CHKRV(env, napi_set_element(env, arr, index, value), "napi_set_element");
        index++;
    }

    napi_value handlerTemp = nullptr;
    CHKRV(env, napi_get_reference_value(env, cbTemp.ref, &handlerTemp), "napi_get_reference_value");
    napi_value result = nullptr;
    CHKRV(env, napi_call_function(env, nullptr, handlerTemp, 1, &arr, &result), "napi_call_function");
}

void JsEventTarget::CallIdsPromiseWork(napi_env env, napi_status status, void* data)
{
    CALL_LOG_ENTER;
    CHKPV(data);
    JsUtil::CallbackInfo cbTemp(env);
    JsUtil jsUtil;
    jsUtil.GetCallbackInfo(data, cbTemp);

    napi_value arr = nullptr;
    CHKRV(env, napi_create_array(env, &arr), "napi_create_array");

    napi_value value = nullptr;
    uint32_t index = 0;
    for (const auto &item : cbTemp.data.ids) {
        CHKRV(env, napi_create_int32(env, item, &value), "napi_create_int32");
        CHKRV(env, napi_set_element(env, arr, index, value), "napi_set_element");
        index++;
    }

    CHKRV(env, napi_resolve_deferred(env, cbTemp.deferred, arr), "napi_resolve_deferred");
}

void JsEventTarget::EmitJsIds(int32_t userData, std::vector<int32_t> ids)
{
    CALL_LOG_ENTER;
    if (CheckEnv(env_)) {
        MMI_LOGE("env_ is nullptr");
        return;
    }
    auto iter = callback_.find(userData);
    if (iter == callback_.end()) {
        MMI_LOGE("Failed to search for userData");
        return;
    }
    iter->second->data.ids = ids;

    napi_value resourceName = nullptr;
    CHKRV(env_, napi_create_string_latin1(env_, "InputDeviceIdsAsync", NAPI_AUTO_LENGTH, &resourceName),
        "napi_create_string_latin1");

    if (iter->second->ref == nullptr) {
        CHKRV(env_, napi_create_async_work(env_, nullptr, resourceName, [](napi_env env, void *data) {},
                                           CallIdsPromiseWork, iter->second, &(iter->second->asyncWork)),
                                           "napi_create_async_work");
    } else {
        CHKRV(env_, napi_create_async_work(env_, nullptr, resourceName, [](napi_env env, void *data) {},
                                           CallIdsAsyncWork, iter->second, &(iter->second->asyncWork)),
                                           "napi_create_async_work");
    }
    CHKRV(env_, napi_queue_async_work(env_, iter->second->asyncWork), "napi_queue_async_work");
}

void JsEventTarget::CallDevAsyncWork(napi_env env, napi_status status, void* data)
{
    CALL_LOG_ENTER;
    CHKPV(data);
    JsUtil::CallbackInfo cbTemp(env);
    JsUtil jsUtil;
    jsUtil.GetCallbackInfo(data, cbTemp);
    CHKPV(cbTemp.data.device);

    napi_value id = nullptr;
    CHKRV(env, napi_create_int32(env, cbTemp.data.device->id, &id), "napi_create_int32");
    napi_value name = nullptr;
    CHKRV(env, napi_create_string_utf8(env, (cbTemp.data.device->name).c_str(), NAPI_AUTO_LENGTH, &name),
        "napi_create_string_utf8");

    napi_value object = nullptr;
    CHKRV(env, napi_create_object(env, &object), "napi_create_object");
    CHKRV(env, napi_set_named_property(env, object, "id", id), "napi_set_named_property");
    CHKRV(env, napi_set_named_property(env, object, "name", name), "napi_set_named_property");

    uint32_t types = cbTemp.data.device->devcieType;
    std::vector<std::string> sources;
    for (const auto & item : g_deviceType) {
        if (types & item.typeBit) {
            sources.push_back(item.deviceTypeName);
        }
    }
    napi_value devSources = nullptr;
    CHKRV(env, napi_create_array(env, &devSources), "napi_create_array");
    uint32_t index = 0;
    napi_value value = nullptr;
    for (const auto &item : sources) {
        CHKRV(env, napi_create_string_utf8(env, item.c_str(), NAPI_AUTO_LENGTH, &value), "napi_create_string_utf8");
        CHKRV(env, napi_set_element(env, devSources, index, value), "napi_set_element");
    }
    CHKRV(env, napi_set_named_property(env, object, "sources", devSources), "napi_set_named_property");

    napi_value axisRanges = nullptr;
    CHKRV(env, napi_create_array(env, &axisRanges), "napi_create_array");
    CHKRV(env, napi_set_named_property(env, object, "axisRanges", axisRanges), "napi_set_named_property");

    napi_value handlerTemp = nullptr;
    CHKRV(env, napi_get_reference_value(env, cbTemp.ref, &handlerTemp), "napi_get_reference_value");
    napi_value result = nullptr;
    CHKRV(env, napi_call_function(env, nullptr, handlerTemp, 1, &object, &result), "napi_call_function");
}

void JsEventTarget::EmitJsDev(int32_t userData, std::shared_ptr<InputDeviceImpl::InputDeviceInfo> device)
{
    CALL_LOG_ENTER;
    CHKPV(device);
    if (CheckEnv(env_)) {
        MMI_LOGE("env_ is nullptr");
        return;
    }
    auto iter = callback_.find(userData);
    if (iter == callback_.end()) {
        MMI_LOGE("failed to search for userData");
        return;
    }
    iter->second->data.device = device;

    napi_value resourceName = nullptr;
    CHKRV(env_, napi_create_string_latin1(env_, "InputDeviceAsync", NAPI_AUTO_LENGTH, &resourceName),
        "napi_create_string_latin1");

    if (iter->second->ref == nullptr) {
        CHKRV(env_, napi_create_async_work(env_, nullptr, resourceName, [](napi_env env, void *data) {},
                                           CallDevPromiseWork, iter->second, &(iter->second->asyncWork)),
                                           "napi_create_async_work");
    } else {
        CHKRV(env_, napi_create_async_work(env_, nullptr, resourceName, [](napi_env env, void *data) {},
                                    CallDevAsyncWork, iter->second, &(iter->second->asyncWork)),
                                    "napi_create_async_work");
    }
    CHKRV(env_, napi_queue_async_work(env_, iter->second->asyncWork), "napi_queue_async_work");
}

void JsEventTarget::CallDevPromiseWork(napi_env env, napi_status status, void* data)
{
    CALL_LOG_ENTER;
    CHKPV(data);
    JsUtil::CallbackInfo cbTemp(env);
    JsUtil jsUtil;
    jsUtil.GetCallbackInfo(data, cbTemp);
    CHKPV(cbTemp.data.device);

    napi_value id = nullptr;
    CHKRV(env, napi_create_int32(env, cbTemp.data.device->id, &id), "napi_create_int32");
    napi_value name = nullptr;
    CHKRV(env, napi_create_string_utf8(env, (cbTemp.data.device->name).c_str(), NAPI_AUTO_LENGTH, &name),
          "napi_create_string_utf8");
    napi_value object = nullptr;
    CHKRV(env, napi_create_object(env, &object), "napi_create_object");
    CHKRV(env, napi_set_named_property(env, object, "id", id), "napi_set_named_property");
    CHKRV(env, napi_set_named_property(env, object, "name", name), "napi_set_named_property");

    uint32_t types = cbTemp.data.device->devcieType;
    if (types <= 0) {
        napi_throw_error(env, nullptr, "devcieType is less than zero");
        MMI_LOGE("devcieType is less than zero");
    }
    std::vector<std::string> sources;
    for (const auto & item : g_deviceType) {
        if (static_cast<uint32_t>(types) & item.typeBit) {
            sources.push_back(item.deviceTypeName);
        }
    }
    napi_value devSources = nullptr;
    CHKRV(env, napi_create_array(env, &devSources), "napi_create_array");

    uint32_t index = 0;
    napi_value value = nullptr;
    for (const auto &item : sources) {
        CHKRV(env, napi_create_string_utf8(env, item.c_str(), NAPI_AUTO_LENGTH, &value), "napi_create_string_utf8");
        CHKRV(env, napi_set_element(env, devSources, index, value), "napi_set_element");
    }
    CHKRV(env, napi_set_named_property(env, object, "sources", devSources), "napi_set_named_property");

    napi_value axisRanges = nullptr;
    CHKRV(env, napi_create_array(env, &axisRanges), "napi_create_array");
    CHKRV(env, napi_set_named_property(env, object, "axisRanges", axisRanges), "napi_set_named_property");
    CHKRV(env, napi_resolve_deferred(env, cbTemp.deferred, object), "napi_resolve_deferred");
}

void JsEventTarget::EmitJsKeystrokeAbility(int32_t userData, std::vector<int32_t> keystrokeAbility)
{
    CALL_LOG_ENTER;
    if (CheckEnv(env_)) {
        MMI_LOGE("env_ is nullptr");
        return;
    }
    auto iter = callback_.find(userData);
    if (iter == callback_.end()) {
        MMI_LOGE("Failed to search for userData");
        return;
    }
    iter->second->data.keystrokeAbility = keystrokeAbility;

    napi_value resourceName = nullptr;
    napi_status state = napi_create_string_latin1(env_, "KeystrokeAbilityAsync", NAPI_AUTO_LENGTH, &resourceName);
    CHKRV(env_, state, "napi_create_string_latin1");

    if (iter->second->ref == nullptr) {
        CHKRV(env_, napi_create_async_work(env_, nullptr, resourceName, [](napi_env env, void *data) {},
                                           CallKeystrokeAbilityPromise, iter->second, &(iter->second->asyncWork)),
                                           "napi_create_async_work");
    } else {
        CHKRV(env_, napi_create_async_work(env_, nullptr, resourceName, [](napi_env env, void *data) {},
                                           CallKeystrokeAbilityAsync, iter->second, &(iter->second->asyncWork)),
                                           "napi_create_async_work");
    }
    CHKRV(env_, napi_queue_async_work(env_, iter->second->asyncWork), "napi_queue_async_work");
}

void JsEventTarget::CallKeystrokeAbilityPromise(napi_env env, napi_status status, void* data)
{
    CALL_LOG_ENTER;
    CHKPV(data);
    JsUtil::CallbackInfo cbTemp(env);
    JsUtil jsUtil;
    jsUtil.GetCallbackInfo(data, cbTemp);

    napi_value keyAbility = nullptr;
    CHKRV(env, napi_create_array(env, &keyAbility), "napi_create_array");
    napi_value keyCode = nullptr;
    napi_value ret = nullptr;
    napi_value isBool = nullptr;
    uint32_t index1 = 0;
    for (auto it = cbTemp.data.keystrokeAbility.begin(); it != cbTemp.data.keystrokeAbility.end(); ++it) {
        napi_value abilityRet = nullptr;
        CHKRV(env, napi_create_array(env, &abilityRet), "napi_create_array");
        uint32_t index2 = 0;
        CHKRV(env, napi_create_int32(env, *it, &keyCode), "napi_create_int32");
        CHKRV(env, napi_set_element(env, abilityRet, index2, keyCode), "napi_set_element");
        ++index2;
        CHKRV(env, napi_create_int32(env, *(++it), &ret), "napi_create_int32");
        CHKRV(env, napi_coerce_to_bool(env, ret, &isBool), "napi_create_int32");
        CHKRV(env, napi_set_element(env, abilityRet, index2, isBool), "napi_set_element");
        CHKRV(env, napi_set_element(env, keyAbility, index1, abilityRet), "napi_set_element");
        ++index1;
    }
    CHKRV(env, napi_resolve_deferred(env, cbTemp.deferred, keyAbility), "napi_resolve_deferred");
}

void JsEventTarget::CallKeystrokeAbilityAsync(napi_env env, napi_status status, void* data)
{
    CALL_LOG_ENTER;
    CHKPV(data);
    JsUtil::CallbackInfo cbTemp(env);
    JsUtil jsUtil;
    jsUtil.GetCallbackInfo(data, cbTemp);

    napi_value keyAbility = nullptr;
    CHKRV(env, napi_create_array(env, &keyAbility), "napi_create_array");
    napi_value keyCode = nullptr;
    napi_value ret = nullptr;
    napi_value isBool = nullptr;
    uint32_t index1 = 0;
    for (auto it = cbTemp.data.keystrokeAbility.begin(); it != cbTemp.data.keystrokeAbility.end(); ++it) {
        napi_value abilityRet = nullptr;
        CHKRV(env, napi_create_array(env, &abilityRet), "napi_create_array");
        uint32_t index2 = 0;
        CHKRV(env, napi_create_int32(env, *it, &keyCode), "napi_create_int32");
        CHKRV(env, napi_set_element(env, abilityRet, index2, keyCode), "napi_set_element");
        ++index2;
        CHKRV(env, napi_create_int32(env, *(++it), &ret), "napi_create_int32");
        CHKRV(env, napi_coerce_to_bool(env, ret, &isBool), "napi_create_int32");
        CHKRV(env, napi_set_element(env, abilityRet, index2, isBool), "napi_set_element");
        CHKRV(env, napi_set_element(env, keyAbility, index1, abilityRet), "napi_set_element");
        ++index1;
    }

    napi_value handlerTemp = nullptr;
    CHKRV(env, napi_get_reference_value(env, cbTemp.ref, &handlerTemp), "napi_get_reference_value");
    napi_value result = nullptr;
    CHKRV(env, napi_call_function(env, nullptr, handlerTemp, 1, &keyAbility, &result), "napi_call_function");
}

napi_value JsEventTarget::CreateCallbackInfo(napi_env env, napi_value handle)
{
    CALL_LOG_ENTER;
    env_ = env;
    JsUtil::CallbackInfo* cb = new (std::nothrow) JsUtil::CallbackInfo(env);
    CHKPP(cb);

    napi_status state = napi_generic_failure;
    if (handle == nullptr) {
        state = napi_create_promise(env_, &cb->deferred, &cb->promise);
        if (state != napi_ok) {
            delete cb;
            cb = nullptr;
            CHKRP(env_, state, "napi_create_reference");
        }
        if (userData_ == INT32_MAX) {
            MMI_LOGE("userData_ exceeds the maximum");
            return nullptr;
        }
        callback_[userData_] = cb;
        ++userData_;
        return cb->promise;
    }

    state = napi_create_reference(env_, handle, 1, &cb->ref);
    if (state != napi_ok) {
        delete cb;
        cb = nullptr;
        CHKRP(env_, state, "napi_create_reference");
    }
    if (userData_ == INT32_MAX) {
        MMI_LOGE("userData_ exceeds the maximum");
        return nullptr;
    }
    callback_[userData_] = cb;
    ++userData_;
    return nullptr;
}

void JsEventTarget::ResetEnv()
{
    CALL_LOG_ENTER;
    env_ = nullptr;

    for (auto& item : callback_) {
        if (item.second == nullptr) {
            continue;
        }
        delete item.second;
        item.second = nullptr;
    }
    std::map<int32_t, JsUtil::CallbackInfo*> empty_map;
    callback_.swap(empty_map);
    callback_.clear();
}

bool JsEventTarget::CheckEnv(napi_env env)
{
    if (env != nullptr) {
        return false;
    }
    for (auto &item : callback_) {
        if (item.second == nullptr) {
            continue;
        }
        delete item.second;
        item.second = nullptr;
    }
    return true;
}
} // namespace MMI
} // namespace OHOS