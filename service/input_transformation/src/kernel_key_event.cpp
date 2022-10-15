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

#include <linux/input.h>

#include "kernel_key_event.h"
#include "i_input_define.h"

namespace OHOS {
namespace MMI {

const std::unique_ptr<KernelKeyEvent> KernelKeyEvent::NULL_EVENT;

const char* KernelKeyEvent::ActionToString(int32_t action) {
    switch (action) {
        CASE_STR(ACTION_NONE);
        CASE_STR(ACTION_DOWN);
        CASE_STR(ACTION_UP);
        default:
        return "ACTION_UNKNOWN";
    }

}
KernelKeyEvent::KernelKeyEvent(int32_t deviceId) 
    : KernelEventBase(deviceId, ACTION_NONE), keyCode_(0) {
    }

int32_t KernelKeyEvent::GetKeyCode() const {
    return keyCode_;
}

void KernelKeyEvent::SetKeyCode(int32_t keyCode) {
    keyCode_ = keyCode;
}

const char* KernelKeyEvent::ActionToStr(int32_t action) const {
    return ActionToString(action);
}

} // namespace MMI
} // namespace OHOS