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

#ifndef I_TOUCH_SCREEN_HANDLE_H
#define I_TOUCH_SCREEN_HANDLE_H

#include <memory>

#include "abs_event.h"
#include "i_device.h"

namespace OHOS {
namespace MMI {
class PointerEvent;
class ITouchScreenHandler {
public:
    static std::shared_ptr<ITouchScreenHandler> CreateInstance();
    virtual ~ITouchScreenHandler() = default;
    virtual std::shared_ptr<PointerEvent> GetPointerEvent() = 0;
    virtual void OnInputEvent(const std::shared_ptr<AbsEvent> event) = 0;
    virtual int32_t BindInputDevice(const std::shared_ptr<IDevice> inputDevice) = 0;
    virtual int32_t UnbindInputDevice(const std::shared_ptr<IDevice> inputDevice) = 0;
    virtual std::shared_ptr<IDevice> GetDevice() = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_TOUCH_SCREEN_HANDLE_H