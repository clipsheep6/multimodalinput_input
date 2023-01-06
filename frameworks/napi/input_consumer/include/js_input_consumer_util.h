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

#ifndef JS_INPUT_CONSUMER_UTIL_H
#define JS_INPUT_CONSUMER_UTIL_H

#include <uv.h>

#include "js_input_consumer_context.h"

namespace OHOS {
namespace MMI {

constexpr int32_t INVALID_SUBID = -1;

class JsInputConsumerUtil final {
public:
    JsInputConsumerUtil() = default;
    DISALLOW_COPY_AND_MOVE(JsInputConsumerUtil);
    ~JsInputConsumerUtil() = default;

    static bool GetNamedPropertyBool(napi_env env, napi_value object, const std::string &name, bool &ret);
    static bool GetNamedPropertyInt32(napi_env env, napi_value object, const std::string &name, int32_t &value);
    static bool GetPreKeys(napi_env env, napi_value object, const std::string &name, std::set<int32_t> &preKeys);

    static bool MatchCombinationKeys(const std::shared_ptr<SubscribeInfo> &event,
        const std::shared_ptr<KeyEvent> &keyEvent);
    static int32_t GetPreSubscribeId(const Callbacks &callbacks, const std::shared_ptr<SubscribeInfo> &event);
    static int32_t AddEventCallback(napi_env env, const std::shared_ptr<SubscribeInfo> &event, Callbacks &callbacks);
    static int32_t UnsubscribeCallback(napi_env env, const std::shared_ptr<SubscribeInfo> &event,
        Callbacks &callbacks, int32_t &subscribeId);
    static int32_t DelEventCallback(napi_env env, std::list<std::shared_ptr<SubscribeInfo>> &infos,
        napi_value &handler1, int32_t &subscribeId);
    static void EmitAsyncCallbackWork(const std::shared_ptr<SubscribeInfo> &event);

private:
    static bool IsMatchKeyAction(bool isFinalKeydown, int32_t keyAction);
    static void UvQueueWorkAsyncCallback(uv_work_t *work, int32_t status);
    static void AsyncWorkFn(napi_env env, const std::shared_ptr<SubscribeInfo> &event, napi_value &result);
    static void SetNamedProperty(napi_env env, napi_value object, const std::string &name, int32_t value);
    static void SetNamedProperty(napi_env env, napi_value object, const std::string &name, std::string value);
};
} // namespace MMI
} // namespace OHOS
#endif // JS_INPUT_CONSUMER_UTIL_H
