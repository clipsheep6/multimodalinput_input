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

#include "key_event_collector.h"
#include "time_utils.h"

namespace OHOS {
namespace MMI {
KeyEventCollector::KeyEventCollector(int32_t deviceId) 
: deviceId_(deviceId), keyEvent_(std::make_shared<KernelKeyEvent>(deviceId)) {
}

std::shared_ptr<const KernelKeyEvent> KeyEventCollector::HandleKeyEvent(int32_t code, int32_t value) {
    keyEvent_->SetKeyCode(code);
    keyEvent_->SetAction(value > 0 ? KernelKeyEvent::ACTION_DOWN : KernelKeyEvent::ACTION_UP);
    keyEvent_->SetActionTime(TimeUtils::GetTimeStampMs());
    return keyEvent_;
}

void KeyEventCollector::AfterProcessed() {
    keyEvent_->SetAction(KernelKeyEvent::ACTION_NONE);
}
} // namespace MMI
} // namespace OHOS