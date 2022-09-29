/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef INPUT_CONTEXT_H
#define INPUT_CONTEXT_H

#include "i_input_context.h"

#include <memory>
#include <list>

#include "i_input_define.h"
#include "i_device_manager.h"

namespace OHOS {
namespace MMI {
class InputContext : public NonCopyable, public IInputContext {
    public:
        static std::unique_ptr<InputContext> CreateInstance();

    protected:
        static int32_t InitInstance(InputContext* context);

    public:
        virtual ~InputContext() = default;

        virtual const std::unique_ptr<IDeviceManager>& GetInputDeviceManager() const override; 

    protected:
        InputContext() = default;
        int32_t SetDeviceManager(std::unique_ptr<IDeviceManager>& inputDeviceManager);

    private:
        std::unique_ptr<IDeviceManager> inputDeviceManager_;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_CONTEXT_H