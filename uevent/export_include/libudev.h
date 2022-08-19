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

#ifndef UEVENT_LIBUDEV_H
#define UEVENT_LIBUDEV_H

#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

struct udev;
struct udev_device;
struct udev_enumerate;
struct udev_list_entry;
struct udev_monitor;

struct udev* udev_new(void);

struct udev* udev_ref(struct udev *udev);

struct udev* udev_unref(struct udev *udev);

struct udev_device* udev_device_new_from_devnum(struct udev *udev, char type, dev_t devnum);

struct udev_device* udev_device_new_from_syspath(struct udev *udev, const char *syspath);

struct udev_device* udev_device_new_from_subsystem_sysname(struct udev *udev,
    const char *subsystem, const char *sysname);

struct udev_device* udev_device_ref(struct udev_device *udev_device);

struct udev_device* udev_device_unref(struct udev_device *udev_device);

const char* udev_device_get_action(struct udev_device *udev_device);

const char* udev_device_get_devnode(struct udev_device *udev_device);

int udev_device_get_is_initialized(struct udev_device *udev_device);

struct udev_device* udev_device_get_parent_with_subsystem_devtype(
    struct udev_device *udev_device, const char *subsystem, const char *devtype);

const char* udev_device_get_sysname(struct udev_device *udev_device);

const char* udev_device_get_syspath(struct udev_device *udev_device);

struct udev_device* udev_device_get_parent(struct udev_device *udev_device);

struct udev_list_entry* udev_device_get_properties_list_entry(struct udev_device *udev_device);

const char* udev_device_get_property_value(struct udev_device *udev_device, const char *key);

const char* udev_device_get_sysattr_value(struct udev_device *udev_device, const char *sysattr);

struct udev* udev_device_get_udev(struct udev_device *udev_device);

int udev_enumerate_add_match_subsystem(struct udev_enumerate *udev_enumerate, const char *subsystem);

struct udev_list_entry* udev_enumerate_get_list_entry(struct udev_enumerate *udev_enumerate);

struct udev_enumerate* udev_enumerate_new(struct udev *udev);

int udev_enumerate_scan_devices(struct udev_enumerate *udev_enumerate);

struct udev_enumerate* udev_enumerate_unref(struct udev_enumerate *udev_enumerate);

const char* udev_list_entry_get_name(struct udev_list_entry *list_entry);

const char* udev_list_entry_get_value(struct udev_list_entry *list_entry);

struct udev_list_entry* udev_list_entry_get_next(struct udev_list_entry *list_entry);

#define udev_list_entry_foreach(list_entry, first_entry) \
        for ((list_entry) = (first_entry); \
             (list_entry) != NULL; \
             (list_entry) = udev_list_entry_get_next(list_entry))

int udev_monitor_enable_receiving(struct udev_monitor *udev_monitor);

int udev_monitor_get_fd(struct udev_monitor *udev_monitor);

int udev_monitor_filter_add_match_subsystem_devtype(
    struct udev_monitor *udev_monitor, const char *subsystem, const char *devtype);

struct udev_monitor* udev_monitor_new_from_netlink(struct udev *udev, const char *name);

struct udev_device* udev_monitor_receive_device(struct udev_monitor *udev_monitor);

struct udev_monitor* udev_monitor_unref(struct udev_monitor *udev_monitor);

#ifdef OHOS_BUILD_ENABLE_UEVENT

void udev_device_show_all_properties(struct udev_device *udev_device);

#endif // OHOS_BUILD_ENABLE_UEVENT

#ifdef __cplusplus
}
#endif
#endif // UEVENT_LIBUDEV_H