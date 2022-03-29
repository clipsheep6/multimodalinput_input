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

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JsEventTarget" };
JsEventTarget::DeviceType g_deviceType[] = {
    {"keyboard", JsEventTarget::EVDEV_UDEV_TAG_KEYBOARD},
    {"mouse", JsEventTarget::EVDEV_UDEV_TAG_MOUSE},
    {"touchpad", JsEventTarget::EVDEV_UDEV_TAG_TOUCHPAD},
    {"touchscreen", JsEventTarget::EVDEV_UDEV_TAG_TOUCHSCREEN},
    {"joystick", JsEventTarget::EVDEV_UDEV_TAG_JOYSTICK},
    {"trackball", JsEventTarget::EVDEV_UDEV_TAG_TRACKBALL},
};
} // namespace

static std::map<int32_t, JsEventTarget::CallbackInfo*> callback_ {};
int32_t JsEventTarget::userData_ = 0;
std::mutex mutex_;

void JsEventTarget::CallIdsAsyncWork(uv_work_t *work, int32_t status)
{
    CHKPV(work);
    CHKPV(work->data);
    CallbackInfo *cb = static_cast<CallbackInfo*>(work->data);
    CallbackInfo cbTemp = *cb;
    delete cb;
    cb = nullptr;
    delete work;
    work = nullptr;
    CHKPV(cbTemp.env);

    napi_handle_scope scope = nullptr;
    napi_status status_ = napi_open_handle_scope(cbTemp.env, &scope);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: failed to open scope");
        MMI_LOGE("failed to open scope");
        return;
    }
    napi_value arr = nullptr;
    status_ = napi_create_array(cbTemp.env, &arr);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_create_array failed");
        MMI_LOGE("call to napi_create_array failed");
        return;
    }

    uint32_t index = 0;
    napi_value value = nullptr;
    for (const auto &item : cbTemp.ids) {
        status_ = napi_create_int64(cbTemp.env, item, &value);
        if (status_ != napi_ok) {
            napi_delete_reference(cbTemp.env, cbTemp.ref);
            napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_create_int64 failed");
            MMI_LOGE("call to napi_create_int64 failed");
            return;
        }
        status_ = napi_set_element(cbTemp.env, arr, index, value);
        if (status_ != napi_ok) {
            napi_delete_reference(cbTemp.env, cbTemp.ref);
            napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_set_element failed");
            MMI_LOGE("call to napi_set_element failed");
            return;
        }
        index++;
    }

    napi_value handlerTemp = nullptr;
    status_ = napi_get_reference_value(cbTemp.env, cbTemp.ref, &handlerTemp);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_get_reference_value failed");
        MMI_LOGE("call to napi_get_reference_value failed");
        return;
    }
    napi_value result = nullptr;
    status_ = napi_call_function(cbTemp.env, nullptr, handlerTemp, 1, &arr, &result);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_call_function failed");
        MMI_LOGE("call to napi_call_function failed");
        return;
    }
    status_ = napi_delete_reference(cbTemp.env, cbTemp.ref);
    if (status_ != napi_ok) {
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_delete_reference failed");
        MMI_LOGE("call to napi_delete_reference failed");
        return;
    }

    status_ = napi_close_handle_scope(cbTemp.env, scope);
    if (status_ != napi_ok) {
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: failed to close scope");
        MMI_LOGE("failed to close scope");
        return;
    }
}

void JsEventTarget::EmitJsIdsAsync(int32_t userData, std::vector<int32_t> ids)
{
    std::lock_guard<std::mutex> guard(mutex_);
    auto iter = callback_.find(userData);
    if (iter == callback_.end()) {
        MMI_LOGE("Failed to search for userData");
        return;
    }
    CHKPV(iter->second);
    if (iter->second->env == nullptr) {
        delete iter->second;
        iter->second = nullptr;
        callback_.erase(iter);
        MMI_LOGE("env is nullptr");
    }
    iter->second->ids = ids;
    uv_loop_s *loop = nullptr;
    napi_status status = napi_get_uv_event_loop(iter->second->env, &loop);
    if (status != napi_ok) {
        MMI_LOGE("napi_get_uv_event_loop failed");
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    CHKPV(work);
    work->data = static_cast<void*>(iter->second);
    int32_t ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, CallIdsAsyncWork);
    if (ret != 0) {
        MMI_LOGE("uv_queue_work failed");
        return;
    }
}

