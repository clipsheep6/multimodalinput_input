/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_MULTIMDOALINPUT_RAW_DATA_H
#define OHOS_MULTIMDOALINPUT_RAW_DATA_H

#include "parcel.h"

namespace OHOS {
namespace MMI {

class RawData {
public:
    RawData();
    RawData(const int32_t dx, const int32_t dy);
    virtual ~RawData();
public:
    // Get or set the dx when the Pointer is move
    int32_t GetDx() const;
    void SetDx(int32_t dx);

    // Get or set the dy when the Pointer is move
    int32_t GetDy() const;
    void SetDy(int32_t dy);

public:
    bool WriteToParcel(Parcel &out) const;
    bool ReadFromParcel(Parcel &in);

private:
    int32_t dx_ = 0;
    int32_t dy_ = 0;
};
}
}// namespace OHOS::MMI
#endif // OHOS_MULTIMDOALINPUT_RAW_DATA_H
