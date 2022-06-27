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

#include "input_device_manager.h"

#include <parameters.h>
#include <unordered_map>

#include "key_event_value_transformation.h"
#include "util_ex.h"

#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
#include "constants_dinput.h"
#include "dinput_manager.h"
#endif

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceManager"};
constexpr int32_t INVALID_DEVICE_ID = -1;
constexpr int32_t SUPPORT_KEY = 1;
const char *SPLIT_SYMBOL = "|";

constexpr int32_t ABS_MT_TOUCH_MAJOR = 0x30;
constexpr int32_t ABS_MT_TOUCH_MINOR = 0x31;
constexpr int32_t ABS_MT_ORIENTATION = 0x34;
constexpr int32_t ABS_MT_POSITION_X  = 0x35;
constexpr int32_t ABS_MT_POSITION_Y = 0x36;
constexpr int32_t ABS_MT_PRESSURE = 0x3a;
constexpr int32_t ABS_MT_WIDTH_MAJOR = 0x32;
constexpr int32_t ABS_MT_WIDTH_MINOR = 0x33;

const std::string BUNDLE_NAME = "ohos.multimodalinput.input";
const std::string DEVICE_ADD = "add";
const std::string DEVICE_REMOVE = "remove";
constexpr int32_t BUS_BLUETOOTH = 0X5;

std::unordered_map<int32_t, std::string> axisType = {
    {ABS_MT_TOUCH_MAJOR, "TOUCH_MAJOR"},
    {ABS_MT_TOUCH_MINOR, "TOUCH_MINOR"},
    {ABS_MT_ORIENTATION, "ORIENTATION"},
    {ABS_MT_POSITION_X, "POSITION_X"},
    {ABS_MT_POSITION_Y, "POSITION_Y"},
    {ABS_MT_PRESSURE, "PRESSURE"},
    {ABS_MT_WIDTH_MAJOR, "WIDTH_MAJOR"},
    {ABS_MT_WIDTH_MINOR, "WIDTH_MINOR"}
};
} // namespace

std::shared_ptr<InputDevice> InputDeviceManager::GetInputDevice(int32_t id) const
{
    CALL_LOG_ENTER;
    auto iter = inputDevice_.find(id);
    if (iter == inputDevice_.end()) {
        MMI_HILOGE("failed to search for the device");
        return nullptr;
    }
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    if (IsRemote(iter->second) && !IsDistributedInput(iter->second)) {
        return nullptr;
    }
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
    std::shared_ptr<InputDevice> inputDevice = std::make_shared<InputDevice>();
    CHKPP(inputDevice);
    inputDevice->SetId(iter->first);
    inputDevice->SetType(static_cast<int32_t>(libinput_device_get_tags(iter->second)));
    const char* name = libinput_device_get_name(iter->second);
    inputDevice->SetName((name == nullptr) ? ("null") : (name));
    inputDevice->SetBustype(libinput_device_get_id_bustype(iter->second));
    inputDevice->SetVersion(libinput_device_get_id_version(iter->second));
    inputDevice->SetProduct(libinput_device_get_id_product(iter->second));
    inputDevice->SetVendor(libinput_device_get_id_vendor(iter->second));
    const char* phys = libinput_device_get_phys(iter->second);
    inputDevice->SetPhys((phys == nullptr) ? ("null") : (phys));
    inputDevice->SetNetworkId((MakeNetworkId(inputDevice->GetPhys())));
    inputDevice->SetRemote(false);
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    std::string::size_type pos = inputDevice->GetName().find(VIRTUAL_DEVICE_NAME);
    inputDevice->SetRemote(pos != inputDevice->GetName().npos);
#endif // OHOS_DISTRIBUTED_INPUT_MODEL

    const char* uniq = libinput_device_get_uniq(iter->second);
    inputDevice->SetUniq((uniq == nullptr) ? ("null") : (uniq));
    InputDevice::AxisInfo axis;
    for (const auto &item : axisType) {
        int32_t min = libinput_device_get_axis_min(iter->second, item.first);
        if (min == -1) {
            MMI_HILOGW("The device does not support this axis");
            continue;
        }
        axis.SetAxisType(item.first);
        axis.SetMinimum(min);
        axis.SetMaximum(libinput_device_get_axis_max(iter->second, item.first));
        axis.SetFuzz(libinput_device_get_axis_fuzz(iter->second, item.first));
        axis.SetFlat(libinput_device_get_axis_flat(iter->second, item.first));
        axis.SetResolution(libinput_device_get_axis_resolution(iter->second, item.first));
        inputDevice->AddAxisInfo(axis);
    }
    return inputDevice;
}

