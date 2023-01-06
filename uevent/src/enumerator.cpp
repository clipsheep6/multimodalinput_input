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
#include "enumerator.h"
#include <dirent.h>
#include <fnmatch.h>
#include <sys/sysmacros.h>

#include "mmi_log.h"
#include "utility.h"


namespace OHOS {
namespace MMI {
namespace UEVENT {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, ::OHOS::MMI::MMI_LOG_DOMAIN, "UeventEnumerator" };
}

Enumerator::Enumerator(std::shared_ptr<UEvent> uevent)
    : uevent_(uevent)
{}

void Enumerator::AddMatchSubsystem(const char *subsystem)
{
    if (Utility::IsEmpty(subsystem)) {
        MMI_HILOGE("Empty subsystem name");
        return;
    }
    auto sItr = subsystemMatchList_.find(subsystem);
    if (sItr != subsystemMatchList_.end()) {
        subsystemMatchList_.erase(sItr);
    }
    const auto [tIter, isOk] = subsystemMatchList_.emplace(subsystem);
    if (!isOk) {
        MMI_HILOGE("Duplicate subsystem");
    }
}

void Enumerator::AddMatchSysname(const char *sysname)
{
    if (Utility::IsEmpty(sysname)) {
        MMI_HILOGE("Empty sysname");
        return;
    }
    auto sItr = sysnameMatchList_.find(sysname);
    if (sItr != sysnameMatchList_.end()) {
        sysnameMatchList_.erase(sItr);
    }
    const auto [tIter, isOk] = sysnameMatchList_.emplace(sysname);
    if (!isOk) {
        MMI_HILOGE("Duplicate sysname");
    }
}

void Enumerator::ScanDevices()
{
    CALL_DEBUG_ENTER;
    if (MatchSubsystem("input")) {
        ScanAndAddInputDevices();
        RemoveMatchSubsystem("input");
        if (!HasMatchSubsystem()) {
            return;
        }
    }
    ScanDevicesAll();
}

void Enumerator::RemoveMatchSubsystem(const char *subsystem)
{
    if (Utility::IsEmpty(subsystem)) {
        MMI_HILOGE("Empty subsystem name");
        return;
    }
    auto tIter = subsystemMatchList_.find(subsystem);
    if (tIter != subsystemMatchList_.end()) {
        subsystemMatchList_.erase(tIter);
    }
}

bool Enumerator::MatchSubsystem(const char *subsystem) const
{
    if (Utility::IsEmpty(subsystem)) {
        MMI_HILOGE("Empty subsystem name");
        return false;
    }
    for (const auto &prop : subsystemNoMatchList_) {
        if (fnmatch(prop.name_.c_str(), subsystem, 0) == 0) {
            return false;
        }
    }
    if (subsystemMatchList_.empty()) {
        return true;
    }
    for (const auto &prop : subsystemMatchList_) {
        if (fnmatch(prop.name_.c_str(), subsystem, 0) == 0) {
            return true;
        }
    }
    return false;
}

bool Enumerator::MatchSysname(const char *sysname) const
{
    if (Utility::IsEmpty(sysname)) {
        MMI_HILOGE("Empty sysname");
        return false;
    }
    if (sysnameMatchList_.empty()) {
        return true;
    }
    for (const auto &prop : sysnameMatchList_) {
        if (fnmatch(prop.name_.c_str(), sysname, 0) == 0) {
            return true;
        }
    }
    return false;
}

bool Enumerator::MatchParent(std::shared_ptr<Device> ueDevice) const
{
    CHKPF(ueDevice);
    if (parentMatch_ == nullptr) {
        return true;
    }
    const char *devPath = ueDevice->GetDevpath();
    CHKPF(devPath);
    const char *parentDevPath = parentMatch_->GetDevpath();
    CHKPF(parentDevPath);
    return Utility::StartWith(devPath, parentDevPath);
}

bool Enumerator::MatchProperty(std::shared_ptr<Device> ueDevice) const
{
    CHKPF(ueDevice);

    /* no match always matches */
    if (propertiesMatchList_.empty()) {
        return true;
    }
    /* loop over matches */
    for (const auto &prop : propertiesMatchList_) {
        const char *matchKey = prop.name_.c_str();
        const char *matchValue = prop.value_.c_str();

        /* loop over device properties */
        const std::set<Property> &props = ueDevice->GetAllProperties();
        for (const auto &prop1 : props) {
            const char *devKey = prop1.name_.c_str();
            const char *devValue = prop1.value_.c_str();

            if (fnmatch(matchKey, devKey, 0) != 0) {
                continue;
            }
            if ((matchValue == nullptr) && (devValue == nullptr)) {
                return true;
            }
            if ((matchValue == nullptr) || (devValue == nullptr)) {
                continue;
            }
            if (fnmatch(matchValue, devValue, 0) == 0) {
                return true;
            }
        }
    }

    return false;
}

static bool MatchSysattrValue(std::shared_ptr<Device> ueDevice,
                              const std::string &sysattr, const std::string &matchVal)
{
    CHKPF(ueDevice);
    const char *val = ueDevice->GetSysattrValue(sysattr.c_str());
    CHKPF(val);
    if (matchVal.empty()) {
        return true;
    }
    if (fnmatch(matchVal.c_str(), val, 0) == 0) {
        return true;
    }

    return false;
}

bool Enumerator::MatchSysattr(std::shared_ptr<Device> ueDevice) const
{
    for (const auto &prop : sysattrNomatchList_) {
        if (MatchSysattrValue(ueDevice, prop.name_, prop.value_)) {
            return false;
        }
    }

    if (sysattrMatchList_.empty()) {
        return true;
    }
    for (const auto &prop : sysattrMatchList_) {
        if (!MatchSysattrValue(ueDevice, prop.name_, prop.value_)) {
            return false;
        }
    }
    return true;
}

void Enumerator::AddSyspath(const char *syspath)
{
    CHKPV(syspath);
    auto [tIter, ok] = syspaths_.emplace(syspath);
    if (!ok) {
        MMI_HILOGD("Duplicate syspath: %{public}s", syspath);
    }
}

int Enumerator::ScanDevicesAll()
{
    struct stat statbuf;

    if (stat("/sys/subsystem", &statbuf) == 0) {
        ScanDir("subsystem", "devices");
    } else {
        ScanDir("bus", "devices");
        ScanDir("class");
    }

    return 0;
}

void Enumerator::ScanDir(const char *basedir, const char *subdir, const char *subsystem)
{
    CHKPV(basedir);
    std::string path { Utility::ConcatAsString("/sys/", basedir) };
    MMI_HILOGD("Scan \'%{public}s\'", path.c_str());

    DIR *dir = opendir(path.c_str());
    CHKPV(dir);

    for (struct dirent *dent = readdir(dir); dent != nullptr; dent = readdir(dir)) {
        if (dent->d_name[0] == '.') {
            continue;
        }
        if (!MatchSubsystem(subsystem != nullptr ? subsystem : dent->d_name)) {
            continue;
        }
        ScanDirAndAddDevices(basedir, dent->d_name, subdir);
    }

    closedir(dir);
}

void Enumerator::ScanDirAndAddDevices(const char *basedir, const char *subdir1, const char *subdir2)
{
    CHKPV(basedir);
    std::ostringstream ss;
    ss << "/sys/" << basedir;
    if (subdir1 != nullptr) {
        ss << "/" << subdir1;
    }
    if (subdir2 != nullptr) {
        ss << "/" << subdir2;
    }
    std::string sPath { ss.str() };

    DIR *dir = opendir(sPath.c_str());
    CHKPV(dir);

    for (struct dirent *dent = readdir(dir); dent != nullptr; dent = readdir(dir)) {
        if (dent->d_name[0] == '.') {
            continue;
        }
        if (!MatchSysname(dent->d_name)) {
            continue;
        }

        std::string syspath { Utility::ConcatAsString(sPath, "/", dent->d_name) };
        std::shared_ptr<Device> ueDevice = Device::NewFromSyspath(uevent_, syspath);
        CHKPC(ueDevice);

        if (!MatchParent(ueDevice)) {
            continue;
        }
        if (!MatchProperty(ueDevice)) {
            continue;
        }
        if (!MatchSysattr(ueDevice)) {
            continue;
        }
        AddSyspath(ueDevice->GetSyspath());
    }

    closedir(dir);
}

void Enumerator::ScanAndAddInputDevices()
{
    CALL_INFO_TRACE;
    DIR *dir = opendir(DEV_INPUT_PATH.c_str());
    CHKPV(dir);

    for (struct dirent *dent = readdir(dir); dent != nullptr; dent = readdir(dir)) {
        const std::string devnode { dent->d_name };
        const std::string devpath { DEV_INPUT_PATH + devnode };
        struct stat statbuf;

        if (stat(devpath.c_str(), &statbuf) != 0) {
            continue;
        }
        if (!S_ISCHR(statbuf.st_mode)) {
            continue;
        }

        const std::string lsyspath { SYS_INPUT_PATH + devnode };
        char rpath[PATH_MAX];
        if (realpath(lsyspath.c_str(), rpath) == nullptr) {
            MMI_HILOGE("Path error: %{public}s", lsyspath.c_str());
            continue;
        }
        AddSyspath(rpath);
    }

    closedir(dir);
}
} // namespace UEVENT
} // namespace MMI
} // namespace OHOS
