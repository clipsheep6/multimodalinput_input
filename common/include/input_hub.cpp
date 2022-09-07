/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "input_hub.h"

#include <cinttypes>
#include <cstring>
#include <filesystem>
#include <sstream>
#include <utility>

#include <dirent.h>
#include <fcntl.h>
#include <regex>
#include <securec.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "anonymous_string.h"
#include "distributed_hardware_log.h"

#include "constants_dinput.h"
#include "dinput_context.h"
#include "dinput_errcode.h"
#include "dh_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
const uint32_t SLEEP_TIME_US = 100 * 1000;
const uint32_t ERROR_MSG_MAX_LEN = 256;
constexpr int32_t MAX_RETRY_COUNT = 10;
}

InputHub::InputHub() : epollFd_(0), iNotifyFd_(0), inputWd_(0), needToScanDevices_(true), nextDeviceId_(1),
    mPendingEventItems{}, pendingEventCount_(0), pendingEventIndex_(0), pendingINotify_(false), deviceChanged_(false),
    inputTypes_(0), isStartCollectEvent_(false), isStartCollectHandler_(false)
{
    Initialize();
}

InputHub::~InputHub()
{
    Release();
}

static std::string ConvertErrNo()
{
    char errMsg[ERROR_MSG_MAX_LEN] = {0};
    strerror_r(errno, errMsg, ERROR_MSG_MAX_LEN);
    std::string errNoMsg(errMsg);
    return errNoMsg;
}

int32_t InputHub::Initialize()
{
    epollFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd_ < 0) {
        DHLOGE("Could not create epoll instance: %s", ConvertErrNo().c_str());
        return ERR_DH_INPUT_HUB_EPOLL_INIT_FAIL;
    }

    iNotifyFd_ = inotify_init();
    inputWd_ = inotify_add_watch(iNotifyFd_, DEVICE_PATH, IN_DELETE | IN_CREATE);
    if (inputWd_ < 0) {
        DHLOGE(
            "Could not register INotify for %s: %s", DEVICE_PATH, ConvertErrNo().c_str());
        return ERR_DH_INPUT_HUB_EPOLL_INIT_FAIL;
    }

    struct epoll_event eventItem = {};
    eventItem.events = EPOLLIN;
    eventItem.data.fd = iNotifyFd_;
    int result = epoll_ctl(epollFd_, EPOLL_CTL_ADD, iNotifyFd_, &eventItem);
    if (result != 0) {
        DHLOGE("Could not add INotify to epoll instance.  errno=%d", errno);
        return ERR_DH_INPUT_HUB_EPOLL_INIT_FAIL;
    }

    return DH_SUCCESS;
}

int32_t InputHub::Release()
{
    CloseAllDevicesLocked();

    ::close(epollFd_);
    ::close(iNotifyFd_);
    StopCollectInputEvents();
    StopCollectInputHandler();
    return DH_SUCCESS;
}

size_t InputHub::StartCollectInputEvents(RawEvent* buffer, size_t bufferSize)
{
    size_t count = 0;
    isStartCollectEvent_ = true;
    while (isStartCollectEvent_) {
        if (needToScanDevices_) {
            needToScanDevices_ = false;
            ScanInputDevices(DEVICE_PATH);
        }

        {
            std::unique_lock<std::mutex> deviceLock(devicesMutex_);
            while (!openingDevices_.empty()) {
                std::unique_ptr<Device> device = std::move(*openingDevices_.rbegin());
                openingDevices_.pop_back();
                DHLOGI("Reporting device opened: id=%s, name=%s\n",
                    GetAnonyInt32(device->id).c_str(), device->path.c_str());
                auto [dev_it, inserted] = devices_.insert_or_assign(device->id, std::move(device));
                if (!inserted) {
                    DHLOGI("Device id %s exists, replaced. \n", GetAnonyInt32(device->id).c_str());
                }
            }
        }

        deviceChanged_ = false;
        count = GetEvents(buffer, bufferSize);
        // readNotify() will modify the list of devices so this must be done after
        // processing all other events to ensure that we read all remaining events
        // before closing the devices.
        if (pendingINotify_ && pendingEventIndex_ >= pendingEventCount_) {
            pendingINotify_ = false;
            ReadNotifyLocked();
            deviceChanged_ = true;
        }

        // Report added or removed devices immediately.
        if (deviceChanged_) {
            continue;
        }

        if (count > 0) {
            break;
        }

        if (RefreshEpollItem(false) < 0) {
            break;
        }
    }

    // All done, return the number of events we read.
    return count;
}

void InputHub::StopCollectInputEvents()
{
    DHLOGI("Stop Collect Input Events Thread");
    isStartCollectEvent_ = false;
}

size_t InputHub::GetEvents(RawEvent* buffer, size_t bufferSize)
{
    RawEvent* event = buffer;
    size_t capacity = bufferSize;
    while (pendingEventIndex_ < pendingEventCount_) {
        std::unique_lock<std::mutex> my_lock(operationMutex_);
        const struct epoll_event& eventItem = mPendingEventItems[pendingEventIndex_++];
        if (eventItem.data.fd == iNotifyFd_) {
            if (eventItem.events & EPOLLIN) {
                pendingINotify_ = true;
            } else {
                DHLOGI("Received no epoll event 0x%08x.", eventItem.events);
            }
            continue;
        }
        struct input_event readBuffer[bufferSize];
        int32_t readSize = read(eventItem.data.fd, readBuffer, sizeof(struct input_event) * capacity);
        size_t count = ReadInputEvent(readSize, *GetDeviceByFdLocked(eventItem.data.fd));
        Device* device = GetSupportDeviceByFd(eventItem.data.fd);
        if (!device) {
            continue;
        }
        if (!device->isShare) {
            continue;
        }
        if (eventItem.events & EPOLLIN) {
            event += CollectEvent(event, capacity, device, readBuffer, count);

            if (capacity == 0) {
                pendingEventIndex_ -= 1;
                break;
            }
        } else if (eventItem.events & EPOLLHUP) {
            DHLOGI("Removing device %s due to epoll hang-up event.", device->identifier.name.c_str());
            deviceChanged_ = true;
            CloseDeviceLocked(*device);
        }
    }
    return event - buffer;
}

