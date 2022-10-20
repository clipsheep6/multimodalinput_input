#include <sys/ioctl.h>

#include <linux/input.h>

#include "IoctlUtils.h"

namespace Input {

    int32_t IoctlUtils::GetInputDeviceName(int32_t inputDeviceFd, std::string& retName) {
        constexpr int32_t MAX_NAME_LEN = 256;
        char name[MAX_NAME_LEN];
        auto retCode = ioctl(inputDeviceFd, EVIOCGNAME(sizeof(name) - 1), &name);
        if (retCode < 0) {
            return -1;
        }
        name[sizeof(name) - 1] = '\0';
        retName = name;
        return 0;
    }
}
