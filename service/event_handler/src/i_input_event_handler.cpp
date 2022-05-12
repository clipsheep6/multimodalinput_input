/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "i_input_event_handler.h"
#include "error_multimodal.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "IInputEventHandler" };
}
int32_t IInputEventHandler::HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    MMI_HILOGW("Keyboard device dose not support");
    return RET_OK;
}
int32_t IInputEventHandler::HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    MMI_HILOGW("Pointer device dose not support");
    return RET_OK;
}
int32_t IInputEventHandler::HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    MMI_HILOGW("Tp device dose not support");
    return RET_OK;
}
void IInputEventHandler::SetNext(std::shared_ptr<IInputEventHandler> nextHandler)
{
    nextHandler_ = nextHandler;
}
} // namespace MMI
} // namespace OHOS