size_t InputHub::CollectEvent(RawEvent* buffer, size_t& capacity, Device* device, struct input_event readBuffer[],
    const size_t count)
{
    std::vector<bool> needFilted(capacity, false);
    bool isTouchEvent = false;
    if ((device->classes & INPUT_DEVICE_CLASS_TOUCH_MT) || (device->classes & INPUT_DEVICE_CLASS_TOUCH)) {
        isTouchEvent = true;
        HandleTouchScreenEvent(readBuffer, count, needFilted);
    }

    RawEvent* event = buffer;
    for (size_t i = 0; i < count; i++) {
        if (needFilted[i]) {
            continue;
        }
        struct input_event& iev = readBuffer[i];
        event->when = ProcessEventTimestamp(iev);
        event->type = iev.type;
        event->code = iev.code;
        event->value = iev.value;
        event->path = device->path;
        event->descriptor = isTouchEvent ? touchDescriptor : device->identifier.descriptor;
        RecordEventLog(event);
        event += 1;
        capacity -= 1;
    }
    return event - buffer;
}

size_t InputHub::ReadInputEvent(int32_t readSize, Device& device)
{
    size_t count = 0;
    if (readSize == 0 || (readSize < 0 && errno == ENODEV)) {
        // Device was removed before INotify noticed.
        DHLOGE("could not get event, removed? (fd: %d size: %d"
            " errno: %d)\n",
            device.fd, readSize, errno);
        deviceChanged_ = true;
        CloseDeviceLocked(device);
    } else if (readSize < 0) {
        if (errno != EAGAIN && errno != EINTR) {
            DHLOGW("could not get event (errno=%d)", errno);
        }
    } else if ((readSize % sizeof(struct input_event)) != 0) {
        DHLOGW("could not get event (wrong size: %d)", readSize);
    } else {
        count = size_t(readSize) / sizeof(struct input_event);
        return count;
    }
    return count;
}

size_t InputHub::DeviceIsExists(InputDeviceEvent* buffer, size_t bufferSize)
{
    InputDeviceEvent* event = buffer;
    size_t capacity = bufferSize;
    // Report any devices that had last been added/removed.
    {
        std::unique_lock<std::mutex> deviceLock(devicesMutex_);
        for (auto it = closingDevices_.begin(); it != closingDevices_.end();) {
            std::unique_ptr<Device> device = std::move(*it);
            DHLOGI("Reporting device closed: id=%s, name=%s\n",
                GetAnonyInt32(device->id).c_str(), device->path.c_str());
            event->type = DeviceType::DEVICE_REMOVED;
            event->deviceInfo = device->identifier;
            event += 1;
            it = closingDevices_.erase(it);
            if (capacity == 0) {
                break;
            }
            capacity--;
        }
    }

    if (needToScanDevices_) {
        needToScanDevices_ = false;
        ScanInputDevices(DEVICE_PATH);
    }

    {
        std::unique_lock<std::mutex> deviceLock(devicesMutex_);
        while (!openingDevices_.empty()) {
            std::unique_ptr<Device> device = std::move(*openingDevices_.rbegin());
            openingDevices_.pop_back();
            DHLOGI("Reporting device opened: id=%s, name=%s\n",
                GetAnonyInt32(device->id).c_str(), device->path.c_str());
            event->type = DeviceType::DEVICE_ADDED;
            event->deviceInfo = device->identifier;
            event += 1;

            auto [dev_it, inserted] = devices_.insert_or_assign(device->id, std::move(device));
            if (!inserted) {
                DHLOGI("Device id %s exists, replaced. \n", GetAnonyInt32(device->id).c_str());
            }
            if (capacity == 0) {
                break;
            }
            capacity--;
        }
    }
    return event - buffer;
}

size_t InputHub::StartCollectInputHandler(InputDeviceEvent* buffer, size_t bufferSize)
{
    size_t count = 0;
    isStartCollectHandler_ = true;
    while (isStartCollectHandler_) {
        count = DeviceIsExists(buffer, bufferSize);
        deviceChanged_ = false;
        GetDeviceHandler();

        if (pendingINotify_ && pendingEventIndex_ >= pendingEventCount_) {
            pendingINotify_ = false;
            ReadNotifyLocked();
            deviceChanged_ = true;
        }

        // Report added or removed devices immediately.
        if (deviceChanged_) {
            continue;
        }
        if (count > 0) {
            break;
        }
        if (RefreshEpollItem(true) < 0) {
            break;
        }
    }

    // All done, return the number of events we read.
    return count;
}

void InputHub::StopCollectInputHandler()
{
    DHLOGI("Stop Collect Input Handler Thread");
    isStartCollectHandler_ = false;
}

