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
#ifndef I_SUBSCRIBER_EVENT_HANDLER_H
#define I_SUBSCRIBER_EVENT_HANDLER_H
#include <memory>

#include "key_option.h"
#include "uds_session.h"

namespace OHOS {
namespace MMI {
class ISubscriberEventHandler {
public:
    virtual int32_t SubscribeKeyEvent(SessionPtr sess, int32_t subscribeId,
            const std::shared_ptr<KeyOption> keyOption) = 0;
    virtual int32_t UnSubscribeKeyEvent(SessionPtr sess, int32_t subscribeId) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif  // I_SUBSCRIBER_EVENT_HANDLER_H
