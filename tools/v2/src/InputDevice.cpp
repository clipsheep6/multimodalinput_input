#include <iostream>
#include <cstring>
#include <functional>
#include <iomanip>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/input.h>
#include <linux/input-event-codes.h>

#include "InputDevice.h"
#include "IEventLooper.h"
#include "Log.h"
#include "EnumUtils.h"
#include "IoctlUtils.h"
#include "TimeUtils.h"
#include "ResourceGuard.h"
#include "RelEvent.h"
#include "IdGenerator.h"

namespace Input {

    std::shared_ptr<InputDevice> InputDevice::Open(const std::string& deviceFile, const IInputContext* context)
    {
        int32_t id = IdGenerator::TakeNextInputDeviceId();
        auto inputDevice = std::shared_ptr<InputDevice>(new InputDevice(id, deviceFile, context));
        auto retCode = inputDevice->Init();
        if (retCode != 0) {
            return nullptr;
        }

        return inputDevice;
    }


    int32_t InputDevice::Init() {
        LOG_D("Enter");

        const auto& looper = context_->GetLooper();
        if (!looper) {
            LOG_E("Leave, null looper");
            return -1;
        }

        fd_ = ::open(deviceFile_.c_str(), O_CLOEXEC | O_RDWR | O_NONBLOCK);
        if (fd_ < 0) {
            LOG_E("Leave deviceFile:$s, open Failed:$s", deviceFile_, strerror(errno));
            return -1;
        }

        auto retCode = IoctlUtils::GetInputDeviceName(fd_, name_);
        if (retCode == -1) {
            LOG_E("Leave deviceFile:$s, null deviceName", deviceFile_);
            return -1;
        }

        retCode = UpdateCapablility();
        if (retCode < 0) {
            LOG_E("Leave deviceFile:$s, UpdateCapablility Failed", deviceFile_);
            return -1;
        }

        seatId_ = "seat0";
        seatName_ = "default0";

        LOG_D("Leave deviceFile:$s", deviceFile_);
        return 0;
    }

    void InputDevice::Uninit() 
    {
        CloseDevice();
    }

    InputDevice::InputDevice(int32_t id, const std::string& deviceFile, const IInputContext* context)
        : id_(id), deviceFile_(deviceFile), context_(context),
        relEventCollector_(id), keyEventCollector_(id), absEventCollector_(id, AbsEvent::SOURCE_TYPE_NONE),
        eventHandler_(IKernelEventHandler::GetDefault())
    {
    }

    InputDevice::~InputDevice()
    {
        Uninit();
    }

    int32_t InputDevice::GetId() const
    {
        return id_;
    }

    const std::string& InputDevice::GetName() const
    {
        return name_;
    }

    const std::string& InputDevice::GetSeatId() const {
        return seatId_;
    }

    const std::string& InputDevice::GetSeatName() const {
        return seatName_;
    }

    const std::string& InputDevice::GetDeviceFile() const
    {
        return deviceFile_;
    }

