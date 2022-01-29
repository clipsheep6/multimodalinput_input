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

#ifndef JS_INPUT_DEVICE_MANAGER_H
#define JS_INPUT_DEVICE_MANAGER_H

#include <memory>
#include "libmmi_util.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "utils/log.h"

namespace OHOS {
namespace MMI {
class JsInputDeviceManager : std::enable_shared_from_this<JsInputDeviceManager> {
public:
    JsInputDeviceManager(napi_env env);
    void ResetEnv();
    void GetDeviceIdsAsync(napi_value handle);
    void GetDeviceAsync(int32_t id, napi_value handle);
    napi_env GetEnv();
    napi_ref GetRef();

    struct DeviceType {
        std::string deviceTypeName;
        uint32_t typeBit;
    };
    static constexpr uint32_t EVDEV_UDEV_TAG_KEYBOARD = (1 << 1);
    static constexpr uint32_t EVDEV_UDEV_TAG_MOUSE = (1 << 2);
    static constexpr uint32_t EVDEV_UDEV_TAG_TOUCHPAD = (1 << 3);
    static constexpr uint32_t EVDEV_UDEV_TAG_TOUCHSCREEN = (1 << 4);
    static constexpr uint32_t EVDEV_UDEV_TAG_TABLET = (1 << 5);
    static constexpr uint32_t EVDEV_UDEV_TAG_JOYSTICK = (1 << 6);
    static constexpr uint32_t EVDEV_UDEV_TAG_ACCELEROMETER = (1 << 7);
    static constexpr uint32_t EVDEV_UDEV_TAG_TABLET_PAD = (1 << 8);
    static constexpr uint32_t EVDEV_UDEV_TAG_POINTINGSTICK = (1 << 9);
    static constexpr uint32_t EVDEV_UDEV_TAG_TRACKBALL = (1 << 10);
    static constexpr uint32_t EVDEV_UDEV_TAG_SWITCH = (1 << 11);
private:
    napi_ref ref_ = nullptr;
    napi_env env_ = nullptr;
};
} // namespace MMI
} // namespace OHOS

#endif // JS_INPUT_DEVICE_MANAGER_H