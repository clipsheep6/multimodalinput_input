#include <cstring>
#include <functional>

#include <dirent.h>
#include <sys/inotify.h>
#include <unistd.h>

#include "LinuxInputDeviceDiscoverer.h"
#include "IInputContext.h"
#include "IEventLooper.h"
#include "IoUtils.h"
#include "InputDevice.h"
#include "IInputDeviceManager.h"
#include "Log.h"

namespace Input {

    std::shared_ptr<IInputDeviceDiscoverer> LinuxInputDeviceDiscoverer::CreateInstance(const IInputContext* context)
    {
        if (context == nullptr) {
            LOG_E("Leave, null context");
            return nullptr;
        }

        auto discoverer = std::shared_ptr<LinuxInputDeviceDiscoverer>(new LinuxInputDeviceDiscoverer(context));
        if (!discoverer->Start()) {
            LOG_E("Leave, Start Failed");
            return nullptr;
        }

        return discoverer;
    }

    LinuxInputDeviceDiscoverer::LinuxInputDeviceDiscoverer(const IInputContext* context)
        : context_(context)
    {
    }

    LinuxInputDeviceDiscoverer::~LinuxInputDeviceDiscoverer()
    {
        ClearInitTask();
        Stop();
    }

    bool LinuxInputDeviceDiscoverer::Start()
    {
        LOG_D("Enter");

        if (context_ == nullptr) {
            LOG_E("Leave, null context_");
            return false;
        }

        const auto& looper = context_->GetLooper();
        if (!looper) {
            LOG_E("Leave, null looper");
            return false;
        }

        initTaskId_ = looper->AddTimer(0, 1, std::bind(&LinuxInputDeviceDiscoverer::StartInternal, this));
        if (initTaskId_ < 0) {
            LOG_E("Leave, Add Init Task Failed");
            return false;
        }

        inotifyFd_ = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
        if (inotifyFd_ < 0) {
            LOG_E("Leave, init inotifyFd_ Failed:$s", strerror(errno));
            return false;
        }

        inotifyWatchId_ = inotify_add_watch(inotifyFd_, "/dev/input/", IN_CREATE | IN_DELETE);
        if (inotifyWatchId_ < 0) {
            LOG_E("Leave, inotify Add Watch Failed:$s", strerror(errno));
            return false;
        }

        auto retCode = looper->AddFd(inotifyFd_, IEventLooper::EVENT_IN, 
                std::bind(&LinuxInputDeviceDiscoverer::OnInputDevicePathNotify, this, std::placeholders::_1,
                    std::placeholders::_2));
        if (retCode < 0) {
            LOG_E("Leave, Add inotifyFd_ to Looper Failed");
            return false;
        }

        LOG_D("Leave");
        return true;
    }

    void LinuxInputDeviceDiscoverer::Stop()
    {
    }

    void LinuxInputDeviceDiscoverer::StartInternal()
    {
        const std::string devicePath("/dev/input/");
        const std::string devicePathPrefix("event");
        auto retCode = IoUtils::ForeachFile(devicePath.c_str(), std::bind(&LinuxInputDeviceDiscoverer::Open, this, std::placeholders::_1));
        if (retCode < 0) {
            LOG_E("Error:$s", strerror(errno));
            // return;
        }
    }

    void LinuxInputDeviceDiscoverer::ClearInitTask()
    {
        if (initTaskId_ < 0) {
            return;
        }

        int32_t initTaskId = initTaskId_;
        initTaskId_ = -1;
        if (context_ == nullptr) {
            LOG_E("null context_");
            return;
        } 
        const auto& looper = context_->GetLooper();
        if (!looper) {
            LOG_E("Leave, null looper");
            return;
        } 

        auto retCode = looper->RemoveTimer(initTaskId);
        if (retCode < 0) {
            LOG_E("Leave, RemoveTimer Failed");
            return;
        }
    }

    void LinuxInputDeviceDiscoverer::Open(const std::string& deviceFile) const
    { 
        LOG_D("Enter $s", deviceFile);
        if (context_ == nullptr) {
            LOG_E("Leave $s, null context_", deviceFile);
            return;
        }

        const auto& inputDeviceManager = context_->GetInputDeviceManager();
        if (!inputDeviceManager) {
            LOG_E("Leave $s, null inputDeviceManager", deviceFile);
            return;
        }

        auto device = InputDevice::Open(deviceFile, context_);
        if (!device) {
            LOG_E("Leave $s, null device", deviceFile);
            return;
        }

        bool success = inputDeviceManager->AddDevice(device);
        if (!success) {
            LOG_E("Leave $s, add device Failed", deviceFile);
            return;
        }

        LOG_D("Leave $s", deviceFile);
    }

    void LinuxInputDeviceDiscoverer::OnInputDevicePathNotify(int32_t fd, int32_t events) const
    {
        if (fd != inotifyFd_) {
            LOG_E("Leave, fd:$s != inotifyFd_:$s", fd, inotifyFd_);
            return;
        }

        constexpr int MAX_NAME_LEN = 128;
        char buffer[sizeof(inotify_event) + MAX_NAME_LEN + 1];
        for (;;) {
            auto retCode = ::read(fd, &buffer[0], sizeof(buffer));
            if (retCode < 0) {
                LOG_E("Leave, read Failed:$s", strerror(errno));
                return;
            }

            if (retCode == 0) {
                LOG_D("Leave");
                break;
            }

            for (size_t curPos = 0; ;) {
                if (curPos == (size_t)retCode) {
                    break;
                }

                if (curPos + sizeof(inotify_event) > (size_t)retCode) {
                    LOG_E("Leave, Invalid inotify_event data size(curPos:$s, retCode:$s)", curPos, retCode);
                    return;
                }

                inotify_event* event = (inotify_event*)(buffer + curPos);
                curPos += sizeof(inotify_event);
                if (event->len <= 0) {
                    continue;
                }
                curPos += event->len;

                std::string deviceFile = std::string("/dev/input/") + event->name;
                if (event->mask == IN_CREATE) {
                    Open(deviceFile);
                    continue;
                } 

                if (event->mask == IN_DELETE) {
                    NotifyDeviceRemoved(deviceFile);
                    continue;
                } 

                LOG_W("Unknown inotify event:$s", event->mask);
            }
        }
        LOG_D("Leave");
    }

    void LinuxInputDeviceDiscoverer::NotifyDeviceRemoved(const std::string& deviceFile) const
    {
        LOG_D("Enter $s", deviceFile);
        if (context_ == nullptr) {
            LOG_E("Leave $s, null context_", deviceFile);
            return;
        }

        const auto& inputDeviceManager = context_->GetInputDeviceManager();
        if (!inputDeviceManager) {
            LOG_E("Leave $s, null inputDeviceManager", deviceFile);
            return;
        }

        auto inputDevice = inputDeviceManager->RemoveDevice(deviceFile);
        if (!inputDevice) {
            LOG_E("Leave $s, null inputDevice", deviceFile);
            return;
        }

        LOG_D("Leave $s", deviceFile);
    }

}
