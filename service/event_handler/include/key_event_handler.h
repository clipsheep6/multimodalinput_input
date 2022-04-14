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

#ifndef KEY_EVENT_HANDLER_H
#define KEY_EVENT_HANDLER_H

#include <memory>

#include "i_input_event_handler.h"

#define KEYSTATUS 0

namespace OHOS {
namespace MMI {
class KeyEventHandler : public IInputEventHandler {
public:
    KeyEventHandler() = default;
    ~KeyEventHandler() = default;
    int32_t HandleLibinputEvent(libinput_event* event) override;
    int32_t HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent) override;

private:
    int32_t Normalize(libinput_event *event, std::shared_ptr<KeyEvent> keyEvent);
    void Repeat(const std::shared_ptr<KeyEvent> keyEvent);
    void AddHandleTimer(int32_t timeout = 300);

private:
    int32_t timerId_ = -1;
};
} // namespace MMI
} // namespace OHOS
#endif // KEY_EVENT_HANDLER_H