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

#include "libudev.h"
#include <memory>
#include <set>
#include <variant>
#include <sys/sysmacros.h>
#include "define_multimodal.h"
#include "mmi_log.h"
#include "uevent.h"
#include "device.h"
#include "enumerator.h"
#include "monitor.h"
#include "utility.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace ::OHOS::MMI;
using namespace ::OHOS::MMI::UEVENT;

namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, ::OHOS::MMI::MMI_LOG_DOMAIN, "libudev" };
constexpr size_t VARIANT_0 { 0 };
constexpr size_t VARIANT_1 { 1 };
}

struct SetIterator {
    const std::set<Property> *set_;
    std::set<Property>::const_iterator iterator_;
};

struct MultisetIterator {
    const std::multiset<Property> *set_;
    std::multiset<Property>::const_iterator iterator_;
};

struct udev_list_entry {
    std::variant<SetIterator, MultisetIterator> iterator_;
};

struct udev {
    int refCount_;
    std::shared_ptr<UEvent> uevent_;
};

struct udev_device {
    int refCount_;
    struct udev *udev_;
    std::shared_ptr<Device> device_;
    udev_list_entry entry_;
};

struct udev_enumerate {
    int refCount_;
    struct udev *udev_;
    std::shared_ptr<Enumerator> enumerate_;
    udev_list_entry entry_;
};

struct udev_monitor {
    int refCount_;
    struct udev *udev_;
    std::shared_ptr<Monitor> monitor_;
};

struct udev* udev_new()
{
    std::shared_ptr<UEvent> uevent { std::make_shared<UEvent>() };
    CHKPP(uevent);
    struct udev *udev = new (std::nothrow) struct udev;
    CHKPP(udev);
    udev->uevent_ = uevent;
    udev->refCount_ = 1;
    return udev;
}

struct udev* udev_ref(struct udev *udev)
{
    CHKPP(udev);
    udev->refCount_++;
    return udev;
}

struct udev* udev_unref(struct udev *udev)
{
    CHKPP(udev);
    udev->refCount_--;
    if (udev->refCount_ > 0) {
        return udev;
    }
    delete udev;
    return nullptr;
}

struct udev_device* udev_device_new(struct udev *udev)
{
    CHKPP(udev);
    std::shared_ptr<Device> ueDevice { std::make_shared<Device>(udev->uevent_) };
    CHKPP(ueDevice);
    struct udev_device *device = new (std::nothrow) struct udev_device;
    CHKPP(device);
    device->refCount_ = 1;
    device->udev_ = udev_ref(udev);
    device->device_ = ueDevice;
    return device;
}

struct udev_device* udev_device_new_from_devnum(struct udev *udev, char type, dev_t devnum)
{
    CHKPP(udev);
    std::shared_ptr<Device> ueDevice = Device::NewFromDevnum(udev->uevent_, type, devnum);
    CHKPP(ueDevice);
    struct udev_device *device = new (std::nothrow) struct udev_device;
    CHKPP(device);
    device->refCount_ = 1;
    device->udev_ = udev_ref(udev);
    device->device_ = ueDevice;
    return device;
}

struct udev_device* udev_device_new_from_syspath(struct udev *udev, const char *syspath)
{
    CHKPP(udev);
    std::shared_ptr<Device> ueDevice = Device::NewFromSyspath(udev->uevent_, syspath);
    CHKPP(ueDevice);
    struct udev_device *device = new (std::nothrow) struct udev_device;
    CHKPP(device);
    device->refCount_ = 1;
    device->udev_ = udev_ref(udev);
    device->device_ = ueDevice;
    return device;
}

struct udev_device* udev_device_new_from_subsystem_sysname(
    struct udev *udev, const char *subsystem, const char *sysname)
{
    CHKPP(udev);
    std::shared_ptr<Device> ueDevice = Device::NewFromSubsystemSysname(udev->uevent_, subsystem, sysname);
    CHKPP(ueDevice);
    struct udev_device *device = new (std::nothrow) struct udev_device;
    CHKPP(device);
    device->refCount_ = 1;
    device->udev_ = udev_ref(udev);
    device->device_ = ueDevice;
    return device;
}

