/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "raw_data.h"

namespace OHOS {
namespace MMI {
OHOS::MMI::RawData::RawData() {}
OHOS::MMI::RawData::~RawData() {}
OHOS::MMI::RawData::RawData(const int32_t dx, const int32_t dy)
{
    dx_ = dx;
    dy_ = dy;
}

int32_t OHOS::MMI::RawData::GetDx() const
{
    return dx_;
}

void OHOS::MMI::RawData::SetDx(int32_t dx)
{
    dx_ = dx;
}

int32_t OHOS::MMI::RawData::GetDy() const
{
    return dy_;
}

void OHOS::MMI::RawData::SetDy(int32_t dy)
{
    dy_ = dy;
}

bool OHOS::MMI::RawData::WriteToParcel(Parcel &out) const
{
    if (!out.WriteInt32(dx_)) {
        return false;
    }

    if (!out.WriteInt32(dy_)) {
        return false;
    }
    
    return true;
}

bool OHOS::MMI::RawData::ReadFromParcel(Parcel &in)
{
    if (!in.ReadInt32(dx_)) {
        return false;
    }

    if (!in.ReadInt32(dy_)) {
        return false;
    }
    
    return true;
}
}
} // namespace OHOS::MMI
