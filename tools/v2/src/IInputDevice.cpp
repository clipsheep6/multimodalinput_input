#include <linux/input-event-codes.h>

#include "IInputDevice.h"
#include "IInputDefine.h"

namespace Input {
    const std::unique_ptr<IInputDevice::AxisInfo> IInputDevice::AxisInfo::NULL_VALUE;

    std::ostream& IInputDevice::AxisInfo::Print(std::ostream& outStream) const
    {
        outStream << '{'
            <<"axis:" << IInputDevice::AxisToString(axis_) << ','
            <<"minimum:" << minimum_ << ','
            <<"maximum:" << maximum_ << ','
            <<"fuzz:" << fuzz_ << ','
            <<"flat:" << flat_ << ','
            <<"resolution:" << resolution_
            << '}';
        return outStream;
    }

    const char* IInputDevice::AxisToString(int32_t axis)
    {
        switch(axis) {
            CASE_STR(AXIS_NONE);
            CASE_STR(AXIS_MT_X);
            CASE_STR(AXIS_MT_Y);
            CASE_STR(AXIS_MAX);
            default:
            return "AXIS_UNKNOWN";
        }
    }

    int32_t IInputDevice::AxisInfo::GetAxis() const
    {
        return axis_; 
    }

    int32_t IInputDevice::AxisInfo::GetMinimum() const
    {
        return minimum_;
    }

    int32_t IInputDevice::AxisInfo::GetMaximum() const
    {
        return maximum_;
    }

    int32_t IInputDevice::AxisInfo::GetFuzz() const
    {
        return fuzz_;    
    }

    int32_t IInputDevice::AxisInfo::GetFlat() const
    {
        return flat_;    
    }

    int32_t IInputDevice::AxisInfo::GetResolution() const
    {
        return resolution_;
    }

    void IInputDevice::AxisInfo::SetAxis(int32_t axis)
    {
        axis_ = axis;
    }

    void IInputDevice::AxisInfo::SetMinimum(int32_t minimum)
    {
        minimum_ = minimum;
    }

    void IInputDevice::AxisInfo::SetMaximum(int32_t maximum)
    {
        maximum_ = maximum;
    }

    void IInputDevice::AxisInfo::SetFuzz(int32_t fuzz)
    {
        fuzz_ = fuzz; 
    }

    void IInputDevice::AxisInfo::SetFlat(int32_t flat)
    {
        flat_ = flat; 
    }

    void IInputDevice::AxisInfo::SetResolution(int32_t resolution)
    {
        resolution_ = resolution; 
    }

    std::ostream& operator<<(std::ostream& outStream, const IInputDevice::AxisInfo& axisInfo)
    {
        return axisInfo.Print(outStream);
    }

    std::ostream& operator<<(std::ostream& outStream, const IInputDevice::AxisInfo* axisInfo)
    {
        if (axisInfo != nullptr) {
            return axisInfo->Print(outStream);
        }

        outStream << "(null)";
        return outStream;
    }

    std::ostream& operator<<(std::ostream& outStream, const std::shared_ptr<IInputDevice::AxisInfo>& axisInfo)
    {
        return operator<<(outStream, axisInfo.get());
    }

    std::ostream& operator<<(std::ostream& outStream, const std::unique_ptr<IInputDevice::AxisInfo>& axisInfo)
    {
        return operator<<(outStream, axisInfo.get());
    }
}

