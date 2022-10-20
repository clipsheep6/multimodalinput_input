#pragma once
#include <map>
#include <list>

#include "IInputDefine.h"
#include "IEventLooper.h"

namespace Input {

class EventLooper : public NonCopyable, public IEventLooper {
    public:
        static std::unique_ptr<EventLooper> CreateInstance(IInputContext* context);
    public:
        virtual ~EventLooper();

        virtual void Run() override;
 
        virtual int32_t AddFd(int fd, int events, std::function<void(int, int)> callback) override;
        virtual int32_t UpdateFd(int fd, int events) override;
        virtual int32_t UpdateFd(int fd, std::function<void(int, int)> callback) override;
        virtual int32_t UpdateFd(int fd, int events, std::function<void(int, int)> callback) override;
        virtual int32_t RemoveFd(int fd) override;

        virtual int32_t AddTimer(int32_t interval, int32_t repeatCount, std::function<void()> callback) override;
        virtual int32_t RemoveTimer(int32_t timerId) override;
        virtual int32_t ExitAsync() override;

    protected:
        EventLooper(int epollFd, IInputContext* context);
        
    private:
        struct FdItem : public NonCopyable {
            int fd_;
            int events_;
            std::function<void(int, int)> callback_;
        };

        struct TimerItem : public NonCopyable {
            int32_t id_;
            int32_t interval_;
            int repeatCount_;
            std::function<void()> callback_;
            int64_t nextCallTime_;
        };

    private:
        FdItem* FindFdItem(int fd);
        int32_t ProcessFdEvent(int events, FdItem* item);
        int32_t TakeNextTimerId();
        int32_t CalcTimeoutMs();
        void ProcessTimers();

    private:
        std::map<int, FdItem> fdItems_;
        std::map<int, TimerItem> timerItems_;

        int epollFd_ {INVALID_FD};
        [[maybe_unused]] IInputContext*const context_;
};

}
