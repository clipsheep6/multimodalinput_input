/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef I_KERNEL_EVENT_BASE_H
#define I_KERNEL_EVENT_BASE_H

#include <cstdint>

namespace OHOS {
namespace MMI {
class KernelEventBase {
public:
    virtual ~KernelEventBase() = default;
    int32_t GetDevIndex() const { return devIndex_; }
    int32_t GetAction() const { return action_; }
    void SetAction(int32_t action) { action_ = action; }
    int64_t GetActionTime() const { return actionTime_; }
    void SetActionTime(int64_t actionTime) { actionTime_ = actionTime; }
protected:
    KernelEventBase(int32_t devIndex, int32_t action) : devIndex_(devIndex), action_(action), actionTime_(-1) {}
private:
    const int32_t devIndex_;
    int32_t action_;
    int64_t actionTime_;
};
} // namespace MMI
} // namespace OHOS
#endif // I_KERNEL_EVENT_BASE_H


