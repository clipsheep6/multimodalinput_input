#pragma once

#include <memory>
#include <functional>

namespace Input {

class IInputContext;
class IEventLooper {
    public:
        static const int EVENT_IN;
        static const int EVENT_OUT;
        static const int EVENT_RDHUP;
        static const int EVENT_PRI;
        static const int EVENT_ERR;
        static const int EVENT_HUP;
        static const int EVENT_ET;
        static const int EVENT_ONESHOT;
        static const int EVENT_WAKEUP;
        static const int EVENT_EXCLUSIVE;
        static const int EVENT_ALL;

        static constexpr int MAX_TIMER_COUNT = 256;
        static constexpr int MIN_TIMER_INTERVAL = 50;
        static constexpr int MAX_TIMER_INTERVAL = 8192;
        static constexpr int MIN_TIMEOUT_MS = MIN_TIMER_INTERVAL - 16;

    public:
        static std::unique_ptr<IEventLooper> CreateInstance(IInputContext* context);
        static const char* EventToString(int event);

    public:
        virtual ~IEventLooper() = default;

        virtual void Run() = 0;

        virtual int32_t AddFd(int fd, int events, std::function<void(int, int)> callback) = 0;
        virtual int32_t UpdateFd(int fd, int events) = 0;
        virtual int32_t UpdateFd(int fd, std::function<void(int, int)> callback) = 0;
        virtual int32_t UpdateFd(int fd, int events, std::function<void(int, int)> callback) = 0;
        virtual int32_t RemoveFd(int fd) = 0;
        virtual int32_t AddTimer(int32_t interval, int32_t repeatCount, std::function<void()> callback) = 0;
        virtual int32_t RemoveTimer(int32_t timerId) = 0;
        virtual int32_t ExitAsync() = 0;
};

}

