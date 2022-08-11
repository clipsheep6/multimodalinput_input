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
#ifdef OHOS_BUILD_ENABLE_COOPERATE
#include <openssl/sha.h>
#endif // OHOS_BUILD_ENABLE_COOPERATE
#include <unordered_map>

#include "dfx_hisysevent.h"
#ifdef OHOS_BUILD_ENABLE_COOPERATE
#include "input_device_cooperate_sm.h"
#endif // OHOS_BUILD_ENABLE_COOPERATE
#include "input_windows_manager.h"
#include "key_event_value_transformation.h"
#ifdef OHOS_BUILD_ENABLE_COOPERATE
#include "softbus_bus_center.h"
#endif // OHOS_BUILD_ENABLE_COOPERATE
#include "util_ex.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceManager"};
constexpr int32_t INVALID_DEVICE_ID = -1;
constexpr int32_t SUPPORT_KEY = 1;

constexpr int32_t ABS_MT_TOUCH_MAJOR = 0x30;
constexpr int32_t ABS_MT_TOUCH_MINOR = 0x31;
constexpr int32_t ABS_MT_ORIENTATION = 0x34;
constexpr int32_t ABS_MT_POSITION_X  = 0x35;
constexpr int32_t ABS_MT_POSITION_Y = 0x36;
constexpr int32_t ABS_MT_PRESSURE = 0x3a;
constexpr int32_t ABS_MT_WIDTH_MAJOR = 0x32;
constexpr int32_t ABS_MT_WIDTH_MINOR = 0x33;

constexpr int32_t BUS_BLUETOOTH = 0X5;
#ifdef OHOS_BUILD_ENABLE_COOPERATE
const char *SPLIT_SYMBOL = "|";
const std::string BUNDLE_NAME = "DBinderBus_" + std::to_string(getpid());
const std::string UNKNOWN_SCREEN_ID = "";
const std::string DH_ID_PREFIX = "Input_";
#endif // OHOS_BUILD_ENABLE_COOPERATE

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
    CALL_DEBUG_ENTER;
    auto iter = inputDevice_.find(id);
    if (iter == inputDevice_.end()) {
        MMI_HILOGE("Failed to search for the device");
        return nullptr;
    }

    std::shared_ptr<InputDevice> inputDevice = std::make_shared<InputDevice>();
    CHKPP(inputDevice);
    inputDevice->SetId(iter->first);
    auto inputDeviceOrgni = iter->second.inputDeviceOrgin_;
    inputDevice->SetType(static_cast<int32_t>(libinput_device_get_tags(inputDeviceOrgni)));
    const char* name = libinput_device_get_name(inputDeviceOrgni);
    inputDevice->SetName((name == nullptr) ? ("null") : (name));
    inputDevice->SetBus(libinput_device_get_id_bustype(inputDeviceOrgni));
    inputDevice->SetVersion(libinput_device_get_id_version(inputDeviceOrgni));
    inputDevice->SetProduct(libinput_device_get_id_product(inputDeviceOrgni));
    inputDevice->SetVendor(libinput_device_get_id_vendor(inputDeviceOrgni));
    const char* phys = libinput_device_get_phys(inputDeviceOrgni);
    inputDevice->SetPhys((phys == nullptr) ? ("null") : (phys));
    const char* uniq = libinput_device_get_uniq(inputDeviceOrgni);
    inputDevice->SetUniq((uniq == nullptr) ? ("null") : (uniq));

    InputDevice::AxisInfo axis;
    for (const auto &item : axisType) {
        int32_t min = libinput_device_get_axis_min(inputDeviceOrgni, item.first);
        if (min == -1) {
            MMI_HILOGD("The device does not support this axis");
            continue;
        }
        if (item.first == ABS_MT_PRESSURE) {
            axis.SetMinimum(0);
            axis.SetMaximum(1);
        } else {
            axis.SetMinimum(min);
            axis.SetMaximum(libinput_device_get_axis_max(inputDeviceOrgni, item.first));
        }
        axis.SetAxisType(item.first);
        axis.SetFuzz(libinput_device_get_axis_fuzz(inputDeviceOrgni, item.first));
        axis.SetFlat(libinput_device_get_axis_flat(inputDeviceOrgni, item.first));
        axis.SetResolution(libinput_device_get_axis_resolution(inputDeviceOrgni, item.first));
        inputDevice->AddAxisInfo(axis);
    }
    return inputDevice;
}

