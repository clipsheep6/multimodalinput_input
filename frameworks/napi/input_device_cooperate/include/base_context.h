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

#ifndef COOPERATE_BASE_CONTEXT_H
#define COOPERATE_BASE_CONTEXT_H

#include <cstddef>
#include <cstdint>

#include <napi/native_api.h>

#include "constants.h"
#include "define_multimodal.h"
#include "refbase.h"

namespace OHOS::MMI {
class BaseContext : public RefBase {
public:
    BaseContext() = delete;
    explicit BaseContext(napi_env env);

    napi_env env_ = nullptr;
    napi_async_work asyncWork_ = nullptr;
    napi_deferred deferred_ = nullptr;
    bool isPromise_ = true;
    napi_ref callbackRef_ = nullptr;
    napi_value callback_ = nullptr;
    int32_t errorCode_ = RET_ERR;
    bool parseOk_ = true;

    virtual void ParseParams(napi_value *params, size_t paramsCount) = 0;
};
} // namespace OHOS::MMI
#endif // COOPERATE_BASE_CONTEXT_H
