#include "PhysicalDisplayState.h"
#include "Utils.h"
#include "Log.h"

namespace Input {

const std::unique_ptr<PhysicalDisplayState> PhysicalDisplayState::NULL_VALUE;

std::unique_ptr<PhysicalDisplayState> PhysicalDisplayState::CreateInstance(const IInputContext* context,
        const PhysicalDisplay& display) {
    LOG_D("Enter display:$s", display);
    if (context == nullptr) {
        LOG_E("Leave display:$s, null context", display);
        return nullptr;
    }

    auto result = std::unique_ptr<PhysicalDisplayState>(new PhysicalDisplayState(context, display));
    LOG_D("Leave");
    return result;
}

PhysicalDisplayState::PhysicalDisplayState(const IInputContext* context, const PhysicalDisplay& display)
    : context_(context), 
    id_(display.id), 
    width_(display.width), height_(display.height), 
    logicalWidth_(display.logicalWidth), logicalHeight_(display.logicalHeight), 
    leftDisplayId_(display.leftDisplayId), upDisplayId_(display.upDisplayId),
    seatId_(display.seatId), seatName_(display.seatName), name_(display.name),
    changed_(CHANGED_NONE)
{
}

int32_t PhysicalDisplayState::GetId() const
{
    return id_;
}

const std::string& PhysicalDisplayState::GetSeatId() const
{
    return seatId_;
}

const std::string& PhysicalDisplayState::GetSeatName() const
{
    return seatName_;
}

int32_t PhysicalDisplayState::GetLeftDisplayId() const
{
    return leftDisplayId_;    
}

int32_t PhysicalDisplayState::GetUpDisplayId() const
{
    return upDisplayId_;
}

int32_t PhysicalDisplayState::GetWidth() const
{
    return width_; 
}

int32_t PhysicalDisplayState::GetHeight() const
{
    return height_;
}

uint32_t PhysicalDisplayState::GetChanged() const
{
    return changed_;
}

uint32_t PhysicalDisplayState::Update(const PhysicalDisplay& display) {
    changed_ = CHANGED_NONE;
    if (Utils::UpdateValue(width_, display.width) || 
            Utils::UpdateValue(height_, display.height)) {
        changed_ |= CHANGED_PHYSICAL_SIZE;
    }

    if (Utils::UpdateValue(logicalWidth_, display.logicalWidth) || 
            Utils::UpdateValue(logicalHeight_, display.logicalHeight)) {
        changed_ |= CHANGED_LOGICAL_SIZE;
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

    if (Utils::UpdateValue(name_, display.name)) {
        changed_ |= CHANGED_NAME;
    }

    return changed_;
}

int32_t PhysicalDisplayState::TransformX(int32_t xPos, int32_t width) const
{
    if (width <= 0) {
        return -1;
    }

    int64_t one = 1;
    auto result = (int32_t)(one * xPos * logicalWidth_ / width);
    LOG_D("xPos:$s width:$s logicalWidth:$s result:$s", xPos, width, logicalWidth_, result);
    return result;
}

int32_t PhysicalDisplayState::TransformY(int32_t yPos, int32_t height) const
{
    if (height <= 0) {
        return -1;
    }

    int64_t one = 1;
    auto result = (int32_t)(one * yPos * logicalHeight_ / height);
    LOG_D("yPos:$s height:$s logicalHeight_:$s result:$s", yPos, height, logicalHeight_, result);
    return result;
}

std::ostream& PhysicalDisplayState::operator<<(std::ostream& outStream) const {
    outStream << '{'
        << "id:" << id_ << ','
        << "width:" << width_ << ','
        << "height:" << height_ << ','
        << "logicalWidth:" << logicalWidth_ << ','
        << "logicalHeight:" << logicalHeight_ << ','
        << "leftDisplayId:" << leftDisplayId_ << ','
        << "upDisplayId:" << upDisplayId_ << ','
        << "seatId:" << seatId_ << ','
        << "seatName:" << seatName_ << ','
        << "name:" << name_ 
        << '}';
    return outStream;
}

std::ostream& operator<<(std::ostream& outStream, const PhysicalDisplay& display)
{
    outStream << '{'
        << "id:" << display.id << ','
        << "width:" << display.width << ','
        << "height:" << display.height << ','
        << "logicalWidth:" << display.logicalWidth << ','
        << "logicalHeight:" << display.logicalHeight << ','
        << "leftDisplayId:" << display.leftDisplayId << ','
        << "upDisplayId:" << display.upDisplayId << ','
        << "seatId:" << display.seatId << ','
        << "seatName:" << display.seatName << ','
        << "name:" << display.name 
        << '}';
    return outStream;
}

std::ostream& operator<<(std::ostream& outStream, const PhysicalDisplayState& display)
{
    return display.operator<<(outStream);
}

}