struct udev_device* udev_device_ref(struct udev_device *udev_device)
{
    CHKPP(udev_device);
    udev_device->refCount_++;
    return udev_device;
}

struct udev_device* udev_device_unref(struct udev_device *udev_device)
{
    CHKPP(udev_device);
    udev_device->refCount_--;
    if (udev_device->refCount_ > 0) {
        return udev_device;
    }
    udev_unref(udev_device->udev_);
    delete udev_device;
    return nullptr;
}

const char* udev_device_get_action(struct udev_device *udev_device)
{
    CHKPP(udev_device);
    CHKPP(udev_device->device_);
    return udev_device->device_->GetAction();
}

const char* udev_device_get_devnode(struct udev_device *udev_device)
{
    CHKPP(udev_device);
    CHKPP(udev_device->device_);
    return udev_device->device_->GetDevnode();
}

int udev_device_get_is_initialized(struct udev_device *udev_device)
{
    CHKPF(udev_device);
    CHKPF(udev_device->device_);
    return true;
}

dev_t udev_device_get_devnum(struct udev_device *udev_device)
{
    if ((udev_device == nullptr) || (udev_device->device_ == nullptr)) {
        return makedev(0, 0);
    }
    return udev_device->device_->GetDevnum();
}

struct udev_device* udev_device_get_parent_with_subsystem_devtype(
    struct udev_device *udev_device, const char *subsystem, const char *devtype)
{
    CHKPP(udev_device);
    CHKPP(udev_device->device_);
    std::shared_ptr<Device> ueDevice = udev_device->device_->GetParentWithSubsystemDevtype(subsystem, devtype);
    CHKPP(ueDevice);
    struct udev_device *device = new (std::nothrow) struct udev_device;
    CHKPP(device);
    device->refCount_ = 1;
    device->udev_ = udev_ref(udev_device->udev_);
    device->device_ = ueDevice;
    return device;
}

const char* udev_device_get_sysname(struct udev_device *udev_device)
{
    CHKPP(udev_device);
    CHKPP(udev_device->device_);
    return udev_device->device_->GetSysname();
}

const char* udev_device_get_sysnum(struct udev_device *udev_device)
{
    CHKPP(udev_device);
    CHKPP(udev_device->device_);
    return udev_device->device_->GetSysnum();
}

const char* udev_device_get_syspath(struct udev_device *udev_device)
{
    CHKPP(udev_device);
    CHKPP(udev_device->device_);
    return udev_device->device_->GetSyspath();
}

struct udev_device* udev_device_get_parent(struct udev_device *udev_device)
{
    CHKPP(udev_device);
    CHKPP(udev_device->device_);
    std::shared_ptr<Device> ueDevice = udev_device->device_->GetParent();
    if (ueDevice == nullptr) {
        return nullptr;
    }
    struct udev_device *device = new (std::nothrow) struct udev_device;
    CHKPP(device);
    device->refCount_ = 1;
    device->udev_ = udev_ref(udev_device->udev_);
    device->device_ = ueDevice;
    return device;
}

struct udev_list_entry* udev_device_get_properties_list_entry(struct udev_device *udev_device)
{
    CHKPP(udev_device);
    CHKPP(udev_device->device_);

    SetIterator sItr;
    sItr.set_ = &udev_device->device_->GetAllProperties();
    sItr.iterator_ = sItr.set_->cbegin();
    if (sItr.iterator_ == sItr.set_->cend()) {
        return nullptr;
    }

    udev_device->entry_.iterator_ = sItr;
    return &udev_device->entry_;
}

const char* udev_device_get_property_value(struct udev_device *udev_device, const char *key)
{
    CHKPP(udev_device);
    CHKPP(udev_device->device_);
    if (Utility::IsEmpty(key)) {
        return nullptr;
    }
    const std::set<Property> &properties = udev_device->device_->GetAllProperties();
    auto rItr = properties.find(key);
    return (rItr != properties.end() ? rItr->value_.c_str() : nullptr);
}

