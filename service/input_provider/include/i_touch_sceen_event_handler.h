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

#ifndef I_TOUCH_SCREEN_EVENT_HANDLE_H
#define I_TOUCH_SCREEN_EVENT_HANDLE_H

#include <memory>
#include "i_input_device.h"

namespace OHOS {
namespace MMI {
class ITouchScreenEventHandler {
public:
    static std::shared_ptr<ITouchScreenEventHandler> CreateInstance();
    virtual ~ITouchScreenEventHandler() = default;
    virtual std::shared_ptr<IInputDevice> GetDevice() = 0;
    virtual void HandleAbsEvent(int32_t code, int32_t value) = 0;
    virtual void HandleKeyEvent(int32_t code, int32_t value) = 0;
    virtual void HandleSyncEvent() = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_TOUCH_SCREEN_EVENT_HANDLE_H