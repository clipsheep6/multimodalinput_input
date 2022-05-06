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

#include "js_util.h"

#include "mmi_log.h"
#include "util_napi.h"
#ifndef OHOS_BUILD_DEVICE_MANAGER_API
#include "error_multimodal.h"
#endif

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JsUtil" };
const std::string GET_REFERENCE = "napi_get_reference_value";
const std::string STRICT_EQUALS = "napi_strict_equals";
const std::string DELETE_REFERENCE = "napi_delete_reference";
const std::string DELETE_ASYNC_WORK = "napi_delete_async_work";
} // namespace
int32_t JsUtil::GetUserData(uv_work_t *work)
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    int32_t *uData = static_cast<int32_t*>(work->data);
    int32_t userData = *uData;
    delete uData;
    delete work;
    return userData;
#else
    return ERROR_UNSUPPORT;
#endif
}

bool JsUtil::IsHandleEquals(napi_env env, napi_value handle, napi_ref ref)
{
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    napi_value handlerTemp = nullptr;
    CHKRB(env, napi_get_reference_value(env, ref, &handlerTemp), GET_REFERENCE);
    bool isEqual = false;
    CHKRB(env, napi_strict_equals(env, handle, handlerTemp, &isEqual), STRICT_EQUALS);
    return isEqual;
#else
    return false;
#endif
}

JsUtil::CallbackInfo::CallbackInfo() {}

JsUtil::CallbackInfo::~CallbackInfo()
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    if (ref != nullptr && env != nullptr) {
        CHKRV(env, napi_delete_reference(env, ref), DELETE_REFERENCE);
        env = nullptr;
    }
#endif
}

AsyncContext::~AsyncContext()
{
    CALL_LOG_ENTER;
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    if (work != nullptr) {
        CHKRV(env, napi_delete_async_work(env, work), DELETE_ASYNC_WORK);
    }
    if (callback != nullptr && env != nullptr) {
        CHKRV(env, napi_delete_reference(env, callback), DELETE_REFERENCE);
        env = nullptr;
    }
#endif
}
} // namespace MMI
} // namespace OHOS