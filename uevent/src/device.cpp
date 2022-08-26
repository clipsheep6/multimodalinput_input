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
#include <cstring>
#include <cinttypes>
#include <fstream>
#include <sstream>

#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#include <securec.h>
#include "mmi_log.h"
#include "device_manager.h"
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
    CHKPP(uevent);
    CHKPP(syspath);

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
    CHKPP(uevent);
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
            int maj, min;

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

int32_t Device::AllocDeviceID()
{
    static int32_t sid { 0 };
    return ++sid;
}

Device::Device(std::shared_ptr<UEvent> uevent)
    : id_(AllocDeviceID()), uevent_(uevent)
{}

const char* Device::GetDevnode()
{
    if (!devnode_.empty()) {
        return devnode_.c_str();
    }
    if (!ueventLoaded_) {
        ReadUeventFile();
    }
    return devnode_.c_str();
}

const char* Device::GetDevtype()
{
    if (!devtypeSet_) {
        devtypeSet_ = true;
        ReadUeventFile();
    }
    return devtype_.c_str();
}

const char* Device::GetSubsystem()
{
    char subsystem[PATH_MAX];

    if (!subsystemSet_) {
        subsystemSet_ = true;

        /* read "subsystem" link */
        if (Utility::GetSysCoreLinkValue("subsystem", syspath_.c_str(), subsystem, sizeof(subsystem)) > 0) {
            SetSubsystem(subsystem);
            return subsystem_.c_str();
        }
        /* implicit names */
        if (Utility::StartWith(devpath_.c_str(), "/module/")) {
            SetSubsystem("module");
            return subsystem_.c_str();
        }
        if (devpath_.find("/drivers/") != std::string::npos) {
            SetSubsystem("drivers");
            return subsystem_.c_str();
        }
        if (Utility::StartWith(devpath_.c_str(), "/subsystem/") ||
            Utility::StartWith(devpath_.c_str(), "/class/") ||
            Utility::StartWith(devpath_.c_str(), "/bus/")) {
            SetSubsystem("subsystem");
            return subsystem_.c_str();
        }
    }

    return subsystem_.c_str();
}

void Device::SetSyspath(const std::string &syspath)
{
    if (!Utility::StartWith(syspath, std::string("/sys"))) {
        return;
    }

    syspath_ = syspath;
    devpath_ = syspath_.substr(std::string("/sys").size());
    AddPropertyInternal(std::string("DEVPATH"), devpath_);

    std::string::size_type spos = syspath_.rfind('/');
    if (spos == std::string::npos) {
        return;
    }
    sysname_ = syspath_.substr(spos + 1);

    std::string::size_type len = 0;
    while (sysname_[len] != '\0') {
        if (sysname_[len] == '!')
            sysname_[len] = '/';
        len++;
    }

    const char *sysnum = &sysname_[len];
    while ((len > 0) && isdigit(sysname_[--len])) {
        sysnum = (sysname_.c_str() + len);
    }
    if ((len <= 0) || (sysnum[0] == '\0')) {
        sysnum_.clear();
    } else {
        sysnum_ = sysname_.substr(sysnum - sysname_.c_str());
    }
}

const char* Device::GetSysattrValue(const char *sysattr)
{
    CALL_DEBUG_ENTER;
    char value[PATH_MAX];
    struct stat statbuf;

    CHKPP(sysattr);
    auto cItr = sysattrValueList_.find(sysattr);
    if (cItr != sysattrValueList_.end()) {
        return cItr->value_.c_str();
    }

    std::string path { Utility::ConcatAsString(GetSyspath(), "/", sysattr) };
    if (lstat(path.c_str(), &statbuf) != 0) {
        AddSysattrValue(sysattr, nullptr);
        return nullptr;
    }

    if (S_ISLNK(statbuf.st_mode)) {
        /*
         * Some core links return only the last element of the target path,
         * these are just values, the paths should not be exposed.
         */
        if (Utility::IsEqual(sysattr, "driver") ||
            Utility::IsEqual(sysattr, "subsystem") ||
            Utility::IsEqual(sysattr, "module")) {
            if (Utility::GetSysCoreLinkValue(sysattr, GetSyspath(), value, sizeof(value)) < 0) {
                return nullptr;
            }
            std::optional<Property> prop { AddSysattrValue(sysattr, value) };
            return (prop ? prop->value_.c_str() : nullptr);
        }

        return nullptr;
    }

    /* skip directories */
    if (S_ISDIR(statbuf.st_mode)) {
        return nullptr;
    }

    /* skip non-readable files */
    if ((statbuf.st_mode & S_IRUSR) == 0) {
        return nullptr;
    }

    /* read attribute value */
    int fd = open(path.c_str(), O_RDONLY|O_CLOEXEC);
    if (fd < 0) {
        return nullptr;
    }
    ssize_t size = read(fd, value, sizeof(value));
    close(fd);
    if (size < 0) {
        return nullptr;
    }
    if (size == sizeof(value)) {
        return nullptr;
    }

    /* got a valid value, store it in cache and return it */
    value[size] = '\0';
    Utility::RemoveTrailingChars(value, '\n');

    std::optional<Property> prop { AddSysattrValue(sysattr, value) };
    return (prop ? prop->value_.c_str() : nullptr);
}