std::string InputDeviceManager::MakeNetworkId(const std::string& phys) const
{
    uint64_t endPos = 0;
    uint64_t startPos = 0;
    std::string networkId = "";
    std::size_t splitPos = phys.find(SPLIT_SYMBOL);
    if (std::string::npos != splitPos) {
        startPos = splitPos + 1;
        endPos = phys.size();
        networkId = phys.substr(startPos,  endPos - startPos);
    }
    return networkId;
}

std::vector<int32_t> InputDeviceManager::GetInputDeviceIds() const
{
    CALL_LOG_ENTER;
    std::vector<int32_t> ids;
    for (const auto &item : inputDevice_) {
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
        if (IsRemote(item.second) && !IsDistributedInput(item.second)) {
            continue;
        }
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
        ids.push_back(item.first);
    }
    return ids;
}

std::vector<bool> InputDeviceManager::SupportKeys(int32_t deviceId, std::vector<int32_t> &keyCodes)
{
    CALL_LOG_ENTER;
    std::vector<bool> keystrokeAbility;
    auto iter = inputDevice_.find(deviceId);
    if (iter == inputDevice_.end()) {
        keystrokeAbility.insert(keystrokeAbility.end(), keyCodes.size(), false);
        return keystrokeAbility;
    }
    for (const auto& item : keyCodes) {
        auto sysKeyCode = InputTransformationKeyValue(item);
        bool ret = libinput_device_has_key(iter->second, sysKeyCode) == SUPPORT_KEY;
        keystrokeAbility.push_back(ret);
    }
    return keystrokeAbility;
}

bool InputDeviceManager::GetDeviceConfig(int32_t deviceId, int32_t &keyboardType)
{
    CALL_LOG_ENTER;
    if (auto iter = inputDevice_.find(deviceId); iter == inputDevice_.end()) {
        MMI_HILOGE("Failed to search for the deviceID");
        return false;
    }
    auto deviceConfig = KeyRepeat->GetDeviceConfig();
    auto it = deviceConfig.find(deviceId);
    if (it == deviceConfig.end()) {
        MMI_HILOGE("Failed to obtain the keyboard type of the configuration file");
        return false;
    }
    keyboardType = it->second.keyboardType;
    MMI_HILOGD("Get keyboard type results from the configuration file:%{public}d", keyboardType);
    return true;
}

int32_t InputDeviceManager::GetKeyboardBusMode(int32_t deviceId)
{
    CALL_LOG_ENTER;
    std::shared_ptr dev = GetInputDevice(deviceId);
    CHKPR(dev, ERROR_NULL_POINTER);
    return dev->GetBustype();
}

int32_t InputDeviceManager::GetDeviceSupportKey(int32_t deviceId)
{
    CALL_LOG_ENTER;
    std::vector <int32_t> keyCodes;
    keyCodes.push_back(KeyEvent::KEYCODE_Q);
    keyCodes.push_back(KeyEvent::KEYCODE_NUMPAD_1);
    keyCodes.push_back(KeyEvent::KEYCODE_HOME);
    keyCodes.push_back(KeyEvent::KEYCODE_CTRL_LEFT);
    keyCodes.push_back(KeyEvent::KEYCODE_SHIFT_RIGHT);
    keyCodes.push_back(KeyEvent::KEYCODE_F20);
    std::vector<bool> supportKey = SupportKeys(deviceId, keyCodes);
    std::map<int32_t, bool> determineKbType;
    for (size_t i = 0; i < keyCodes.size(); i++) {
        determineKbType[keyCodes[i]] = supportKey[i];
    }
    int32_t keyboardType = 0;
    if (determineKbType[KeyEvent::KEYCODE_HOME] && GetKeyboardBusMode(deviceId) == BUS_BLUETOOTH) {
        keyboardType = KEYBOARD_TYPE_REMOTECONTROL;
        MMI_HILOGD("The keyboard type is remote control:%{public}d", keyboardType);
    } else if (determineKbType[KeyEvent::KEYCODE_NUMPAD_1] && !determineKbType[KeyEvent::KEYCODE_Q]) {
        keyboardType = KEYBOARD_TYPE_DIGITALKEYBOARD;
        MMI_HILOGD("The keyboard type is digital keyboard:%{public}d", keyboardType);
    } else if (determineKbType[KeyEvent::KEYCODE_Q]) {
        keyboardType = KEYBOARD_TYPE_ALPHABETICKEYBOARD;
        MMI_HILOGD("The keyboard type is standard:%{public}d", keyboardType);
    } else if (determineKbType[KeyEvent::KEYCODE_CTRL_LEFT] && determineKbType[KeyEvent::KEYCODE_SHIFT_RIGHT]
        && determineKbType[KeyEvent::KEYCODE_F20]) {
        keyboardType = KEYBOARD_TYPE_HANDWRITINGPEN;
        MMI_HILOGD("The keyboard type is handwriting pen:%{public}d", keyboardType);
    } else {
        keyboardType = KEYBOARD_TYPE_UNKNOWN;
        MMI_HILOGW("Undefined keyboard type");
    }
    MMI_HILOGD("Get keyboard type results by supporting keys:%{public}d", keyboardType);
    return keyboardType;
}