void InputHub::GetDeviceHandler()
{
    while (pendingEventIndex_ < pendingEventCount_) {
        std::unique_lock<std::mutex> my_lock(operationMutex_);
        const struct epoll_event& eventItem = mPendingEventItems[pendingEventIndex_++];
        if (eventItem.data.fd == iNotifyFd_) {
            if (eventItem.events & EPOLLIN) {
                pendingINotify_ = true;
            } else {
                DHLOGI(
                    "Received unexpected epoll event 0x%08x for INotify.", eventItem.events);
            }
            continue;
        }

        Device* device = GetDeviceByFdLocked(eventItem.data.fd);
        if (!device) {
            DHLOGE(
                "Received unexpected epoll event 0x%08x for unknown fd %d.",
                eventItem.events, eventItem.data.fd);
            continue;
        }

        if (eventItem.events & EPOLLHUP) {
            DHLOGI("Removing device %s due to epoll hang-up event.",
                device->identifier.name.c_str());
            deviceChanged_ = true;
            CloseDeviceLocked(*device);
        }
    }
}

int32_t InputHub::RefreshEpollItem(bool isSleep)
{
    pendingEventIndex_ = 0;
    int pollResult = epoll_wait(epollFd_, mPendingEventItems, EPOLL_MAX_EVENTS, 0);
    if (pollResult == 0) {
        // Timed out.
        pendingEventCount_ = 0;
        return ERR_DH_INPUT_HUB_EPOLL_WAIT_TIMEOUT;
    }

    if (pollResult < 0) {
        // An error occurred.
        pendingEventCount_ = 0;

        // Sleep after errors to avoid locking up the system.
        // Hopefully the error is transient.
        if (errno != EINTR) {
            DHLOGE("poll failed (errno=%d)\n", errno);
            usleep(SLEEP_TIME_US);
        }
    } else {
        // Some events occurred.
        pendingEventCount_ = pollResult;
    }
    if (isSleep) {
        usleep(SLEEP_TIME_US);
    }
    return DH_SUCCESS;
}

std::vector<InputDevice> InputHub::GetAllInputDevices()
{
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    std::vector<InputDevice> vecDevice;
    for (const auto& [id, device] : devices_) {
        vecDevice.push_back(device->identifier);
    }
    return vecDevice;
}

void InputHub::ScanInputDevices(const std::string& dirname)
{
    DIR *dir;
    struct dirent *de;
    dir = opendir(dirname.c_str());
    if (dir == nullptr) {
        DHLOGE("error opendir /dev/input :%{public}s\n", ConvertErrNo().c_str());
        return;
    }
    size_t dirNameFirstPos = 0;
    size_t dirNameSecondPos = 1;
    size_t dirNameThirdPos = 2;
    while ((de = readdir(dir))) {
        /*
         * The maximum value of d_name defined in the linux kernel is 260. Therefore,
         * The d_name length does not need to be verified.
         */
        if (de->d_name[dirNameFirstPos] == '.' && (de->d_name[dirNameSecondPos] == '\0' ||
            (de->d_name[dirNameSecondPos] == '.' && de->d_name[dirNameThirdPos] == '\0'))) {
            continue;
        }
        std::string devName = dirname + "/" + std::string(de->d_name);
        OpenInputDeviceLocked(devName);
    }
    closedir(dir);
}

int32_t InputHub::OpenInputDeviceLocked(const std::string& devicePath)
{
    {
        std::unique_lock<std::mutex> deviceLock(devicesMutex_);
        for (const auto& [deviceId, device] : devices_) {
            if (device->path == devicePath) {
                return DH_SUCCESS; // device was already registered
            }
        }
    }

    std::unique_lock<std::mutex> my_lock(operationMutex_);
    DHLOGI("Opening device: %s", devicePath.c_str());
    chmod(devicePath.c_str(), S_IWRITE | S_IREAD);
    char canonicalDevicePath[PATH_MAX + 1] = {0x00};
    if (devicePath.length() == 0 || devicePath.length() > PATH_MAX ||
        realpath(devicePath.c_str(), canonicalDevicePath) == nullptr) {
        DHLOGE("path check fail, error path: %s", devicePath.c_str());
        return ERR_DH_INPUT_HUB_OPEN_DEVICEPATH_FAIL;
    }
    struct stat s;
    if ((stat(canonicalDevicePath, &s) == 0) && (s.st_mode & S_IFDIR)) {
        DHLOGI("path: %s is a dir.", devicePath.c_str());
        return DH_SUCCESS;
    }

    int fd = open(canonicalDevicePath, O_RDWR | O_CLOEXEC | O_NONBLOCK);
    int32_t count = 0;
    while ((fd < 0) && (count < MAX_RETRY_COUNT)) {
        ++count;
        usleep(SLEEP_TIME_US);
        fd = open(canonicalDevicePath, O_RDWR | O_CLOEXEC | O_NONBLOCK);
        DHLOGE("could not open %s, %s; retry %d\n", devicePath.c_str(), ConvertErrNo().c_str(), count);
    }
    if (count >= MAX_RETRY_COUNT) {
        DHLOGE("could not open %s, %s\n", devicePath.c_str(), ConvertErrNo().c_str());
        return ERR_DH_INPUT_HUB_OPEN_DEVICEPATH_FAIL;
    }

    InputDevice identifier;
    if (QueryInputDeviceInfo(fd, identifier) < 0) {
        return ERR_DH_INPUT_HUB_QUERY_INPUT_DEVICE_INFO_FAIL;
    }
    GenerateDescriptor(identifier);

    // Allocate device. (The device object takes ownership of the fd at this point.)
    int32_t deviceId = nextDeviceId_++;
    std::unique_ptr<Device> device = std::make_unique<Device>(fd, deviceId, devicePath, identifier);
    RecordDeviceLog(deviceId, devicePath, identifier);

    if (MakeDevice(fd, std::move(device)) < 0) {
        return ERR_DH_INPUT_HUB_MAKE_DEVICE_FAIL;
    }

    return DH_SUCCESS;
}

