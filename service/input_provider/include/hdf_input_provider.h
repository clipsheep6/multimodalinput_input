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

#ifndef HDF_INPUT_PROVIDER
#define HDF_INPUT_PROVIDER

#include <memory>
#include <map>

#include "i_input_provider.h"

namespace OHOS {
namespace MMI {
class HDFInputProvider : public IInputProvider {
public:
    HDFInputProvider() = default;
    virtual ~HDFInputProvider() = default;
    virtual int32_t Enable() override;
    virtual int32_t Disable() override;
    virtual std::string GetName() override;
};
} // namespace MMI
} // namespace OHOS
#endif // HDF_INPUT_PROVIDER