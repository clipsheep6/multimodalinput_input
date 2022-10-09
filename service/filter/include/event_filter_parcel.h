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

#ifndef EVENT_FILTER_PARCEL_H
#define EVENT_FILTER_PARCEL_H

#include "nocopyable.h"
#include "parcel.h"

#include "pointer_event.h"

namespace OHOS {
namespace MMI {
class PointerEventParcel final : public Parcelable {
public:
    PointerEventParcel() = default;
    DISALLOW_COPY_AND_MOVE(PointerEventParcel);
    ~PointerEventParcel() override = default;

    bool Marshalling(Parcel& out) const override;
    static PointerEventParcel *Unmarshalling(Parcel& in);
    mutable std::shared_ptr<PointerEvent> data_ { nullptr };
};
} // namespace MMI
} // namespace OHOS
#endif // EVENT_FILTER_PARCEL_H