dev_t Device::GetDevnum()
{
    if (!ueventLoaded_) {
        ReadUeventFile();
    }
    return devnum_;
}

void Device::ReadUeventFile()
{
    if (ueventLoaded_) {
        return;
    }
    const char *syspath = GetSyspath();
    if (Utility::IsEmpty(syspath)) {
        return;
    }
    std::string sfname { Utility::ConcatAsString(syspath, "/uevent") };
    std::ifstream f(sfname, std::ios::in);
    if (!f.is_open()) {
        MMI_HILOGW("Opening \'%{public}s\' failed", sfname.c_str());
        return;
    }

    ueventLoaded_ = true;
    std::string line;
    int dmajor = 0;
    int dminor = 0;

    while (std::getline(f, line)) {
        Utility::RemoveTrailingChars(line, std::string("\n\r"));

        if (Utility::StartWith(line, std::string("DEVTYPE="))) {
            SetDevtype(line.substr(std::string("DEVTYPE=").size()));
            continue;
        }
        if (Utility::StartWith(line, std::string("IFINDEX="))) {
            SetIfindex(std::strtoull(&line[std::string("IFINDEX=").size()], nullptr, BASE10));
            continue;
        }
        if (Utility::StartWith(line, std::string("DEVNAME="))) {
            SetDevnode(line.substr(std::string("DEVNAME=").size()));
            continue;
        }

        if (Utility::StartWith(line, std::string("MAJOR="))) {
            dmajor = std::strtoull(&line[std::string("MAJOR=").size()], nullptr, BASE10);
        } else if (Utility::StartWith(line, std::string("MINOR="))) {
            dminor = std::strtoull(&line[std::string("MINOR=").size()], nullptr, BASE10);
        } else if (Utility::StartWith(line, std::string("DEVMODE="))) {
            SetDevnodeMode(std::strtoul(&line[std::string("DEVMODE=").size()], nullptr, BASE8));
        }

        AddPropertyFromString(line);
    }

    SetDevnum(makedev(dmajor, dminor));
}

void Device::SetDevnodeMode(mode_t mode)
{
    char buf[UEVENT_NUMBER_MAX_WIDTH];

    devnodeMode_ = mode;
    if (sprintf_s(buf, sizeof(buf), "%#o", devnodeMode_) > 0) {
        AddPropertyInternal("DEVMODE", buf);
    }
}

void Device::SetDevtype(const std::string &devtype)
{
    devtype_ = devtype;
    AddPropertyInternal(std::string("DEVTYPE"), devtype_);
}

void Device::SetIfindex(int ifIndex)
{
    ifIndex_ = ifIndex;
    AddPropertyInternal(std::string("IFINDEX"), std::to_string(ifIndex_));
}

void Device::SetDevnode(const std::string &devnode)
{
    if (devnode.empty() || (devnode[0] != '/')) {
        devnode_ = Utility::ConcatAsString("/dev/", devnode);
    } else {
        devnode_ = devnode;
    }
    AddPropertyInternal(std::string("DEVNAME"), devnode_);
}

void Device::SetSubsystem(const std::string &subsystem)
{
    subsystem_ = subsystem;
    subsystemSet_ = true;
    AddPropertyInternal(std::string("SUBSYSTEM"), subsystem_);
}

void Device::SetSubsystem(const char *subsystem)
{
    CHKPV(subsystem);
    SetSubsystem(std::string(subsystem));
}

void Device::SetAction(const std::string &action)
{
    action_ = action;
    AddPropertyInternal(std::string("ACTION"), action_);
}

void Device::SetAction(const char *action)
{
    CHKPV(action);
    SetAction(std::string(action));
}

void Device::SetDevnum(dev_t devnum)
{
    devnum_ = devnum;
    AddPropertyInternal(std::string("MAJOR"), std::to_string(major(devnum_)));
    AddPropertyInternal(std::string("MINOR"), std::to_string(minor(devnum_)));
}

const std::set<Property>& Device::GetAllProperties()
{
    if (!ueventLoaded_) {
        ReadUeventFile();
    }
    return propertiesList_;
}

std::shared_ptr<Device> Device::GetParent()
{
    if (!parentSet_) {
        parentSet_ = true;
        parentDevice_ = DeviceNewFromParent();
    }
    return parentDevice_;
}

