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

#ifndef I_EVENT_HANDLE_H
#define I_EVENT_HANDLE_H

#include "i_input_device.h"

#include <memory>
#include <linux/input.h>

#include "i_event_handler_manager.h"

namespace OHOS {
namespace MMI {

class IEventHandler {
public:
    static std::shared_ptr<IEventHandler> CreateInstance();
    virtual ~IEventHandler() = default;
    virtual void SetDeviceId(int32_t deviceId) { deviceId_ = deviceId; }
    virtual int32_t GetDeviceId() { return deviceId_ ; }
    virtual void HandleEvent(const struct input_event& event) = 0;
    virtual void SetAxisInfo(std::shared_ptr<IInputDevice::AxisInfo> xInfo, std::shared_ptr<IInputDevice::AxisInfo> yInfo) = 0;
    virtual void SetEventHandlerManager(std::shared_ptr<IEventHandlerManager> eventhandleManager) = 0;

private:
    int32_t deviceId_ = -1;
};
} // namespace MMI
} // namespace OHOS
#endif // I_EVENT_HANDLE_H