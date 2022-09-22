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
#ifndef UTIL_NAPI_ERROR_H
#define UTIL_NAPI_ERROR_H

#include<map>
#include<string>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "securec.h"

#include "utils/log.h"

namespace OHOS {
namespace MMI {

struct NapiError {
    std::string ErrorCode;
    std::string msg;
};

enum NapiErrorCode : int32_t {
    COMMON_PERMISSION_CHECK_ERROR = 201,
    COMMON_PARAMETER_ERROR = 401,
    DEVICE_ID_ERROR = 3900001,
    POINTER_WINDOW_ID_ERROR = 4000001,
    POINTER_NO_PERMISSION = 4000002,
    MONITOR_REGISTER_EXCEED_MAX = 4100001,
    CONSUMER_PARAMETER_ERROR = 4200401,
    SIMULATOR_PARAMETER_ERROR = 4300401,
    COOPERATOR_TARGET_DEV_DESCRIPTOR_ERROR = 4400001,
    COOPERATOR_DEVICE_ID_ERROE = 4400002,
    COOPERATOR_FAIL = 4400003,
};

const std::map<int32_t, NapiError> NAPI_ERRORS = {
    {COMMON_PERMISSION_CHECK_ERROR,  {"201", "Permission denied. An attempt was made to %s forbidden by permission:%s."}},
    {COMMON_PARAMETER_ERROR,  {"401", "Parameter error. The type of %s must be %s."}},
    {DEVICE_ID_ERROR, {"3900001", "Invalid input device ID"}},
    {POINTER_WINDOW_ID_ERROR, {"4000001", "Invalid window ID"}},
    {POINTER_NO_PERMISSION, {"4000002", "No permission to set the current window style"}},
    {MONITOR_REGISTER_EXCEED_MAX, {"4100001", "Maximum number of listeners exceeded for a single process"}},
    {CONSUMER_PARAMETER_ERROR, {"401", "Incorrect keyoptions in the event subscription or unsubscription API"}},
    {SIMULATOR_PARAMETER_ERROR, {"401", "Invalid KeyEvent orchestration settings for input event injection"}},
    {COOPERATOR_TARGET_DEV_DESCRIPTOR_ERROR, {"4400001", "Incorrect descriptor for the target device"}},
    {COOPERATOR_DEVICE_ID_ERROE, {"4400002", " Incorrect ID of the input device for screen hop"}},
    {COOPERATOR_FAIL, {"4400003", "Screen hop failed"}},
};

#define THROWERR_API9(env, code, ...) \
    do { \
        MMI_HILOGE("%{public}s", (#code)); \
        auto res = UtilNapiError::GetApiError(code); \
        if (res != nullptr) { \
            char buf[100]; \
            if (sprintf_s(buf, sizeof(buf), res->msg.c_str(), ##__VA_ARGS__) > 0) { \
                napi_throw_error(env, res->ErrorCode.c_str(), buf); \
            } \
        }\
    } while (0)

namespace UtilNapiError {
NapiError* GetApiError(int32_t code);
} // namespace UtilNapiError
} // namespace MMI
} // namespace OHOS
#endif // UTIL_NAPI_ERROR_H