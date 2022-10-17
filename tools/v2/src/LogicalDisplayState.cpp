#include "LogicalDisplayState.h"
#include "Utils.h"
#include "StreamUtils.h"
#include "PointerEvent.h"
#include "Utils.h"
#include "Log.h"

namespace Input {

const std::shared_ptr<LogicalDisplayState> LogicalDisplayState::NULL_VALUE;

std::shared_ptr<InputWindowState> LogicalDisplayState::UpdateAssist::ProvideInputWindowState(const IInputContext* context, const WindowState& window)
{
    return InputWindowState::CreateInstance(context, window);
}

void LogicalDisplayState::UpdateAssist::RecordWindowAdded(std::shared_ptr<InputWindowState> inputWindow)
{
}

void LogicalDisplayState::UpdateAssist::RecordWindowUpdate(std::shared_ptr<InputWindowState> inputWindow)
{
}

void LogicalDisplayState::UpdateAssist::RecordWindowRemoved(std::shared_ptr<InputWindowState> inputWindow)
{
}

std::shared_ptr<LogicalDisplayState> LogicalDisplayState::CreateInstance(const IInputContext* context,
        const LogicalDisplay& display, LogicalDisplayState::UpdateAssist& assist) {
    LOG_D("Enter display:$s", display);
    if (context == nullptr) {
        LOG_E("Leave display:$s, null context", display);
        return nullptr;
    }

    auto result = std::unique_ptr<LogicalDisplayState>(new LogicalDisplayState(context, display, assist));
    LOG_D("Leave");
    return result;
}

LogicalDisplayState::LogicalDisplayState(const IInputContext* context, const LogicalDisplay& display, 
        LogicalDisplayState::UpdateAssist& assist)
    : context_(context), 
    id_(display.id), 
    topLeftX_(display.topLeftX), topLeftY_(display.topLeftY), 
    width_(display.width), height_(display.height), 
    leftDisplayId_(display.leftDisplayId), upDisplayId_(display.upDisplayId),
    seatId_(display.seatId), seatName_(display.seatName),
    changed_(CHANGED_NONE), assist_(assist), 
    pointerEvent_(new PointerEvent())
{
    pointerEvent_->SetSourceType(PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    UpdateWindows(display.windows);
}

int32_t LogicalDisplayState::GetId() const
{
    return id_;
}

int32_t LogicalDisplayState::GetWidth() const
{
    return width_;
}

int32_t LogicalDisplayState::GetHeight() const
{
    return height_;
}

int32_t LogicalDisplayState::GetTopLeftX() const
{
    return topLeftX_;
}

int32_t LogicalDisplayState::GetTopLeftY() const
{
    return topLeftY_;    
}

const std::string& LogicalDisplayState::GetSeatId() const
{
    return seatId_;
}

const std::string& LogicalDisplayState::GetSeatName() const
{
    return seatName_;
}

std::list<int32_t> LogicalDisplayState::GetWindowIds() const
{
    std::list<int32_t> result;
    for (const auto& window : inputWindows_) {
        result.push_back(window->GetId());
    }
    return result;
}

uint32_t LogicalDisplayState::GetChanged() const
{
    return changed_;
}

std::shared_ptr<InputWindowState> LogicalDisplayState::FindWindow(int32_t windowId) const
{
    for (auto& window : inputWindows_) {
        return window;
    }

    return nullptr;
}

void LogicalDisplayState::RemoveAllWindows()
{
    std::list<std::shared_ptr<InputWindowState>> inputWindows;
    std::swap(inputWindows, inputWindows_);
    for (auto inputWindow : inputWindows) {
        assist_.RecordWindowRemoved(inputWindow);
    }
}

std::shared_ptr<InputWindowState> LogicalDisplayState::RemoveWindow(int32_t windowId)
{
    for (auto it = inputWindows_.begin(); it != inputWindows_.end(); ++it) {
        if ((*it)->GetId() == windowId) {
            std::shared_ptr<InputWindowState> result = *it;
            inputWindows_.erase(it++);
            assist_.RecordWindowRemoved(result);
            return result;
        }
    }

    return nullptr;
}

uint32_t LogicalDisplayState::Update(const LogicalDisplay& display)
{
    changed_ = CHANGED_NONE;

    if (Utils::UpdateValue(topLeftX_, display.topLeftX) || 
            Utils::UpdateValue(topLeftY_, display.topLeftY)) {
        changed_ |= CHANGED_POSITION;
    }

    if (Utils::UpdateValue(width_, display.width) || 
            Utils::UpdateValue(height_, display.height)) {
        changed_ |= CHANGED_SIZE;
    }

    if (Utils::UpdateValue(leftDisplayId_, display.leftDisplayId) || 
            Utils::UpdateValue(upDisplayId_, display.upDisplayId)) {
        changed_ |= CHANGED_LOCATION;
    }

    if (Utils::UpdateValue(seatId_, display.seatId)) {
        changed_ |= CHANGED_SEAT_ID;
    }

    if (Utils::UpdateValue(seatName_, display.seatName)) {
        changed_ |= CHANGED_SEAT_NAME;
    }

    UpdateWindows(display.windows);
    return changed_;
}

void LogicalDisplayState::UpdateWindows(const std::vector<WindowState>& windows)
{
    auto dstIt = inputWindows_.begin();
    auto srcIt = windows.begin();
    for (;;) {
        if (srcIt == windows.end()) {
            std::list<std::shared_ptr<InputWindowState>> removedWindows;
            while (dstIt != inputWindows_.end()) {
                removedWindows.push_back(*dstIt);
                inputWindows_.erase(dstIt++);
            }
            for (auto& inputWindow : removedWindows) {
                assist_.RecordWindowRemoved(inputWindow);
            }
            break;
        }

        if (dstIt == inputWindows_.end()) {
            while (srcIt != windows.end()) {
                auto inputWindow = assist_.ProvideInputWindowState(context_, *srcIt++);
                if (!inputWindow) {
                    LOG_NERVER();
                    continue;
                }
                inputWindows_.push_back(inputWindow);
                assist_.RecordWindowAdded(inputWindow);
            }
            break;
        }

        int32_t windowId = srcIt->id;
        auto it = dstIt;
        for (++it; it != inputWindows_.end(); ++it) {
            if ((*it)->GetId() == windowId) {
                std::swap(*dstIt, *it);
                break;
            }
        }
        if ((*dstIt)->GetId() == windowId) {
            (*dstIt)->Update(*srcIt);
            assist_.RecordWindowUpdate(*dstIt);
            ++dstIt;
            ++srcIt;
            continue;
        }

        auto inputWindow = assist_.ProvideInputWindowState(context_, *srcIt++);
        if (!inputWindow) {
            LOG_NERVER();
            continue;
        }

        inputWindows_.insert(dstIt, inputWindow);
        assist_.RecordWindowAdded(inputWindow);
    }
}

std::shared_ptr<PointerEvent> LogicalDisplayState::HandleEvent(int32_t pointerAction, int64_t actionTime,
        const std::shared_ptr<const PointerEvent::PointerItem>& pointer) const
{
    if (!pointer) {
        LOG_D("Leave, null pointer");
        return nullptr;
    }

    int32_t prePointerAction = pointerEvent_->GetPointerAction();
    if (prePointerAction == PointerEvent::POINTER_ACTION_UP) {
        int32_t prePointerId = pointerEvent_->GetPointerId();
        if (!pointerEvent_->RemovePointer(prePointerId)) {
            LOG_W("Remove Pre Released Pointer");
        }

        pointerEvent_->SetPointerId(-1);
        pointerEvent_->SetPointerAction(PointerEvent::POINTER_ACTION_NONE);
    }

    int32_t inputDeviceId = pointer->GetDeviceId();
    int32_t pointerId = pointer->GetId();
    int64_t pointerIdKey = Utils::Combine(inputDeviceId, pointerId);
    auto it = pointerIdMap_.find(pointerIdKey);
    int32_t newPointerId = -1;

    if (pointerAction == PointerEvent::POINTER_ACTION_DOWN) {
        if (it != pointerIdMap_.end()) {
            LOG_E("Leave, Repeat PointerDown inputDeviceId:$s pointerId:$s", inputDeviceId, pointerId);
            return nullptr;
        }

        newPointerId = nextPointerId_++;
        std::shared_ptr<PointerEvent::PointerItem> newPointer(new PointerEvent::PointerItem(*pointer.get()));
        newPointer->SetId(newPointerId);

        auto retCode = pointerEvent_->AddPointer(newPointer);
        if (retCode < 0) {
            LOG_E("Leave, AddPointer Failed");
            return nullptr;
        }
        pointerIdMap_[pointerIdKey] = newPointerId;
    } else if (pointerAction == PointerEvent::POINTER_ACTION_MOVE || pointerAction == PointerEvent::POINTER_ACTION_UP) {
        if (it == pointerIdMap_.end()) {
            LOG_E("Leave, Move or Up Not Exist PointerId inputDeviceId:$s pointerId:$s", inputDeviceId, pointerId);
            return nullptr;
        }

        newPointerId = it->second;
        auto newPointer = pointerEvent_->GetPointerItem(newPointerId);
        if (!newPointer) {
            LOG_E("Leave, Move or Up Not Exist Pointer inputDeviceId:$s pointerId:$s", inputDeviceId, pointerId);
            return nullptr;
        }

        *newPointer.get() = *pointer.get();
        newPointer->SetId(newPointerId);
    } else {
        LOG_E("Leave, Invalid pointerAction:$s", PointerEvent::ActionToString(pointerAction));
        return nullptr;
    }

    pointerEvent_->SetPointerId(newPointerId);
    pointerEvent_->SetActionTime(actionTime);
    pointerEvent_->SetPointerAction(pointerAction);
    pointerEvent_->AssignNewId();
    return pointerEvent_;
}

bool LogicalDisplayState::Transform(int32_t globalX, int32_t globalY, bool canOutOfDisplay,
        int32_t& displayX, int32_t& displayY) const
{
    LOG_D("Enter, globalX:$s, globalY:$s,  topLeftX_:$s, width_:$s, topLeftY_:$s, height_:$s",
            globalX, globalY, topLeftX_, width_, topLeftY_, height_);

    if (!canOutOfDisplay) {
        if (globalX < topLeftX_) {
            return false;
        }

        if (globalX > topLeftX_ + width_) {
            return false;
        }

        if (globalY < topLeftY_) {
            return false;
        }

        if (globalY > topLeftY_ + height_) {
            return false;
        }
    }

    displayX = globalX - topLeftX_;
    displayY = globalY - topLeftY_;
    LOG_D("Leave, displayX:$s displayY:$s", displayX, displayY);
    return true;
}

void LogicalDisplayState::LogicalDisplayState::MakeInDisplay(int32_t& displayX, int32_t& displayY) const
{
    if (displayX < 0) {
        displayX = 0;
    } else if (displayX > width_) {
        displayX = width_;
    }

    if (displayY < 0) {
        displayY = 0;
    } else if (displayY > height_) {
        displayY = height_;
    }
}

std::ostream& LogicalDisplayState::operator<<(std::ostream& outStream) const {
    outStream << '{'
        << "id:" << id_ << ','
        << "topLeftX:" << topLeftX_ << ','
        << "topLeftY:" << topLeftY_ << ','
        << "width:" << width_ << ','
        << "height:" << height_ << ','
        << "leftDisplayId:" << leftDisplayId_ << ','
        << "upDisplayId:" << upDisplayId_ << ','
        << "seatId:" << seatId_ << ','
        << "seatName:" << seatName_ << ','
        << "inputWindows" << inputWindows_
        << '}';
    return outStream;
}

std::ostream& operator<<(std::ostream& outStream, const LogicalDisplay& display)
{
    outStream << '{'
        << "id:" << display.id << ','
        << "topLeftX:" << display.topLeftX << ','
        << "topLeftY:" << display.topLeftY  << ','
        << "width:" << display.width << ','
        << "height:" << display.height << ','
        << "leftDisplayId:" << display.leftDisplayId << ','
        << "upDisplayId:" << display.upDisplayId << ','
        << "seatId:" << display.seatId << ','
        << "seatName:" << display.seatName << ','
        << '}';
    return outStream;
}

std::ostream& operator<<(std::ostream& outStream, const LogicalDisplayState& display)
{
    return display.operator<<(outStream);
}

}
