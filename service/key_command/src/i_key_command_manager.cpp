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

#include "i_key_command_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "IKeyCommandManager" };
} // namespace

std::shared_ptr<IKeyCommandManager> IKeyCommandManager::CreateIstance()
{
    return std::make_shared<IKeyCommandManager>();    

}

int32_t IKeyCommandManager::HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPR(keyEvent, ERROR_NULL_POINTER);
    CHKPR(nextHandler_, ERROR_NULL_POINTER);
    return nextHandler_->HandleKeyEvent(keyEvent);
}
} // namespace MMI
} // namespace OHOS