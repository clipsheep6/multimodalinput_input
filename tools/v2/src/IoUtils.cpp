#include <iostream>
#include <sys/types.h>
#include <dirent.h>

#include "IoUtils.h"
#include "IInputDefine.h"

namespace Input {

const char* IoUtils::DTypeToString(int32_t dType)
{
    switch (dType) {
        CASE_STR(DT_UNKNOWN);
        CASE_STR(DT_FIFO);
        CASE_STR(DT_CHR);
        CASE_STR(DT_DIR);
        CASE_STR(DT_BLK);
        CASE_STR(DT_REG);
        CASE_STR(DT_LNK);
        CASE_STR(DT_SOCK);
        CASE_STR(DT_WHT);
        default:
        return "DT_UNKNOWN_DEFAULT";
    }
}

int32_t IoUtils::ForeachFile(const std::string& directory, std::function<void(const std::string&)> callback)
{
    if (directory.empty()) {
        errno = EINVAL;
        return -1;
    }

    DIR* dir = ::opendir(directory.c_str());
    if (dir == nullptr) {
        return -1;
    }

    struct dirent* item = nullptr;
    errno = 0;
    while((item = ::readdir(dir)) != nullptr) {
        if (item->d_type != DT_CHR) {
            continue;
        }

        callback(directory + item->d_name);
    }

    ::closedir(dir);
    return 0;
}

}
