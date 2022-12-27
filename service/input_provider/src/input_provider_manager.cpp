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
#include <dlfcn.h>
#include <memory>

#include "define_multimodal.h"
#include "i_input_provider.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputProviderManager" };
} // namespace

int32_t InputProviderManager::AddInputProvider(std::shared_ptr<IInputProvider> inputProvider, void *handle, int32_t queueReadFd)
{
    CALL_DEBUG_ENTER;
    auto it = inputProviders.find(queueReadFd);
    if (it != inputProviders.end()) {
        MMI_HILOGE("Duplicate add provider, queueReadFd:%{public}d", queueReadFd);
        return RET_ERR;
    }
    inputProviders.emplace(queueReadFd, std::make_tuple(inputProvider, handle));
    return RET_OK;
}

std::shared_ptr<IInputProvider> InputProviderManager::GetInputProvider(std::string name)
{
    for (const auto &item : inputProviders) {
        auto [inputProvider, handle] = item.second;
        if (inputProvider == nullptr) {
            continue;
        }
        if (inputProvider->GetName() == name) {
            return inputProvider;
        }
    }
    return nullptr;
}

std::shared_ptr<IInputProvider> InputProviderManager::GetProviderByQueueReadFd(int32_t queueReadFd)
{
    auto it = inputProviders.find(queueReadFd);
    if (it == inputProviders.end()) {
        return nullptr;
    }
    return std::get<0>(it->second);
}

void InputProviderManager::RemoveAllInputProvider()
{
    auto it = inputProviders.begin();
    while (it != inputProviders.end()) {
        auto [inputProvider, handle] = it->second;
        if (inputProvider != nullptr) {
            inputProvider->Disable();
        }
        if (handle != nullptr) {
            dlclose(handle);
        }
        inputProviders.erase(it++);
    }
    
}
} // namespace MMI
} // namespace OHOS