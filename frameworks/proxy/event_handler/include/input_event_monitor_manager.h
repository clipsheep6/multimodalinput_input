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
#ifndef INPUT_EVENT_MONITOR_MANAGER_H
#define INPUT_EVENT_MONITOR_MANAGER_H

#include <list>

#include "nocopyable.h"

#include "key_event.h"
#include "multimodal_event_handler.h"
#include "pointer_event.h"
#include "proto.h"
#include "i_input_event_monitor_manager.h"

namespace OHOS {
namespace MMI {
struct MonitorItem {
    int32_t id;
    std::function<void (std::shared_ptr<KeyEvent>)> keyEventMonitor;
    bool operator == (const MonitorItem& item) const
    {
        return id == item.id;
    }
    std::string name;
    std::function<void (std::shared_ptr<PointerEvent>)> TouchPadEventMonitor;
};

class InputEventMonitorManager : public IInputEventMonitorManager{
public:
    InputEventMonitorManager();
    DISALLOW_COPY_AND_MOVE(InputEventMonitorManager);
    ~InputEventMonitorManager();

    int32_t AddInputEventMontior(std::function<void (std::shared_ptr<KeyEvent>)> keyEventMonitor) override;
    void RemoveInputEventMontior(int32_t monitorId) override;
    int32_t OnMonitorInputEvent(std::shared_ptr<KeyEvent> keyEvent) override;

    int32_t AddInputEventTouchpadMontior(std::function<void (std::shared_ptr<PointerEvent>)>
                                        TouchPadEventMonitor) override;
    void RemoveInputEventTouchpadMontior(int32_t monitorId) override;
    int32_t OnTouchpadMonitorInputEvent(std::shared_ptr<PointerEvent> pointerEvent) override;

public:
    static constexpr int32_t INVALID_MONITOR_ID { -1 };

private:
    std::list<MonitorItem> monitors_;
};

} // namespace MMI
} // namespace OHOS
#endif // INPUT_EVENT_MONITOR_MANAGER_H