std::vector<int32_t> InputDeviceManager::GetInputDeviceIds() const
{
    CALL_DEBUG_ENTER;
    std::vector<int32_t> ids;
    for (const auto &item : inputDevice_) {
        ids.push_back(item.first);
    }
    return ids;
}

std::vector<bool> InputDeviceManager::SupportKeys(int32_t deviceId, std::vector<int32_t> &keyCodes)
{
    CALL_DEBUG_ENTER;
    std::vector<bool> keystrokeAbility;
    auto iter = inputDevice_.find(deviceId);
    if (iter == inputDevice_.end()) {
        keystrokeAbility.insert(keystrokeAbility.end(), keyCodes.size(), false);
        return keystrokeAbility;
    }
    for (const auto& item : keyCodes) {
        bool ret = false;
        for (const auto &it : KeyMapMgr->InputTransferKeyValue(deviceId, item)) {
            ret |= libinput_device_has_key(iter->second.inputDeviceOrgin_, it) == SUPPORT_KEY;
        }
        keystrokeAbility.push_back(ret);
    }
    return keystrokeAbility;
}

bool InputDeviceManager::GetDeviceConfig(int32_t deviceId, int32_t &keyboardType)
{
    CALL_DEBUG_ENTER;
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
    CALL_DEBUG_ENTER;
    std::shared_ptr dev = GetInputDevice(deviceId);
    CHKPR(dev, ERROR_NULL_POINTER);
    return dev->GetBus();
}

