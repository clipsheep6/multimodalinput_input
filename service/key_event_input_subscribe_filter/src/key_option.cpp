/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "key_option.h"

namespace OHOS {
namespace MMI {
std::vector<int32_t> KeyOption::GetPreKeys() const
{
    return preKeys_;
}
void KeyOption::SetPreKeys(const std::vector<int32_t> &preKeys)
{
    preKeys_ = preKeys;
    preKeySize_ = preKeys_.size();
}

uint32_t KeyOption::GetPreKeySize()
{
    return preKeySize_;
}

int32_t KeyOption::GetFinalKey() const
{
    return finalKey_;
}

void KeyOption::SetFinalKey(int32_t finalKey)
{
    finalKey_ = finalKey;
}

bool KeyOption::IsFinalKeyDown() const
{
    return isFinalKeyDown_;
}
void KeyOption::SetFinalKeyDown(bool pressed)
{
    isFinalKeyDown_ = pressed;
}

int32_t KeyOption::GetFinalKeyDownDuration() const
{
    return finalKeyDownDuration_;
}
void KeyOption::SetFinalKeyDownDuration(int32_t duration)
{
    finalKeyDownDuration_ = duration;
}
}
}