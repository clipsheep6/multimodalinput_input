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

#ifndef POINTER_STYLE_H
#define POINTER_STYLE_H

#include <iostream>

namespace OHOS {
namespace MMI {
struct PointerStyle {
    int32_t size { -1 };
    uint32_t color { 0x00000000 };
    int32_t id { 0 };
    int32_t options { 0 };
    bool operator==(const PointerStyle &rhs) const
    {
        return id == rhs.id && size == rhs.size && color == rhs.color && options == rhs.options;
    }
};
} // namespace MMI
} // namespace OHOS
#endif // POINTER_STYLE_H