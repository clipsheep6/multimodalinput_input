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

#include "input_provider_manager.h"

#include <cstddef>
#include <memory>

#include "define_multimodal.h"
#include "i_input_provider.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputProviderManager" };
} // namespace

int32_t InputProviderManager::AddInputProvider(std::shared_ptr<IInputProvider> inputProvider)
{
    CALL_DEBUG_ENTER;
    inputProviders.push_back(inputProvider);
    return RET_OK;
}

int32_t InputProviderManager::RemoveInputProvider(std::shared_ptr<IInputProvider> inputProvider)
{
    CALL_DEBUG_ENTER;
    std::vector<std::shared_ptr<IInputProvider>>::iterator itor = std::find(inputProviders.begin(),
        inputProviders.end(), inputProvider);
    if (itor != inputProviders.end()) {
        inputProviders.erase(itor);
        return RET_OK;
    }
    MMI_HILOGE("Remove input provider failed");
    return RET_ERR;
}

std::shared_ptr<IInputProvider> InputProviderManager::GetInputProvider(std::string name)
{
    for (const auto &item : inputProviders) {
        if (item->GetName() == name) {
          return item;
        }
    }
    return nullptr;
}
} // namespace MMI
} // namespace OHOS