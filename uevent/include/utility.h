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

#ifndef UEVENT_UTIL_H
#define UEVENT_UTIL_H

#include <sstream>
#include <string>

namespace OHOS {
namespace MMI {
namespace UEVENT {

template<typename, typename = std::void_t<>>
struct IsStreamable : public std::false_type {};

template<typename T>
struct IsStreamable<T, std::void_t<decltype(operator<<(std::declval<std::ostream>(), std::declval<T>()))>>
    : public std::true_type {};

class Utility {
public:
    static size_t CopyNulstr(char *dest, size_t size, const char *src);
    static bool StartWith(const char *str, const char *prefix);
    static bool StartWith(const std::string &str, const std::string &prefix);

    static ssize_t GetSysCoreLinkValue(const char *slink, const char *syspath, char *value, size_t size);
    static void RemoveTrailingChars(char *path, char c);
    static void RemoveTrailingChars(std::string &path, const std::string &toRemoved);
    static bool IsEmpty(const char *str);
    static bool IsEqual(const char *s1, const char *s2);

    template <typename... Args,
              typename = std::enable_if_t<(IsStreamable<Args>::value && ...), std::string>>
    static std::string ConcatAsString(Args&&... args)
    {
        std::ostringstream ss;
        (..., (ss << std::forward<Args>(args)));
        return ss.str();
    }
};

inline bool Utility::IsEmpty(const char *str)
{
    return ((str == nullptr) || (str[0] == '\0'));
}

inline bool Utility::IsEqual(const char *s1, const char *s2)
{
    if (IsEmpty(s1)) {
        return IsEmpty(s2);
    } else if (IsEmpty(s2)) {
        return false;
    }
    return (strcmp(s1, s2) == 0);
}
} // UEVENT
} // MMI
} // OHOS

#endif // UEVENT_UTIL_H
