/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_POINTER_EVENT_FILTER_DEATH_RECIPIENT_H
#define OHOS_POINTER_EVENT_FILTER_DEATH_RECIPIENT_H

#include "iremote_broker.h"

namespace OHOS {
namespace MMI {
class EventFilterDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit EventFilterDeathRecipient(const std::function<void(const wptr<IRemoteObject> &object)>
                                                  &deathCallback);
    virtual ~EventFilterDeathRecipient() = default;
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;

private:
    std::function<void(const wptr<IRemoteObject> &object)> deathCallback_;
};
} // namespace MMI
} // namespace OHOS

#endif // OHOS_POINTER_EVENT_FILTER_DEATH_RECIPIENT_H
