#include <algorithm>

#include "WindowStateManager.h"
#include "IInputContext.h"
#include "ISeatManager.h"
#include "ISeat.h"
#include "ITouchScreenSeat.h"
#include "StreamUtils.h"
#include "Log.h"

namespace Input {

    std::unique_ptr<WindowStateManager> WindowStateManager::CreateInstance(const IInputContext* context) {
        if (context == nullptr) {
            return nullptr;
        }

        return std::unique_ptr<WindowStateManager>(new WindowStateManager(context));
    }

    WindowStateManager::WindowStateManager(const IInputContext* context)
        : context_(context) 
    {
    }

    void WindowStateManager::UpdateDisplayInfo(const std::vector<PhysicalDisplay>& physicalDisplays, 
            const std::vector<LogicalDisplay>& logicalDisplays) {
        LOG_D("Enter");
        if (!IsValidData(physicalDisplays, logicalDisplays)) {
            LOG_E("Leave");
            return;
        }

        StartUpdate();

        UpdateDisplays(physicalDisplays);
        UpdateDisplays(logicalDisplays);

        FinishUpdate();

        LOG_D("Leave");
    }

    std::shared_ptr<LogicalDisplayState> WindowStateManager::GetLogicalDisplay(int32_t displayId) const {
        for (auto& display : logicalDisplays_) {
            if (display->GetId() == displayId) {
                return display;
            }
        }
        return nullptr;
    }

    std::shared_ptr<PhysicalDisplayState> WindowStateManager::GetPhysicalDisplay(int32_t displayId) const
    {
        for (auto& display : physicalDisplays_) {
            if (display->GetId() == displayId) {
                return display;
            }
        }
        return nullptr;
    }

    std::shared_ptr<PhysicalDisplayState> WindowStateManager::FindPhysicalDisplay(const std::string& seatId, 
            const std::string& seatName) const
    {
        for (auto& display : physicalDisplays_) {
            if (display->GetSeatId() == seatId && display->GetSeatName() == seatName) {
                return display;
            }
        }
        return nullptr;
    }

    std::list<std::shared_ptr<LogicalDisplayState>> WindowStateManager::GetLogicalDisplays() const
    {
        return logicalDisplays_;
    }