std::shared_ptr<Device> Device::DeviceNewFromParent()
{
    std::shared_ptr<Device> parentDevice = nullptr;

    const char *syspath = GetSyspath();
    if (Utility::IsEmpty(syspath)) {
        return nullptr;
    }

    char path[PATH_MAX];
    Utility::CopyNulstr(path, sizeof(path), syspath);
    if (!Utility::StartWith(path, "/sys/")) {
        return nullptr;
    }
    char *subdir = path + strlen("/sys/");

    for (;;) {
        char *pos = strrchr(subdir, '/');
        if ((pos == nullptr) || (pos < &subdir[ARRAY_POS_2])) {
            break;
        }
        pos[0] = '\0';

        parentDevice = Device::NewFromSyspath(uevent_, path);
        if (parentDevice != nullptr) {
            return parentDevice;
        }
    }

    return nullptr;
}

std::shared_ptr<Device> Device::GetParentWithSubsystemDevtype(const char *subsystem, const char *devtype)
{
    CHKPP(subsystem);
    std::shared_ptr<Device> parent = GetParent();

    for (; parent != nullptr; parent = parent->GetParent()) {
        const char *parent_subsystem;
        const char *parent_devtype;

        parent_subsystem = parent->GetSubsystem();
        if ((parent_subsystem != nullptr) && Utility::IsEqual(parent_subsystem, subsystem)) {
            CHKPB(devtype);
            parent_devtype = parent->GetDevtype();
            if ((parent_devtype != nullptr) && Utility::IsEqual(parent_devtype, devtype)) {
                break;
            }
        }
    }

    return parent;
}

std::optional<Property> Device::AddProperty(const char *key, const char *value)
{
    return AddPropertyInternal(key, value);
}

std::optional<Property> Device::AddPropertyInternal(const char *name, const char *value)
{
    if (Utility::IsEmpty(name)) {
        return std::nullopt;
    }
    auto sItr = propertiesList_.find(name);
    if (sItr != propertiesList_.end()) {
        propertiesList_.erase(sItr);
    }
    if (value == nullptr) {
        return std::nullopt;
    }
    auto [rItr, isOk] = propertiesList_.emplace(name, value);
    if (!isOk) {
        return std::nullopt;
    }
    return *rItr;
}

std::optional<Property> Device::AddPropertyInternal(const std::string &name, const std::string &value)
{
    if (name.empty()) {
        MMI_HILOGE("Empty name");
        return std::nullopt;
    }
    auto sItr = propertiesList_.find(name);
    if (sItr != propertiesList_.end()) {
        propertiesList_.erase(sItr);
    }
    if (value.empty()) {
        MMI_HILOGD("Empty value, no property added");
        return std::nullopt;
    }
    auto [rItr, isOk] = propertiesList_.emplace(name, value);
    if (!isOk) {
        return std::nullopt;
    }
    return *rItr;
}

std::optional<Property> Device::AddPropertyFromString(const char *property)
{
    if (Utility::IsEmpty(property)) {
        MMI_HILOGE("Empty property string");
        return std::nullopt;
    }
    char name[UEVENT_LINE_SIZE];
    if (strcpy_s(name, sizeof(name), property) != EOK) {
        return std::nullopt;
    }
    char *val = strchr(name, '=');
    if (val == nullptr) {
        MMI_HILOGE("Invalid property string, no property added");
        return std::nullopt;
    }
    val[0] = '\0';
    val = &val[1];
    if (val[0] == '\0') {
        val = nullptr;
    }
    return AddPropertyInternal(name, val);
}

std::optional<Property> Device::AddPropertyFromString(const std::string &property)
{
    std::string::size_type t = property.find('=');
    if (t == std::string::npos) {
        MMI_HILOGE("Invalid property string, no property added");
        return std::nullopt;
    }
    return AddPropertyInternal(property.substr(0, t), property.substr(t + 1));
}

std::optional<Property> Device::AddSysattrValue(const char *sysattr, const char *sysattrValue)
{
    if (Utility::IsEmpty(sysattr)) {
        return std::nullopt;
    }
    auto sItr = sysattrValueList_.find(sysattr);
    if (sItr != sysattrValueList_.end()) {
        sysattrValueList_.erase(sItr);
    }
    auto [rItr, isOk] = sysattrValueList_.emplace(sysattr, sysattrValue);
    if (!isOk) {
        return std::nullopt;
    }
    return *rItr;
}

void Device::ShowAllProperties() const
{
    CALL_DEBUG_ENTER;
    MMI_HILOGD("syspath=%{public}s", syspath_.c_str());
    for (const auto &prop : propertiesList_) {
        MMI_HILOGD("name=%{public}s, value=%{public}s", prop.name_.c_str(), prop.value_.c_str());
    }
}
} // namespace UEVENT
} // namespace MMI
} // namespace OHOS