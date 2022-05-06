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

#include "js_input_device_context.h"

namespace OHOS {
namespace MMI {
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
static napi_module mmiInputDeviceModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = JsInputDeviceContext::Export,
    .nm_modname = "inputDevice",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};
#endif

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    napi_module_register(&mmiInputDeviceModule);
#endif
}
} // namespace MMI
} // namespace OHOS