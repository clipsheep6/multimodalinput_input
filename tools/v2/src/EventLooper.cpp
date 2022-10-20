#include <list>

#include <sys/epoll.h>
#include <unistd.h>
#include <limits>
#include <cstring>

#include "EventLooper.h"
#include "TimeUtils.h"
#include "Log.h"

namespace Input 
{
    std::unique_ptr<EventLooper> EventLooper::CreateInstance(IInputContext* context)
    {
        LOG_D("Enter");
        if (context == nullptr) {
            LOG_E("Leave, null context");
            return nullptr;
        }
        auto epollFd = epoll_create1(EPOLL_CLOEXEC);
        if (epollFd < 0) {
            LOG_E("Leave, epoll_create1 Failed:$s", strerror(errno));
            return nullptr;
        }

        auto result = std::unique_ptr<EventLooper>(new EventLooper(epollFd, context));
        LOG_D("Leave");
        return result;
    }

    EventLooper::EventLooper(int epollFd, IInputContext* context)
        : epollFd_(epollFd), context_(context)
    {
    }

    EventLooper::~EventLooper()
    {
        int fd = epollFd_;
        epollFd_ = INVALID_FD;
        ::close(fd);
    }

    void EventLooper::Run() {
        constexpr size_t MAX_EPOLL_EVENT = 64;
        struct epoll_event epollEvents[MAX_EPOLL_EVENT];
        while (true) {
            auto timeoutMs = CalcTimeoutMs();
            auto ret = epoll_wait(epollFd_, &epollEvents[0], MAX_EPOLL_EVENT, timeoutMs);
            if (ret == -1) {
                if (errno == EINTR) {
                    continue;
                }
                LOG_E("epoll_wailt");
                break;
            }

            if (ret == 0) {
                ProcessTimers();
                continue;
            }

            for (auto i = 0; i < ret; ++i) {
                struct epoll_event& item = epollEvents[i];
                auto it = fdItems_.find(item.data.fd);
                if (it == fdItems_.end()) {
                    continue;
                }

                //ProcessFdEvent(item.events, (FdItem*)item.data.ptr);
                ProcessFdEvent(item.events, &it->second);
            }
        }
    }

    int32_t EventLooper::AddFd(int fd, int events, std::function<void(int, int)> callback) 
    {
        if (FindFdItem(fd) != nullptr) {
            errno = EEXIST;
            return -1;
        }

        if (events == 0) {
            errno = EINVAL;
            return -1;
        }

        if ((events & (~IEventLooper::EVENT_ALL)) != 0) {
            errno = EINVAL;
            return -1;
        }

        if (!callback) {
            errno = EINVAL;
            return -1;
        }

        auto& item = fdItems_[fd];
        item.fd_ = fd;
        item.events_ = events;
        item.callback_ = callback;

        struct epoll_event epollEvent;
        epollEvent.events = events;
        //epollEvent.data.ptr = &item;
        epollEvent.data.fd = fd;

        auto retCode = epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &epollEvent);
        if (retCode != 0) {
            fdItems_.erase(fd);
            return -1;
        }