    void WindowStateManager::OnDisplayAdded(const std::shared_ptr<PhysicalDisplayState>& display)
    {
        LOG_D("Enter $s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        const auto& seatManager = GetSeatManager();
        if (!seatManager) {
            LOG_E("Leave, null seatManager");
            return;
        }

        seatManager->OnDisplayAdded(display);
        LOG_D("Leave");
    }

    void WindowStateManager::OnDisplayRemoved(const std::shared_ptr<PhysicalDisplayState>& display)
    {
        LOG_D("Enter $s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        const auto& seatManager = GetSeatManager();
        if (!seatManager) {
            LOG_E("Leave, null seatManager");
            return;
        }

        seatManager->OnDisplayRemoved(display);
        LOG_D("Leave");
    }

    void WindowStateManager::OnDisplayChanged(const std::shared_ptr<PhysicalDisplayState>& display)
    {
        LOG_D("Enter $s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        const auto& seatManager = GetSeatManager();
        if (!seatManager) {
            LOG_E("Leave, null seatManager");
            return;
        }

        seatManager->OnDisplayChanged(display);
        LOG_D("Leave");
    }

    void WindowStateManager::OnWindowAdded(std::shared_ptr<InputWindowState> window)
    {
        LOG_D("Enter window:$s", window);
        LOG_D("Leave");
    }

    void WindowStateManager::OnWindowRemoved(std::shared_ptr<InputWindowState> window)
    {
        LOG_D("Enter window:$s", window);
        LOG_D("Leave");
    }

    void WindowStateManager::OnWindowChanged(std::shared_ptr<InputWindowState> window)
    {
        LOG_D("Enter window:$s", window);
        LOG_D("Leave");
    }

    void WindowStateManager::OnDisplayAdded(const std::shared_ptr<LogicalDisplayState>& display)
    {
        LOG_D("Enter $s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        const auto& seatManager = GetSeatManager();
        if (!seatManager) {
            LOG_E("Leave, null seatManager");
            return;
        }

        seatManager->OnDisplayAdded(display);
        LOG_D("Leave");
    }

    void WindowStateManager::OnDisplayRemoved(const std::shared_ptr<LogicalDisplayState>& display) {
        LOG_D("Enter $s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        const auto& seatManager = GetSeatManager();
        if (!seatManager) {
            LOG_E("Leave, null seatManager");
            return;
        }

        seatManager->OnDisplayRemoved(display);
        LOG_D("Leave");
    }

    void WindowStateManager::OnDisplayChanged(const std::shared_ptr<LogicalDisplayState>& display) {
        LOG_D("Enter $s", display);
        if (!display) {
            LOG_E("Leave, null display");
            return;
        }

        const auto& seatManager = GetSeatManager();
        if (!seatManager) {
            LOG_E("Leave, null seatManager");
            return;
        }

        seatManager->OnDisplayRemoved(display);
        LOG_D("Leave");
    }

    void WindowStateManager::StartUpdate()
    {
        addedPhysicalDisplays_.clear();
        removedPhysicalDisplays_.clear();
        changedPhysicalDisplays_.clear();

        addedLogicalDisplays_.clear();
        removedLogicalDisplays_.clear();
        changedLogicalDisplays_.clear();

        addedWindows_.clear();
        removedWindows_.clear();
        changedWindows_.clear();
    }

    void WindowStateManager::FinishUpdate()
    {
        LOG_D("Enter");
        for (const auto& display : removedPhysicalDisplays_) {
            OnDisplayRemoved(display);
        }

        for (const auto& display : changedPhysicalDisplays_) {
            OnDisplayChanged(display);
        }

        for (const auto& display : addedPhysicalDisplays_) {
            OnDisplayAdded(display);
        }

        for (auto window : removedWindows_) {
            OnWindowRemoved(window);
        }

        for (auto displayId : addedLogicalDisplays_) {
            OnDisplayAdded(displayId);
        }

        for (auto window : changedWindows_) {
            OnWindowChanged(window);
        }

        for (auto windowId : addedWindows_) {
            OnWindowAdded(windowId);
        }

        for (const auto& display : changedLogicalDisplays_) {
            OnDisplayChanged(display);
        }

        for (const auto& display : removedLogicalDisplays_) {
            OnDisplayRemoved(display);
        }
        LOG_D("Leave");
    }

    void WindowStateManager::UpdateDisplays(const std::vector<PhysicalDisplay>& displays)
    {
        auto dstIt = physicalDisplays_.begin();
        auto srcIt = displays.begin();
        for (;;) {
            if (srcIt == displays.end()) {
                RemoveDisplays(dstIt, physicalDisplays_.end());
                break;
            }

            if (dstIt == physicalDisplays_.end()) {
                AddDisplays(srcIt, displays.end());
                break;
            }

            if (SwapDisplay(dstIt, physicalDisplays_.end(), srcIt->id)) {
                UpdateDisplay(*dstIt++, *srcIt++);
                continue;
            }

            std::shared_ptr<PhysicalDisplayState> physicalDisplay = PhysicalDisplayState::CreateInstance(context_, *srcIt++);
            if (!physicalDisplay) {
                LOG_E("Create PhysicalDisplayState Failed, Should Nerve come here!");
                continue;
            }
            physicalDisplays_.insert(dstIt, physicalDisplay);
            addedPhysicalDisplays_.push_back(physicalDisplay);
        }
    }

    void WindowStateManager::UpdateDisplay(const std::shared_ptr<PhysicalDisplayState>& dst, 
            const PhysicalDisplay& src)
    {
        if (dst->GetId() == src.id) {
            return;
        }

        uint32_t changed = dst->Update(src);
        if (changed == PhysicalDisplayState::CHANGED_NONE) {
            return;
        }
        changedPhysicalDisplays_.push_back(dst);
    }

    void WindowStateManager::RemoveDisplays(std::list<std::shared_ptr<PhysicalDisplayState>>::iterator& first,
            std::list<std::shared_ptr<PhysicalDisplayState>>::iterator last)
    {
        while (first != last) {
            std::shared_ptr<PhysicalDisplayState> display = std::move(*first);
            physicalDisplays_.erase(first++);
            removedPhysicalDisplays_.push_back(std::move(display));
        }
    }

    void WindowStateManager::AddDisplays(std::vector<PhysicalDisplay>::const_iterator& first, 
            std::vector<PhysicalDisplay>::const_iterator last)
    {
        while (first != last) {
            const PhysicalDisplay& display = *first++;
            std::shared_ptr<PhysicalDisplayState> physicalDisplay = PhysicalDisplayState::CreateInstance(context_, display);
            if (!physicalDisplay) {
                LOG_NERVER();
                continue;
            }
            physicalDisplays_.push_back(physicalDisplay);
            addedPhysicalDisplays_.push_back(physicalDisplay);
        }
    }

    bool WindowStateManager::SwapDisplay(std::list<std::shared_ptr<PhysicalDisplayState>>::iterator& first,
            std::list<std::shared_ptr<PhysicalDisplayState>>::iterator last, int32_t displayId)
    {
        auto it = first;
        for (++it; it != last; ++it) {
            if ((*it)->GetId() == displayId) {
                (*first).swap(*it);
                return true;
            }
        }
        return false;
    }

    void WindowStateManager::UpdateDisplays(const std::vector<LogicalDisplay>& displays)
    {
        auto dstIt = logicalDisplays_.begin();
        auto srcIt = displays.begin();
        for (;;) {
            if (srcIt == displays.end()) {
                RemoveDisplays(dstIt, logicalDisplays_.end());
                break;
            }

            if (dstIt == logicalDisplays_.end()) {
                AddDisplays(srcIt, displays.end());
                break;
            }

            if (SwapDisplay(dstIt, logicalDisplays_.end(), srcIt->id)) {
                UpdateDisplay(*dstIt++, *srcIt++);
                continue;
            }

            std::shared_ptr<LogicalDisplayState> logicalDisplay = LogicalDisplayState::CreateInstance(context_, *srcIt++, *this);
            if (!logicalDisplay) {
                LOG_NERVER();
                continue;
            }
            logicalDisplays_.insert(dstIt, logicalDisplay);
            addedLogicalDisplays_.push_back(logicalDisplay);
        }
    }

    void WindowStateManager::UpdateDisplay(const std::shared_ptr<LogicalDisplayState>& dst, 
            const LogicalDisplay& src)
    {
        if (dst->GetId() == src.id) {
            LOG_NERVER();
            return;
        }

        uint32_t changed = dst->Update(src);
        if (changed != LogicalDisplayState::CHANGED_NONE) {
            changedLogicalDisplays_.push_back(dst);
        }
    }

    void WindowStateManager::RemoveDisplays(std::list<std::shared_ptr<LogicalDisplayState>>::iterator& first,
            std::list<std::shared_ptr<LogicalDisplayState>>::iterator last)
    {
        while (first != last) {
            std::shared_ptr<LogicalDisplayState> display = *first;
            logicalDisplays_.erase(first++);
            display->RemoveAllWindows();
            removedLogicalDisplays_.push_back(display);
        }
    }

    void WindowStateManager::AddDisplays(std::vector<LogicalDisplay>::const_iterator& first, 
            std::vector<LogicalDisplay>::const_iterator last)
    {
        while (first != last) {
            const LogicalDisplay& src = *first++;
            std::shared_ptr<LogicalDisplayState> logicalDisplay = LogicalDisplayState::CreateInstance(context_, src, *this);
            if (!logicalDisplay) {
                LOG_NERVER();
                continue;
            }

            logicalDisplays_.push_back(logicalDisplay);
            addedLogicalDisplays_.push_back(logicalDisplay);
        }
    }

    bool WindowStateManager::SwapDisplay(std::list<std::shared_ptr<LogicalDisplayState>>::iterator& first,
            std::list<std::shared_ptr<LogicalDisplayState>>::iterator last, int32_t displayId)
    {
        auto it = first;
        for (++it; it != last; ++it) {
            if ((*it)->GetId() == displayId) {
                (*first).swap(*it);
                return true;
            }
        }
        return false;
    }

    bool WindowStateManager::IsValidData(const std::vector<PhysicalDisplay>& physicalDisplays, 
            const std::vector<LogicalDisplay>& logicalDisplays) const {

        LOG_D("Enter physicalDisplays logicalDisplays");

        std::set<int32_t> displayIds;
        std::map<std::string, std::list<std::string>> seatInfos;
        for (const auto& item : physicalDisplays) {
            if (!IsValidData(item)) {
                LOG_E("Leave physicalDisplays logicalDisplays, Invalid physicalDisplay:$s", item);
                return false;
            }

            auto [it, success] = displayIds.insert(item.id);
            if (!success) {
                LOG_E("Leave physicalDisplays logicalDisplays, repeat displayId$s in displays:$s ", 
                        item.id, physicalDisplays);
                return false;
            }

            auto& seatNames = seatInfos[item.seatId];
            for (auto& seatName : seatNames) {
                if (seatName == item.seatName) {
                    LOG_E("Leave physicalDisplays two diplay with same seatId&seatName");
                    return false;
                }
            }

            seatNames.push_back(item.seatName);
        }

        for (const auto& item : physicalDisplays) {
            if (item.leftDisplayId >=0 && 0 == displayIds.count(item.leftDisplayId)) {
                LOG_E("Leave physicalDisplays logicalDisplays, Invalid leftDisplayId for physicalDisplay:$s, displays:$s",
                        item, physicalDisplays);
                return false;
            }

            if (item.upDisplayId >=0 && 0 == displayIds.count(item.upDisplayId)) {
                LOG_E("Leave physicalDisplays logicalDisplays, Invalid upDisplayId for physicalDisplay:$s, displays:$s",
                        item, physicalDisplays);
                return false;
            }
        }

        displayIds.clear();
        std::set<int32_t> windowIds;
        for (const auto& item : logicalDisplays) {
            if (!IsValidData(item, windowIds)) {
                LOG_E("Leave physicalDisplays logicalDisplays, Invalid logicalDisplay:$s of displays:$s", item, logicalDisplays);
                return false;
            }

            auto [it, success] = displayIds.insert(item.id);
            if (!success) {
                LOG_E("Leave physicalDisplays logicalDisplays, repeat displayId:$s int displays:$s", item.id, logicalDisplays);
                return false;
            }
        }

        for (const auto& item : logicalDisplays) {
            if (item.leftDisplayId >=0 && 0 == displayIds.count(item.leftDisplayId)) {
                LOG_E("Leave physicalDisplays logicalDisplays, Invalid leftDisplayId of logicalDisplay:$s, displays:$s",
                        item, logicalDisplays);
                return false;
            }

            if (item.upDisplayId >=0 && 0 == displayIds.count(item.upDisplayId)) {
                LOG_E("Leave physicalDisplays logicalDisplays, Invalid upDisplayId of logicalDisplay:$s, displays:$s",
                        item, logicalDisplays);
                return false;
            }
            for (const auto& window : item.windows) {
                if (window.agentWindowId < 0) {
                    continue;
                }

                if (0 == windowIds.count(window.agentWindowId)) {
                    LOG_E("Leave physicalDisplays logicalDisplays, invalid agentWindowId of window:$s, allWindowId:$s", 
                            window.agentWindowId, windowIds);
                    return false;
                }
            }
        }

        LOG_D("Leave physicalDisplays logicalDisplays");
        return true;
    }

    bool WindowStateManager::IsValidData(const PhysicalDisplay& display) const
    {
        LOG_D("Enter physicalDisplay:$s", display);
        if (display.id < 0) {
            LOG_E("Leave physicalDisplay:$s, display.id < 0", display);
            return false;
        } 

        if (display.topLeftX != 0) {
            LOG_E("Leave physicalDisplay:$s, display.topLeftX != 0", display);
            return false;
        }

        if (display.topLeftY != 0) {
            LOG_E("Leave physicalDisplay:$s, display.topLeftY != 0", display);
            return false;
        }

        if (display.width <= 0) {
            LOG_E("Leave physicalDisplay:$s, display.width <= 0", display);
            return false;
        }

        if (display.height <= 0) {
            LOG_E("Leave physicalDisplay:$s, display.height <= 0", display);
            return false;
        }

        if (display.logicalWidth <= 0) {
            LOG_E("Leave physicalDisplay:$s, display.logicalWidth <= 0", display);
            return false;
        }

        if (display.logicalHeight <= 0) {
            LOG_E("Leave physicalDisplay:$s, display.logicalHeight <= 0", display);
            return false;
        }

        LOG_D("Leave");
        return true;
    }

    bool WindowStateManager::IsValidData(const LogicalDisplay& display, std::set<int32_t>& windowIds) const
    {
        LOG_D("Enter logicalDisplay:$s windowIds:$s", display, windowIds);
        if (display.id < 0) {
            LOG_E("Leave logicalDisplay:$s, display.id < 0", display);
            return false;
        } 

        if (display.topLeftX < 0) {
            LOG_E("Leave logicalDisplay$s, display.topLeftX < 0", display);
            return false;
        }

        if (display.topLeftY < 0) {
            LOG_E("Leave logicalDisplay:$s, display.topLeftY < 0", display);
            return false;
        }

        if (display.width <= 0) {
            LOG_E("Leave logicalDisplay:$s, display.width <= 0", display);
            return false;
        }

        if (display.height <= 0) {
            LOG_E("Leave logicalDisplay:$s, display.height <= 0", display);
            return false;
        }

        for (const auto& window : display.windows) {
            if (!IsValidData(window, display.id)) {
                LOG_E("Leave logicalDisplay:$s windowIds:$s, InvalidWindow:$s", display, windowIds, window);
                return false;
            }

            auto [it, success] = windowIds.insert(window.id);
            if (!success) {
                LOG_E("Leave logicalDisplay:$s windowIds:$s, Repeat id of window:$s", display, windowIds, window);
                return false;
            }
        }

        LOG_D("Leave logicalDisplay:$s windowIds:$s", display, windowIds);
        return true;
    }

    bool WindowStateManager::IsValidData(const WindowState& window, int32_t displayId) const
    {
        LOG_D("Enter window:$s, displayId:$s", window, displayId);
        if (window.displayId != displayId) {
            LOG_E("Leave window:$s, displayId:$s, window.displayId != displayId", window, displayId);
            return false;
        } 

        if (window.id < 0) {
            LOG_E("Leave window:$s, displayId:$s, window.id < 0", window, displayId);
            return false;
        }

        if (window.width <= 0) {
            LOG_E("Leave window:$s, displayId:$s, window.width < 0", window, displayId);
            return false;
        }

        if (window.height <= 0) {
            LOG_E("Leave window:$s, displayId:$s, window.height < 0", window, displayId);
            return false;
        }

        return true;
    }

    std::shared_ptr<InputWindowState> WindowStateManager::FindWindow(
            const std::list<std::shared_ptr<InputWindowState>>& windows, int32_t windowId)
    {
        for (auto& window : windows) {
            if (window->GetId() == windowId) {
                return window;
            }
        }
        return nullptr;
    }

    std::shared_ptr<InputWindowState> WindowStateManager::RemoveWindow(
            std::list<std::shared_ptr<InputWindowState>>& windows, int32_t windowId)
    {
        for (auto it = windows.begin(); it != windows.end(); ++it) {
            std::shared_ptr<InputWindowState> window = *it;
            if (window->GetId() == windowId) {
                windows.erase(it);
                return window;
            }
        }
        return nullptr;
    }

    std::shared_ptr<InputWindowState> WindowStateManager::FindWindow(int32_t windowId)  const
    {
        for (const auto& display : logicalDisplays_) {
            auto window = display->FindWindow(windowId);
            if (window) {
                return window;
            }
        }
        return nullptr;
    }

    std::shared_ptr<InputWindowState> WindowStateManager::RemoveWindow(int32_t windowId)
    {
        for (auto& display : logicalDisplays_) {
            auto window = display->RemoveWindow(windowId);
            if (window) {
                return window;
            }
        }

        return nullptr;
    }

    std::shared_ptr<InputWindowState> WindowStateManager::ProvideInputWindowState(const IInputContext* context, const WindowState& window)
    {
        if (context_ != context) {
            return nullptr;
        }

        auto inputWindow = RemoveWindow(window.id);
        if (inputWindow) {
            return inputWindow;
        }

        inputWindow = RemoveWindow(removedWindows_, window.id);
        if (inputWindow) {
            return inputWindow;
        }

        return InputWindowState::CreateInstance(context_, window);
    }

    void WindowStateManager::RecordWindowAdded(std::shared_ptr<InputWindowState> inputWindow) 
    {
        if (!inputWindow) {
            return;
        }

        RemoveWindow(changedWindows_, inputWindow->GetId());
        RemoveWindow(addedWindows_, inputWindow->GetId());
        RemoveWindow(removedWindows_, inputWindow->GetId());

        if (inputWindow->GetChanged() == InputWindowState::CHANGED_NONE) {
            addedWindows_.push_back(inputWindow);
            return;
        }

        changedWindows_.push_back(inputWindow);
    }

    void WindowStateManager::RecordWindowUpdate(std::shared_ptr<InputWindowState> inputWindow)
    {
        if (!inputWindow) {
            return;
        }

        RemoveWindow(changedWindows_, inputWindow->GetId());
        RemoveWindow(addedWindows_, inputWindow->GetId());
        RemoveWindow(removedWindows_, inputWindow->GetId());

        if (inputWindow->GetChanged() == InputWindowState::CHANGED_NONE) {
            addedWindows_.push_back(inputWindow);
            return;
        }

        changedWindows_.push_back(inputWindow);
    }

    void WindowStateManager::RecordWindowRemoved(std::shared_ptr<InputWindowState> inputWindow)
    {
        if (!inputWindow) {
            return;
        }

        RemoveWindow(changedWindows_, inputWindow->GetId());
        RemoveWindow(addedWindows_, inputWindow->GetId());
        RemoveWindow(removedWindows_, inputWindow->GetId());
        removedWindows_.push_back(inputWindow);
    }

    const std::unique_ptr<ISeatManager>& WindowStateManager::GetSeatManager() const
    {
        if (context_ == nullptr) {
            return ISeatManager::NULL_VALUE;
        }

        return context_->GetSeatManager();
    }

    int32_t WindowStateManager::TransformPhysicalDisplayCoordinateToPhysicalGlobalCoordinate(
            int32_t physicalDisplayId, int32_t physicalDisplayX, int32_t physicalDisplayY, 
            int32_t& physicalGlobalX, int32_t& physicalGlobalY) const
    {
        LOG_D("Enter physicalDisplayId:$s, physicalDisplayX:$s, physicalDisplayY:$s", 
                physicalDisplayId, physicalDisplayX, physicalDisplayY);
        auto display = GetPhysicalDisplay(physicalDisplayId);
        if (!display) {
            LOG_E("Leave, null PhysicalDisplay of $s", physicalDisplayId);
            return -1;
        }

        std::set<int32_t> processed;
        int32_t globalX = physicalDisplayX;
        auto curDisplay = display;
        for (;;) {
            auto leftDisplayId = curDisplay->GetLeftDisplayId();
            if (leftDisplayId < 0) {
                break;
            }

            const auto [it, isSuccess] = processed.insert(leftDisplayId);
            if (!isSuccess) {
                LOG_E("Leave, leftDisplay Cycle");
                return -1;
            }

            curDisplay = GetPhysicalDisplay(leftDisplayId);
            if (!curDisplay) {
                LOG_E("Leave, null leftDisplay of $s", leftDisplayId);
                return -1;
            }

            globalX += curDisplay->GetWidth();
        }

        int32_t globalY = physicalDisplayY;
        processed.clear();
        curDisplay = display;
        for (;;) {
            auto upDisplayId = curDisplay->GetUpDisplayId();
            if (upDisplayId < 0) {
                break;
            }

            const auto [it, isSuccess] = processed.insert(upDisplayId);
            if (!isSuccess) {
                LOG_E("Leave, upDisplay Cycle");
                return -1;
            }

            curDisplay = GetPhysicalDisplay(upDisplayId);
            if (!curDisplay) {
                LOG_E("Leave, null upDisplay of $s", upDisplayId);
                return -1;
            }

            globalY += curDisplay->GetHeight();
        }

        physicalGlobalX = globalX;
        physicalGlobalY = globalY;

        LOG_D("Leave physicalGlobalX:$s, physicalGlobalY:$s", 
                physicalGlobalX, physicalGlobalY);
        return 0;
    }

    std::shared_ptr<const LogicalDisplayState> WindowStateManager::TransformPhysicalGlobalCoordinateToLogicalDisplayCoordinate(
            int32_t physicalGlobalX, int32_t physicalGlobalY,
            int32_t& logicalDisplayX, int32_t& logicalDisplayY) const
    {
        for (const auto& display : logicalDisplays_) {
            if (!display->Transform(physicalGlobalX, physicalGlobalX, false, logicalDisplayX, logicalDisplayY)) {
                continue;
            }
            return display;
        }

        return nullptr;
    }
}

