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

#ifndef UEVENT_ENUMERATOR_H
#define UEVENT_ENUMERATOR_H

#include <memory>
#include <set>

#include "uevent.h"
#include "device.h"

namespace OHOS {
namespace MMI {
namespace UEVENT {

class Enumerator {
public:
    explicit Enumerator(std::shared_ptr<UEvent> uevent);
    ~Enumerator() = default;
    DISALLOW_COPY_AND_MOVE(Enumerator);

    void AddMatchSubsystem(const char *subsystem);
    void AddMatchSysname(const char *sysName);
    void ScanDevices();
    const std::set<Property>& GetSyspaths() const;

private:
    bool HasMatchSubsystem() const;
    void RemoveMatchSubsystem(const char *subsystem);
    bool MatchSubsystem(const char *subsystem) const;
    bool MatchSysname(const char *sysName) const;
    bool MatchParent(std::shared_ptr<Device> ueDevice) const;
    bool MatchProperty(std::shared_ptr<Device> ueDevice) const;
    bool MatchSysattr(std::shared_ptr<Device> ueDevice) const;

    void AddSyspath(const char *syspath);
    int ScanDevicesAll();
    void ScanDir(const char *basedir, const char *subdir = nullptr, const char *subsystem = nullptr);
    void ScanDirAndAddDevices(const char *basedir, const char *subdir1, const char *subdir2);
    void ScanAndAddInputDevices();

private:
    std::shared_ptr<UEvent> uevent_;
    std::shared_ptr<Device> parentMatch_;
    std::set<Property> subsystemMatchList_;
    std::set<Property> subsystemNoMatchList_;
    std::set<Property> sysnameMatchList_;
    std::multiset<Property> propertiesMatchList_;
    std::multiset<Property> sysattrNomatchList_;
    std::multiset<Property> sysattrMatchList_;
    std::set<Property> syspaths_;
};

inline const std::set<Property>& Enumerator::GetSyspaths() const
{
    return syspaths_;
}

inline bool Enumerator::HasMatchSubsystem() const
{
    return !subsystemMatchList_.empty();
}
} // UEVENT
} // MMI
} // OHOS

#endif // UEVENT_ENUMERATOR_H
