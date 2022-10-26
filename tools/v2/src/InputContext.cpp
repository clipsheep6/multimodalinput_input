#include <iostream>

#include <cstring>

#include "InputContext.h"
#include "Log.h"

namespace Input {

std::unique_ptr<InputContext> InputContext::CreateInstance()
{
    LOG_D("Enter");

    auto inputContext = std::unique_ptr<InputContext>(new InputContext());
    auto retCode = InitInstance(inputContext.get());
    if (retCode != 0) {
        LOG_E("Leave");
        return nullptr;
    }
    LOG_D("Leave");
    return inputContext;
}

int32_t InputContext::InitInstance(InputContext* inputContext) {
    LOG_D("Enter");
    auto looper = IEventLooper::CreateInstance(inputContext);
    auto retCode = inputContext->SetLooper(looper);
    if (retCode == -1) {
        LOG_E("Leave, SetLooper Failed");
        return -1;
    }

    auto seatManager = ISeatManager::CreateInstance(inputContext);
    retCode = inputContext->SetSeatManager(seatManager);
    if (retCode == -1) {
        LOG_E("Leave, SetSeatManager Failed");
        return -1;
    }

    auto deviceManager = IInputDeviceManager::CreateInstance(inputContext);
    retCode = inputContext->SetDeviceManager(deviceManager);
    if (retCode == -1) {
        LOG_E("Leave, SetDeviceManager Failed");
        return -1;
    }

    auto windowStateManager = IWindowStateManager::CreateInstance(inputContext);
    retCode = inputContext->SetWindowStateManager(windowStateManager);
    if (retCode == -1) {
        LOG_E("Leave, SetWindowStateManager Failed");
        return -1;
    }

    auto eventDispatcher = IEventDispatcher::CreateInstance(inputContext);
    retCode = inputContext->SetEventDispatcher(eventDispatcher);
    if (retCode == -1) {
        LOG_E("Leave, SetEventDispatcher Failed");
        return -1;
    }

    inputContext->inputDeviceDiscoverers_ = IInputDeviceDiscoverer::Create(inputContext);

    auto robot = std::unique_ptr<Robot>(new Robot(inputContext));
    retCode = robot->Start();
    if (retCode == -1) {
        LOG_E("Leave, Robot Start Failed");
        return -1;
    }
    inputContext->robot_ = std::move(robot);
    LOG_D("Leave");
    return 0;
}

const std::unique_ptr<IEventLooper>& InputContext::GetLooper() const {
    return looper_;
}

const std::unique_ptr<IInputDeviceManager>& InputContext::GetInputDeviceManager() const {
    return inputDeviceManager_;
}

const std::unique_ptr<ISeatManager>& InputContext::GetSeatManager() const {
    return seatManager_;
}

const std::unique_ptr<IWindowStateManager>& InputContext::GetWindowStateManager() const {
    return windowStateManager_;
}

const std::unique_ptr<IEventDispatcher>& InputContext::GetEventDispatcher() const {
    return eventDispatcher_;
}

void InputContext::Run() {
    looper_->Run();
}

int32_t InputContext::SetLooper(std::unique_ptr<IEventLooper>& looper) {
    if (!looper) {
        return -1;
    }

    looper_ = std::move(looper);
    return 0;
}

int32_t InputContext::SetDeviceManager(std::unique_ptr<IInputDeviceManager>& inputDeviceManager) {
    if (!inputDeviceManager) {
        return -1;
    }

    inputDeviceManager_ = std::move(inputDeviceManager);
    return 0;
}

int32_t InputContext::SetSeatManager(std::unique_ptr<ISeatManager>& seatManager) {
    if (!seatManager) {
        return -1;
    }

    seatManager_ = std::move(seatManager);
    return 0;
}

int32_t InputContext::SetWindowStateManager(std::unique_ptr<IWindowStateManager>& windowStateManager) {
    if (!windowStateManager) {
        return -1;
    }

    windowStateManager_ = std::move(windowStateManager);
    return 0;
}

int32_t InputContext::SetEventDispatcher(std::unique_ptr<IEventDispatcher>& eventDispatcher) {
    if (!eventDispatcher) {
        return -1;
    }

    eventDispatcher_ = std::move(eventDispatcher);
    return 0;
}

}
