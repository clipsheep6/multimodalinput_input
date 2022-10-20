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

#ifndef KERNEL_KEY_EVENT_H
#define KERNEL_KEY_EVENT_H

#include <memory>

#include "kernel_event_base.h"

namespace OHOS {
namespace MMI {
    class KernelKeyEvent : public KernelEventBase {
        public:
            static const std::unique_ptr<KernelKeyEvent> NULL_EVENT;

            static constexpr int32_t ACTION_NONE = 0;
            static constexpr int32_t ACTION_DOWN = 1;
            static constexpr int32_t ACTION_UP = 2;
            
        public:
            static const char* ActionToString(int32_t action);

        public:
            KernelKeyEvent(int32_t deviceId);
            ~KernelKeyEvent() = default;

            int32_t GetKeyCode() const;
            void SetKeyCode(int32_t keyCode);

        protected:
            virtual const char* ActionToStr(int32_t action) const override;

        private:
            int32_t keyCode_;
    };
} // namespace MMI
} // namespace OHOS
#endif // KERNEL_KEY_EVENT_H