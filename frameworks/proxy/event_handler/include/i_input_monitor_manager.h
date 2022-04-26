/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef I_INPUT_MONITOR_MANAGER_H
#define I_INPUT_MONITOR_MANAGER_H

#include <memory>

#include "nocopyable.h"

#include "i_input_event_consumer.h"
#include "define_multimodal.h"

namespace OHOS {
namespace MMI {
class IInputMonitorManager {
public:
    IInputMonitorManager() = default;
    DISALLOW_COPY_AND_MOVE(IInputMonitorManager);
    virtual ~IInputMonitorManager() = default;
    static std::shared_ptr<IInputMonitorManager> GetInstance();

    virtual int32_t AddMonitor(std::shared_ptr<IInputEventConsumer> monitor)
    {
        return RET_ERR;
    }
    virtual void RemoveMonitor(int32_t monitorId)
    {
        return;
    }
    virtual void MarkConsumed(int32_t monitorId, int32_t eventId)
    {
        return;
    }
    virtual void MoveMouse(int32_t offsetX, int32_t offsetY)
    {
        return;
    }
public:
    static inline std::shared_ptr<IInputMonitorManager> iInputMonitorManager_ = nullptr;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_MONITOR_MANAGER_H