    std::shared_ptr<IInputDevice::AxisInfo> InputDevice::GetAxisInfo(int32_t axis) const
    {
        LOG_D("Enter device:$s axis:$s", GetName(), IInputDevice::AxisToString(axis));
        auto it = axises_.find(axis);
        if (it != axises_.end()) {
            LOG_D("Leave deivce:$s axis:$s, result:$s", GetName(), IInputDevice::AxisToString(axis), it->second);
            return it->second;
        }

        int32_t absCode = -1;
        switch (axis) {
            case IInputDevice::AXIS_MT_X:
                absCode = ABS_MT_POSITION_X;
                break;
            case IInputDevice::AXIS_MT_Y:
                absCode = ABS_MT_POSITION_Y;
                break;
            default:
                LOG_E("Leave device:$s axis:$s, Unknown axis", GetName(), IInputDevice::AxisToString(axis));
                return nullptr;
        }

        if (!HasEventCode(EV_ABS, absCode)) {
            LOG_E("Leave device:$s axis:$s, absCode:$s, InputDevice Not support axis", GetName(), IInputDevice::AxisToString(axis),
                    EnumUtils::InputEventAbsCodeToString(absCode));
            return nullptr;
        }

        struct input_absinfo abs;
        auto retCode = ::ioctl(fd_, EVIOCGABS(absCode), &abs);
        if (retCode < 0) {
            LOG_E("Leave device:$s axis:$s, absCode:$s ::ioctl Failed:$s", GetName(), IInputDevice::AxisToString(axis),
                    EnumUtils::InputEventAbsCodeToString(absCode), strerror(errno));
            return nullptr;
        }

        auto axisInfo = std::make_shared<IInputDevice::AxisInfo>();
        axisInfo->SetAxis(axis);
        axisInfo->SetMinimum(abs.minimum);
        axisInfo->SetMaximum(abs.maximum);
        axisInfo->SetFlat(abs.flat);
        axisInfo->SetFuzz(abs.fuzz);
        axisInfo->SetResolution(abs.resolution);

        axises_[axis] = axisInfo;
        LOG_D("Leave device:$s axis:$s, axisInfo:$s", GetName(), IInputDevice::AxisToString(axis), axisInfo);
        return axisInfo;
    }

    void InputDevice::OnFdEvent(int fd, int event)
    {
        LOG_D("Enter fd:$s event:$s", fd, IEventLooper::EventToString(event));
        if (fd != fd_) {
            context_->GetLooper()->RemoveFd(fd);
            LOG_E("Leave fd:$s event:$s, fd_:$s mismatch", fd, IEventLooper::EventToString(event), fd_);
            return;
        }

        if (event == IEventLooper::EVENT_IN) {
            ReadEvents();
            LOG_D("Leave fd:$s event:$s", fd, IEventLooper::EventToString(event));
            return;
        }

        if (event == IEventLooper::EVENT_HUP) {
            context_->GetLooper()->RemoveFd(fd);
            LOG_D("Leave fd:$s event:$s", fd, IEventLooper::EventToString(event));
            return;
        }

        LOG_E("Leave fd:$s event:$s, Unknown event", fd, IEventLooper::EventToString(event));
    }

    void InputDevice::ReadEvents() {
        LOG_D("Enter deviceFile:$s", deviceFile_);
        constexpr size_t MAX_COUNT_ONCE = 8;
        struct input_event inputEvent[MAX_COUNT_ONCE];

        while (true) {
            auto ret = ::read(fd_, &inputEvent[0], sizeof(inputEvent));
            if (ret < 0) {
                auto err = errno;
                if (err == EINTR) {
                    continue;
                }

                if (err == EAGAIN || err == EWOULDBLOCK) {
                    break;
                }

                LOG_E("Error:$s", strerror(err));
                CloseDevice();
                LOG_E("Leave deviceFile:$s, ReadError:$s", deviceFile_, strerror(err));
                return;
            }

            if (ret == 0) {
                break;
            }

            if ((ret % ((int)sizeof(inputEvent[0]))) != 0) {
                CloseDevice();
                LOG_E("Leave deviceFile:$s, Wrong DataSize", deviceFile_);
                break;
            }

            for (int i = 0; i < (ret / ((int)sizeof(inputEvent[0]))); ++i) {
                ProcessEventItem(&inputEvent[i]);
            }
        }

        LOG_D("Leave deviceFile:$s", deviceFile_);
    }

    int32_t InputDevice::CloseDevice() {
        LOG_D("Enter deviceFile:$s", deviceFile_);
        if (fd_ < 0) {
            LOG_D("Leave deviceFile:$s, fd < 0", deviceFile_);
            return 0;
        }

        auto fd = fd_;
        fd_ = -1;

        if (context_ != nullptr) {
            const auto& looper = context_->GetLooper();
            if (!looper) {
                looper->RemoveFd(fd);
            }
        }

        ::close(fd);

        LOG_D("Leave deviceFile:$s", deviceFile_);
        return 0;
    }

