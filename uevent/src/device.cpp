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
#include "device.h"

#include <fstream>
#include <sstream>

#include <cstring>
#include <cinttypes>
#include <fcntl.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#include <securec.h>

#include "device_manager.h"
#include "mmi_log.h"
#include "utility.h"

namespace OHOS {
namespace MMI {
namespace UEVENT {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, ::OHOS::MMI::MMI_LOG_DOMAIN, "UeventDevice" };
}

std::shared_ptr<Device> Device::NewFromDevnum(std::shared_ptr<UEvent> uevent, char type, dev_t devnum)
{
    CALL_DEBUG_ENTER;
    std::ostringstream syspath;
    syspath << "/sys/dev/";

    if (type == 'b') {
        syspath << "block/";
    } else if (type == 'c') {
        syspath << "char/";
    } else {
        errno = EINVAL;
        return nullptr;
    }

    /* use /sys/dev/{block,char}/<maj>:<min> link */
    syspath << major(devnum) << ":" << minor(devnum);
    return NewFromSyspath(uevent, syspath.str());
}

std::shared_ptr<Device> Device::NewFromSyspath(std::shared_ptr<UEvent> uevent, const char *syspath)
{
    CALL_DEBUG_ENTER;
    if (!Utility::StartWith(syspath, "/sys")) {
        MMI_HILOGE("\'%{public}s\' is not in sys", syspath);
        return nullptr;
    }
    const char *subdir = syspath + strlen("/sys");
    const char *pos = strrchr(subdir, '/');
    if ((pos == nullptr) || (pos[1] == '\0') || (pos < &subdir[ARRAY_POS_2])) {
        MMI_HILOGE("\'%{public}s\' is invalid syspath", syspath);
        return nullptr;
    }

    char rpath[PATH_MAX];
    struct stat statbuf;
    if (realpath(syspath, rpath) == nullptr) {
        MMI_HILOGE("\'%{public}s\' is not a real path", syspath);
        return nullptr;
    }
    if (Utility::StartWith(rpath + strlen("/sys"), "/devices/")) {
        std::string sfname { Utility::ConcatAsString(rpath, "/uevent") };
        if (stat(sfname.c_str(), &statbuf) != 0) {
            MMI_HILOGE("Stat \'%{public}s\' failed", sfname.c_str());
            return nullptr;
        }
    } else {
        if (stat(rpath, &statbuf) != 0) {
            MMI_HILOGE("Stat \'%{public}s\' failed", rpath);
            return nullptr;
        }
        if (!S_ISDIR(statbuf.st_mode)) {
            MMI_HILOGE("\'%{public}s\' is not a directory", rpath);
            return nullptr;
        }
    }

    auto devMgr { UEDevMgr };
    CHKPP(devMgr);
    std::shared_ptr<Device> ueDev { devMgr->AddInputDevice(uevent, std::string(rpath)) };
    if (ueDev != nullptr) {
        return ueDev;
    }

    ueDev = std::make_shared<Device>(uevent);
    CHKPP(ueDev);
    ueDev->SetSyspath(std::string(rpath));
    return ueDev;
}

std::shared_ptr<Device> Device::NewFromSyspath(std::shared_ptr<UEvent> uevent, const std::string &syspath)
{
    return NewFromSyspath(uevent, syspath.c_str());
}

std::shared_ptr<Device> Device::NewFromSubsystemSysname(std::shared_ptr<UEvent> uevent,
                                                        const char *subsystem, const char *sysname)
{
    CALL_DEBUG_ENTER;
    CHKPP(subsystem);
    CHKPP(sysname);

    if (Utility::IsEqual(subsystem, "subsystem")) {
        return NewFromSubsystemSysname1(uevent, subsystem, sysname);
    }
    if (Utility::IsEqual(subsystem, "module")) {
        std::string path = Utility::ConcatAsString("/sys/module/", sysname);
        struct stat statbuf;
        if (stat(path.c_str(), &statbuf) == 0) {
            return NewFromSyspath(uevent, path);
        }
        return nullptr;
    }
    if (Utility::IsEqual(subsystem, "drivers")) {
        return NewFromSubsystemSysname2(uevent, subsystem, sysname);
    }
    return NewFromSubsystemSysname3(uevent, subsystem, sysname);
}

std::shared_ptr<Device> Device::NewFromSubsystemSysname1(
    std::shared_ptr<UEvent> uevent, const char *subsystem, const char *sysname)
{
    if (!Utility::IsEqual(subsystem, "subsystem")) {
        return nullptr;
    }
    struct stat statbuf;
    std::string path = Utility::ConcatAsString("/sys/subsystem/", sysname);
    if (stat(path.c_str(), &statbuf) == 0) {
        return NewFromSyspath(uevent, path);
    }
    path = Utility::ConcatAsString("/sys/bus/", sysname);
    if (stat(path.c_str(), &statbuf) == 0) {
        return NewFromSyspath(uevent, path);
    }
    path = Utility::ConcatAsString("/sys/class/", sysname);
    if (stat(path.c_str(), &statbuf) == 0) {
        return NewFromSyspath(uevent, path);
    }
    return nullptr;
}

std::shared_ptr<Device> Device::NewFromSubsystemSysname2(
    std::shared_ptr<UEvent> uevent, const char *subsystem, const char *sysname)
{
    if (!Utility::IsEqual(subsystem, "drivers")) {
        return nullptr;
    }
    std::string s { sysname };
    std::string::size_type pos = s.find(':');
    if (pos != std::string::npos) {
        std::string subsys { s.substr(0, pos) };
        std::string driver { s.substr(pos + 1) };
        struct stat statbuf;

        std::string path = Utility::ConcatAsString("/sys/subsystem/", subsys, "/drivers/", driver);
        if (stat(path.c_str(), &statbuf) == 0) {
            return NewFromSyspath(uevent, path);
        }
        path = Utility::ConcatAsString("/sys/bus/", subsys, "/drivers/", driver);
        if (stat(path.c_str(), &statbuf) == 0) {
            return NewFromSyspath(uevent, path);
        }
    }
    return nullptr;
}

std::shared_ptr<Device> Device::NewFromSubsystemSysname3(
    std::shared_ptr<UEvent> uevent, const char *subsystem, const char *sysname)
{
    struct stat statbuf;

    std::string path = Utility::ConcatAsString("/sys/subsystem/", subsystem, "/devices/", sysname);
    if (stat(path.c_str(), &statbuf) == 0) {
        return NewFromSyspath(uevent, path);
    }
    path = Utility::ConcatAsString("/sys/bus/", subsystem, "/devices/", sysname);
    if (stat(path.c_str(), &statbuf) == 0) {
        return NewFromSyspath(uevent, path);
    }
    path = Utility::ConcatAsString("/sys/class/", subsystem, "/", sysname);
    if (stat(path.c_str(), &statbuf) == 0) {
        return NewFromSyspath(uevent, path);
    }

    return nullptr;
}

std::shared_ptr<Device> Device::NewFromDeviceID(std::shared_ptr<UEvent> uevent, const char *id)
{
    if (Utility::IsEmpty(id)) {
        return nullptr;
    }
    switch (id[0]) {
        case 'b':
        case 'c': {
            char type;
            int maj;
            int min;

            if (sscanf_s(id, "%c%i:%i", &type, &maj, &min) != ARG_NUM_3) {
                return nullptr;
            }
            return NewFromDevnum(uevent, type, makedev(maj, min));
        }
        case 'n': {
            return NewFromNet(uevent, id);
        }
        case '+': {
            char subsys[PATH_MAX];
            Utility::CopyNulstr(subsys, sizeof(subsys), &id[1]);
            char *sysname = strchr(subsys, ':');
            CHKPP(sysname);
            sysname[0] = '\0';
            sysname = &sysname[1];
            return NewFromSubsystemSysname(uevent, subsys, sysname);
        }
        default: {
            MMI_HILOGW("Unrecognized identity.");
            return nullptr;
        }
    }
}

std::shared_ptr<Device> Device::NewFromNet(std::shared_ptr<UEvent> uevent, const char *id)
{
    int ifIndex = std::strtoul(&id[1], nullptr, BASE10);
    if (ifIndex <= 0) {
        return nullptr;
    }

    int sk = socket(PF_INET, SOCK_DGRAM, 0);
    if (sk < 0) {
        return nullptr;
    }

    struct ifreq ifr {};
    ifr.ifr_ifindex = ifIndex;
    if (ioctl(sk, SIOCGIFNAME, &ifr) != 0) {
        close(sk);
        return nullptr;
    }
    close(sk);

    std::shared_ptr<Device> ueDevice = NewFromSubsystemSysname(uevent, "net", ifr.ifr_name);
    CHKPP(ueDevice);
    if (ueDevice->GetIfIndex() == ifIndex) {
        return ueDevice;
    }

    return nullptr;
}

} // namespace UEVENT
} // namespace MMI
} // namespace OHOS