void JsEventTarget::CallDevAsyncWork(uv_work_t *work, int32_t status)
{
    CHKPV(work);
    CHKPV(work->data);
    CallbackInfo *cb = static_cast<CallbackInfo*>(work->data);
    CallbackInfo cbTemp = *cb;
    delete cb;
    cb = nullptr;
    delete work;
    work = nullptr;
    CHKPV(cbTemp.env);

    napi_handle_scope scope = nullptr;
    napi_status status_ = napi_open_handle_scope(cbTemp.env, &scope);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: failed to open scope");
        MMI_LOGE("failed to open scope");
        return;
    }

    napi_value id = nullptr;
    status_ = napi_create_int64(cbTemp.env, cbTemp.device->id, &id);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_create_int64 failed");
        MMI_LOGE("call to napi_create_int64 failed");
        return;
    }
    napi_value name = nullptr;
    status_ = napi_create_string_utf8(cbTemp.env, (cbTemp.device->name).c_str(), NAPI_AUTO_LENGTH, &name);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_create_string_utf8 failed");
        MMI_LOGE("call to napi_create_string_utf8 failed");
        return;
    }

    napi_value object = nullptr;
    status_ = napi_create_object(cbTemp.env, &object);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_create_object failed");
        MMI_LOGE("call to napi_create_object failed");
        return;
    }

    status_ = napi_set_named_property(cbTemp.env, object, "id", id);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_set_named_property failed");
        MMI_LOGE("call to napi_set_named_property failed");
        return;
    }
    status_ = napi_set_named_property(cbTemp.env, object, "name", name);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_set_named_property failed");
        MMI_LOGE("call to napi_set_named_property failed");
        return;
    }

    uint32_t types = cbTemp.device->devcieType;
    std::vector<std::string> sources;
    for (const auto & item : g_deviceType) {
        if (types & item.typeBit) {
            sources.push_back(item.deviceTypeName);
        }
    }
    napi_value devSources = nullptr;
    status_ = napi_create_array(cbTemp.env, &devSources);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_create_array failed");
        MMI_LOGE("call to napi_create_array failed");
        return;
    }
    uint32_t index = 0;
    napi_value value = nullptr;
    for (const auto &item : sources) {
        status_ = napi_create_string_utf8(cbTemp.env, item.c_str(), NAPI_AUTO_LENGTH, &value);
        if (status_ != napi_ok) {
            napi_delete_reference(cbTemp.env, cbTemp.ref);
            napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_create_string_utf8 failed");
            MMI_LOGE("call to napi_create_string_utf8 failed");
            return;
        }
        status_ = napi_set_element(cbTemp.env, devSources, index, value);
        if (status_ != napi_ok) {
            napi_delete_reference(cbTemp.env, cbTemp.ref);
            napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_set_element failed");
            MMI_LOGE("call to napi_set_element failed");
            return;
        }
    }
    status_ = napi_set_named_property(cbTemp.env, object, "sources", devSources);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_set_named_property failed");
        MMI_LOGE("call to napi_set_named_property failed");
        return;
    }

    napi_value axisRanges = nullptr;
    status_ = napi_create_array(cbTemp.env, &axisRanges);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_create_array failed");
        MMI_LOGE("call to napi_create_array failed");
        return;
    }
    status_ = napi_set_named_property(cbTemp.env, object, "axisRanges", axisRanges);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_set_named_property failed");
        MMI_LOGE("call to napi_set_named_property failed");
        return;
    }

    napi_value handlerTemp = nullptr;
    status_ = napi_get_reference_value(cbTemp.env, cbTemp.ref, &handlerTemp);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_get_reference_value failed");
        MMI_LOGE("call to napi_get_reference_value failed");
        return;
    }
    napi_value result = nullptr;
    status_ = napi_call_function(cbTemp.env, nullptr, handlerTemp, 1, &object, &result);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_call_function failed");
        MMI_LOGE("call to napi_call_function failed");
        return;
    }
    status_ = napi_delete_reference(cbTemp.env, cbTemp.ref);
    if (status_ != napi_ok) {
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_delete_reference failed");
        MMI_LOGE("call to napi_delete_reference failed");
        return;
    }

    status_ = napi_close_handle_scope(cbTemp.env, scope);
    if (status_ != napi_ok) {
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: failed to close scope");
        MMI_LOGE("failed to close scope");
        return;
    }
}

