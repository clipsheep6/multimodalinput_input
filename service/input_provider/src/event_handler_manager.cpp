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

#include "event_handler_manager.h"

#include "input_event_handler.h"

namespace OHOS {
namespace MMI {

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventHandlerManager" };
} // namespace
void EventHandlerManager::DispatchEvent(std::shared_ptr<PointerEvent> event)
{
    CHKPV(event);
    auto inputEventNormalizeHandler = InputHandler->GetEventNormalizeHandler();
    CHKPV(inputEventNormalizeHandler);
    inputEventNormalizeHandler->HandleTouchEvent(event);
    if (event->GetPointerAction() == PointerEvent::POINTER_ACTION_UP) {
        ResetTouchUpEvent(event);
    }
}
void EventHandlerManager::DispatchEvent(std::shared_ptr<KeyEvent> event)
{

}

void EventHandlerManager::ResetTouchUpEvent(std::shared_ptr<PointerEvent> event)
{
    CHKPV(event);
    event->RemovePointerItem(event->GetPointerId());
    if (event->GetPointerIds().empty()) {
        MMI_HILOGD("This touch event is final finger up remove this finger");
        event->Reset();
    }
}
} // namespace MMI
} // namespace OHOS