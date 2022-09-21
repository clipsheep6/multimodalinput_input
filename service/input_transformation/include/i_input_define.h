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

#ifndef I_INPUT_DEFINE_H
#define I_INPUT_DEFINE_H

#include <cstdint>
#include <cstddef>

namespace OHOS {
namespace MMI {

constexpr int INVALID_FD = -1;

struct NonCopyable {
    NonCopyable() = default;
    virtual ~NonCopyable() = default;;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = delete;
};

#define CASE_STR(item) case item: do {return #item;} while(0)

inline constexpr size_t LongsOfBits(int32_t bitsCount)  {
    return (bitsCount / (sizeof(long) * 8)) + !!(bitsCount % (sizeof(long) * 8));
}


#define LENTH_OF_ARRAY(arr) (sizeof(arr) / sizeof(arr[0]))
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_DEFINE_H