int32_t InputHub::QueryInputDeviceInfo(int fd, InputDevice& identifier)
{
    char buffer[256] = {0};
    // Get device name.
    if (ioctl(fd, EVIOCGNAME(sizeof(buffer) - 1), &buffer) < 1) {
        DHLOGE(
            "Could not get device name for %s", ConvertErrNo().c_str());
    } else {
        buffer[sizeof(buffer) - 1] = '\0';
        identifier.name = buffer;
    }

    // If the device is already a virtual device, don't monitor it.
    if (identifier.name.find(VIRTUAL_DEVICE_NAME) != std::string::npos) {
        return ERR_DH_INPUT_HUB_IS_VIRTUAL_DEVICE;
    }
    // Get device driver version.
    int driverVersion;
    if (ioctl(fd, EVIOCGVERSION, &driverVersion)) {
        DHLOGE("could not get driver version for %s\n", ConvertErrNo().c_str());
        close(fd);
        return ERR_DH_INPUT_HUB_QUERY_INPUT_DEVICE_INFO_FAIL;
    }
    // Get device identifier.
    struct input_id inputId;
    if (ioctl(fd, EVIOCGID, &inputId)) {
        DHLOGE("could not get device input id for %s\n", ConvertErrNo().c_str());
        close(fd);
        return ERR_DH_INPUT_HUB_QUERY_INPUT_DEVICE_INFO_FAIL;
    }
    identifier.bus = inputId.bustype;
    identifier.product = inputId.product;
    identifier.vendor = inputId.vendor;
    identifier.version = inputId.version;
    // Get device physical location.
    if (ioctl(fd, EVIOCGPHYS(sizeof(buffer) - 1), &buffer) < 1) {
        DHLOGE("could not get location for %s\n", ConvertErrNo().c_str());
    } else {
        buffer[sizeof(buffer) - 1] = '\0';
        identifier.location = buffer;
    }
    // Get device unique id.
    if (ioctl(fd, EVIOCGUNIQ(sizeof(buffer) - 1), &buffer) < 1) {
        DHLOGE("could not get idstring for %s\n", ConvertErrNo().c_str());
    } else {
        buffer[sizeof(buffer) - 1] = '\0';
        identifier.uniqueId = buffer;
    }

    return DH_SUCCESS;
}

int32_t InputHub::MakeDevice(int fd, std::unique_ptr<Device> device)
{
    // Figure out the kinds of events the device reports.
    ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(device->keyBitmask)), device->keyBitmask);
    ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(device->absBitmask)), device->absBitmask);
    ioctl(fd, EVIOCGBIT(EV_REL, sizeof(device->relBitmask)), device->relBitmask);

    // See if this is a multi-touch touchscreen device.
    if (TestBit(BTN_TOUCH, device->keyBitmask) &&
        TestBit(ABS_MT_POSITION_X, device->absBitmask) &&
        TestBit(ABS_MT_POSITION_Y, device->absBitmask)) {
        QueryLocalTouchScreenInfo(fd);
        device->classes |= INPUT_DEVICE_CLASS_TOUCH | INPUT_DEVICE_CLASS_TOUCH_MT;
    } else if (TestBit(BTN_TOUCH, device->keyBitmask) &&
               TestBit(ABS_X, device->absBitmask) &&
               TestBit(ABS_Y, device->absBitmask)) {
        QueryLocalTouchScreenInfo(fd);
        device->classes |= INPUT_DEVICE_CLASS_TOUCH;
    }

    // See if this is a cursor device such as a trackball or mouse.
    if (TestBit(BTN_MOUSE, device->keyBitmask)
        && TestBit(REL_X, device->relBitmask)
        && TestBit(REL_Y, device->relBitmask)) {
        device->classes |= INPUT_DEVICE_CLASS_CURSOR;
    }

    // for Linux version 4.14.116, touchpad recognized as mouse and keyboard at same time,
    // need to avoid device->classes to be 0x09, which mmi can't handler.
    // See if this is a keyboard.
    if (device->classes == 0) {
        bool haveKeyboardKeys = ContainsNonZeroByte(device->keyBitmask, 0, SizeofBitArray(BTN_MISC));
        if (haveKeyboardKeys) {
            device->classes |= INPUT_DEVICE_CLASS_KEYBOARD;
        }
    }

    // If the device isn't recognized as something we handle, don't monitor it.
    if (device->classes == 0) {
        DHLOGI("Dropping device: name='%s'", device->identifier.name.c_str());
        return ERR_DH_INPUT_HUB_MAKE_DEVICE_FAIL;
    }

    if (RegisterDeviceForEpollLocked(*device) != DH_SUCCESS) {
        return ERR_DH_INPUT_HUB_MAKE_DEVICE_FAIL;
    }

    device->identifier.classes = device->classes;
    if (device->classes & inputTypes_) {
        device->isShare = true;
    }
    DHLOGI("inputType=%d", inputTypes_.load());
    DHLOGI("New device: fd=%d, name='%s', classes=0x%x, isShare=%d",
        fd, device->identifier.name.c_str(), device->classes, device->isShare);

    AddDeviceLocked(std::move(device));
    return DH_SUCCESS;
}