void JsEventTarget::EmitJsDevAsync(int32_t userData, std::shared_ptr<InputDeviceImpl::InputDeviceInfo> device)
{
    std::lock_guard<std::mutex> guard(mutex_);
    CHKPV(device);
    auto iter = callback_.find(userData);
    if (iter == callback_.end()) {
        MMI_LOGE("Failed to search for userData");
        return;
    }
    CHKPV(iter->second);
    if (iter->second->env == nullptr) {
        delete iter->second;
        iter->second = nullptr;
        callback_.erase(iter);
        MMI_LOGE("env is nullptr");
    }
    iter->second->device = device;
    uv_loop_s *loop = nullptr;
    napi_status status = napi_get_uv_event_loop(iter->second->env, &loop);
    if (status != napi_ok) {
        MMI_LOGE("napi_get_uv_event_loop failed");
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    CHKPV(work);
    work->data = static_cast<void*>(iter->second);
    int32_t ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, CallDevAsyncWork);
    if (ret != 0) {
        MMI_LOGE("uv_queue_work failed");
        return;
    }
}

void JsEventTarget::CallIdsPromiseWork(uv_work_t *work, int32_t status)
{
    CHKPV(work);
    CHKPV(work->data);
    CallbackInfo *cb = static_cast<CallbackInfo*>(work->data);
    CallbackInfo cbTemp = *cb;
    delete cb;
    cb = nullptr;
    delete work;
    work = nullptr;
    CHKPV(cbTemp.env);

    napi_handle_scope scope = nullptr;
    napi_status status_ = napi_open_handle_scope(cbTemp.env, &scope);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: failed to open scope");
        MMI_LOGE("failed to open scope");
        return;
    }
    napi_value arr = nullptr;
    status_ = napi_create_array(cbTemp.env, &arr);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_create_array failed");
        MMI_LOGE("call to napi_create_array failed");
        return;
    }
    uint32_t index = 0;
    napi_value value = nullptr;
    for (const auto &item : cbTemp.ids) {
        status_ = napi_create_int64(cbTemp.env, item, &value);
        if (status_ != napi_ok) {
            napi_delete_reference(cbTemp.env, cbTemp.ref);
            napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_create_int64 failed");
            MMI_LOGE("call to napi_create_int64 failed");
            return;
        }
        status_ = napi_set_element(cbTemp.env, arr, index, value);
        if (status_ != napi_ok) {
            napi_delete_reference(cbTemp.env, cbTemp.ref);
            napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_set_element failed");
            MMI_LOGE("call to napi_set_element failed");
            return;
        }
        index++;
    }

    status_ = napi_resolve_deferred(cbTemp.env, cbTemp.deferred, arr);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_call_function failed");
        MMI_LOGE("call to napi_call_function failed");
        return;
    }

    status_ = napi_close_handle_scope(cbTemp.env, scope);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: failed to close scope");
        MMI_LOGE("failed to close scope");
        return;
    }
}