        return 0;
    }

    int32_t EventLooper::UpdateFd(int fd, int events) 
    {
        auto item = FindFdItem(fd);
        if (item == nullptr) {
            errno = EINVAL;
            return -1;
        }

        if (events == 0) {
            errno = EINVAL;
            return -1;
        }

        if ((events & (~IEventLooper::EVENT_ALL)) != 0) {
            errno = EINVAL;
            return -1;
        }

        if (item->events_ == events) {
            return 0;
        }

        struct epoll_event epollEvent;
        epollEvent.events = events;
        //epollEvent.data.ptr = &item;
        epollEvent.data.fd = fd;
        auto retCode = epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &epollEvent);
        if (retCode != 0) {
            return -1;
        }

        item->events_ = events;
        return 0;
    }

    int32_t EventLooper::UpdateFd(int fd, std::function<void(int, int)> callback) 
    {
        auto item = FindFdItem(fd);
        if (item == nullptr) {
            errno = EINVAL;
            return -1;
        }

        if (!callback) {
            errno = EINVAL;
            return -1;
        }

        item->callback_ = callback;
        return 0;
    }

    int32_t EventLooper::UpdateFd(int fd, int events, std::function<void(int, int)> callback) 
    {
        auto item = FindFdItem(fd);
        if (item == nullptr) {
            errno = EINVAL;
            return -1;
        }

        if (events == 0) {
            errno = EINVAL;
            return -1;
        }

        if ((events & (~IEventLooper::EVENT_ALL)) != 0) {
            errno = EINVAL;
            return -1;
        }

        if (item->events_ == events) {
            return 0;
        }

        struct epoll_event epollEvent;
        epollEvent.events = events;
        // epollEvent.data.ptr = &item;
        epollEvent.data.fd = fd;
        auto retCode = epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &epollEvent);
        if (retCode != 0) {
            return -1;
        }

        item->events_ = events;
        item->callback_ = callback;
        return 0;
    }

    int32_t EventLooper::RemoveFd(int fd) 
    {
        if (!FindFdItem(fd)) {
            errno = EINVAL;
            return -1;    
        }

        auto retCode = epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr);
        if (retCode != 0) {
            return -1;
        }

        fdItems_.erase(fd);
        return 0;
    }

    int32_t EventLooper::AddTimer(int32_t interval, int32_t repeatCount, std::function<void()> callback) 
    {
        LOG_D("Enter interval:$s repeatCount:$s", interval, repeatCount);
        if (!callback) {
            LOG_E("Enter interval:$s repeatCount:$s, null callback", interval, repeatCount);
            return -1;
        }
        if (timerItems_.size() >= IEventLooper::MAX_TIMER_COUNT) {
            LOG_E("Leave interval:$s repeatCount:$s, MAX TIMER COUNT:$s", 
                    interval, repeatCount, IEventLooper::MAX_TIMER_COUNT);
            return -1;
        }

        if (interval < IEventLooper::MIN_TIMER_INTERVAL) {
            interval = IEventLooper::MIN_TIMER_INTERVAL;
        } else if (interval > IEventLooper::MAX_TIMER_INTERVAL) {
            interval = IEventLooper::MAX_TIMER_INTERVAL;
        }

        auto curTime = TimeUtils::GetMonotonicTimeMs();

        auto id = TakeNextTimerId();
        auto& item = timerItems_[id];
        item.id_ = id;
        item.interval_ = interval;
        item.repeatCount_ = repeatCount;
        item.callback_ = callback;
        item.nextCallTime_ = curTime + interval;

        LOG_D("Leave interval:$s repeatCount:$s", interval, repeatCount);
        return 0;
    }

    int32_t EventLooper::RemoveTimer(int32_t timerId) 
    {
        LOG_D("Enter timerId:$s", timerId);
        auto count = timerItems_.erase(timerId);
        if (count != 1) {
            LOG_E("Leave timerId:$s, Not Exist", timerId);
            return -1;
        }

        LOG_D("Leave timerId:$s", timerId);
        return 0;
    }

    int32_t EventLooper::ExitAsync()
    {
        return 0;
    }

    EventLooper::FdItem* EventLooper::FindFdItem(int fd) {
        auto it = fdItems_.find(fd);
        if (it == fdItems_.end()) {
            return nullptr;
        }

        return &it->second;
    }

    int32_t EventLooper::ProcessFdEvent(int events, FdItem* item) {
        if (item == nullptr) {
            errno = EINVAL;
            return -1;
        }

        item->callback_(item->fd_, events);
        return 0;
    }

    int32_t EventLooper::TakeNextTimerId() {
        int32_t timerId = 0;
        for (auto& item : timerItems_) {
            if (item.first > timerId) {
                break;
            }

            timerId = item.first + 1;
        }
        return timerId;
    }

    int32_t EventLooper::CalcTimeoutMs() {
        LOG_D("Enter");
        if (timerItems_.empty()) {
            LOG_D("Leave, timeoutMs:-1, NoTimer");
            return -1;
        }

        auto curTime = TimeUtils::GetMonotonicTimeMs();
        int64_t timeoutMs = IEventLooper::MAX_TIMER_INTERVAL;
        for (auto& item : timerItems_) {
            const auto& timer = item.second;
            if (timer.nextCallTime_ - curTime < timeoutMs) {
                timeoutMs = timer.nextCallTime_ - curTime;
            }
        }

        if (timeoutMs < IEventLooper::MIN_TIMEOUT_MS) {
            timeoutMs = IEventLooper::MIN_TIMEOUT_MS;
        } else if (timeoutMs > IEventLooper::MAX_TIMER_INTERVAL) {
            timeoutMs = IEventLooper::MAX_TIMER_INTERVAL;
        }

        LOG_D("Leave, timeoutMs:$s, NoTimer", timeoutMs);
        return (int32_t)timeoutMs;
    }

    void EventLooper::ProcessTimers() {
        LOG_D("Enter");
        auto curTime = TimeUtils::GetMonotonicTimeMs();
        std::list<std::function<void()>> callbacks;

        for (auto it = timerItems_.begin(); it != timerItems_.end(); ) {
            auto& timer = it->second;
            if (timer.nextCallTime_ > curTime) {
                ++it;
                continue;
            }

            callbacks.push_back(timer.callback_);
            if (timer.repeatCount_ > 0) {
                --timer.repeatCount_;
                if (timer.repeatCount_ == 0) {
                    it = timerItems_.erase(it);
                    continue;
                }
            }

            timer.nextCallTime_ += timer.interval_;
        }

        for (auto& func : callbacks) {
            func();
        }

        LOG_D("Leave");
    }
}