const char* udev_device_get_sysattr_value(struct udev_device *udev_device, const char *sysattr)
{
    CHKPP(udev_device);
    CHKPP(udev_device->device_);
    return udev_device->device_->GetSysattrValue(sysattr);
}

struct udev* udev_device_get_udev(struct udev_device *udev_device)
{
    CHKPP(udev_device);
    return udev_device->udev_;
}

int udev_enumerate_add_match_sysname(struct udev_enumerate *udev_enumerate, const char *sysname)
{
    CHKPR(udev_enumerate, -1);
    CHKPR(udev_enumerate->enumerate_, -1);
    if (Utility::IsEmpty(sysname)) {
        return -1;
    }
    udev_enumerate->enumerate_->AddMatchSysname(sysname);
    return 0;
}

int udev_enumerate_add_match_subsystem(struct udev_enumerate *udev_enumerate, const char *subsystem)
{
    CHKPR(udev_enumerate, -1);
    CHKPR(udev_enumerate->enumerate_, -1);
    if (Utility::IsEmpty(subsystem)) {
        return -1;
    }
    udev_enumerate->enumerate_->AddMatchSubsystem(subsystem);
    return 0;
}

struct udev_list_entry* udev_enumerate_get_list_entry(struct udev_enumerate *udev_enumerate)
{
    CHKPP(udev_enumerate);
    CHKPP(udev_enumerate->enumerate_);

    SetIterator sIter;
    sIter.set_ = &udev_enumerate->enumerate_->GetSyspaths();
    sIter.iterator_ = sIter.set_->cbegin();
    if (sIter.iterator_ == sIter.set_->cend()) {
        return nullptr;
    }

    udev_enumerate->entry_.iterator_ = sIter;
    return &udev_enumerate->entry_;
}

struct udev_enumerate* udev_enumerate_new(struct udev *udev)
{
    CHKPP(udev);
    std::shared_ptr<Enumerator> ueEnumerate { std::make_shared<Enumerator>(udev->uevent_) };
    CHKPP(ueEnumerate);
    struct udev_enumerate *enumerate = new (std::nothrow) struct udev_enumerate;
    CHKPP(enumerate);
    enumerate->refCount_ = 1;
    enumerate->udev_ = udev_ref(udev);
    enumerate->enumerate_ = ueEnumerate;
    return enumerate;
}

int udev_enumerate_scan_devices(struct udev_enumerate *udev_enumerate)
{
    CHKPR(udev_enumerate, -EINVAL);
    CHKPR(udev_enumerate->enumerate_, -EINVAL);
    udev_enumerate->enumerate_->ScanDevices();
    return 0;
}

struct udev_enumerate* udev_enumerate_unref(struct udev_enumerate *udev_enumerate)
{
    CHKPP(udev_enumerate);
    udev_enumerate->refCount_--;
    if (udev_enumerate->refCount_ > 0) {
        return udev_enumerate;
    }
    udev_unref(udev_enumerate->udev_);
    delete udev_enumerate;
    return nullptr;
}

const char* udev_list_entry_get_name(struct udev_list_entry *list_entry)
{
    CHKPP(list_entry);
    const char *name = nullptr;

    switch (list_entry->iterator_.index()) {
        case VARIANT_0: {
            const auto sItr = std::get_if<VARIANT_0>(&list_entry->iterator_);
            if (sItr == nullptr) {
                break;
            }
            name = sItr->iterator_->name_.c_str();
            break;
        }
        case VARIANT_1: {
            const auto mItr = std::get_if<VARIANT_1>(&list_entry->iterator_);
            if (mItr == nullptr) {
                break;
            }
            name = mItr->iterator_->name_.c_str();
            break;
        }
        default: {
            break;
        }
    }

    return name;
}

