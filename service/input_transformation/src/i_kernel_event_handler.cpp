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

#include "i_kernel_event_handler.h"

namespace OHOS {
namespace MMI {

class DefaultHandler : public IKernelEventHandler {
    virtual void OnInputEvent(const std::shared_ptr<const KernelKeyEvent> event) override
    {}
    virtual void OnInputEvent(const std::shared_ptr<const AbsEvent> event) override
    {}
};

const std::shared_ptr<IKernelEventHandler> IKernelEventHandler::GetDefault()
{
    static std::shared_ptr<IKernelEventHandler> defaultHandler = std::make_shared<DefaultHandler>();
    return defaultHandler;
}
} // namespace MMI
} // namespace OHOS