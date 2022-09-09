#ifndef C5102E32_B38C_4E71_8063_A57DBA96B8A1
#define C5102E32_B38C_4E71_8063_A57DBA96B8A1


#include <unistd.h>

struct MmiHdfEvent {

};

class HdfAdapter {
    typedef std::function<void(void *event)> HdfEventCallback;
public:
    int32_t Init();
    int32_t DeInit();
    int32_t GetInputFd() const;
    void EventDispatch(struct epoll_event& ev);
    void OnEventHandler();
    void OnEventCallBack(void *data);
private:
    int32_t fd_ { -1 };
    HdfEventCallback callback_ = nullptr;
};

void HdfAdapter::EventDispatch(struct epoll_event& ev)
{
    CALL_DEBUG_ENTER;
    CHKPV(ev.data.ptr);
    if ((ev.events & EPOLLERR) || (ev.events & EPOLLHUP)) {
        MMI_HILOGF("Epoll unrecoverable error,"
            "The service must be restarted. fd:%{public}d", fd);
        free(ev.data.ptr);
        ev.data.ptr = nullptr;
        return;
    }
    auto data = static_cast<const MmiHdfEvent *>(ev.data.ptr);
    CHKPV(data);
    OnEventHandler(*data);
}

void HdfAdapter::OnEventHandler(const MmiHdfEvent &data)
{
    CALL_DEBUG_ENTER;
    CHKPV(callback_);
    callback_(data);   
}

int32_t HdfAdapter::Init()
{
   int fds[2] = {};
   int32_t ret = pipe(fds);
   close(fds[1]);
   fd_ = fds[0];
}

void HdfAdapter::OnEventCallBack(void *data)
{
    if (callback_ == nullptr) {
        return;
    }

    callback_(data);
}



#endif /* C5102E32_B38C_4E71_8063_A57DBA96B8A1 */
