#include <sys/epoll.h>

#include "IEventLooper.h"
#include "IInputDefine.h"

namespace Input {
    const int IEventLooper::EVENT_IN = EPOLLIN;
    const int IEventLooper::EVENT_OUT = EPOLLOUT;
    const int IEventLooper::EVENT_RDHUP = EPOLLRDHUP;
    const int IEventLooper::EVENT_PRI = EPOLLPRI;
    const int IEventLooper::EVENT_ERR = EPOLLERR;
    const int IEventLooper::EVENT_HUP = EPOLLHUP;
    const int IEventLooper::EVENT_ET = EPOLLET;
    const int IEventLooper::EVENT_ONESHOT = EPOLLONESHOT;
    const int IEventLooper::EVENT_WAKEUP = EPOLLWAKEUP;
    const int IEventLooper::EVENT_EXCLUSIVE = EPOLLEXCLUSIVE;

    const int IEventLooper::EVENT_ALL = IEventLooper::EVENT_IN | IEventLooper::EVENT_OUT | IEventLooper::EVENT_RDHUP |
        IEventLooper::EVENT_PRI | IEventLooper::EVENT_ERR | IEventLooper::EVENT_HUP | IEventLooper::EVENT_ET |
        IEventLooper::EVENT_ONESHOT | IEventLooper::EVENT_WAKEUP | IEventLooper::EVENT_EXCLUSIVE;


    const char* IEventLooper::EventToString(int event)
    {
        switch (event) {
            CASE_STR(EVENT_IN);
            CASE_STR(EVENT_OUT);
            CASE_STR(EVENT_RDHUP);
            CASE_STR(EVENT_PRI);
            CASE_STR(EVENT_ERR);
            CASE_STR(EVENT_HUP);
            CASE_STR(EVENT_ET);
            CASE_STR(EVENT_ONESHOT);
            CASE_STR(EVENT_WAKEUP);
            CASE_STR(EVENT_EXCLUSIVE);
            default:
                return "EVENT_UNKNOWN";

        }
    }
}