int32_t InputDeviceManager::GetKeyboardType(int32_t deviceId)
{
    CALL_LOG_ENTER;
    int32_t keyboardType = KEYBOARD_TYPE_NONE;
    if (auto iter = inputDevice_.find(deviceId); iter == inputDevice_.end()) {
        MMI_HILOGE("Failed to search for the deviceID");
        return keyboardType;
    }
    if (GetDeviceConfig(deviceId, keyboardType)) {
        return keyboardType;
    }
    keyboardType = GetDeviceSupportKey(deviceId);
    return keyboardType;
}

void InputDeviceManager::AddDevMonitor(SessionPtr sess, std::function<void(std::string, int32_t)> callback)
{
    CALL_LOG_ENTER;
    auto iter = devMonitor_.find(sess);
    if (iter == devMonitor_.end()) {
        devMonitor_[sess] = callback;
    }
}

void InputDeviceManager::RemoveDevMonitor(SessionPtr sess)
{
    CALL_LOG_ENTER;
    auto iter = devMonitor_.find(sess);
    if (iter == devMonitor_.end()) {
        MMI_HILOGE("session does not exist");
        return;
    }
    devMonitor_.erase(iter);
}

bool InputDeviceManager::HasPointerDevice()
{
    for (auto it = inputDevice_.begin(); it != inputDevice_.end(); ++it) {
        if (IsPointerDevice(it->second)) {
            return true;
        }
    }
    return false;
}

void InputDeviceManager::OnInputDeviceAdded(struct libinput_device *inputDevice)
{
    CALL_LOG_ENTER;
    CHKPV(inputDevice);
    for (const auto& item : inputDevice_) {
        if (item.second == inputDevice) {
            MMI_HILOGI("the device already exists");
            return;
        } 
    }
    if (nextId_ == INT32_MAX) {
        MMI_HILOGE("the nextId_ exceeded the upper limit");
        return;
    }
    inputDevice_[nextId_] = inputDevice;
#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
    bool hasPointerDevice = false;
    bool isRemote = IsRemote(inputDevice);
    bool isDistributedInput = IsDistributedInput(inputDevice);
    if (isDistributedInput || !isRemote) {
        HandleDeviceChanged(DEVICE_ADD, nextId_);
    }
    if (IsPointerDevice(inputDevice)) {
        bool needShowMouse = hasPointerDevice || !isRemote;
        NotifyPointerDevice(true, needShowMouse);
    }
#else
    for (const auto &item : devMonitor_) {
        CHKPC(item.first);
        item.second("add", nextId_);
    }
    ++nextId_;
    if (IsPointerDevice(inputDevice)) {
        NotifyPointerDevice(true, true);
        OHOS::system::SetParameter(INPUT_POINTER_DEVICE, "true");
    }
#endif // OHOS_DISTRIBUTED_INPUT_MODEL
}

