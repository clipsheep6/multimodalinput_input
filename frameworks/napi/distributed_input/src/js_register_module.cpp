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

#include "js_input_dinput_context.h"
#include <cinttypes>
#include "input_manager.h"

namespace OHOS {
namespace MMI {
static napi_module mmiInputDInputModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = JsInputDinputContext::Export,
    .nm_modname = "distributedInput",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&mmiInputDInputModule);
}
}
}
