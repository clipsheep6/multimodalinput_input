#include "InputWindowState.h"
#include "Utils.h"
#include "Log.h"

namespace Input {

std::unique_ptr<InputWindowState> InputWindowState::CreateInstance(const IInputContext* context,
        const WindowState& windowState) {
    LOG_D("Enter windowState:$s", windowState);
    if (context == nullptr) {
        LOG_E("Enter windowState:$s, null context", windowState);
        return nullptr;
    }

    auto result = std::unique_ptr<InputWindowState>(new InputWindowState(context, windowState));
    LOG_D("Leave");
    return result;
}

InputWindowState::InputWindowState(const IInputContext* context, const WindowState& windowState)
    : context_(context), 
    id_(windowState.id), 
    topLeftX_(windowState.topLeftX), topLeftY_(windowState.topLeftY), 
    width_(windowState.width), height_(windowState.height), 
    displayId_(windowState.displayId), agentWindowId_(windowState.agentWindowId),
    changed_(CHANGED_NONE)
{
}

int32_t InputWindowState::GetId() const
{
    return id_;    
}

int32_t InputWindowState::GetDisplayId() const
{
    return displayId_;
}

uint32_t InputWindowState::GetChanged() const
{
    return changed_;
}

uint32_t InputWindowState::Update(const WindowState& windowState) {
    changed_ = CHANGED_NONE;

    if (Utils::UpdateValue(topLeftX_, windowState.topLeftX) || 
            Utils::UpdateValue(topLeftY_, windowState.topLeftY)) {
        changed_ |= CHANGED_POSITION;
    }

    if (Utils::UpdateValue(width_, windowState.width) || 
            Utils::UpdateValue(height_, windowState.height)) {
        changed_ |= CHANGED_SIZE;
    }

    if (Utils::UpdateValue(displayId_, windowState.displayId) ) {
        changed_ |= CHANGED_DISPLAY;
    }

    if (Utils::UpdateValue(agentWindowId_, windowState.agentWindowId) ) {
        changed_ |= CHANGED_DISPLAY;
    }

    return changed_;
}

std::ostream& InputWindowState::operator<<(std::ostream& outStream) const {
    outStream << '{'
        << "id:" << id_ << ','
        << "topLeftX:" << topLeftX_ << ','
        << "topLeftY:" << topLeftY_ << ','
        << "width:" << width_ << ','
        << "height:" << height_ << ','
        << "displayId:" << displayId_ << ','
        << "agentWindowId:" << agentWindowId_ << ','
        << "changed:" << changed_
        << '}';
    return outStream;
}

std::ostream& operator<<(std::ostream& outStream, const WindowState& windowState)
{
    outStream << '{'
        << "id:" << windowState.id << ','
        << "topLeftX:" << windowState.topLeftX << ','
        << "topLeftY:" << windowState.topLeftY  << ','
        << "width:" << windowState.width << ','
        << "height:" << windowState.height << ','
        << "displayId:" << windowState.displayId << ','
        << "agentWindowId:" << windowState.agentWindowId << ','
        << '}';
    return outStream;
}

std::ostream& operator<<(std::ostream& outStream, const InputWindowState& windowState)
{
    return windowState.operator<<(outStream);
}

}