void InputDeviceManager::HandleDeviceChanged(std::string changedType, int32_t id)
{
    CALL_LOG_ENTER;
    for (const auto& item : devMonitor_) {
        CHKPC(item.first);
        item.second(changedType, id);
    }
}
void InputDeviceManager::OnInputDeviceRemoved(struct libinput_device *inputDevice)
{
    CALL_LOG_ENTER;
    CHKPV(inputDevice);
    int32_t deviceId = INVALID_DEVICE_ID;
    for (auto it = inputDevice_.begin(); it != inputDevice_.end(); ++it) {
        if (it->second == inputDevice) {
            deviceId = it->first;
            inputDevice_.erase(it);
            break;
        }
    }
    HandleDeviceChanged(DEVICE_REMOVE, deviceId);
    ScanPointerDevice();
}

void InputDeviceManager::ScanPointerDevice()
{
    if (!HasPointerDevice()) {
        NotifyPointerDevice(false, true);
    }
}

bool InputDeviceManager::IsPointerDevice(struct libinput_device* device)
{
    CHKPF(device);
    enum evdev_device_udev_tags udevTags = libinput_device_get_tags(device);
    MMI_HILOGD("udev tag:%{public}d", static_cast<int32_t>(udevTags));
    return udevTags & (EVDEV_UDEV_TAG_MOUSE | EVDEV_UDEV_TAG_TRACKBALL | EVDEV_UDEV_TAG_POINTINGSTICK | 
    EVDEV_UDEV_TAG_TOUCHPAD | EVDEV_UDEV_TAG_TABLET_PAD);
}

void InputDeviceManager::Attach(std::shared_ptr<IDeviceObserver> observer)
{
    CALL_LOG_ENTER;
    observers_.push_back(observer);
}

void InputDeviceManager::Detach(std::shared_ptr<IDeviceObserver> observer)
{
    CALL_LOG_ENTER;
    observers_.remove(observer);
}

void InputDeviceManager::NotifyPointerDevice(bool hasPointerDevice, bool isPointerVisible)
{
    MMI_HILOGI("observers_ size:%{public}zu", observers_.size());
    for (auto observer = observers_.begin(); observer != observers_.end(); observer++) {
        (*observer)->UpdatePointerDevice(hasPointerDevice, isPointerVisible);
    }
}

int32_t InputDeviceManager::FindInputDeviceId(struct libinput_device* inputDevice)
{
    CALL_LOG_ENTER;
    CHKPR(inputDevice, INVALID_DEVICE_ID);
    for (const auto& item : inputDevice_) {
        if (item.second == inputDevice) {
            MMI_HILOGI("find input device id success");
            return item.first;
        }
    }
    MMI_HILOGE("find input device id failed");
    return INVALID_DEVICE_ID;
}

std::string InputDeviceManager::FindInputDeviceName(int32_t deviceId)
{
    CALL_LOG_ENTER;
    const std::string DEFAULT_SEAT_NAME = "default0";
    auto iter = inputDeviceSeat_.find(deviceId);
    if (iter == inputDeviceSeat_.end()) {
        MMI_HILOGE("find input device id fail. return default seatName:%{public}s", DEFAULT_SEAT_NAME.c_str());
        return DEFAULT_SEAT_NAME;
    }
    return iter->second;
}

void InputDeviceManager::SetLastTouchDeviceId(int32_t lastTouchDeviceID)
{
    lastTouchDeviceID_ = lastTouchDeviceID;
}

int32_t InputDeviceManager::SetInputDeviceSeatName(const std::string& seatName, DeviceUniqId& deviceUniqId)
{
    auto device = inputDevice_.find(lastTouchDeviceID_);
    if (device == inputDevice_.end()) {
        MMI_HILOGE("device does not exist");
        return RET_ERR;
    }
    inputDeviceSeat_[lastTouchDeviceID_] = seatName;
    auto bus = libinput_device_get_id_bustype(device->second);
    auto version = libinput_device_get_id_version(device->second);
    auto product = libinput_device_get_id_product(device->second);
    auto vendor = libinput_device_get_id_vendor(device->second);
    auto udevTags = libinput_device_get_tags(device->second);
    auto deviceUniq = libinput_device_get_uniq(device->second);
    auto uniq = (deviceUniq == nullptr) ? ("null") : (deviceUniq);
    deviceUniqId = std::make_tuple(bus, version, product, vendor, udevTags, uniq);
    return RET_OK;
}

