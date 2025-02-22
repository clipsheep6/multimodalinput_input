/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef DISPLAY_EVENT_MONITOR_STUB_H
#define DISPLAY_EVENT_MONITOR_STUB_H

#include <string>

namespace OHOS {
namespace MMI {
class DisplayEventMonitor final {
public:
    DisplayEventMonitor() {}
    virtual ~DisplayEventMonitor() {}
    static std::shared_ptr<DisplayEventMonitor> GetInstance()
    {
        std::shared_ptr<DisplayEventMonitor> instance = std::make_shared<DisplayEventMonitor>();
        return instance;
    }
    void UpdateShieldStatusOnScreenOn() {}
    void UpdateShieldStatusOnScreenOff() {}
    void InitCommonEventSubscriber() {}
    bool IsCommonEventSubscriberInit()
    {
        return false;
    }
    void SetScreenStatus(const std::string &screenStatus)
    {
        screenStatus_ = screenStatus;
    }
    const std::string GetScreenStatus()
    {
        return screenStatus_;
    }
    void SetScreenLocked(bool isLocked)
    {
        isScreenLocked_ = isLocked;
    }
    bool GetScreenLocked() const
    {
        return isScreenLocked_;
    }
private:
    std::string screenStatus_;
    bool isScreenLocked_ { false };
};
#define DISPLAY_MONITOR DisplayEventMonitor::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // DISPLAY_EVENT_MONITOR_STUB_H
