/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UEVENT_MONITOR_H
#define UEVENT_MONITOR_H

#include <set>
#include <memory>

#include <sys/inotify.h>
#include <sys/socket.h>

#include <nocopyable.h>
#include <linux/netlink.h>
#include "uevent.h"
#include "device.h"

namespace OHOS {
namespace MMI {
namespace UEVENT {

class Monitor {
public:
    static std::shared_ptr<Monitor> NewFromInotify(std::shared_ptr<UEvent> uevent);

public:
    explicit Monitor(std::shared_ptr<UEvent> uevent);
    ~Monitor() = default;
    DISALLOW_COPY_AND_MOVE(Monitor);

    int GetFd() const;
    bool EnableReceiving();
    std::shared_ptr<Device> ReceiveDevice();

private:
    bool OpenConnection();
    bool IsActive() const;
    std::shared_ptr<Device> HandleInotifyEvent(struct inotify_event *event) const;
    std::shared_ptr<Device> AddDevice(const std::string &devnode) const;
    std::shared_ptr<Device> RemoveDevice(const std::string &devnode) const;

private:
    std::shared_ptr<UEvent> uevent_;
    int inotifyFd_ { -1 };
    int devInputWd_ { -1 };
};

inline int Monitor::GetFd() const
{
    return inotifyFd_;
}

inline bool Monitor::IsActive() const
{
    return (inotifyFd_ >= 0);
}
} // UEVENT
} // MMI
} // OHOS

#endif // UEVENT_MONITOR_H