    void InputDevice::ProcessEventItem(struct input_event* eventItem) {
        auto type = eventItem->type;
        auto code = eventItem->code;
        auto value = eventItem->value;

        if (code == EV_ABS || code == EV_SYN) {
            LOG_D("deviceFile:$s, Type:$s, Code:$s, Value:$s", deviceFile_, 
                    EnumUtils::InputEventTypeToString(type), 
                    EnumUtils::InputEventCodeToString(type, code), 
                    value);
        }

        switch (type) {
            case EV_SYN:
                ProcessSyncEvent(code, value);
                break;
            case EV_KEY:
                ProcessKeyEvent(code, value);
                break;
            case EV_REL:
                ProcessRelEvent(code, value);
                break;
            case EV_ABS:
                ProcessAbsEvent(code, value);
                break;
            case EV_MSC:
                ProcessMscEvent(code, value);
                break;
            case EV_SW:
                ProcessSwEvent(code, value);
                break;
            case EV_LED:
                ProcessLedEvent(code, value);
                break;
            case EV_SND:
                ProcessSndEvent(code, value);
                break;
            case EV_REP:
                ProcessRepEvent(code, value);
                break;
            case EV_FF:
                ProcessFfEvent(code, value);
                break;
            case EV_PWR:
                ProcessPwrEvent(code, value);
                break;
            case EV_FF_STATUS:
                ProcessFfStatusEvent(code, value);
                break;
            default:
                break;
        }
    }

    int32_t InputDevice::UpdateCapablility() {
        auto retCode = UpdateInputProperty();
        if (retCode < 0) {
            return -1;
        }

        retCode = UpdateBitStat(0, EV_MAX, &evBit[0], LENTH_OF_ARRAY(evBit));
        if (retCode < 0) {
            return -1;
        }

        retCode = UpdateBitStat(EV_REL, REL_MAX, &relBit[0], LENTH_OF_ARRAY(relBit));
        if (retCode < 0) {
            return -1;
        }

        retCode = UpdateBitStat(EV_ABS, ABS_MAX, &absBit[0], LENTH_OF_ARRAY(absBit));
        if (retCode < 0) {
            return -1;
        }

        if (HasMouseCapability()) {
            capabilities_ |= IInputDevice::CAPABILITY_MOUSE;
        }

        if (HasKeyboardCapability()) {
            capabilities_ |= IInputDevice::CAPABILITY_KEYBOARD;
        }

        if (HasTouchscreenCapability()) {
            capabilities_ |= IInputDevice::CAPABILITY_TOUCHSCREEN;
            absEventCollector_.SetSourceType(AbsEvent::SOURCE_TYPE_TOUCHSCREEN);
        }

        if (HasTouchpadCapability()) {
            capabilities_ |= IInputDevice::CAPABILITY_TOUCHPAD;
            absEventCollector_.SetSourceType(AbsEvent::SOURCE_TYPE_TOUCHPAD);
            // capabilities_ |= IInputDevice::CAPABILITY_TOUCHSCREEN;
            // absEventCollector_.SetSourceType(AbsEvent::SOURCE_TYPE_TOUCHSCREEN);
        }

        return 0;
    }

    int32_t InputDevice::UpdateInputProperty() {
        auto ret = ioctl(fd_, EVIOCGPROP(sizeof(inputProperty)), &inputProperty[0]);
        if (ret < 0) {
            return -1;
        }

        for (int32_t property = 0; property <= INPUT_PROP_MAX; ++property) {
            bool has = HasInputProperty(property);
            LOG_D("InputDevice:$0 (Name:$1) $2 Property:$3", deviceFile_,  GetName(),
                    has ? "has" : "hasn't", 
                    EnumUtils::InputPropertyToString(property));
        }
        return 0;
    }