int32_t InputHub::QueryLocalTouchScreenInfo(int fd)
{
    LocalTouchScreenInfo info = DInputContext::GetInstance().GetLocalTouchScreenInfo();

    InputDevice identifier;
    if (QueryInputDeviceInfo(fd, identifier) < 0) {
        return ERR_DH_INPUT_HUB_QUERY_INPUT_DEVICE_INFO_FAIL;
    }
    identifier.classes |= INPUT_DEVICE_CLASS_TOUCH_MT;
    info.localAbsInfo.deviceInfo = identifier;

    struct input_absinfo absInfo;
    ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &absInfo);
    info.localAbsInfo.absXMin = absInfo.minimum;
    info.localAbsInfo.absXMax = absInfo.maximum;
    info.localAbsInfo.absMtPositionXMin = absInfo.minimum;
    info.localAbsInfo.absMtPositionXMax = absInfo.maximum;
    info.sinkPhyWidth = (uint32_t)(absInfo.maximum + 1);

    ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &absInfo);
    info.localAbsInfo.absYMin = absInfo.minimum;
    info.localAbsInfo.absYMax = absInfo.maximum;
    info.localAbsInfo.absMtPositionYMin = absInfo.minimum;
    info.localAbsInfo.absMtPositionYMax = absInfo.maximum;
    info.sinkPhyHeight = (uint32_t)(absInfo.maximum + 1);

    ioctl(fd, EVIOCGABS(ABS_PRESSURE), &absInfo);
    info.localAbsInfo.absPressureMin = absInfo.minimum;
    info.localAbsInfo.absPressureMax = absInfo.maximum;
    info.localAbsInfo.absMtPressureMin = absInfo.minimum;
    info.localAbsInfo.absMtPressureMax = absInfo.maximum;

    ioctl(fd, EVIOCGABS(ABS_MT_TOUCH_MAJOR), &absInfo);
    info.localAbsInfo.absMtTouchMajorMin = absInfo.minimum;
    info.localAbsInfo.absMtTouchMajorMax = absInfo.maximum;

    ioctl(fd, EVIOCGABS(ABS_MT_TOUCH_MINOR), &absInfo);
    info.localAbsInfo.absMtTouchMinorMin = absInfo.minimum;
    info.localAbsInfo.absMtTouchMinorMax = absInfo.maximum;

    ioctl(fd, EVIOCGABS(ABS_MT_ORIENTATION), &absInfo);
    info.localAbsInfo.absMtOrientationMin = absInfo.minimum;
    info.localAbsInfo.absMtOrientationMax = absInfo.maximum;

    ioctl(fd, EVIOCGABS(ABS_MT_BLOB_ID), &absInfo);
    info.localAbsInfo.absMtBlobIdMin = absInfo.minimum;
    info.localAbsInfo.absMtBlobIdMax = absInfo.maximum;

    ioctl(fd, EVIOCGABS(ABS_MT_TRACKING_ID), &absInfo);
    info.localAbsInfo.absMtTrackingIdMin = absInfo.minimum;
    info.localAbsInfo.absMtTrackingIdMax = absInfo.maximum;

    DInputContext::GetInstance().SetLocalTouchScreenInfo(info);
    return DH_SUCCESS;
}

std::string InputHub::StringPrintf(const char* format, ...) const
{
    static const int kSpaceLength = 1024;
    char space[kSpaceLength];

    va_list ap;
    va_start(ap, format);
    std::string result;
    int32_t ret = vsnprintf_s(space, sizeof(space), sizeof(space) - 1, format, ap);
    if (ret >= DH_SUCCESS && (size_t)ret < sizeof(space)) {
        result = space;
    } else {
        return "the buffer is overflow!";
    }
    va_end(ap);
    return result;
}

void InputHub::GenerateDescriptor(InputDevice& identifier) const
{
    std::string rawDescriptor;
    rawDescriptor += StringPrintf(":%04x:%04x:", identifier.vendor,
        identifier.product);
    // add handling for USB devices to not uniqueify kbs that show up twice
    if (!identifier.uniqueId.empty()) {
        rawDescriptor += "uniqueId:";
        rawDescriptor += identifier.uniqueId;
    } else if (!identifier.location.empty()) {
        rawDescriptor += "location:";
        rawDescriptor += identifier.location;
    }
    if (!identifier.name.empty()) {
        rawDescriptor += "name:";
        std::string name = identifier.name;
        rawDescriptor += regex_replace(name, std::regex(" "), "");
    }

    identifier.descriptor = DH_ID_PREFIX + Sha256(rawDescriptor);
    DHLOGI("Created descriptor: raw=%s, cooked=%s", rawDescriptor.c_str(),
        GetAnonyString(identifier.descriptor).c_str());
}

int32_t InputHub::RegisterDeviceForEpollLocked(const Device& device)
{
    int32_t result = RegisterFdForEpoll(device.fd);
    if (result != DH_SUCCESS) {
        DHLOGE("Could not add input device fd to epoll for device %d", device.id);
        return result;
    }
    return result;
}

int32_t InputHub::RegisterFdForEpoll(int fd)
{
    struct epoll_event eventItem = {};
    eventItem.events = EPOLLIN | EPOLLWAKEUP;
    eventItem.data.fd = fd;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &eventItem)) {
        DHLOGE(
            "Could not add fd to epoll instance: %s", ConvertErrNo().c_str());
        return -errno;
    }
    return DH_SUCCESS;
}

