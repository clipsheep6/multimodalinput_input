#pragma once
#include <vector>
#include <list>
#include <set>
#include <map>
#include <memory>
#include <functional>

#include "IWindowStateManager.h"
#include "IInputDefine.h"
#include "PhysicalDisplayState.h"
#include "LogicalDisplayState.h"
#include "InputWindowState.h"

namespace Input {

    class ISeat;
    class ITouchScreenSeat;
    class ISeatManager;
    class WindowStateManager : public LogicalDisplayState::UpdateAssist, public IWindowStateManager {
        public:
            static std::unique_ptr<WindowStateManager> CreateInstance(const IInputContext* context);

        public:
            virtual void UpdateDisplayInfo(const std::vector<PhysicalDisplay>& physicalDisplays, 
                    const std::vector<LogicalDisplay>& logicalDisplays) override;

            virtual std::shared_ptr<LogicalDisplayState> GetLogicalDisplay(int32_t displayId) const override;
            virtual std::shared_ptr<PhysicalDisplayState> GetPhysicalDisplay(int32_t displayId) const override;
            virtual std::shared_ptr<PhysicalDisplayState> FindPhysicalDisplay(const std::string& seatId, 
                    const std::string& seatName) const override;
            virtual std::list<std::shared_ptr<LogicalDisplayState>> GetLogicalDisplays() const override;
            virtual int32_t TransformPhysicalDisplayCoordinateToPhysicalGlobalCoordinate(
                    int32_t physicalDisplayId, int32_t physicalDisplayX, int32_t physicalDisplayY, 
                    int32_t& physicalGlobalX, int32_t& physicalGlobalY) const override;
            virtual std::shared_ptr<const LogicalDisplayState> TransformPhysicalGlobalCoordinateToLogicalDisplayCoordinate(
                    int32_t physicalGlobalX, int32_t physicalGlobalY, 
                    int32_t& logicalDisplayX, int32_t& logicalDisplayY) const override;

        protected:
            WindowStateManager(const IInputContext* context);

            virtual std::shared_ptr<InputWindowState> ProvideInputWindowState(const IInputContext* context, const WindowState& window) override;
            virtual void RecordWindowAdded(std::shared_ptr<InputWindowState> inputWindow) override;
            virtual void RecordWindowUpdate(std::shared_ptr<InputWindowState> inputWindow) override;
            virtual void RecordWindowRemoved(std::shared_ptr<InputWindowState> inputWindow) override;

        private:
            void StartUpdate();
            void FinishUpdate();

            void UpdateDisplays(const std::vector<PhysicalDisplay>& displays);
            void UpdateDisplay(const std::shared_ptr<PhysicalDisplayState>& physicalDisplay, 
                    const PhysicalDisplay& display);

            void RemoveDisplays(std::list<std::shared_ptr<PhysicalDisplayState>>::iterator& first,
                    std::list<std::shared_ptr<PhysicalDisplayState>>::iterator last);

            void AddDisplays(std::vector<PhysicalDisplay>::const_iterator& first, 
                    std::vector<PhysicalDisplay>::const_iterator last);
            int32_t AddDisplay(const PhysicalDisplay& display);

            bool SwapDisplay(std::list<std::shared_ptr<PhysicalDisplayState>>::iterator& first,
                    std::list<std::shared_ptr<PhysicalDisplayState>>::iterator last, int32_t displayId);

            void UpdateDisplays(const std::vector<LogicalDisplay>& src);
            void UpdateDisplay(const std::shared_ptr<LogicalDisplayState>& dst, 
                    const LogicalDisplay& src);

            void RemoveDisplays(std::list<std::shared_ptr<LogicalDisplayState>>::iterator& first,
                    std::list<std::shared_ptr<LogicalDisplayState>>::iterator last);

            void AddDisplays(std::vector<LogicalDisplay>::const_iterator& first, 
                    std::vector<LogicalDisplay>::const_iterator last);
            int32_t AddDisplay(const LogicalDisplay& display);

            bool SwapDisplay(std::list<std::shared_ptr<LogicalDisplayState>>::iterator& first,
                    std::list<std::shared_ptr<LogicalDisplayState>>::iterator last, int32_t displayId);

            bool IsValidData(const std::vector<PhysicalDisplay>& physicalDisplays, 
                    const std::vector<LogicalDisplay>& logicalDisplays) const;
            bool IsValidData(const PhysicalDisplay& display) const;
            bool IsValidData(const LogicalDisplay& display, std::set<int32_t>& windowIds) const;
            bool IsValidData(const WindowState& window, int32_t displayId) const;

            void OnDisplayAdded(const std::shared_ptr<PhysicalDisplayState>& display);
            void OnDisplayRemoved(const std::shared_ptr<PhysicalDisplayState>& display);
            void OnDisplayChanged(const std::shared_ptr<PhysicalDisplayState>& display);

            void OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display);
            void OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display);
            void OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display);

            void OnWindowAdded(std::shared_ptr<InputWindowState> window);
            void OnWindowRemoved(std::shared_ptr<InputWindowState> window);
            void OnWindowChanged(std::shared_ptr<InputWindowState> window);

            std::shared_ptr<InputWindowState> FindWindow(const std::list<std::shared_ptr<InputWindowState>>& windows,
                    int32_t windowId);

            std::shared_ptr<InputWindowState> RemoveWindow(std::list<std::shared_ptr<InputWindowState>>& windows,
                    int32_t windowId);

            std::shared_ptr<InputWindowState> FindWindow(int32_t windowId) const;
            std::shared_ptr<InputWindowState> RemoveWindow(int32_t windowId);

            void RecordAddedWindow(std::shared_ptr<InputWindowState> window);
            void RecordRemovedWindow(std::shared_ptr<InputWindowState> window);
            void RecordChangedWindow(std::shared_ptr<InputWindowState> window);

            const std::unique_ptr<ISeatManager>& GetSeatManager() const;

        private:
            const IInputContext* const context_;
            std::list<std::shared_ptr<PhysicalDisplayState>> physicalDisplays_;
            std::list<std::shared_ptr<LogicalDisplayState>> logicalDisplays_;

            std::list<std::shared_ptr<PhysicalDisplayState>> addedPhysicalDisplays_;
            std::list<std::shared_ptr<PhysicalDisplayState>> removedPhysicalDisplays_;
            std::list<std::shared_ptr<PhysicalDisplayState>> changedPhysicalDisplays_;

            std::list<std::shared_ptr<LogicalDisplayState>> addedLogicalDisplays_;
            std::list<std::shared_ptr<LogicalDisplayState>> removedLogicalDisplays_;
            std::list<std::shared_ptr<LogicalDisplayState>> changedLogicalDisplays_;

            std::list<std::shared_ptr<InputWindowState>> addedWindows_;
            std::list<std::shared_ptr<InputWindowState>> removedWindows_;
            std::list<std::shared_ptr<InputWindowState>> changedWindows_;
    };

}
