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

#include "kernel_event_base.h"

#include "mmi_log.h"

namespace OHOS {
namespace MMI {
KernelEventBase::KernelEventBase(int32_t devIndex, int32_t action)
    : devIndex_(devIndex), action_(action), actionTime_(-1)
{}

int32_t KernelEventBase::GetDevIndex() const // TODO:
{
    return devIndex_;
}

int32_t KernelEventBase::GetAction() const
{
    return action_;
}

int64_t KernelEventBase::GetActionTime() const
{
    return actionTime_;
}

void KernelEventBase::SetAction(int32_t action)
{
    action_ = action;
}

void KernelEventBase::SetActionTime(int64_t actionTime)
{
    actionTime_ = actionTime;
}

std::ostream& operator<<(std::ostream& os, const KernelEventBase &r)
{
    return os << '{'
        << "devIndex:" << r.devIndex_ << ','
        << "action:" << r.ActionToStr(r.action_) << ','
        << "actionTime:" << r.actionTime_
        << '}';
}
} // namespace MMI
} // namespace OHOS