int32_t InputDeviceManager::GetDeviceSupportKey(int32_t deviceId)
{
    CALL_DEBUG_ENTER;
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
    CALL_DEBUG_ENTER;
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

void InputDeviceManager::AddDevListener(SessionPtr sess, std::function<void(int32_t, const std::string&)> callback)
{
    CALL_DEBUG_ENTER;
    auto ret = devListener_.insert({ sess, callback });
    if (!ret.second) {
        MMI_HILOGE("Session is duplicated");
        return;
    }
}

void InputDeviceManager::RemoveDevListener(SessionPtr sess)
{
    CALL_DEBUG_ENTER;
    auto iter = devListener_.find(sess);
    if (iter == devListener_.end()) {
        MMI_HILOGE("Session does not exist");
        return;
    }
    devListener_.erase(iter);
}

#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
bool InputDeviceManager::HasPointerDevice()
{
    for (auto it = inputDevice_.begin(); it != inputDevice_.end(); ++it) {
        if (IsPointerDevice(it->second.inputDeviceOrgin_)) {
            return true;
        }
    }
    return false;
}
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING

void InputDeviceManager::OnInputDeviceAdded(struct libinput_device *inputDevice)
{
    CALL_DEBUG_ENTER;
    CHKPV(inputDevice);
    bool hasLocalPointer = false;
    for (const auto& item : inputDevice_) {
        if (item.second.inputDeviceOrgin_ == inputDevice) {
            MMI_HILOGI("The device is already existent");
            DfxHisysevent::OnDeviceConnect(item.first, OHOS::HiviewDFX::HiSysEvent::EventType::FAULT);
            return;
        }
        if (!item.second.isRemote_ && IsPointerDevice(item.second.inputDeviceOrgin_)) {
            hasLocalPointer = true;
        }
    }
    if (nextId_ == INT32_MAX) {
        MMI_HILOGE("The nextId_ exceeded the upper limit");
        DfxHisysevent::OnDeviceConnect(INT32_MAX, OHOS::HiviewDFX::HiSysEvent::EventType::FAULT);
        return;
    }
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    if (IsPointerDevice(inputDevice) && !HasPointerDevice()) {
#ifdef OHOS_BUILD_ENABLE_POINTER
        WinMgr->DispatchPointer(PointerEvent::POINTER_ACTION_ENTER_WINDOW);
#endif // OHOS_BUILD_ENABLE_POINTER
    }
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING
    struct InputDeviceObj obj;
    MakeInputDeviceObj(obj, inputDevice);
    inputDevice_[nextId_] = obj;
    for (const auto &item : devListener_) {
        CHKPC(item.first);
        item.second(nextId_, "add");
    }
    ++nextId_;
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    if (IsKeyboard(inputDevice)) {
        InputDevCooSM->OnKeyboardOnline(obj.dhid_);
    }
#endif
    if (IsPointerDevice(inputDevice)) {
        bool visible = !obj.isRemote_ || hasLocalPointer;
        NotifyPointerDevice(true, visible);
        OHOS::system::SetParameter(INPUT_POINTER_DEVICE, "true");
        MMI_HILOGI("Set para input.pointer.device true");
    }
    DfxHisysevent::OnDeviceConnect(nextId_ - 1, OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR);
}

void InputDeviceManager::MakeInputDeviceObj(struct InputDeviceObj &obj, struct libinput_device *inputDevice)
{
    obj.inputDeviceOrgin_ = inputDevice;
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    obj.isRemote_ = IsRemote(inputDevice);
    if (obj.isRemote_) {
        obj.networkIdOrgin_ = MakeNetworkId(libinput_device_get_phys(inputDevice));
    }
    obj.fd_ = libinput_device_get_fd(inputDevice);
    MMI_HILOGE("networkIdOrgin: %{public}s, fd:%{public}d", obj.networkIdOrgin_.c_str(), obj.fd_);
    obj.dhid_ = GenerateDescriptor(inputDevice, obj.isRemote_);
#endif // OHOS_BUILD_ENABLE_COOPERATE
}

void InputDeviceManager::OnInputDeviceRemoved(struct libinput_device *inputDevice)
{
    CALL_DEBUG_ENTER;
    CHKPV(inputDevice);
    int32_t deviceId = INVALID_DEVICE_ID;
    std::string removeDhid = "";
    for (auto it = inputDevice_.begin(); it != inputDevice_.end(); ++it) {
        if (it->second.inputDeviceOrgin_ == inputDevice) {
            deviceId = it->first;
            removeDhid = it->second.dhid_;
            DfxHisysevent::OnDeviceDisconnect(deviceId, OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR);
            inputDevice_.erase(it);
            break;
        }
    }
#ifdef OHOS_BUILD_ENABLE_POINTER_DRAWING
    if (IsPointerDevice(inputDevice) && !HasPointerDevice()) {
#ifdef OHOS_BUILD_ENABLE_POINTER
        WinMgr->DispatchPointer(PointerEvent::POINTER_ACTION_LEAVE_WINDOW);
#endif // OHOS_BUILD_ENABLE_POINTER
    }
#endif // OHOS_BUILD_ENABLE_POINTER_DRAWING
#ifdef OHOS_BUILD_ENABLE_COOPERATE
    if (IsPointerDevice(inputDevice)) {
        InputDevCooSM->OnPointerOffline(removeDhid);
    }
#endif // OHOS_BUILD_ENABLE_COOPERATE
    for (const auto &item : devListener_) {
        CHKPC(item.first);
        item.second(deviceId, "remove");
    }
    ScanPointerDevice();
    if (deviceId == INVALID_DEVICE_ID) {
        DfxHisysevent::OnDeviceDisconnect(INVALID_DEVICE_ID, OHOS::HiviewDFX::HiSysEvent::EventType::FAULT);
    }
}

void InputDeviceManager::ScanPointerDevice()
{
    bool hasPointerDevice = false;
    for (auto it = inputDevice_.begin(); it != inputDevice_.end(); ++it) {
        if (IsPointerDevice(it->second.inputDeviceOrgin_)) {
            hasPointerDevice = true;
            break;
        }
    }
    if (!hasPointerDevice) {
        NotifyPointerDevice(false, false);
        OHOS::system::SetParameter(INPUT_POINTER_DEVICE, "false");
        MMI_HILOGI("Set para input.pointer.device false");
    }
}

bool InputDeviceManager::IsPointerDevice(struct libinput_device* device) const
{
    CHKPF(device);
    enum evdev_device_udev_tags udevTags = libinput_device_get_tags(device);
    MMI_HILOGD("udev tag:%{public}d", static_cast<int32_t>(udevTags));
    return udevTags & (EVDEV_UDEV_TAG_MOUSE | EVDEV_UDEV_TAG_TRACKBALL | EVDEV_UDEV_TAG_POINTINGSTICK |
    EVDEV_UDEV_TAG_TOUCHPAD | EVDEV_UDEV_TAG_TABLET_PAD);
}

void InputDeviceManager::Attach(std::shared_ptr<IDeviceObserver> observer)
{
    CALL_DEBUG_ENTER;
    observers_.push_back(observer);
}

void InputDeviceManager::Detach(std::shared_ptr<IDeviceObserver> observer)
{
    CALL_DEBUG_ENTER;
    observers_.remove(observer);
}

void InputDeviceManager::NotifyPointerDevice(bool hasPointerDevice, bool isVisible)
{
    MMI_HILOGI("observers_ size:%{public}zu", observers_.size());
    for (auto observer = observers_.begin(); observer != observers_.end(); observer++) {
        (*observer)->UpdatePointerDevice(hasPointerDevice, isVisible);
    }
}

int32_t InputDeviceManager::FindInputDeviceId(struct libinput_device* inputDevice)
{
    CALL_DEBUG_ENTER;
    CHKPR(inputDevice, INVALID_DEVICE_ID);
    for (const auto& item : inputDevice_) {
        if (item.second.inputDeviceOrgin_ == inputDevice) {
            MMI_HILOGI("Find input device id success");
            return item.first;
        }
    }
    MMI_HILOGE("Find input device id failed");
    return INVALID_DEVICE_ID;
}

void InputDeviceManager::Dump(int32_t fd, const std::vector<std::string> &args)
{
    CALL_DEBUG_ENTER;
    mprintf(fd, "Device information:\t");
    mprintf(fd, "Input devices: count=%d", inputDevice_.size());
    for (const auto &item : inputDevice_) {
        std::shared_ptr<InputDevice> inputDevice = GetInputDevice(item.first);
        CHKPV(inputDevice);
        mprintf(fd,
                "deviceId:%d | deviceName:%s | deviceType:%d | bus:%d | version:%d "
                "| product:%d | vendor:%d | phys:%s\t",
                inputDevice->GetId(), inputDevice->GetName().c_str(), inputDevice->GetType(),
                inputDevice->GetBus(), inputDevice->GetVersion(), inputDevice->GetProduct(),
                inputDevice->GetVendor(), inputDevice->GetPhys().c_str());
        std::vector<InputDevice::AxisInfo> axisinfo = inputDevice->GetAxisInfo();
        mprintf(fd, "axis: count=%d", axisinfo.size());
        for (const auto &axis : axisinfo) {
            auto iter = axisType.find(axis.GetAxisType());
            if (iter == axisType.end()) {
                MMI_HILOGE("The axisType is not found");
                return;
            }
            mprintf(fd,
                    "\t axisType:%s | minimum:%d | maximum:%d | fuzz:%d | flat:%d | resolution:%d\t",
                    iter->second.c_str(), axis.GetMinimum(), axis.GetMaximum(), axis.GetFuzz(),
                    axis.GetFlat(), axis.GetResolution());
        }
    }
}

void InputDeviceManager::DumpDeviceList(int32_t fd, const std::vector<std::string> &args)
{
    CALL_DEBUG_ENTER;
    std::vector<int32_t> ids = GetInputDeviceIds();
    mprintf(fd, "Total device:%d, Device list:\t", int32_t { ids.size() });
    for (const auto &item : inputDevice_) {
        std::shared_ptr<InputDevice> inputDevice = GetInputDevice(item.first);
        CHKPV(inputDevice);
        int32_t deviceId = inputDevice->GetId();
        mprintf(fd,
                "deviceId:%d | deviceName:%s | deviceType:%d | bus:%d | version:%d | product:%d | vendor:%d\t",
                deviceId, inputDevice->GetName().c_str(), inputDevice->GetType(), inputDevice->GetBus(),
                inputDevice->GetVersion(), inputDevice->GetProduct(), inputDevice->GetVendor());
    }
}

#ifdef OHOS_BUILD_ENABLE_COOPERATE
std::vector<int32_t> InputDeviceManager::GetPointerKeyboardFds(int32_t pointerId)
{
    std::vector<int32_t> fds;
    auto iter = inputDevice_.find(pointerId);
    if (iter == inputDevice_.end()) {
        return fds;
    }
    if (!IsPointerDevice(iter->second.inputDeviceOrgin_)) {
        MMI_HILOGI("input pointerId not pointer!!!");
        return fds;
    }

    fds.push_back(iter->second.fd_);
    MMI_HILOGI("fd: %{public}d, type:%{public}s", fds.back(), "pointer");
    auto pointerNetworkId = iter->second.networkIdOrgin_;
    bool isRemote_ = iter->second.isRemote_;
    if (!isRemote_) {
        GetLocalDeviceId(pointerNetworkId);
    }
    for (const auto &item : inputDevice_) {
        auto networkId = item.second.networkIdOrgin_;
        if (!item.second.isRemote_) {
            GetLocalDeviceId(networkId);
        }
        if (networkId.compare(pointerNetworkId) != 0) {
            continue;
        }
        if (IsKeyboard(item.second.inputDeviceOrgin_)) {
            fds.push_back(item.second.fd_);
            MMI_HILOGI("fd: %{public}d, type:%{public}s", fds.back(), "keyboard");
        }
    }
    return fds;
}

std::string InputDeviceManager::GetOrginNetworkId(int32_t id)
{
    auto iter = inputDevice_.find(id);
    if (iter == inputDevice_.end()) {
        MMI_HILOGE("failed to search for the device: id %{public}d", id);
        return "";
    }
    auto networkId = iter->second.networkIdOrgin_;
    if (networkId.empty()) {
        GetLocalDeviceId(networkId);
    }
    return networkId;
}

std::string InputDeviceManager::GetOrginNetworkId(const std::string& dhid)
{
    if (dhid.empty()) {
        return "";
    }

    std::string networkId = "";
    for (auto iter : inputDevice_) {
        if (iter.second.dhid_.compare(dhid) == 0) {
            networkId = iter.second.networkIdOrgin_;
            if (networkId.empty()) {
                GetLocalDeviceId(networkId);
            }
        }
    }
    return networkId;
}

void InputDeviceManager::GetLocalDeviceId(std::string& networkId)
{
    auto localNode = std::make_unique<NodeBasicInfo>();
    int32_t errCode = GetLocalNodeDeviceInfo(BUNDLE_NAME.c_str(), localNode.get());
    if (errCode != RET_OK) {
        MMI_HILOGE("GetLocalNodeDeviceInfo errCode = %{public}d", errCode);
        networkId = "";
    }
    networkId = localNode->networkId;
}

std::string InputDeviceManager::GetDhid(int32_t deviceId)
{
    auto dev = inputDevice_.find(deviceId);
    if (dev != inputDevice_.end()) {
        return dev->second.dhid_;
    }
    return "";
}

std::vector<std::string> InputDeviceManager::GetPointerKeyboardUnqs(int32_t pointerId)
{
    std::vector<std::string> unqs;
    auto iter = inputDevice_.find(pointerId);
    if (iter == inputDevice_.end()) {
        return unqs;
    }
    if (!IsPointerDevice(iter->second.inputDeviceOrgin_)) {
        return unqs;
    }
    unqs.push_back(iter->second.dhid_);
    MMI_HILOGI("unq: %{public}s, type:%{public}s", unqs.back().c_str(), "pointer");
    auto pointerNetworkId = iter->second.networkIdOrgin_;
    if (!iter->second.isRemote_) {
        GetLocalDeviceId(pointerNetworkId);
    }
    for (const auto &item : inputDevice_) {
        auto networkId = item.second.networkIdOrgin_;
        if (!item.second.isRemote_) {
            GetLocalDeviceId(networkId);
        }
        if (networkId.compare(pointerNetworkId) != 0) {
            continue;
        }
        if (KEYBOARD_TYPE_ALPHABETICKEYBOARD == GetDeviceSupportKey(item.first)) {
            unqs.push_back(item.second.dhid_);
            MMI_HILOGI("unq: %{public}s, type:%{public}s", unqs.back().c_str(), "supportkey");
        }
    }
    return unqs;
}

std::vector<std::string> InputDeviceManager::GetPointerKeyboardUnqs(const std::string& dhid)
{
    int32_t pointerId = -1;
    for (const auto &iter : inputDevice_) {
        if (iter.second.dhid_.compare(dhid) == 0) {
            pointerId = iter.first;
        }
    }
    return GetPointerKeyboardUnqs(pointerId);
}

bool InputDeviceManager::HasLocalPointerDevice() const
{
    for (auto it = inputDevice_.begin(); it != inputDevice_.end(); ++it) {
        if (!it->second.isRemote_ && IsPointerDevice(it->second.inputDeviceOrgin_)) {
            return true;
        }
    }
    return false;
}

bool InputDeviceManager::IsRemote(struct libinput_device* inputDevice) const
{
    CHKPR(inputDevice, false);
    bool isRemote = false;
    const char* name = libinput_device_get_name(inputDevice);
    if (name == nullptr || name[0] == '\0') {
        return false;
    }
    std::string strName = name;
    std::string::size_type pos = strName.find(VIRTUAL_DEVICE_NAME);
    if (pos != strName.npos) {
        isRemote = true;
    }
    MMI_HILOGD("isRemote = %{public}s", isRemote == true ? "true" : "false");
    return isRemote;
}

bool InputDeviceManager::IsRemote(int32_t id) const
{
    bool isRemote = false;
    auto device = inputDevice_.find(id);
    if (device != inputDevice_.end()) {
        isRemote = device->second.isRemote_;
    }
    MMI_HILOGD("isRemote = %{public}s", isRemote == true ? "true" : "false");
    return isRemote;
}

std::string InputDeviceManager::MakeNetworkId(const char* phys) const
{
    std::string networkId = "";
    if (phys == nullptr || phys[0] == '\0') {
        return networkId;
    }
    std::string strPhys = phys;
    std::vector<std::string> strList = Split(strPhys, SPLIT_SYMBOL);
    if (strList.size() == 3) {
        networkId = strList[1];
    }
    return networkId;
}

bool InputDeviceManager::IsKeyboard(struct libinput_device* device) const
{
    CHKPF(device);
    enum evdev_device_udev_tags udevTags = libinput_device_get_tags(device);
    MMI_HILOGD("udev tag:%{public}d", static_cast<int32_t>(udevTags));
    return udevTags & EVDEV_UDEV_TAG_KEYBOARD;
}

std::string InputDeviceManager::StringPrintf(const char *format, ...) const
{
    static const int kSpaceLength = 1024;
    char space[kSpaceLength];

    va_list ap;
    va_start(ap, format);
    std::string result;
    int32_t ret = vsnprintf_s(space, sizeof(space), sizeof(space) - 1, format, ap);
    if (ret >= RET_OK && (size_t)ret < sizeof(space)) {
        result = space;
    } else {
        return "the buffer is overflow!";
    }
    va_end(ap);
    return result;
}

std::string InputDeviceManager::Sha256(const std::string &in) const
{
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, reinterpret_cast<const u_char *>(in.c_str()), in.size());
    u_char digest[SHA_DIGEST_LENGTH];
    SHA256_Final(digest, &ctx);

    std::string out;
    for (size_t i = 0; i < SHA_DIGEST_LENGTH; i++) {
        out += StringPrintf("%02x", digest[i]);
    }
    return out;
}

