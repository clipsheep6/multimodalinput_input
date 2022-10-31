#include <functional>

#include "Robot.h"
#include "IInputContext.h"
#include "IWindowStateManager.h"
#include "IEventLooper.h"
#include "DisplayInfo.h"
#include "Log.h"

namespace Input {

    Robot::Robot(const IInputContext* context) 
    : context_(context) {
    }

    int32_t Robot::Start() {
        LOG_D("Enter");

        if (context_ == nullptr) {
            LOG_E("Leave, Null Context");
            return -1;
        }

        const auto& looper = context_->GetLooper();
        if (!looper) {
            LOG_E("Leave, Null looper");
            return -1;
        }

        auto ret = looper->AddTimer(2000, 1, std::bind(&Robot::AddDefaultDisplay, this));
        if (ret < 0) {
            LOG_E("Leave, AddTimer Failed");
            return -1;
        }

        LOG_D("Leave");
        return 0;
    }

    int32_t Robot::Stop() {
        return 0;
    }

    int32_t Robot::AddDefaultDisplay() {
        LOG_D("Enter");

        std::vector<PhysicalDisplay> physicalDisplays;
        std::vector<LogicalDisplay> logicalDisplays;

        PhysicalDisplay physicalDisplay = {
            .id = 0,
            .topLeftX = 0,
            .topLeftY = 0,
            .width = 1024,
            .height = 768,
            .logicalWidth = 1024,
            .logicalHeight = 768,
            .leftDisplayId = -1,
            .upDisplayId = -1,
            .seatId = std::string("seat0"),
            .seatName = std::string("default0"),
            .name = "Display0"
        };
        physicalDisplays.push_back(physicalDisplay);

        WindowState window = {
            .id = 0,
            .topLeftX = 0,
            .topLeftY = 0,
            .width = 1024,
            .height = 768,
            .displayId = 0,
            .agentWindowId = 0
        };

        LogicalDisplay logicalDisplay = {
            .id = 0,
            .topLeftX = 0,
            .topLeftY = 0,
            .width = 1024,
            .height = 768,
            .leftDisplayId = -1,
            .upDisplayId = -1,
            .seatId = "seat0",
            .seatName = std::string("default0"),
            .windows = {window}
        };

        logicalDisplays.push_back(logicalDisplay);

        const auto& windowStateManager = context_->GetWindowStateManager();
        if (!windowStateManager) {
            LOG_E("Leave");
            return -1;
        }

        windowStateManager->UpdateDisplayInfo(physicalDisplays, logicalDisplays);
        LOG_D("Leave");
        return 0;
    }

}