#ifdef OHOS_DISTRIBUTED_INPUT_MODEL
bool InputDeviceManager::IsRemote(struct libinput_device* inputDevice) const
{
    CHKPR(inputDevice, false);
    bool isRemote = false;
    std::string name = libinput_device_get_name(inputDevice);
    std::string::size_type pos = name.find(VIRTUAL_DEVICE_NAME);
    if (pos != name.npos) {
        isRemote = true;
    }
    MMI_HILOGD("isRemote:%{public}s", isRemote == true ? "true" : "false");
    return isRemote;
}

std::shared_ptr<InputDevice> InputDeviceManager::GetRemoteInputDevice(int32_t id)
{
    CALL_LOG_ENTER;
    auto item = inputDevice_.find(id);
    if (item == inputDevice_.end()) {
        MMI_HILOGW("failed to search for the device");
        return nullptr;
    }
    if (!IsRemote(item->second)) {
        return nullptr;
    }
    std::shared_ptr<InputDevice> inputDevice = MakeInputDevice(item->first, item->second);
    return inputDevice;
}

bool InputDeviceManager::IsDistributedInput(struct libinput_device* device) const
{
    CHKPF(device);
    enum evdev_device_udev_tags udevTags = libinput_device_get_tags(device);
    uint32_t dinputType = DeviceUdevTagsToDinputType(udevTags);
    if (dinputType != 0) {
        return DInputMgr->IsDistributedInput(dinputType);
    }
    return false;
}

uint32_t InputDeviceManager::DeviceUdevTagsToDinputType(enum evdev_device_udev_tags udevTags) const
{
    CALL_LOG_ENTER;
    MMI_HILOGD("udev tag:%{public}d", static_cast<int32_t>(udevTags));
    if (udevTags & EVDEV_UDEV_TAG_MOUSE) {
        return MOUSE_ABILITY;
    } else if (udevTags & EVDEV_UDEV_TAG_KEYBOARD) {
        return KEYBOARD_ABILITY;
    }
    return DEFAULT_ABILITY;
}

void InputDeviceManager::OnStartRemoteInput(const std::string& deviceId, const uint32_t& inputTypes)
{
    NotifyDeviceChanged(deviceId, inputTypes, DEVICE_ADD);
}

void InputDeviceManager::OnStopRemoteInput(const std::string& deviceId, const uint32_t& inputTypes)
{
    NotifyDeviceChanged(deviceId, inputTypes, DEVICE_REMOVE);
}

void InputDeviceManager::NotifyDeviceChanged(const std::string& deviceId,
    const uint32_t& inputTypes, const std::string& changedType)
{
    CALL_LOG_ENTER;
    bool hasLocalPointer = false;
    for (const auto& item : inputDevice_) {
        if (!IsRemote(item.second)) {
            hasLocalPointer = IsPointerDevice(item.second);
            continue;
        }
        if (MOUSE_ABILITY & inputTypes) {
            if ((changedType == DEVICE_REMOVE) && !hasLocalPointer) {
                SetPointerVisible(getpid(), hasLocalPointer);
            }
        }
    }
}

void InputDeviceManager::Dump(int32_t fd, const std::vector<std::string> &args)
{
    CALL_LOG_ENTER;
    mprintf(fd, "Device information:\t");
    mprintf(fd, "Input devices: count=%d", inputDevice_.size());
    for (const auto &item : inputDevice_) {
        std::shared_ptr<InputDevice> inputDevice = GetInputDevice(item.first);
        CHKPV(inputDevice);
        mprintf(fd,
                "deviceId:%d | deviceName:%s | deviceType:%d | bus:%d | version:%d "
                "| product:%d | vendor:%d | phys:%s\t",
                inputDevice->GetId(), inputDevice->GetName().c_str(), inputDevice->GetType(),
                inputDevice->GetBustype(), inputDevice->GetVersion(), inputDevice->GetProduct(),
                inputDevice->GetVendor(), inputDevice->GetPhys().c_str());
        std::vector<InputDevice::AxisInfo> axisinfo = inputDevice->GetAxisInfo();
        mprintf(fd, "axis: count=%d", axisinfo.size());
        for (const auto &axis : axisinfo) {
            auto iter = axisType.find(axis.GetAxisType());
            if (iter == axisType.end()) {
                MMI_HILOGE("AxisType is not found");
                return;
            }
            mprintf(fd,
                    "\t axisType:%s | minimum:%d | maximum:%d | fuzz:%d | flat:%d | resolution:%d\t",
                    iter->second.c_str(), axis.GetMinimum(), axis.GetMaximum(), axis.GetFuzz(),
                    axis.GetFlat(), axis.GetResolution());
        }
    }
}

