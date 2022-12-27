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

#ifndef I_INPUT_PROVIDER_MANAGER
#define I_INPUT_PROVIDER_MANAGER

#include <memory>

namespace OHOS {
namespace MMI {
class IInputProvider;
class IInputProviderManager {
public:
    virtual int32_t AddInputProvider(std::shared_ptr<IInputProvider> inputProvider, void *handle, int32_t queueReadFd) = 0;
    virtual std::shared_ptr<IInputProvider> GetInputProvider(std::string name) = 0;
    virtual std::shared_ptr<IInputProvider> GetProviderByQueueReadFd(int32_t queueReadFd) = 0;
    virtual void RemoveAllInputProvider() = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_PROVIDER_MANAGER