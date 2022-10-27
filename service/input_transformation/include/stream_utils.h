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

#ifndef STREAM_UTIL_H
#define STREAM_UTIL_H

#include <deque>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

namespace OHOS {
namespace MMI {

template <typename T>
std::ostream& operator<<(std::ostream& outStream, const std::shared_ptr<T>& item) {
    if (item) {
        return item->operator<<(outStream);
    }

    return outStream << "(null)";
}

template <typename T>
std::ostream& operator<<(std::ostream& outStream, const std::unique_ptr<T>& item) {
    if (item) {
        return item->operator<<(outStream);
    }
    return outStream << "(null)";
}

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& outStream, const std::pair<T1, T2>& item) {
    return outStream << item.first << ':' << item.second;
}

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& outStream, const std::map<T1, T2>& items) {
    outStream << '{';

    bool isFirst = true;
    for (const auto& item : items) {
        if (isFirst) {
            isFirst = false;
        } else {
            outStream << ',';
        }
        outStream << item;
    }

    outStream << '}';
    return outStream;
}


template <typename T>
std::ostream& operator<<(std::ostream& outStream, const std::list<T>& items) {
    outStream << '[';

    bool isFirst = true;
    for (const auto& item : items) {
        if (isFirst) {
            isFirst = false;
        } else {
            outStream << ',';
        }
        outStream << item;
    }

    outStream << ']';

    return outStream;
}

template <typename T>
std::ostream& operator<<(std::ostream& outStream, const std::vector<T>& items) {
    outStream << '[';

    bool isFirst = true;
    for (const auto& item : items) {
        if (isFirst) {
            isFirst = false;
        } else {
            outStream << ',';
        }
        outStream << item;
    }

    outStream << ']';

    return outStream;
}

template <typename T>
std::ostream& operator<<(std::ostream& outStream, const std::deque<T>& items) {
    outStream << '[';

    bool isFirst = true;
    for (const auto& item : items) {
        if (isFirst) {
            isFirst = false;
        } else {
            outStream << ',';
        }
        outStream << item;
    }

    outStream << ']';

    return outStream;
}

template <typename T>
std::ostream& operator<<(std::ostream& outStream, const std::set<T>& items) {
    outStream << '[';

    bool isFirst = true;
    for (const auto& item : items) {
        if (isFirst) {
            isFirst = false;
        } else {
            outStream << ',';
        }
        outStream << item;
    }

    outStream << ']';

    return outStream;
}
} // namespace MMI
} // namespace OHOS
#endif // STREAM_UTIL_H