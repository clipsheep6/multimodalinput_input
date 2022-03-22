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

#include "js_util.h"

namespace OHOS {
namespace MMI {
void JsUtil::GetCallbackInfo(void* data, CallbackInfo& temp)
{
    CallbackInfo *cb = static_cast<CallbackInfo*>(data);
    temp.ref = cb->ref;
    temp.asyncWork = cb->asyncWork;
    temp.deferred = cb->deferred;
    temp.promise = cb->promise;
    temp.data.ids = cb->data.ids;
    temp.data.device = cb->data.device;
    temp.data.keystrokeAbility = cb->data.keystrokeAbility;
    delete cb;
    cb = nullptr;
}
} // namespace MMI
} // namespace OHOS