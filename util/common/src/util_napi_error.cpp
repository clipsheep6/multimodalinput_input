/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "util_napi_error.h"

#include "error_multimodal.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace UtilNapiError {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "UtilNapiError" };

const std::map<int32_t, NapiError> NATIVE_ERRORS = {
    { ERROR_NOT_SYSAPI, { COMMON_USE_SYSAPI_ERROR,
        "Permission verification failed, application which is not a system application uses system API." } },
    { INVALID_PARAMETER_FILE_PATH, { COMMON_PARAMETER_ERROR, "Parameter error. Invalid or inaccessible file path."} },
    { INVALID_PARAMETER_CURSOR_STYLE, { COMMON_PARAMETER_ERROR, "Parameter error. Invalid cursor style value."} },
};
} // namespace

bool GetApiError(int32_t code, NapiError& codeMsg)
{
    auto iter = NAPI_ERRORS.find(code);
    if (iter == NAPI_ERRORS.end()) {
        MMI_HILOGE("Error code %{public}d not found", code);
        return false;
    }
    codeMsg = iter->second;
    return true;
}

napi_value CreateBusinessError(napi_env env, int32_t code, std::string msg)
{
    napi_value businessError = nullptr;
    napi_value errorCode = nullptr;
    napi_value errorMsg = nullptr;
    napi_create_int32(env, code, &errorCode);
    napi_create_string_utf8(env, msg.c_str(), NAPI_AUTO_LENGTH, &errorMsg);
    napi_create_error(env, nullptr, errorMsg, &businessError);
    napi_set_named_property(env, businessError, ERR_CODE.c_str(), errorCode);
    return businessError;
}

napi_value CreateBusinessError(napi_env env, int32_t code)
{
    NapiError codeMsg { INTERNAL_ERROR, "Internal Error." };
    auto iter = NATIVE_ERRORS.find(code);
    if (iter != NATIVE_ERRORS.end()) {
        codeMsg = iter->second;
    }

    return CreateBusinessError(env, codeMsg.errorCode, codeMsg.msg);
}
} // namespace OHOS::MMI::UtilNapiError
} // namespace MMI
} // namespace OHOS