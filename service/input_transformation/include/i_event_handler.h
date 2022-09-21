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

#ifndef I_EVENT_HANDLER_H
#define I_EVENT_HANDLER_H

#include <memory>
#include <list>

namespace OHOS {
namespace MMI {
    // class KeyEvent;
    class PointerEvent;
    // class IInputContext;
    class IEventHandler {
        public:
            static std::list<std::shared_ptr<IEventHandler>> PrepareHandlers();

            virtual ~IEventHandler() = default;
            // virtual bool HandleEvent(const std::shared_ptr<const KeyEvent>& event) = 0;
            virtual bool HandleEvent(const std::shared_ptr<const PointerEvent>& event) = 0;
            virtual const std::string& GetName() const = 0;
    };
} // namespace MMI
} // namespace OHOS
#endif // I_EVENT_HANDLER_H