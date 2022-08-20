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
#ifndef UEVENT_DEVICE_H
#define UEVENT_DEVICE_H
#include <memory>
#include <optional>
#include <set>
#include <string>

#include <sys/stat.h>

#include <nocopyable.h>

#include "uevent.h"

namespace OHOS {
namespace MMI {
namespace UEVENT {
class Device {
public:
    static std::shared_ptr<Device> NewFromDevnum(std::shared_ptr<UEvent> uevent, char type, dev_t devnum);
    static std::shared_ptr<Device> NewFromSyspath(std::shared_ptr<UEvent> uevent, const char *strSyspath);
    static std::shared_ptr<Device> NewFromDeviceID(std::shared_ptr<UEvent> uevent, const char *id);
    static std::shared_ptr<Device> NewFromSubsystemSysname(std::shared_ptr<UEvent> uevent,
                                                           const char *subsystem, const char *sysname);

    static std::shared_ptr<Device> NewFromSubsystemSysname(std::shared_ptr<UEvent> uevent,
                                                           const std::string &subsystem, const std::string &sysname);
    static std::shared_ptr<Device> NewFromSyspath(std::shared_ptr<UEvent> uevent, const std::string &syspath);

public:
    explicit Device(std::shared_ptr<UEvent> uevent);
    ~Device() = default;
    DISALLOW_COPY_AND_MOVE(Device);

    const char* GetAction() const;
    int GetIfIndex() const;
    const char* GetDevnode();
    const char* GetDevtype();
    const char* GetSubsystem();
    const char* GetSysname() const;
    const char* GetSyspath() const;
    const char* GetDevpath() const;
    const char* GetSysattrValue(const char *sysattr);
    dev_t GetDevnum();
    const std::set<Property>& GetAllProperties();
    const char* GetSysnum() const;
    std::shared_ptr<Device> GetParent();
    std::shared_ptr<Device> GetParentWithSubsystemDevtype(const char *subsystem, const char *devtype);
    void ShowAllProperties() const;

    std::optional<Property> AddProperty(const char *key, const char *value);
    void SetAction(const char *action);
    void SetDevnode(const std::string &devnode);
    void SetDevnodeMode(mode_t mode);
    void SetSyspath(const std::string &syspath);

private:
    static std::shared_ptr<Device> NewFromSubsystemSysname1(
        std::shared_ptr<UEvent> uevent, const char *subsystem, const char *sysname);
    static std::shared_ptr<Device> NewFromSubsystemSysname2(
        std::shared_ptr<UEvent> uevent, const char *subsystem, const char *sysname);
    static std::shared_ptr<Device> NewFromSubsystemSysname3(
        std::shared_ptr<UEvent> uevent, const char *subsystem, const char *sysname);
    static std::shared_ptr<Device> NewFromNet(std::shared_ptr<UEvent> uevent, const char *id);
    static int32_t AllocDeviceID();

private:
    std::optional<Property> AddPropertyInternal(const char *name, const char *value);
    std::optional<Property> AddPropertyFromString(const char *property);
    std::optional<Property> AddSysattrValue(const char *sysattr, const char *sysattrValue);

    std::optional<Property> AddPropertyInternal(const std::string &name, const std::string &value);
    std::optional<Property> AddPropertyFromString(const std::string &property);

    void SetIfindex(int ifIndex);
    void SetSubsystem(const char *subsystem);
    void SetDevnum(dev_t devnum);

    void SetDevtype(const std::string &devtype);
    void SetSubsystem(const std::string &subsystem);
    void SetAction(const std::string &action);

    void ReadUeventFile();
    std::shared_ptr<Device> DeviceNewFromParent();

private:
    bool ueventLoaded_ { false };
    bool parentSet_ { false };
    bool subsystemSet_ { false };
    bool devtypeSet_ { false };
    const int32_t id_ {};
    int ifIndex_ {};
    int major_ {};
    int minor_ {};
    dev_t devnum_ {};
    mode_t devnodeMode_ {};
    std::shared_ptr<UEvent> uevent_;
    std::shared_ptr<Device> parentDevice_;
    std::string action_;
    std::string devnode_;
    std::string devtype_;
    std::string subsystem_;
    std::string sysname_;
    std::string sysnum_;
    std::string syspath_;
    std::string devpath_;
    std::set<Property> propertiesList_;
    std::set<Property> sysattrValueList_;
};

inline const char* Device::GetAction() const
{
    return action_.c_str();
}

inline int Device::GetIfIndex() const
{
    return ifIndex_;
}

inline const char* Device::GetSysname() const
{
    return sysname_.c_str();
}

inline const char* Device::GetSyspath() const
{
    return syspath_.c_str();
}

inline const char* Device::GetDevpath() const
{
    return devpath_.c_str();
}

inline const char* Device::GetSysnum() const
{
    return sysnum_.c_str();
}
} // UEVENT
} // MMI
} // OHOS

#endif // UEVENT_DEVICE_H
