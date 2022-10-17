#pragma once
#include <memory>
#include <list>

#include "IInputDefine.h"
#include "IInputContext.h"
#include "IEventLooper.h"
#include "IInputDeviceManager.h"
#include "ISeatManager.h"
#include "IWindowStateManager.h"
#include "IEventDispatcher.h"
#include "IInputDeviceDiscoverer.h"

#include "Robot.h"

namespace Input {

class InputContext : public NonCopyable, public IInputContext {
    public:
        static std::unique_ptr<InputContext> CreateInstance();

    protected:
        static int32_t InitInstance(InputContext* context);

    public:
        virtual ~InputContext() = default;

        virtual const std::unique_ptr<IEventLooper>& GetLooper() const override; 
        virtual const std::unique_ptr<ISeatManager>& GetSeatManager() const override; 
        virtual const std::unique_ptr<IInputDeviceManager>& GetInputDeviceManager() const override; 
        virtual const std::unique_ptr<IWindowStateManager>& GetWindowStateManager() const override; 
        virtual const std::unique_ptr<IEventDispatcher>& GetEventDispatcher() const override; 

        void Run();

    protected:
        InputContext() = default;
        int32_t SetLooper(std::unique_ptr<IEventLooper>& looper);
        int32_t SetDeviceManager(std::unique_ptr<IInputDeviceManager>& inputDeviceManager);
        int32_t SetSeatManager(std::unique_ptr<ISeatManager>& seatManager);
        int32_t SetWindowStateManager(std::unique_ptr<IWindowStateManager>& windowStateManager);
        int32_t SetEventDispatcher(std::unique_ptr<IEventDispatcher>& eventDispatcher);

    private:
        std::unique_ptr<IEventLooper> looper_;
        std::unique_ptr<IInputDeviceManager> inputDeviceManager_;
        std::unique_ptr<ISeatManager> seatManager_;
        std::unique_ptr<IWindowStateManager> windowStateManager_;
        std::unique_ptr<IEventDispatcher> eventDispatcher_;
        std::list<std::shared_ptr<IInputDeviceDiscoverer>> inputDeviceDiscoverers_;
        std::unique_ptr<Robot> robot_;
};

}