const char* udev_list_entry_get_value(struct udev_list_entry *list_entry)
{
    CHKPP(list_entry);
    const char *value = nullptr;

    switch (list_entry->iterator_.index()) {
        case VARIANT_0: {
            const auto sItr = std::get_if<VARIANT_0>(&list_entry->iterator_);
            if (sItr == nullptr) {
                break;
            }
            value = sItr->iterator_->value_.c_str();
            break;
        }
        case VARIANT_1: {
            const auto mItr = std::get_if<VARIANT_1>(&list_entry->iterator_);
            if (mItr == nullptr) {
                break;
            }
            value = mItr->iterator_->value_.c_str();
            break;
        }
        default: {
            break;
        }
    }

    return value;
}

struct udev_list_entry* udev_list_entry_get_next(struct udev_list_entry *list_entry)
{
    CHKPP(list_entry);

    switch (list_entry->iterator_.index()) {
        case VARIANT_0: {
            auto sItr = std::get_if<VARIANT_0>(&list_entry->iterator_);
            if (sItr == nullptr) {
                break;
            }
            if (++sItr->iterator_ == sItr->set_->cend()) {
                list_entry = nullptr;
            }
            break;
        }
        case VARIANT_1: {
            auto mItr = std::get_if<VARIANT_1>(&list_entry->iterator_);
            if (mItr == nullptr) {
                break;
            }
            if (++mItr->iterator_ == mItr->set_->cend()) {
                list_entry = nullptr;
            }
            break;
        }
        default: {
            list_entry = nullptr;
            break;
        }
    }

    return list_entry;
}

int udev_monitor_enable_receiving(struct udev_monitor *udev_monitor)
{
    CHKPR(udev_monitor, -1);
    CHKPR(udev_monitor->monitor_, -1);
    if (!udev_monitor->monitor_->EnableReceiving()) {
        return -1;
    }
    return 0;
}

int udev_monitor_get_fd(struct udev_monitor *udev_monitor)
{
    CHKPR(udev_monitor, -1);
    CHKPR(udev_monitor->monitor_, -1);
    return udev_monitor->monitor_->GetFd();
}

int udev_monitor_filter_add_match_subsystem_devtype(struct udev_monitor *udev_monitor,
                                                    const char *subsystem, const char *devtype)
{
    return 0;
}

struct udev_monitor* udev_monitor_new_from_netlink(struct udev *udev, const char *name)
{
    CHKPP(udev);
    std::shared_ptr<Monitor> ueMonitor = Monitor::NewFromInotify(udev->uevent_);
    CHKPP(ueMonitor);
    struct udev_monitor *monitor = new (std::nothrow) struct udev_monitor;
    CHKPP(monitor);
    monitor->refCount_ = 1;
    monitor->udev_ = udev_ref(udev);
    monitor->monitor_ = ueMonitor;
    return monitor;
}

struct udev_device* udev_monitor_receive_device(struct udev_monitor *udev_monitor)
{
    CHKPP(udev_monitor);
    CHKPP(udev_monitor->monitor_);
    std::shared_ptr<Device> ueDevice = udev_monitor->monitor_->ReceiveDevice();
    CHKPP(ueDevice);
    struct udev_device *device = new (std::nothrow) struct udev_device;
    CHKPP(device);
    device->refCount_ = 1;
    device->udev_ = udev_ref(udev_monitor->udev_);
    device->device_ = ueDevice;
    return device;
}

struct udev_monitor* udev_monitor_unref(struct udev_monitor *udev_monitor)
{
    CHKPP(udev_monitor);
    udev_monitor->refCount_--;
    if (udev_monitor->refCount_ > 0) {
        return udev_monitor;
    }
    udev_unref(udev_monitor->udev_);
    delete udev_monitor;
    return nullptr;
}

#ifdef OHOS_BUILD_ENABLE_UEVENT

void udev_device_show_all_properties(struct udev_device *udev_device)
{
    CHKPV(udev_device);
    CHKPV(udev_device->device_);
    udev_device->device_->ShowAllProperties();
}

#endif // OHOS_BUILD_ENABLE_UEVENT

#ifdef __cplusplus
}
#endif
