/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef OHOS_INPUT_MONITOR_H
#define OHOS_INPUT_MONITOR_H
#include <map>
#include <vector>
#include <algorithm>
#include "uds_session.h"
#include "singleton.h"
#include <list>
#include "key_event.h"
#include "pointer_event.h"
#include "struct_multimodal.h"

namespace OHOS {
namespace MMI {
struct MonitorItem {
    int32_t eventType;
    SessionPtr session;
    bool operator == (const struct MonitorItem& item)
    {
        return this->eventType == item.eventType && this->session == item.session;
    }
};

class InputEventMonitorManager {
public:
    InputEventMonitorManager();
    virtual ~InputEventMonitorManager();

    int32_t AddInputEventMontior(int32_t eventType, SessionPtr session);
    void RemoveInputEventMontior(int32_t eventType, SessionPtr session);
    void ReportKeyEvent(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent);

    int32_t AddInputEventTouchpadMontior(int32_t eventType, SessionPtr session);
    void RemoveInputEventTouchpadMontior(int32_t eventType, SessionPtr session);
    bool ReportTouchpadEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent);

private:
    std::mutex mu_;
    std::list<MonitorItem> monitors_;
    std::list<MonitorItem> monitorsTouch_;

    void OnMonitorInputEvent(std::shared_ptr<KeyEvent> keyEvent);
    void OnTouchpadMonitorInputEvent(std::shared_ptr<PointerEvent> PointerEvent);
};
}
}
#define InputMonitorServiceMgr OHOS::Singleton<OHOS::MMI::InputEventMonitorManager>::GetInstance()
#endif
