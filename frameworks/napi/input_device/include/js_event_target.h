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

#ifndef JS_EVENT_TARGET_H
#define JS_EVENT_TARGET_H

#include "define_multimodal.h"
#include "error_multimodal.h"
#include "input_device_impl.h"
#include "js_util.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "utils/log.h"
#include "util_napi.h"


namespace OHOS {
namespace MMI {
class JsEventTarget {
public:
    JsEventTarget() = default;
    DISALLOW_COPY_AND_MOVE(JsEventTarget);
    static void EmitJsIds(int32_t userData, std::vector<int32_t> ids);
    static void EmitJsDev(int32_t userData, std::shared_ptr<InputDeviceImpl::InputDeviceInfo> device);
    static void EmitJsKeystrokeAbility(int32_t userData, std::vector<int32_t> keystrokeAbility);
    napi_value CreateCallbackInfo(napi_env env, napi_value handle);
    void ResetEnv();
    static int32_t userData_;
    static napi_env env_;

    struct DeviceType {
        std::string deviceTypeName;
        uint32_t typeBit;
    };

private:
    static bool CheckEnv(napi_env env);
    static void CallIdsPromiseWork(napi_env env, napi_status status, void* data);
    static void CallIdsAsyncWork(napi_env env, napi_status status, void* data);
    static void CallDevAsyncWork(napi_env env, napi_status status, void* data);
    static void CallDevPromiseWork(napi_env env, napi_status status, void* data);
    static void CallKeystrokeAbilityPromise(napi_env env, napi_status status, void* data);
    static void CallKeystrokeAbilityAsync(napi_env env, napi_status status, void* data);
};
} // namespace MMI
} // namespace OHOS

#endif // JS_EVENT_TARGET_H