void InputHub::AddDeviceLocked(std::unique_ptr<Device> device)
{
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    openingDevices_.push_back(std::move(device));
}

void InputHub::CloseDeviceLocked(Device& device)
{
    DHLOGI("Removed device: path=%s name=%s id=%s fd=%d classes=0x%x",
        device.path.c_str(), device.identifier.name.c_str(), GetAnonyInt32(device.id).c_str(),
        device.fd, device.classes);

    UnregisterDeviceFromEpollLocked(device);
    device.Close();
    {
        std::unique_lock<std::mutex> devicesLock(devicesMutex_);
        closingDevices_.push_back(std::move(devices_[device.id]));
        devices_.erase(device.id);
    }
}

void InputHub::CloseDeviceForAllLocked(Device& device)
{
    DHLOGI("Removed device: path=%s name=%s id=%s fd=%d classes=0x%x",
        device.path.c_str(), device.identifier.name.c_str(), GetAnonyInt32(device.id).c_str(),
        device.fd, device.classes);

    UnregisterDeviceFromEpollLocked(device);
    device.Close();
    closingDevices_.push_back(std::move(devices_[device.id]));
    devices_.erase(device.id);
}

int32_t InputHub::UnregisterDeviceFromEpollLocked(const Device& device) const
{
    if (device.HasValidFd()) {
        int32_t result = UnregisterFdFromEpoll(device.fd);
        if (result != DH_SUCCESS) {
            DHLOGE("Could not remove input device fd from epoll for device %s",
                GetAnonyInt32(device.id).c_str());
            return result;
        }
    }
    return DH_SUCCESS;
}

int32_t InputHub::UnregisterFdFromEpoll(int fd) const
{
    if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr)) {
        DHLOGE("Could not remove fd from epoll instance: %s", ConvertErrNo().c_str());
        return ERR_DH_INPUT_HUB_UNREGISTER_FD_FAIL;
    }
    return DH_SUCCESS;
}

int32_t InputHub::ReadNotifyLocked()
{
    size_t res;
    char event_buf[512];
    size_t event_size;
    size_t event_pos = 0;
    struct inotify_event *event;

    DHLOGI("readNotify nfd: %d\n", iNotifyFd_);
    res = (size_t)read(iNotifyFd_, event_buf, sizeof(event_buf));
    if (res < sizeof(*event)) {
        if (errno == EINTR)
            return DH_SUCCESS;
        DHLOGE("could not get event, %s\n", ConvertErrNo().c_str());
        return ERR_DH_INPUT_HUB_GET_EVENT_FAIL;
    }

    while (res >= sizeof(*event)) {
        event = (struct inotify_event *)(event_buf + event_pos);
        JudgeDeviceOpenOrClose(*event);
        event_size = sizeof(*event) + event->len;
        res -= event_size;
        event_pos += event_size;
    }
    return DH_SUCCESS;
}

void InputHub::JudgeDeviceOpenOrClose(const inotify_event& event)
{
    if (event.len) {
        if (event.wd == inputWd_) {
            std::string filename = std::string(DEVICE_PATH) + "/" + event.name;
            if (event.mask & IN_CREATE) {
                OpenInputDeviceLocked(filename);
            } else {
                DHLOGI(
                    "Removing device '%s' due to inotify event\n", filename.c_str());
                CloseDeviceByPathLocked(filename);
            }
        } else {
            DHLOGI("Unexpected inotify event, wd = %i", event.wd);
        }
    }
}

void InputHub::CloseDeviceByPathLocked(const std::string& devicePath)
{
    Device* device = GetDeviceByPathLocked(devicePath);
    if (device) {
        CloseDeviceLocked(*device);
        return;
    }
    DHLOGI(
        "Remove device: %s not found, device may already have been removed.", devicePath.c_str());
}

void InputHub::CloseAllDevicesLocked()
{
    DHLOGI("Close All Devices");
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    while (!devices_.empty()) {
        CloseDeviceForAllLocked(*(devices_.begin()->second));
    }
}

InputHub::Device* InputHub::GetDeviceByDescriptorLocked(const std::string& descriptor)
{
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    for (const auto& device : openingDevices_) {
        if (device && descriptor == device->identifier.descriptor) {
            return device.get();
        }
    }

    for (const auto& [id, device] : devices_) {
        if (device && descriptor == device->identifier.descriptor) {
            return device.get();
        }
    }

    return nullptr;
}

InputHub::Device* InputHub::GetDeviceByPathLocked(const std::string& devicePath)
{
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    for (const auto& [id, device] : devices_) {
        if (device->path == devicePath) {
            return device.get();
        }
    }
    return nullptr;
}

InputHub::Device* InputHub::GetDeviceByFdLocked(int fd)
{
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    for (const auto& [id, device] : devices_) {
        if (device->fd == fd) {
            return device.get();
        }
    }
    return nullptr;
}

InputHub::Device* InputHub::GetSupportDeviceByFd(int fd)
{
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    for (const auto& [id, device] : devices_) {
        if (device->fd == fd) {
            return device.get();
        }
    }
    return nullptr;
}

bool InputHub::ContainsNonZeroByte(const uint8_t* array, uint32_t startIndex, uint32_t endIndex)
{
    const uint8_t* end = array + endIndex;
    array += startIndex;
    while (array != end) {
        if (*(array++) != 0) {
            return true;
        }
    }
    return false;
}

int64_t InputHub::ProcessEventTimestamp(const input_event& event)
{
    const int64_t inputEventTime = event.input_event_sec * 1000000000LL + event.input_event_usec * 1000LL;
    return inputEventTime;
}

