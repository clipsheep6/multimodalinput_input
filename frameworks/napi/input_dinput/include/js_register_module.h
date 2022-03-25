/*
 * @Descripttion: 
 * @version: 
 * @Author: sueRimn
 * @Date: 2022-02-11 15:10:15
 * @LastEditors: sueRimn
 * @LastEditTime: 2022-02-18 08:59:15
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
#ifndef OHOS_JS_INPUT_DINPUT_REGISTER_MODULE_H
#define OHOS_JS_INPUT_DINPUT_REGISTER_MODULE_H

#include <stdio.h>
#include <map>
#include <list>
#include <string.h>
#include <iostream>
#include <uv.h>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "utils/log.h"
#include "libmmi_util.h"

namespace OHOS {
namespace MMI {
#define IM_NAPI_BUF_LENGTH (256)
    class HandleNapi{
        public:
        static void GetParameter(napi_env env, napi_callback_info info, napi_ref& first);
        static void GetParameter(napi_env env, napi_callback_info info, int32_t& first, napi_ref& second);
        static void GetParameter(napi_env env, napi_callback_info info, std::string& first, napi_ref& second);
        static void GetParameter(napi_env env, napi_callback_info info, int32_t& first, int32_t& second, napi_ref& third);
        static void HandleCallBack(uv_work_t* work, bool returnResult);
        static void HandleCallBack(uv_work_t* work, int32_t returnResult);

    };
} // namespace MMI
} // namespace OHOS

#endif