void JsEventTarget::EmitJsIdsPromise(int32_t userData, std::vector<int32_t> ids)
{
    std::lock_guard<std::mutex> guard(mutex_);
    auto iter = callback_.find(userData);
    if (iter == callback_.end()) {
        MMI_LOGE("Failed to search for userData");
        return;
    }
    CHKPV(iter->second);
    if (iter->second->env == nullptr) {
        delete iter->second;
        iter->second = nullptr;
        callback_.erase(iter);
        MMI_LOGE("env is nullptr");
    }
    iter->second->ids = ids;
    uv_loop_s *loop = nullptr;
    napi_status status = napi_get_uv_event_loop(iter->second->env, &loop);
    if (status != napi_ok) {
        MMI_LOGE("napi_get_uv_event_loop failed");
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    CHKPV(work);
    work->data = static_cast<void*>(iter->second);
    int32_t ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, CallIdsPromiseWork);
    if (ret != 0) {
        MMI_LOGE("uv_queue_work failed");
        return;
    }
}

void JsEventTarget::CallDevPromiseWork(uv_work_t *work, int32_t status)
{
    CHKPV(work);
    CHKPV(work->data);
    CallbackInfo *cb = static_cast<CallbackInfo*>(work->data);
    CallbackInfo cbTemp = *cb;
    delete cb;
    cb = nullptr;
    delete work;
    work = nullptr;
    CHKPV(cbTemp.env);

    napi_handle_scope scope = nullptr;
    napi_status status_ = napi_open_handle_scope(cbTemp.env, &scope);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: failed to open scope");
        MMI_LOGE("failed to open scope");
        return;
    }

    napi_value id = nullptr;
    status_ = napi_create_int64(cbTemp.env, cbTemp.device->id, &id);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "napi_create_int64 failed");
        MMI_LOGE("napi_create_int64 failed");
        return;
    }
    napi_value name = nullptr;
    status_ = napi_create_string_utf8(cbTemp.env, (cbTemp.device->name).c_str(), NAPI_AUTO_LENGTH, &name);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "napi_create_string_utf8 failed");
        MMI_LOGE("napi_create_string_utf8 failed");
        return;
    }
    napi_value object = nullptr;
    status_ = napi_create_object(cbTemp.env, &object);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "napi_create_object failed");
        MMI_LOGE("napi_create_object failed");
        return;
    }

    status_ = napi_set_named_property(cbTemp.env, object, "id", id);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "napi_set_named_property set id failed");
        MMI_LOGE("napi_set_named_property set id failed");
        return;
    }
    status_ = napi_set_named_property(cbTemp.env, object, "name", name);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "napi_set_named_property set name failed");
        MMI_LOGE("napi_set_named_property set name failed");
        return;
    }

    uint32_t types = cbTemp.device->devcieType;
    if (types <= 0) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "devcieType is less than zero");
        MMI_LOGE("devcieType is less than zero");
    }
    std::vector<std::string> sources;
    for (const auto & item : g_deviceType) {
        if (static_cast<uint32_t>(types) & item.typeBit) {
            sources.push_back(item.deviceTypeName);
        }
    }
    napi_value devSources = nullptr;
    status_ = napi_create_array(cbTemp.env, &devSources);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "napi_create_array failed");
        MMI_LOGE("napi_create_array failed");
        return;
    }

    uint32_t index = 0;
    napi_value value = nullptr;
    for (const auto &item : sources) {
        status_ = napi_create_string_utf8(cbTemp.env, item.c_str(), NAPI_AUTO_LENGTH, &value);
        if (status_ != napi_ok) {
            napi_delete_reference(cbTemp.env, cbTemp.ref);
            napi_throw_error(cbTemp.env, nullptr, "napi_create_string_utf8 failed");
            MMI_LOGE("napi_create_string_utf8 failed");
            return;
        }
        status_ = napi_set_element(cbTemp.env, devSources, index, value);
        if (status_ != napi_ok) {
            napi_delete_reference(cbTemp.env, cbTemp.ref);
            napi_throw_error(cbTemp.env, nullptr, "napi_set_element failed");
            MMI_LOGE("napi_set_element failed");
        }
    }
    status_ = napi_set_named_property(cbTemp.env, object, "sources", devSources);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_set_named_property failed");
        MMI_LOGE("call to napi_set_named_property failed");
        return;
    }

    napi_value axisRanges = nullptr;
    status_ = napi_create_array(cbTemp.env, &axisRanges);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_create_array failed");
        MMI_LOGE("call to napi_create_array failed");
        return;
    }
    status_ = napi_set_named_property(cbTemp.env, object, "axisRanges", axisRanges);
    if (status_ != napi_ok) {
        napi_delete_reference(cbTemp.env, cbTemp.ref);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_set_named_property failed");
        MMI_LOGE("call to napi_set_named_property failed");
        return;
    }

    status_ = napi_resolve_deferred(cbTemp.env, cbTemp.deferred, object);
    if (status_ != napi_ok) {
        napi_delete_async_work(cbTemp.env, cbTemp.asyncWork);
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: call to napi_call_function failed");
        MMI_LOGE("call to napi_call_function failed");
        return;
    }

    status_ = napi_close_handle_scope(cbTemp.env, scope);
    if (status_ != napi_ok) {
        napi_throw_error(cbTemp.env, nullptr, "JsEventTarget: failed to close scope");
        MMI_LOGE("failed to close scope");
        return;
    }
}

