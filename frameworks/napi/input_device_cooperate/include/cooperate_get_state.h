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

#ifndef COOPERATE_GET_STATE_H
#define COOPERATE_GET_STATE_H

#include <cstddef>
#include <string>

#include <napi/native_api.h>

#include "base_context.h"

namespace OHOS::MMI::CooperateGetState {
class Context : public BaseContext {
public:
    Context() = delete;
    explicit Context(napi_env env);

    std::string deviceDescriptor_;
    bool state_;

    void ParseParams(napi_value *params, size_t paramsCount) override;
};

napi_value GetState(napi_env env, napi_callback_info info);
} // namespace OHOS::MMI::CooperateGetState
#endif // COOPERATE_GET_STATE_H
