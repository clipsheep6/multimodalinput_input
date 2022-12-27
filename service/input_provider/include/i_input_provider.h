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

#ifndef I_INPUT_PROVIDER_H
#define I_INPUT_PROVIDER_H

#include <memory>
#include <string>
#include <nocopyable.h>
#include <sys/epoll.h>

#include "i_input_context.h"

namespace OHOS {
namespace MMI {
class IInputProvider {
public:
    IInputProvider() = default;
    DISALLOW_COPY_AND_MOVE(IInputProvider);
    virtual ~IInputProvider() = default;
    virtual void BindContext(std::shared_ptr<IInputContext> context) { iInputContext_ = context; }
    virtual int32_t Enable() = 0;
    virtual int32_t Disable() = 0;
    virtual void EventDispatch(epoll_event &ev) = 0;
    virtual std::string GetName() = 0;

protected:
    std::shared_ptr<IInputContext> GetInputContext() { return iInputContext_; }
    
private:
    std::shared_ptr<IInputContext> iInputContext_;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_PROVIDER_H