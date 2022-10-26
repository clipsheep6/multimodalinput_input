#pragma once
#include <memory>

namespace Input {

class IEventLooper;
class ISeatManager;
class IInputDeviceManager;
class IWindowStateManager;
class IEventDispatcher;

class IInputContext {
    public:
        static std::unique_ptr<IInputContext> CreateInstance();

        virtual ~IInputContext() = default;
        virtual const std::unique_ptr<IEventLooper>& GetLooper() const = 0; 
        virtual const std::unique_ptr<ISeatManager>& GetSeatManager() const = 0; 
        virtual const std::unique_ptr<IInputDeviceManager>& GetInputDeviceManager() const = 0; 
        virtual const std::unique_ptr<IWindowStateManager>& GetWindowStateManager() const = 0; 
        virtual const std::unique_ptr<IEventDispatcher>& GetEventDispatcher() const = 0; 
};

}
