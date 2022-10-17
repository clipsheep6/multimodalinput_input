#include <cstdint>

#include "IdGenerator.h"

namespace Input {

    int32_t IdGenerator::TakeNextInputDeviceId() {
        static int32_t nextId = 0;
        return nextId++;
    }

}