bool InputHub::TestBit(uint32_t bit, const uint8_t* array)
{
    constexpr int units = 8;
    return (array)[(bit) / units] & (1 << ((bit) % units));
}

uint32_t InputHub::SizeofBitArray(uint32_t bit)
{
    constexpr int round = 7;
    constexpr int divisor = 8;
    return ((bit) + round) / divisor;
}

bool InputHub::IsSupportInputTypes(uint32_t classes)
{
    return classes & inputTypes_;
}

void InputHub::SaveAffectDhId(bool isEnable, const std::string &dhId, AffectDhIds &affDhIds)
{
    if (isEnable) {
        affDhIds.sharingDhIds.push_back(dhId);
    } else {
        affDhIds.noSharingDhIds.push_back(dhId);
    }
}

AffectDhIds InputHub::SetSupportInputType(bool enabled, const uint32_t &inputTypes)
{
    AffectDhIds affDhIds;
    inputTypes_ = inputTypes;
    DHLOGI("SetSupportInputType: inputTypes=0x%x,", inputTypes_.load());
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[id, device] : devices_) {
        if (device->classes & inputTypes_) {
            device->isShare = enabled;
            DHLOGW("ByType dhid:%s, isshare:%d", device->identifier.descriptor.c_str(), enabled);
            SaveAffectDhId(enabled, device->identifier.descriptor, affDhIds);
        }
    }

    return affDhIds;
}

void InputHub::GetDeviceDhIdByFd(int32_t fd, std::string &dhId)
{
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[id, device] : devices_) {
        if (device->fd == fd) {
            dhId = device->identifier.descriptor;
            return;
        }
    }
    dhId.clear();
}

AffectDhIds InputHub::SetSharingDevices(bool enabled, std::vector<std::string> dhIds)
{
    AffectDhIds affDhIds;
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    for (auto dhId : dhIds) {
        for (const auto &[id, device] : devices_) {
            if (device->identifier.descriptor == dhId) {
                device->isShare = enabled;
                DHLOGW("dhid:%s, isshare:%d", device->identifier.descriptor.c_str(), enabled);
                SaveAffectDhId(enabled, device->identifier.descriptor, affDhIds);
                break;
            }
        }
    }

    return affDhIds;
}

void InputHub::GetShareMousePathByDhId(std::vector<std::string> dhIds, std::string &path, std::string &dhId)
{
    DHLOGI("GetShareMousePathByDhId: devices_.size:%d,", devices_.size());
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    for (auto dhId_ : dhIds) {
        for (const auto &[id, device] : devices_) {
            DHLOGI("descriptor:%s, isShare[%d], type[%d]", device->identifier.descriptor.c_str(),
                   device->isShare, device->classes);
            if ((device->identifier.descriptor == dhId_) &&
                ((device->classes & INPUT_DEVICE_CLASS_CURSOR) != 0)) {
                dhId = dhId_;
                path = device->path;
                return; // return First shared mouse
            }
        }
    }
}

void InputHub::GetDevicesInfoByType(const uint32_t inputTypes, std::map<int32_t, std::string> &datas)
{
    uint32_t input_types_ = 0;

    if ((inputTypes & static_cast<uint32_t>(DInputDeviceType::MOUSE)) != 0) {
        input_types_ |= INPUT_DEVICE_CLASS_CURSOR;
    }

    if ((inputTypes & static_cast<uint32_t>(DInputDeviceType::KEYBOARD)) != 0) {
        input_types_ |= INPUT_DEVICE_CLASS_KEYBOARD;
    }

    if ((inputTypes & static_cast<uint32_t>(DInputDeviceType::TOUCHSCREEN)) != 0) {
        input_types_ |= INPUT_DEVICE_CLASS_TOUCH | INPUT_DEVICE_CLASS_TOUCH_MT;
    }

    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[id, device] : devices_) {
        if (device->classes & input_types_) {
            datas.insert(std::pair<int32_t, std::string>(device->fd, device->identifier.descriptor));
        }
    }
}

void InputHub::GetDevicesInfoByDhId(std::vector<std::string> dhidsVec, std::map<int32_t, std::string> &datas)
{
    for (auto dhId : dhidsVec) {
        std::unique_lock<std::mutex> deviceLock(devicesMutex_);
        for (const auto &[id, device] : devices_) {
            if (device->identifier.descriptor == dhId) {
                datas.insert(std::pair<int32_t, std::string>(device->fd, dhId));
            }
        }
    }
}

bool InputHub::IsAllDevicesStoped()
{
    std::unique_lock<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[id, device] : devices_) {
        if (device->isShare) {
            return false;
        }
    }
    return true;
}

void InputHub::RecordDeviceLog(const int32_t deviceId, const std::string& devicePath, const InputDevice& identifier)
{
    DHLOGI("add device %d: %s\n", deviceId, devicePath.c_str());
    DHLOGI("  bus:        %04x\n"
           "  vendor      %04x\n"
           "  product     %04x\n"
           "  version     %04x\n",
        identifier.bus, identifier.vendor, identifier.product, identifier.version);
    DHLOGI("  name:       \"%s\"\n", identifier.name.c_str());
    DHLOGI("  location:   \"%s\"\n", identifier.location.c_str());
    DHLOGI("  unique id:  \"%s\"\n", identifier.uniqueId.c_str());
    DHLOGI("  descriptor: \"%s\"\n", GetAnonyString(identifier.descriptor).c_str());
}