void InputDeviceManager::SetPointerVisible(int32_t pid, bool visible)
{
    MMI_HILOGW("observers_ size:%{public}zu", observers_.size());
    for (auto observer = observers_.begin(); observer != observers_.end(); observer++) {
        CHKPV(*observer);
        (*observer)->SetPointerVisible(pid, visible);
    }
}

int32_t InputDeviceManager::GetRemoteInputAbility(std::string deviceId, sptr<ICallDinput> ablitity)
{
    MMI_HILOGD("deviceId:%{public}s", GetAnonyString(deviceId).c_str());
    std::set<int32_t> remotInputAbility;
    for (const auto& item : inputDevice_) {
        if (IsRemote(item.second)) {
            auto phys = libinput_device_get_phys(item.second);
            std::string networkId = MakeNetworkId(phys);
            if (networkId.compare(deviceId) == 0) {
                int32_t type = static_cast<int32_t>(libinput_device_get_tags(item.second));
                MMI_HILOGD("type:%{public}d", type);
                remotInputAbility.insert(type);
            }
        }
    }
    CHKPR(ablitity, ERROR_NULL_POINTER);
    return ablitity->HandleRemoteInputAbility(remotInputAbility);
}

void InputDeviceManager::OnDeviceOffline()
{
    bool hasLocalPointer = false;
    for (const auto& item : inputDevice_) {
        if (IsPointerDevice(item.second) && !IsRemote(item.second)) {
            hasLocalPointer = true;
            break;
        }
    }
    SetPointerVisible(getpid(), hasLocalPointer);
}

bool InputDeviceManager::InitDeviceManager()
{
    CALL_LOG_ENTER;
    initCallback_ = std::make_shared<DeviceInitCallBack>();
    int32_t ret =
        DistributedHardware::DeviceManager::GetInstance().InitDeviceManager(BUNDLE_NAME, initCallback_);
    if (ret != 0) {
        MMI_HILOGW("init device manager failed, ret:%{public}d", ret);
        return false;
    }
    stateCallback_ = std::make_shared<MmiDeviceStateCallback>();
    ret =
        DistributedHardware::DeviceManager::GetInstance().RegisterDevStateCallback(BUNDLE_NAME, "", stateCallback_);
    if (ret != 0) {
        MMI_HILOGW("register devStateCallback failed, ret:%{public}d", ret);
        return false;
    }
    MMI_HILOGD("register device manager success");
    return true;
}

void InputDeviceManager::DeviceInitCallBack::OnRemoteDied()
{
    CALL_LOG_ENTER;
}

void InputDeviceManager::MmiDeviceStateCallback::OnDeviceOnline(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    CALL_LOG_ENTER;
}

void InputDeviceManager::MmiDeviceStateCallback::OnDeviceOffline(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    CALL_LOG_ENTER;
    InputDevMgr->OnDeviceOffline();
}

void InputDeviceManager::MmiDeviceStateCallback::OnDeviceChanged(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    CALL_LOG_ENTER;
}

void InputDeviceManager::MmiDeviceStateCallback::OnDeviceReady(const DistributedHardware::DmDeviceInfo &deviceInfo)
{
    CALL_LOG_ENTER;
}
#endif // OHOS_DISTRIBUTED_INPUT_MODEL

void InputDeviceManager::DumpDeviceList(int32_t fd, const std::vector<std::string> &args)
{
    CALL_LOG_ENTER;
    std::vector<int32_t> ids = GetInputDeviceIds();
    mprintf(fd, "Total device:%d, Device list:\t", int32_t { ids.size() });
    for (const auto &item : inputDevice_) {
        std::shared_ptr<InputDevice> inputDevice = GetInputDevice(item.first);
        CHKPV(inputDevice);
        int32_t deviceId = inputDevice->GetId();
        mprintf(fd,
                "deviceId:%d | deviceName:%s | deviceType:%d | bus:%d | version:%d | product:%d | vendor:%d\t",
                deviceId, inputDevice->GetName().c_str(), inputDevice->GetType(), inputDevice->GetBustype(),
                inputDevice->GetVersion(), inputDevice->GetProduct(), inputDevice->GetVendor());
    }
}
} // namespace MMI
} // namespace OHOS
