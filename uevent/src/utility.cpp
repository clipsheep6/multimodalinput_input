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
#include "utility.h"
#include <unistd.h>

#include "mmi_log.h"
#include "uevent.h"


namespace OHOS {
namespace MMI {
namespace UEVENT {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, ::OHOS::MMI::MMI_LOG_DOMAIN, "UeventUtil" };
}

size_t Utility::CopyNulstr(char *dest, size_t size, const char *src)
{
    CHKPR(dest, 0);
    CHKPR(src, 0);

    size_t len = strlen(src);
    if (len >= size) {
        if (size >= 1) {
            len = size - 1;
        } else {
            len = 0;
        }
    }
    if (len > 0) {
        mempcpy(dest, src, len);
    }
    if (size > 0) {
        dest[len] = '\0';
    }
    return len;
}

bool Utility::StartWith(const char *str, const char *prefix)
{
    size_t prefixlen = strlen(prefix);
    return (prefixlen > 0 ? (strncmp(str, prefix, strlen(prefix)) == 0) : false);
}

bool Utility::StartWith(const std::string &str, const std::string &prefix)
{
    if (str.size() < prefix.size()) {
        return false;
    }
    return (str.compare(0, prefix.size(), prefix) == 0);
}

ssize_t Utility::GetSysCoreLinkValue(const char *slink, const char *syspath, char *value, size_t size)
{
    CHKPR(slink, -1);
    CHKPR(syspath, -1);
    char target[PATH_MAX];

    std::string sPath { Utility::ConcatAsString(syspath, "/", slink) };
    ssize_t len = readlink(sPath.c_str(), target, sizeof(target));
    if ((len <= 0) || (len >= static_cast<ssize_t>(sizeof(target)))) {
        return -1;
    }
    target[len] = '\0';
    const char *pos = strrchr(target, '/');
    CHKPR(pos, -1);
    pos = &pos[1];
    return CopyNulstr(value, size, pos);
}

void Utility::RemoveTrailingChars(char *path, char c)
{
    CHKPV(path);
    size_t len = strlen(path);
    while (len > 0 && path[len-1] == c) {
        path[--len] = '\0';
    }
}

void Utility::RemoveTrailingChars(std::string &path, const std::string &toRemoved)
{
    while (!path.empty() && (toRemoved.find(path.back()) != std::string::npos)) {
        path.pop_back();
    }
}
} // namespace UEVENT
} // namespace MMI
} // namespace OHOS
