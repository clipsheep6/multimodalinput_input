#pragma once

#include <string>

namespace Input {

    namespace IoctlUtils {
        int32_t GetInputDeviceName(int32_t inputDeviceFd, std::string& retName);
    };

}
