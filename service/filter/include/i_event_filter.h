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

#ifndef I_EVENT_FILTER_H
#define I_EVENT_FILTER_H

#include "iremote_broker.h"

#include "key_event.h"
#include "pointer_event.h"

namespace OHOS {
namespace MMI {
class IEventFilter : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.multimodalinput.IEventFilter");
    virtual bool HandleKeyEvent(const std::shared_ptr<KeyEvent> event) = 0;
    virtual bool HandlePointerEvent(const std::shared_ptr<PointerEvent> event) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_EVENT_FILTER_H