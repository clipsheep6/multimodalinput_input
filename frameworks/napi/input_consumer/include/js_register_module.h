/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JS_REGISTER_MODULE_H
#define JS_REGISTER_MODULE_H

#include <list>

#include "napi/native_node_api.h"

#include "define_multimodal.h"
#include "key_option.h"

#define SUCCESS_CODE 0
#define ERROR_CODE (-1)
#define UNREGISTERED_CODE (-2)
#define PRE_KEY_MAX_COUNT 4

enum JS_CALLBACK_EVENT {
    JS_CALLBACK_EVENT_FAILED = -1,
    JS_CALLBACK_EVENT_SUCCESS = 1,
    JS_CALLBACK_EVENT_EXIST = 2,
    JS_CALLBACK_EVENT_NOT_EXIST = 3,
};

namespace OHOS {
namespace MMI {
extern std::mutex sCallBacksMutex;
class JsCommon {
public:
    static bool TypeOf(napi_env env, napi_value value, napi_valuetype type);
    static void ThrowError(napi_env env, int32_t code);
};

struct KeyEventMonitorInfo : RefBase {
    napi_env env{ nullptr };
    napi_async_work asyncWork{ nullptr };
    std::string eventType;
    std::string name;
    napi_ref callback{ nullptr };
    int32_t subscribeId{ 0 };
    std::shared_ptr<KeyOption> keyOption{ nullptr };
    ~KeyEventMonitorInfo();
};
typedef std::map<std::string, std::list<sptr<KeyEventMonitorInfo>>> Callbacks;
} // namespace MMI
} // namespace OHOS
#endif // JS_REGISTER_MODULE_H