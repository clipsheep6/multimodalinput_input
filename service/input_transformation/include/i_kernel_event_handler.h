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

#ifndef I_KERNEL_EVENT_HANDLER_H
#define I_KERNEL_EVENT_HANDLER_H

#include <string>
#include <memory>

namespace OHOS {
namespace MMI {
class KernelKeyEvent;
class AbsEvent;
class ITouchScreenHandler;
class IKernelEventHandler {
public:
    static const std::shared_ptr<IKernelEventHandler>& GetDefault();

    virtual ~IKernelEventHandler() = default;
    virtual void OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event) = 0;
    virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_KERNEL_EVENT_HANDLER_H