void JsEventTarget::EmitJsDevPromise(int32_t userData, std::shared_ptr<InputDeviceImpl::InputDeviceInfo> device)
{
    std::lock_guard<std::mutex> guard(mutex_);
    CHKPV(device);
    auto iter = callback_.find(userData);
    if (iter == callback_.end()) {
        MMI_LOGE("Failed to search for userData");
        return;
    }
    CHKPV(iter->second);
    if (iter->second->env == nullptr) {
        delete iter->second;
        iter->second = nullptr;
        callback_.erase(iter);
        MMI_LOGE("env is nullptr");
    }
    iter->second->device = device;
    uv_loop_s *loop = nullptr;
    napi_status status = napi_get_uv_event_loop(iter->second->env, &loop);
    if (status != napi_ok) {
        MMI_LOGE("napi_get_uv_event_loop failed");
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    CHKPV(work);
    work->data = static_cast<void*>(iter->second);
    int32_t ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, CallDevPromiseWork);
    if (ret != 0) {
        MMI_LOGE("uv_queue_work failed");
        return;
    }
}

napi_value JsEventTarget::CreateCallbackInfo(napi_env env, napi_value handle)
{
    std::lock_guard<std::mutex> guard(mutex_);
    CallbackInfo* cb = new (std::nothrow) CallbackInfo;
    CHKPP(cb);
    cb->env = env;
    napi_status status = napi_generic_failure;
    if (handle == nullptr) {
        status = napi_create_promise(env, &cb->deferred, &cb->promise);
        if (status != napi_ok) {
            delete cb;
            cb = nullptr;
            napi_throw_error(env, nullptr, "JsEventTarget: failed to create promise");
            MMI_LOGE("failed to create promise");
            return nullptr;
        }
        callback_[userData_] = cb;
        if (userData_ == INT32_MAX) {
            MMI_LOGE("userData_ exceeds the maximum");
            return nullptr;
        }
        ++userData_;
        return cb->promise;
    }

    status = napi_create_reference(env, handle, 1, &cb->ref);
    if (status != napi_ok) {
        delete cb;
        cb = nullptr;
        napi_throw_error(env, nullptr, "JsEventTarget: call to napi_create_reference failed");
        MMI_LOGE("call to napi_create_reference failed");
        return nullptr;
    }
    callback_[userData_] = cb;
    if (userData_ == INT32_MAX) {
        MMI_LOGE("userData_ exceeds the maximum");
        return nullptr;
    }
    ++userData_;
    return nullptr;
}

void JsEventTarget::ResetEnv()
{
    std::lock_guard<std::mutex> guard(mutex_);
    for (auto &item : callback_) {
        if (item.second == nullptr || item.second->env == nullptr) {
            continue;
        }
        item.second->env = nullptr;
    }
}
} // namespace MMI
} // namespace OHOS