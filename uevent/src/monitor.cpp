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
#include "monitor.h"
#include <sys/inotify.h>
#include <unistd.h>

#include "mmi_log.h"
#include "device_manager.h"
#include "utility.h"


namespace OHOS {
namespace MMI {
namespace UEVENT {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, ::OHOS::MMI::MMI_LOG_DOMAIN, "UeventMonitor" };
}

std::shared_ptr<Monitor> Monitor::NewFromInotify(std::shared_ptr<UEvent> uevent)
{
    CALL_INFO_TRACE;
    std::shared_ptr<Monitor> monitor { std::make_shared<Monitor>(uevent) };
    CHKPP(monitor);
    if (!monitor->OpenConnection()) {
        return nullptr;
    }
    return monitor;
}

Monitor::Monitor(std::shared_ptr<UEvent> uevent)
    : uevent_(uevent)
{}

bool Monitor::OpenConnection()
{
    inotifyFd_ = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    if (inotifyFd_ < 0) {
        MMI_HILOGE("Initializing inotify failed: %{public}s", strerror(errno));
        return false;
    }
    return true;
}

bool Monitor::EnableReceiving()
{
    CALL_INFO_TRACE;
    if (!IsActive()) {
        MMI_HILOGE("Monitor is not active");
        return false;
    }
    devInputWd_ = inotify_add_watch(inotifyFd_, DEV_INPUT_PATH.c_str(), IN_CREATE | IN_DELETE);
    if (devInputWd_ < 0) {
        MMI_HILOGE("Watching (\'%{public}s\') failed: %{public}s", DEV_INPUT_PATH.c_str(), strerror(errno));
        return false;
    }

    return true;
}

std::shared_ptr<Device> Monitor::ReceiveDevice()
{
    CALL_INFO_TRACE;
    char buf[sizeof(struct inotify_event) + NAME_MAX + 1];
    size_t bufSize { sizeof(struct inotify_event) };
    ssize_t numRead { 0 };

    do {
        bufSize += sizeof(struct inotify_event);
        numRead = ::read(inotifyFd_, buf, bufSize);
    } while ((numRead < 0) && (errno == EINVAL) &&
             (bufSize + sizeof(struct inotify_event) <= sizeof(buf)));

    if (numRead < 0) {
        MMI_HILOGE("Reading failed: %{public}s", strerror(errno));
        return nullptr;
    }
    if (numRead == 0) {
        MMI_HILOGW("End of file encountered");
        return nullptr;
    }
    MMI_HILOGD("Read %{public}zd bytes from inotify events", numRead);
    std::shared_ptr<Device> ueDevice;

    for (char *p = buf; p < buf + numRead;) {
        struct inotify_event *event = reinterpret_cast<struct inotify_event *>(p);
        ueDevice = HandleInotifyEvent(event);
        p += sizeof(struct inotify_event) + event->len;
    }

    return ueDevice;
}

std::shared_ptr<Device> Monitor::HandleInotifyEvent(struct inotify_event *event) const
{
    CALL_DEBUG_ENTER;
    if (Utility::IsEmpty(event->name)) {
        return nullptr;
    }
    std::string devnode { event->name };

    if ((event->mask & IN_CREATE) == IN_CREATE) {
        return AddDevice(devnode);
    } else if ((event->mask & IN_DELETE) == IN_DELETE) {
        return RemoveDevice(devnode);
    }

    return nullptr;
}

std::shared_ptr<Device> Monitor::AddDevice(const std::string &devnode) const
{
    CALL_DEBUG_ENTER;
    auto devmgr { UEDevMgr };
    CHKPP(devmgr);
    std::shared_ptr<Device> ueDev { devmgr->AddDevice(uevent_, devnode) };
    CHKPP(ueDev);
    ueDev->SetAction("add");
    return ueDev;
}

std::shared_ptr<Device> Monitor::RemoveDevice(const std::string &devnode) const
{
    CALL_DEBUG_ENTER;
    auto devmgr { UEDevMgr };
    CHKPP(devmgr);
    auto ueDev = devmgr->RemoveDevice(devnode);
    CHKPP(ueDev);
    ueDev->SetAction("remove");
    return ueDev;
}
} // namespace UEVENT
} // namespace MMI
} // namespace OHOS
