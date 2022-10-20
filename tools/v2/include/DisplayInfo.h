#pragma once

#include <vector>
#include <string>

namespace Input {
    struct WindowState {
        int32_t id;
        int32_t topLeftX;
        int32_t topLeftY;
        int32_t width;
        int32_t height;
        int32_t displayId;
        int32_t agentWindowId;
    };

    struct PhysicalDisplay {
        int32_t id;
        int32_t topLeftX;
        int32_t topLeftY;
        int32_t width;
        int32_t height;
        int32_t logicalWidth;
        int32_t logicalHeight;
        int32_t leftDisplayId;
        int32_t upDisplayId;
        std::string seatId;
        std::string seatName;
        std::string name;
    };

    struct LogicalDisplay {
        int32_t id;
        int32_t topLeftX;
        int32_t topLeftY;
        int32_t width;
        int32_t height;
        int32_t leftDisplayId;
        int32_t upDisplayId;
        std::string seatId;
        std::string seatName;
        std::vector<WindowState> windows;
    };

}
