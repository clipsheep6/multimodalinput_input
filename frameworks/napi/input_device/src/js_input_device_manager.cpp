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

#include "js_input_device_manager.h"
#include "input_device_impl.h"

namespace OHOS {
namespace MMI {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JsInputDeviceManager" };
JsInputDeviceManager::DeviceType g_deviceType[] = {
    {"keyboard", JsInputDeviceManager::EVDEV_UDEV_TAG_KEYBOARD},
    {"mouse", JsInputDeviceManager::EVDEV_UDEV_TAG_MOUSE},
    {"touchpad", JsInputDeviceManager::EVDEV_UDEV_TAG_TOUCHPAD},
    {"touchscreen", JsInputDeviceManager::EVDEV_UDEV_TAG_TOUCHSCREEN},
    {"joystick", JsInputDeviceManager::EVDEV_UDEV_TAG_JOYSTICK},
    {"trackball", JsInputDeviceManager::EVDEV_UDEV_TAG_TRACKBALL},
};
}
JsInputDeviceManager::JsInputDeviceManager(napi_env env) : env_(env) {}

void JsInputDeviceManager::GetDeviceIdsAsync(napi_value handle)
{
    if (env_ == nullptr) {
        MMI_LOGE("env_ is nullptr");
        return;
    }

    napi_status status = napi_create_reference(env_, handle, 1, &ref_);
    if (status != napi_ok) {
        MMI_LOGE("failed to create ref_ reference");
        return;
    }

    auto jsIds = shared_from_this();
    if (jsIds == nullptr) {
        MMI_LOGE("jsIds this pointer is nullptr");
        return;
    }
    auto &instance = InputDeviceImpl::GetInstance();
    instance.GetInputDeviceIdsAsync([jsIds](std::vector<int32_t> ids) {
        napi_value arr = nullptr;
        napi_env env = jsIds->GetEnv();
        napi_handle_scope scope = nullptr;
        napi_status status = napi_open_handle_scope(env, &scope);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: failed to open scope");
            return;
        }

        status = napi_create_array(env, &arr);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_create_array failed");
            return;
        }
        uint32_t index = 0;
        napi_value value = nullptr;
        for (const auto &item : ids) {
            status = napi_create_int64(env, item, &value);
            if (status != napi_ok) {
                napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_create_int64 failed");
                return;
            }
            status = napi_set_element(env, arr, index, value);
            if (status != napi_ok) {
                napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_set_element failed");
                return;
            }
            ++index;
        }

        napi_value result = nullptr;
        napi_value handlerTemp = nullptr;
        napi_ref handleRef = jsIds->GetRef();
        status = napi_get_reference_value(env, handleRef, &handlerTemp);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_get_reference_value failed");
            return;
        }
        uint32_t refCount = 0;
        status = napi_call_function(env, nullptr, handlerTemp, 1, &arr, &result);
        if (status != napi_ok) {
            napi_reference_unref(env, handleRef, &refCount);
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_call_function failed");
            return;
        }

        status = napi_reference_unref(env, handleRef, &refCount);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_reference_unref failed");
            return;
        }
        status = napi_close_handle_scope(env, scope);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: failed to close scope");
            return;
        }
    });
}

void JsInputDeviceManager::GetDeviceAsync(int32_t id, napi_value handle)
{
    napi_status status = napi_create_reference(env_, handle, 1, &ref_);
    if (status != napi_ok) {
        MMI_LOGE("failed to create ref_ reference");
        return;
    }

    auto jsDev = shared_from_this();
    if (jsDev == nullptr) {
        MMI_LOGE("jsDev this pointer is nullptr");
        return;
    }
    auto &instance = InputDeviceImpl::GetInstance();
    instance.GetInputDeviceAsync(id, [jsDev](std::shared_ptr<InputDeviceImpl::InputDeviceInfo> device) {
        napi_env env = jsDev->GetEnv();
        napi_handle_scope scope = nullptr;
        napi_status status = napi_open_handle_scope(env, &scope);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: failed to open scope");
            return;
        }
        napi_value id = nullptr;
        status = napi_create_int64(env, device->id, &id);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_create_int64 failed");
            return;
        }
        napi_value name = nullptr;
        status = napi_create_string_utf8(env, (device->name).c_str(), NAPI_AUTO_LENGTH, &name);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_create_string_utf8 failed");
            return;
        }

        napi_value object = nullptr;
        status = napi_create_object(env, &object);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_create_object failed");
            return;
        }

        status = napi_set_named_property(env, object, "id", id);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_set_named_property failed");
            return;
        }
        status = napi_set_named_property(env, object, "name", name);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_set_named_property failed");
            return;
        }

        int32_t types = device->devcieType;
        std::vector<std::string> sources;
        for (const auto & item : g_deviceType) {
            if (types & item.typeBit) {
                sources.push_back(item.deviceTypeName);
            }
        }
        napi_value devSources = nullptr;
        status = napi_create_array(env, &devSources);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_create_array failed");
            return;
        }
        uint32_t index = 0;
        napi_value value = nullptr;
        for (const auto &item : sources) {
            status = napi_create_string_utf8(env, item.c_str(), NAPI_AUTO_LENGTH, &value);
            if (status != napi_ok) {
                napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_create_string_utf8 failed");
                return;
            }
            status = napi_set_element(env, devSources, index, value);
            if (status != napi_ok) {
                napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_set_element failed");
            }
        }
        status = napi_set_named_property(env, object, "sources", devSources);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "GetDevice:uv_queue_work call to napi_set_named_property failed");
            return;
        }

        napi_value axisRanges = nullptr;
        status = napi_create_array(env, &axisRanges);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "GetDevice:uv_queue_work call to napi_create_array failed");
            return;
        }
        status = napi_set_named_property(env, object, "axisRanges", axisRanges);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "GetDevice:uv_queue_work call to napi_set_named_property failed");
            return;
        }

        napi_value handlerTemp = nullptr;
        napi_ref handleRef = jsDev->GetRef();
        status = napi_get_reference_value(env, handleRef, &handlerTemp);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_get_reference_value failed");
            return;
        }
        napi_value result = nullptr;
        status = napi_call_function(env, nullptr, handlerTemp, 1, &object, &result);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_call_function failed");
            return;
        }

        uint32_t refCount = 0;
        status = napi_reference_unref(env, handleRef, &refCount);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: call to napi_reference_unref failed");
            return;
        }

        status = napi_close_handle_scope(env, scope);
        if (status != napi_ok) {
            napi_throw_error(env, nullptr, "JsInputDeviceManager: failed to close scope");
            return;
        }
    });
}

napi_env JsInputDeviceManager::GetEnv()
{
    return env_;
}

napi_ref JsInputDeviceManager::GetRef()
{
    return ref_;
}

void JsInputDeviceManager::ResetEnv()
{
    env_ = nullptr;
}
} // namespace MMI
} // namespace OHOS