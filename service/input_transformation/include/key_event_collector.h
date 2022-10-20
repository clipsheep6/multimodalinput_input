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

#ifndef KEY_EVENT_COLLECTOR_H
#define KEY_EVENT_COLLECTOR_H

#include "i_input_define.h"
#include "kernel_key_event.h"

namespace OHOS {
namespace MMI {
class KeyEventCollector : public NonCopyable {
    public:
        KeyEventCollector(int32_t deviceId);
        virtual ~KeyEventCollector() = default;

        const std::shared_ptr<KernelKeyEvent>& HandleKeyEvent(int32_t eventCode, int32_t eventValue);

        void AfterProcessed();

    private:
        [[maybe_unused]] int32_t deviceId_;
        std::shared_ptr<KernelKeyEvent> keyEvent_;
};
} // namespace MMI
} // namespace OHOS
#endif // KEY_EVENT_COLLECTOR_H