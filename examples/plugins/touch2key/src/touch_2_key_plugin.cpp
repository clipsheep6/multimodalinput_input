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

#include "touch_2_key_plugin.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "Touch2KeyPlugin" };
}

bool Touch2KeyPlugin::Init(IInputEventPluginContext *context)
{
    context_ = context;
    if (context_ == nullptr) {
        MMI_HILOGE("Context assignment failed");
        return false;
    }
    context_->SetEventHandler(std::make_shared<Touch2KeyHandler>(context));
    return true;
}
} // namespace MMI
} // namespace OHOS