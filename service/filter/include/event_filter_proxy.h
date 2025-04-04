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

#ifndef EVENT_FILTER_PROXY_H
#define EVENT_FILTER_PROXY_H
#include "iremote_proxy.h"

#include "i_event_filter.h"

namespace OHOS {
namespace MMI {
class EventFilterProxy final : public IRemoteProxy<IEventFilter> {
public:
    explicit EventFilterProxy(const sptr<IRemoteObject> &impl);
    DISALLOW_COPY_AND_MOVE(EventFilterProxy);
    ~EventFilterProxy() override = default;
    bool HandleKeyEvent(const std::shared_ptr<KeyEvent> event) override;
    bool HandlePointerEvent(const std::shared_ptr<PointerEvent> event) override;
private:
    static inline BrokerDelegator<EventFilterProxy> delegator_;
};
} // namespace MMI
} // namespace OHOS
#endif // EVENT_FILTER_PROXY_H