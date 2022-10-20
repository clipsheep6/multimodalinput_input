#pragma once

#include <string>
#include <functional>

namespace Input {

namespace IoUtils {
    int32_t ForeachFile(const std::string& directory, std::function<void(const std::string&)> callback);
    const char* DTypeToString(int32_t dType);
};

}
