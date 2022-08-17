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
#ifndef UTIL_NAPI_H
#define UTIL_NAPI_H

#include <cstdint>
#include <string>

#include <napi/native_api.h>

#include "mmi_log.h"

namespace OHOS {
namespace MMI {
#define CHKRV(env, state, desc) \
    do { \
        if ((state) != napi_ok) { \
            MMI_HILOGE("%{public}s failed", std::string(desc).c_str()); \
            auto infoTemp = std::string(__FUNCTION__)+ ": " + std::string(desc) + " failed"; \
            napi_throw_error(env, nullptr, infoTemp.c_str()); \
            return; \
        } \
    } while (0)

#define CHKRP(env, state, desc) \
    do { \
        if ((state) != napi_ok) { \
            MMI_HILOGE("%{public}s failed", std::string(desc).c_str()); \
            auto infoTemp = std::string(__FUNCTION__)+ ": " + std::string(desc) + " failed"; \
            napi_throw_error(env, nullptr, infoTemp.c_str()); \
            return nullptr; \
        } \
    } while (0)

#define CHKRF(env, state, desc) \
    do { \
        if ((state) != napi_ok) { \
            MMI_HILOGE("%{public}s failed", std::string(desc).c_str()); \
            auto infoTemp = std::string(__FUNCTION__)+ ": " + std::string(desc) + " failed"; \
            napi_throw_error(env, nullptr, infoTemp.c_str()); \
            return false; \
        } \
    } while (0)

#define THROWERR(env, desc) \
    do { \
        MMI_HILOGE("%{public}s", (#desc)); \
        auto infoTemp = std::string(__FUNCTION__)+ ": " + #desc; \
        napi_throw_error(env, nullptr, infoTemp.c_str()); \
    } while (0)

namespace UtilNapi {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "UtilNapi" };
} // namespace

bool TypeOf(napi_env env, napi_value value, napi_valuetype type);
napi_value CreateErrorMessage(napi_env env, int32_t errorCode, const std::string &errorMessage);

template <typename Context>
void CallFunction(napi_env env, Context *context, size_t argc, const napi_value *argv)
{
    CHKPV(context);
    if (context->callbackRef_ == nullptr) {
        return;
    }
    napi_value callback = nullptr;
    CHKRV(env, napi_get_reference_value(env, context->callbackRef_, &callback), "napi_get_reference_value");
    napi_value result = nullptr;
    CHKRV(env, napi_call_function(env, nullptr, callback, argc, argv, &result), "napi_call_function");
}
} // namespace UtilNapi
} // namespace MMI
} // namespace OHOS
#endif // UTIL_NAPI_H
