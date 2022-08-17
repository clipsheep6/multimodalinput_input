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

#ifndef COOPERATE_STOP_H
#define COOPERATE_STOP_H

#include <cstddef>

#include <napi/native_api.h>

#include "base_context.h"

namespace OHOS::MMI::CooperateStop {
class Context : public BaseContext {
public:
    Context() = delete;
    explicit Context(napi_env env);

    void ParseParams(napi_value *params, size_t paramsCount) override;
};

napi_value Stop(napi_env env, napi_callback_info info);
} // namespace OHOS::MMI::CooperateStop
#endif // COOPERATE_STOP_H
