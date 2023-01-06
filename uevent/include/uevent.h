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

#ifndef UEVENT_COMMON_H
#define UEVENT_COMMON_H

#include <string>
#include <nocopyable.h>

namespace OHOS {
namespace MMI {
namespace UEVENT {

inline constexpr size_t UEVENT_NUMBER_MAX_WIDTH { 32 };
inline constexpr size_t UEVENT_LINE_SIZE { 16384 };
inline constexpr size_t ARRAY_POS_2 { 2 };
inline constexpr int ARG_NUM_3 { 3 };
inline constexpr int BASE8 { 8 };
inline constexpr int BASE10 { 10 };
inline const std::string EMPTY_STRING {};
inline const std::string DEV_INPUT_PATH { "/dev/input/" };
inline const std::string SYS_INPUT_PATH { "/sys/class/input/" };

struct Property {
    std::string name_;
    std::string value_;
    int num_ {};

    Property(const char *name)
        : name_(name != nullptr ? name : "") {}

    Property(const std::string &name)
        : name_(name) {}

    Property(const char *name, const char *value)
        : name_(name != nullptr ? name : ""), value_(value != nullptr ? value : "") {}

    Property(const std::string &name, const std::string &value)
        : name_(name), value_(value) {}

    Property(const std::string &name, const std::string &value, int num)
        : name_(name), value_(value), num_(num) {}

    bool operator<(const Property &other) const;
    bool operator!=(const Property &other) const;
};

inline bool Property::operator<(const Property &other) const
{
    return (name_ < other.name_);
}

inline bool Property::operator!=(const Property &other) const
{
    if (name_ != other.name_) {
        return true;
    }
    if (value_ != other.value_) {
        return true;
    }
    if (num_ != other.num_) {
        return true;
    }

    return false;
}

class UEvent {
public:
    UEvent() = default;
    ~UEvent() = default;
    DISALLOW_COPY_AND_MOVE(UEvent);
};
} // UEVENT
} // MMI
} // OHOS

#endif // UEVENT_COMMON_H