void InputHub::RecordEventLog(const RawEvent* event)
{
    std::string eventType = "";
    switch (event->type) {
        case EV_KEY:
            eventType = "EV_KEY";
            break;
        case EV_REL:
            eventType = "EV_REL";
            break;
        case EV_ABS:
            eventType = "EV_ABS";
            break;
        case EV_SYN:
            eventType = "EV_SYN";
            break;
        default:
            eventType = "other type " + std::to_string(event->type);
            break;
    }
    DHLOGD("1.E2E-Test Sink collect event, EventType: %s, Code: %d, Value: %d, Path: %s, descriptor: %s,"
        "When:%" PRId64 "", eventType.c_str(), event->code, event->value, event->path.c_str(),
        GetAnonyString(event->descriptor).c_str(), event->when);
}

void InputHub::HandleTouchScreenEvent(struct input_event readBuffer[], const size_t count,
    std::vector<bool>& needFilted)
{
    std::vector<std::pair<size_t, size_t>> absIndexs;
    int32_t firstIndex = -1;
    int32_t lastIndex = -1;

    for (size_t i = 0; i < count; i++) {
        struct input_event& iev = readBuffer[i];
        if ((iev.type == EV_ABS) && (iev.code == ABS_X || iev.code == ABS_MT_POSITION_X)) {
            firstIndex = (int32_t)i;
        } else if (iev.type  == EV_SYN) {
            lastIndex = (int32_t)i;
        }
        if ((firstIndex >= 0) && (lastIndex > firstIndex)) {
            absIndexs.emplace_back(std::make_pair((size_t)firstIndex, (size_t)lastIndex));
        }
    }

    AbsInfo absInfo = {
        .absX = 0,
        .absY = 0,
        .absXIndex = -1,
        .absYIndex = -1,
    };
    for (auto iter : absIndexs) {
        absInfo.absXIndex = -1;
        absInfo.absYIndex = -1;

        for (size_t j = iter.first; j <= iter.second; j++) {
            struct input_event &iev = readBuffer[j];
            if (iev.code == ABS_X || iev.code == ABS_MT_POSITION_X) {
                absInfo.absX = iev.value;
                absInfo.absXIndex = (int32_t)j;
            }
            if (iev.code == ABS_Y || iev.code == ABS_MT_POSITION_Y) {
                absInfo.absY = iev.value;
                absInfo.absYIndex = (int32_t)j;
            }
        }

        if ((absInfo.absXIndex < 0) || (absInfo.absYIndex < 0)) {
            for (size_t j = iter.first; j <= iter.second; j++) {
                needFilted[j] = true;
            }
            continue;
        }
        if (!CheckTouchPointRegion(readBuffer, absInfo)) {
            for (size_t j = iter.first; j <= iter.second; j++) {
                needFilted[j] = true;
            }
        }
    }
}

bool InputHub::CheckTouchPointRegion(struct input_event readBuffer[], const AbsInfo& absInfo)
{
    auto sinkInfos = DInputContext::GetInstance().GetAllSinkScreenInfo();

    for (const auto& [id, sinkInfo] : sinkInfos) {
        auto info = sinkInfo.transformInfo;
        if ((absInfo.absX >= info.sinkWinPhyX) && (absInfo.absX <= (info.sinkWinPhyX + info.sinkProjPhyWidth))
            && (absInfo.absY >= info.sinkWinPhyY)  && (absInfo.absY <= (info.sinkWinPhyY + info.sinkProjPhyHeight))) {
            touchDescriptor = sinkInfo.srcScreenInfo.sourcePhyId;
            readBuffer[absInfo.absXIndex].value = (absInfo.absX - info.sinkWinPhyX) * info.coeffWidth;
            readBuffer[absInfo.absYIndex].value = (absInfo.absY - info.sinkWinPhyY) * info.coeffHeight;
            return true;
        }
    }
    return false;
}

InputHub::Device::Device(int fd, int32_t id, const std::string& path,
    const InputDevice& identifier) : next(nullptr), fd(fd), id(id), path(path), identifier(identifier),
    classes(0), enabled(false), isShare(false), isVirtual(fd < 0) {
    memset_s(keyBitmask, sizeof(keyBitmask), 0, sizeof(keyBitmask));
    memset_s(absBitmask, sizeof(absBitmask), 0, sizeof(absBitmask));
    memset_s(relBitmask, sizeof(relBitmask), 0, sizeof(relBitmask));
}

InputHub::Device::~Device()
{
    Close();
}

void InputHub::Device::Close()
{
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }
}

int32_t InputHub::Device::Enable()
{
    char canonicalPath[PATH_MAX + 1] = {0x00};

    if (path.length() == 0 || path.length() > PATH_MAX || realpath(path.c_str(), canonicalPath) == nullptr) {
        DHLOGE("path check fail, error path: %s", path.c_str());
        return ERR_DH_INPUT_HUB_DEVICE_ENABLE_FAIL;
    }
    fd = open(canonicalPath, O_RDWR | O_CLOEXEC | O_NONBLOCK);
    if (fd < 0) {
        DHLOGE("could not open %s, %s\n", path.c_str(), ConvertErrNo().c_str());
        return ERR_DH_INPUT_HUB_DEVICE_ENABLE_FAIL;
    }
    enabled = true;
    return DH_SUCCESS;
}

int32_t InputHub::Device::Disable()
{
    Close();
    enabled = false;
    return DH_SUCCESS;
}

bool InputHub::Device::HasValidFd() const
{
    return !isVirtual && enabled;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
