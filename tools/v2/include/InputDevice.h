#pragma once
#include <memory>
#include <map>

#include <linux/input-event-codes.h>
#include <linux/input.h>

#include "IInputDefine.h"
#include "IInputDevice.h"
#include "IInputContext.h"
#include "IEventCollector.h"
#include "ISeat.h"
#include "ITouchScreenSeat.h"
#include "RelEventCollector.h"
#include "KeyEventCollector.h"
#include "AbsEventCollector.h"
#include "KernelKeyEvent.h"
#include "AbsEvent.h"

struct input_event;

namespace Input {

class IKernelEventHandler;
class IEventLooper;
class InputDevice : public NonCopyable, public IInputDevice
{
    public:
        static std::shared_ptr<InputDevice> Open(const std::string& deviceFile, const IInputContext* context);

    public:
        virtual ~InputDevice();
        virtual int32_t GetId() const override;
        virtual const std::string& GetName() const override;
        virtual const std::string& GetSeatId() const override;
        virtual const std::string& GetSeatName() const override;
        virtual const std::string& GetDeviceFile() const override;
        virtual std::shared_ptr<AxisInfo> GetAxisInfo(int32_t axis) const override;
        virtual bool HasCapability(int32_t capability) const override;
        virtual int32_t StartReceiveEvents(const std::shared_ptr<IKernelEventHandler>& eventHandler) override;
        virtual int32_t StopReceiveEvents() override;

    protected:
        int32_t Init();
        void Uninit();

    private:
        InputDevice(int32_t id, const std::string& deviceFile, const IInputContext* context);

    private:
        void OnFdEvent(int fd, int event);
        void ReadEvents();
        int32_t CloseDevice();
        void ProcessEventItem(struct input_event* eventItem);
        int32_t UpdateCapablility();
        int32_t UpdateInputProperty();
        int32_t UpdateBitStat(int32_t evType, int32_t maxValue, unsigned long* resultValue, size_t len);
        bool TestBit(int32_t bitIndex, const unsigned long* bitMap, size_t count) const;
        bool HasInputProperty(int32_t property);
        bool HasMouseCapability();
        bool HasKeyboardCapability();
        bool HasTouchscreenCapability();
        bool HasTouchpadCapability();

        bool HasEventType(int32_t evType) const;
        bool HasEventCode(int32_t evType, int32_t evCode) const;

        void ProcessSyncEvent(int32_t code, int32_t value);
        void ProcessKeyEvent(int32_t code, int32_t value);
        void ProcessRelEvent(int32_t code, int32_t value);
        void ProcessAbsEvent(int32_t code, int32_t value);
        void ProcessMscEvent(int32_t code, int32_t value);
        void ProcessSwEvent(int32_t code, int32_t value);
        void ProcessLedEvent(int32_t code, int32_t value);
        void ProcessSndEvent(int32_t code, int32_t value);
        void ProcessRepEvent(int32_t code, int32_t value);
        void ProcessFfEvent(int32_t code, int32_t value);
        void ProcessPwrEvent(int32_t code, int32_t value);
        void ProcessFfStatusEvent(int32_t code, int32_t value);

        void OnEventCollected(const std::shared_ptr<const RelEvent>& event);
        void OnEventCollected(const std::shared_ptr<const KernelKeyEvent>& event);
        void OnEventCollected(const std::shared_ptr<const AbsEvent>& event);

    private:
        const int32_t id_;
        const std::string deviceFile_;
        const IInputContext* const context_;
        int32_t fd_;
        std::string seatId_;
        std::string seatName_;
        std::string name_;

        int32_t capabilities_ {IInputDevice::CAPABILITY_UNKNOWN};
        unsigned long inputProperty[LongsOfBits(INPUT_PROP_MAX)];
        unsigned long evBit[LongsOfBits(EV_MAX)];
        unsigned long relBit[LongsOfBits(REL_MAX)];
        unsigned long absBit[LongsOfBits(ABS_MAX)];

        RelEventCollector relEventCollector_;
        KeyEventCollector keyEventCollector_;
        AbsEventCollector absEventCollector_;

        mutable std::map<int32_t, std::shared_ptr<IInputDevice::AxisInfo>> axises_;

        std::shared_ptr<IKernelEventHandler> eventHandler_;
};

}