std::string InputDeviceManager::GenerateDescriptor(struct libinput_device *inputDevice, bool isRemote) const
{
    std::string descriptor = "";
    const char*  location = libinput_device_get_phys(inputDevice);
    if (isRemote && location != nullptr) {
        MMI_HILOGI("isRemote location: %{public}s", location);
        std::vector<std::string> strList = Split(location, SPLIT_SYMBOL);
        if (strList.size() == 3) {
            descriptor = strList[2];
        }
        return descriptor;
    }

    uint16_t vendor = libinput_device_get_id_vendor(inputDevice);
    const char* name = libinput_device_get_name(inputDevice);
    const char* uniqueId = libinput_device_get_uniq(inputDevice);
    uint16_t product = libinput_device_get_id_product(inputDevice);
    std::string rawDescriptor;
    rawDescriptor += StringPrintf(":%04x:%04x:", vendor, product);
    // add handling for USB devices to not uniqueify kbs that show up twice
    if (uniqueId != nullptr && uniqueId[0] != '\0') {
        rawDescriptor += "uniqueId:";
        rawDescriptor += uniqueId;
    } else if (location != nullptr) {
        rawDescriptor += "location:";
        rawDescriptor += location;
    }

    if (vendor == 0 && product == 0) {
        // If we don't know the vendor and product id, then the device is probably
        // built-in so we need to rely on other information to uniquely identify
        // the input device.  Usually we try to avoid relying on the device name
        // but for built-in input device, they are unlikely to ever change.
        if (name != nullptr && name[0] != '\0') {
            rawDescriptor += "name:";
            rawDescriptor += name;
        }
    }
    descriptor = DH_ID_PREFIX + Sha256(rawDescriptor);
    MMI_HILOGE("Created descriptor: raw=%{public}s, cooked=%{public}s", rawDescriptor.c_str(), descriptor.c_str());
    return descriptor;
}

std::vector<std::string> InputDeviceManager::Split(std::string str, std::string pattern) const
{
    int32_t position;
    std::vector<std::string> result;
    str += pattern;
    int32_t length = static_cast<int32_t>(str.size());
    for (int32_t i = 0; i < length; i++) {
        position = static_cast<int32_t>(str.find(pattern, i));
        if (position < length) {
            std::string tmp = str.substr(i, position - i);
            result.push_back(tmp);
            i = position + static_cast<int32_t>(pattern.size()) - 1;
        }
    }
    return result;
}
#endif // OHOS_BUILD_ENABLE_COOPERATE
} // namespace MMI
} // namespace OHOS
