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

#include "i_event_transformer.h"

// #include "TouchPadEventTransformer.h"
// #include "Log.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {

std::list<std::shared_ptr<IEventTransformer>> IEventTransformer::CreateTransformers(const std::shared_ptr<INewEventListener>& listener)
{
    std::list<std::shared_ptr<IEventTransformer>> result;
    // if (context == nullptr) {
    //     MMI_HILOGE("Leave, null context");
    //     return result;
    // }

    if (!listener) {
        MMI_HILOGE("Leave, null listener");
        return result;
    }

    // std::shared_ptr<IEventTransformer> transformer = TouchPadEventTransformer::Create(context, listener);
    // if (!transformer) {
    //     MMI_HILOGW("TouchPadEventTransformer Create Failed");
    // } else {
    //     result.push_back(transformer);
    // }

    return result;
}
} // namespace MMI
} // namespace OHOS