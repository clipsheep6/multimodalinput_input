#pragma once
#include <vector>
#include <list>
#include <memory>

#include "DisplayInfo.h"
#include "LogicalDisplayState.h"
#include "PhysicalDisplayState.h"

namespace Input {

    class IInputContext;
    class IWindowStateManager {
        public:
            static std::unique_ptr<IWindowStateManager> CreateInstance(const IInputContext* context);

            virtual ~IWindowStateManager() = default;

            virtual void UpdateDisplayInfo(const std::vector<PhysicalDisplay>& physicalDisplays, 
                    const std::vector<LogicalDisplay>& logicalDisplays) = 0;

            virtual std::shared_ptr<LogicalDisplayState> GetLogicalDisplay(int32_t displayId) const = 0;
            virtual std::shared_ptr<PhysicalDisplayState> GetPhysicalDisplay(int32_t displayId) const = 0;
            virtual std::shared_ptr<PhysicalDisplayState> FindPhysicalDisplay(const std::string& seatId, 
                    const std::string& seatName) const = 0;
            virtual std::list<std::shared_ptr<LogicalDisplayState>> GetLogicalDisplays() const = 0;

            virtual int32_t TransformPhysicalDisplayCoordinateToPhysicalGlobalCoordinate(
                    int32_t physicalDisplayId, int32_t physicalDisplayX, int32_t physicalDisplayY, 
                    int32_t& physicalGlobalX, int32_t& physicalGlobalY) const = 0;
            virtual std::shared_ptr<const LogicalDisplayState> TransformPhysicalGlobalCoordinateToLogicalDisplayCoordinate(
                    int32_t physicalGlobalX, int32_t physicalGlobalY, int32_t& logicalDisplayX, int32_t& logicalDisplayY) const = 0;
    };

}
