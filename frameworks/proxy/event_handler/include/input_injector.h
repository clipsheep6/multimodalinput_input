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

#ifndef INPUT_INJECTOR_H
#define INPUT_INJECTOR_H

#include "nocopyable.h"

#include "key_event.h"
#include "pointer_event.h"

namespace OHOS {
namespace MMI {
class InputInjector final {
public:
    InputInjector() = default;
    DISALLOW_MOVE(InputInjector);
    ~InputInjector() = default;
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    void InjectKeyEvent(const std::shared_ptr<KeyEvent> &keyEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#if defined(OHOS_BUILD_ENABLE_POINTER) || defined(OHOS_BUILD_ENABLE_TOUCH)
    void InjectPointerEvent(std::shared_ptr<PointerEvent> &pointerEvent);
#endif // OHOS_BUILD_ENABLE_POINTER || OHOS_BUILD_ENABLE_TOUCH
private:
    std::mutex mtx_;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_INJECTOR_H