    int32_t InputDevice::UpdateBitStat(int32_t evType, int32_t maxValue, unsigned long* resultValue, size_t len) {
        auto ret = ioctl(fd_, EVIOCGBIT(evType, maxValue), resultValue);
        if (ret < 0) {
            LOG_E("Leave, Failed for $s", EnumUtils::InputEventTypeToString(evType));
            return -1;
        }

        /*
           std::stringstream debugStr;
           debugStr << EnumUtils::InputEventTypeToString(evType) << " : ";
           for (auto i = len; i > 0; --i) {
           debugStr << std::hex << std::setw(16) << std::setfill('0') << resultValue[i-1];
           }
           debugStr << "[END]";
           LOG_D("Device:$s BITSTAT:$s", GetName(), debugStr.str());
           */

        const char* typeStr = "";
        if (evType != 0) {
            typeStr = EnumUtils::InputEventTypeToString(evType);
        }

        for (int32_t item = 0; item <= maxValue; ++item) {
            const char* has = TestBit(item, resultValue, len) ? "has" : "hasn't";
            const char* valueStr = (evType == 0 ? EnumUtils::InputEventTypeToString(item) : EnumUtils::InputEventCodeToString(evType, item));
            LOG_D("InputDeviceBit:$s (Name:$s) $s $s:$s($s)", deviceFile_,  GetName(),
                    has, typeStr, valueStr, item
                 );
        }

        return 0;
    }

    bool InputDevice::TestBit(int32_t bitIndex, const unsigned long* bitMap, size_t count) const {
        if (bitIndex < 0) {
            return false;
        }

        size_t idx = bitIndex / (sizeof(unsigned long) * 8);
        if (idx >= count) {
            return false;
        }

        size_t offset = bitIndex % (sizeof(unsigned long) * 8);
        unsigned long bitOne = 1UL;
        return (bitMap[idx] & (bitOne << offset)) != 0;
    }

    bool InputDevice::HasInputProperty(int32_t property) {
        return TestBit(property, &inputProperty[0], sizeof(inputProperty) / sizeof(inputProperty[0]));
    }

    bool InputDevice::HasMouseCapability() {
        if (HasEventCode(EV_REL, REL_X) && HasEventCode(EV_REL, REL_Y)) {
            return true;
        }
        return false;
    }

    bool InputDevice::HasKeyboardCapability() {
        if (!HasEventType(EV_KEY)) {
            return false;
        }

        return true;
    }

    bool InputDevice::HasTouchscreenCapability() {
        if (!HasEventType(EV_ABS)) {
            return false;
        }

        if (HasInputProperty(INPUT_PROP_POINTER)) {
            return false;
        }

        return true;
    }

    bool InputDevice::HasTouchpadCapability() {
        if (!HasEventType(EV_ABS)) {
            return false;
        }

        if (!HasInputProperty(INPUT_PROP_POINTER)) {
            return false;
        }

        return true;
    }

    bool InputDevice::HasEventType(int32_t evType) const {
        return TestBit(evType, &evBit[0], LENTH_OF_ARRAY(evBit));
    }

    bool InputDevice::HasEventCode(int32_t evType, int32_t evCode) const {
        if (!HasEventType(evType)) {
            return false;
        }

        switch(evType) {
            case EV_REL:
                return TestBit(evCode, &relBit[0], LENTH_OF_ARRAY(relBit));
            case EV_ABS:
                return TestBit(evCode, &absBit[0], LENTH_OF_ARRAY(absBit));
            default:
                LOG_E("NOT IMPLEMENTATION");
                return false;
        }
    }

    void InputDevice::ProcessSyncEvent(int32_t code, int32_t value) {
        {
            auto event = relEventCollector_.HandleSyncEvent(code, value);
            if (event) {
                OnEventCollected(event);
                relEventCollector_.AfterProcessed();
            }
        }
        {
            const auto& event = absEventCollector_.HandleSyncEvent(code, value);
            if (event) {
                OnEventCollected(event);
                absEventCollector_.AfterProcessed();
            }
        }
    }

    void InputDevice::ProcessKeyEvent(int32_t code, int32_t value) {
        LOG_D("Enter code:$s value:$s", EnumUtils::InputEventKeyCodeToString(code), value);
        auto event = keyEventCollector_.HandleKeyEvent(code, value);
        if (event) {
            OnEventCollected(event);
            keyEventCollector_.AfterProcessed();
            LOG_D("Leave code:$s value:$s KeyEvent", EnumUtils::InputEventKeyCodeToString(code), value);
            return;
        }
        LOG_D("Leave code:$s value:$s", EnumUtils::InputEventKeyCodeToString(code), value);
    }

