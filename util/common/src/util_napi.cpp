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

#include "util_napi.h"

namespace OHOS::MMI::UtilNapi {
bool TypeOf(napi_env env, napi_value value, napi_valuetype type)
{
    napi_valuetype valueType = napi_undefined;
    CHKRF(env, napi_typeof(env, value, &valueType), "napi_typeof");
    return (valueType == type);
}

napi_value CreateErrorMessage(napi_env env, int32_t errorCode, const std::string &errorMessage)
{
    napi_value code = nullptr;
    CHKRP(env, napi_create_int32(env, errorCode, &code), "create_int32");
    napi_value message = nullptr;
    CHKRP(env, napi_create_string_utf8(env, errorMessage.c_str(), NAPI_AUTO_LENGTH, &message), "create_string");

    napi_value result = nullptr;
    CHKRP(env, napi_create_object(env, &result), "create_object");
    CHKRP(env, napi_set_named_property(env, result, "code", code), "set_named_property");
    CHKRP(env, napi_set_named_property(env, result, "message", message), "set_named_property");
    return result;
}
} // namespace OHOS::MMI::UtilNapi
