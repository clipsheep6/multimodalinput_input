#pragma once
#include "IInputDeviceDiscoverer.h"
#include "IInputDefine.h"

namespace Input {

    class IInputContext;
    class LinuxInputDeviceDiscoverer : public NonCopyable, public IInputDeviceDiscoverer {
        public:
            static std::shared_ptr<IInputDeviceDiscoverer> CreateInstance(const IInputContext* context);
        public:
            virtual ~LinuxInputDeviceDiscoverer();

        protected:
            LinuxInputDeviceDiscoverer(const IInputContext* context);

        private:
            bool Start();
            void Stop();

            void StartInternal();
            void ClearInitTask();

            void Open(const std::string& deviceFile) const;
            void OnInputDevicePathNotify(int32_t fd, int32_t events) const;
            void NotifyDeviceRemoved(const std::string& deviceFile) const;
            
        private:
            const IInputContext* const context_;
            int32_t initTaskId_{-1};
            int32_t inotifyFd_ {-1};
            int32_t inotifyWatchId_{-1};
    };
}

