/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef I_INPUT_EVENT_MONITOR_MANAGER_H
#define I_INPUT_EVENT_MONITOR_MANAGER_H

#include "key_event.h"
#include "pointer_event.h"
#include "define_multimodal.h"

namespace OHOS {
namespace MMI {
class IInputEventMonitorManager {
public:
    IInputEventMonitorManager() = default;
    virtual ~IInputEventMonitorManager() = default;

    static std::shared_ptr<IInputEventMonitorManager> GetInstance();

    virtual int32_t AddInputEventMontior(std::function<void (std::shared_ptr<KeyEvent>)> keyEventMonitor)
    {
        return RET_ERR;
    }
    virtual void RemoveInputEventMontior(int32_t monitorId)
    {
        return;
    }
    virtual int32_t OnMonitorInputEvent(std::shared_ptr<KeyEvent> keyEvent)
    {
        return RET_ERR;
    }

    virtual int32_t AddInputEventTouchpadMontior(std::function<void (std::shared_ptr<PointerEvent>)>TouchPadEventMonitor)
    {
        return RET_ERR;
    }
    virtual void RemoveInputEventTouchpadMontior(int32_t monitorId)
    {
        return;
    }
    virtual int32_t OnTouchpadMonitorInputEvent(std::shared_ptr<PointerEvent> pointerEvent)
    {
        return RET_ERR;
    }

public:
    static inline std::shared_ptr<IInputEventMonitorManager> iInputEventMonitorManager_ = nullptr;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_EVENT_MONITOR_MANAGER_H
