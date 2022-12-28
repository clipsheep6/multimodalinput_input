/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef JS_INPUT_CONSUMER_CONTEXT_H
#define JS_INPUT_CONSUMER_CONTEXT_H

#include <list>
#include <map>

#include "key_event.h"
#include "key_option.h"
#include "util_napi.h"

namespace OHOS {
namespace MMI {
struct SubscribeInfo {
    SubscribeInfo() {}
    SubscribeInfo(napi_env env) : env(env) {}
    napi_env env { nullptr };
    std::string eventType;
    std::shared_ptr<KeyEvent> keyEvent { nullptr };
    napi_ref callback { nullptr };
    int32_t subscribeId { 0 };
    std::shared_ptr<KeyOption> keyOption { nullptr };
};

typedef std::map<std::string, std::list<std::shared_ptr<SubscribeInfo>>> Callbacks;

class JsInputConsumerContext final {
public:
    JsInputConsumerContext() = default;
    DISALLOW_COPY_AND_MOVE(JsInputConsumerContext);
    ~JsInputConsumerContext() = default;
    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value JsOff(napi_env env, napi_callback_info info);
    static napi_value JsOn(napi_env env, napi_callback_info info);
    static int32_t GetSubEventInfo(napi_env env, size_t argc, napi_value* argv,
        std::shared_ptr<SubscribeInfo> &event);
    static std::shared_ptr<KeyOption> CreateKeyOption(napi_env env, napi_value object);
    static std::string GetSubKeyNames(const std::shared_ptr<KeyOption> &keyOption);
    static void SubKeyEventCallback(std::shared_ptr<KeyEvent> keyEvent);

private:
    static Callbacks callbacks;
};
} // namespace MMI
} // namespace OHOS
#endif // JS_INPUT_CONSUMER_CONTEXT_H
