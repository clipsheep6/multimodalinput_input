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

#include "touch_2_key_handler.h"
#include "mmi_log.h"
#include "error_multimodal.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "Touch2KeyHandler"};
} // namespace

void Touch2KeyHandler::HandleKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
{
    context_->StatBegin();
    MMI_HILOGI("is user plugin");
    context_->StatEnd();
    nextHandler_->HandleKeyEvent(keyEvent);
}

void Touch2KeyHandler::HandlePointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    context_->StatBegin();
    MMI_HILOGI("is user plugin");
    context_->StatEnd();
    nextHandler_->HandlePointerEvent(pointerEvent);
}

void Touch2KeyHandler::HandleTouchEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    context_->StatBegin();
    MMI_HILOGI("is user plugin");
    context_->StatEnd();
    nextHandler_->HandleTouchEvent(pointerEvent);
}
} // namespace MMI
} // namespace OHOS