    void InputDevice::ProcessRelEvent(int32_t code, int32_t value) {
        LOG_D("Enter code:$s value:$s", EnumUtils::InputEventRelCodeToString(code), value);
        relEventCollector_.HandleRelEvent(code, value);
        LOG_D("Leave code:$s value:$s", EnumUtils::InputEventRelCodeToString(code), value);
    }

    void InputDevice::ProcessAbsEvent(int32_t code, int32_t value) {
        const auto& event = absEventCollector_.HandleAbsEvent(code, value);
        if (event) {
            OnEventCollected(event);
            absEventCollector_.AfterProcessed();
        }
    }

    void InputDevice::ProcessMscEvent(int32_t code, int32_t value) {
    }

    void InputDevice::ProcessSwEvent(int32_t code, int32_t value) {
    }

    void InputDevice::ProcessLedEvent(int32_t code, int32_t value) {
    }

    void InputDevice::ProcessSndEvent(int32_t code, int32_t value) {
    }

    void InputDevice::ProcessRepEvent(int32_t code, int32_t value) {
    }

    void InputDevice::ProcessFfEvent(int32_t code, int32_t value) {
    }

    void InputDevice::ProcessPwrEvent(int32_t code, int32_t value) {
    }

    void InputDevice::ProcessFfStatusEvent(int32_t code, int32_t value) {
    }

    void InputDevice::OnEventCollected(const std::shared_ptr<const RelEvent>& event) {
        if (!event) {
            return;
        }

        LOG_D("Enter");
        eventHandler_->OnInputEvent(event);
        LOG_D("Leave");
    }

    void InputDevice::OnEventCollected(const std::shared_ptr<const KernelKeyEvent>& event) {
        if (!event) {
            return;
        }

        LOG_D("Enter KernelKeyEvent");
        eventHandler_->OnInputEvent(event);
        LOG_D("Leave KernelKeyEvent");
    }

    void InputDevice::OnEventCollected(const std::shared_ptr<const AbsEvent>& event) {
        if (!event) {
            return;
        }

        LOG_D("Enter");
        eventHandler_->OnInputEvent(event);
        LOG_D("Leave");
        return;
    }

    bool InputDevice::HasCapability(int32_t capability) const
    {
        return (capabilities_ & capability) != 0;
    }

    int32_t InputDevice::StartReceiveEvents(const std::shared_ptr<IKernelEventHandler>& eventHandler)
    {
        LOG_D("Enter");
        if (!eventHandler) {
            LOG_E("Leave, null eventHandler");
            return -1;
        }

        if (context_ == nullptr) {
            LOG_E("Leave, null context_");
            return -1;
        }

        const auto& looper = context_->GetLooper();
        if (!looper) {
            LOG_E("Leave, null looper");
            return -1;
        }

        auto retCode = looper->AddFd(fd_, IEventLooper::EVENT_IN, 
                std::bind(&InputDevice::OnFdEvent, this, std::placeholders::_1, std::placeholders::_2));
        if (retCode == -1) {
            LOG_E("Leave deviceFile:$s, AddFd Failed", deviceFile_);
            return -1;
        }

        eventHandler_ = eventHandler;
        LOG_D("Leave");
        return 0;
    }

    int32_t InputDevice::StopReceiveEvents()
    {
        eventHandler_ = IKernelEventHandler::GetDefault();

        if (context_ == nullptr) {
            LOG_E("Leave, null context_");
            return -1;
        }

        const auto& looper = context_->GetLooper();
        if (!looper) {
            LOG_E("Leave, null looper");
            return -1;
        }

        auto retCode = looper->RemoveFd(fd_);
        if (retCode == -1) {
            LOG_E("Leave deviceFile:$s, RemoveFd Failed", deviceFile_);
            return -1;
        }

        return 0;
    }

}
