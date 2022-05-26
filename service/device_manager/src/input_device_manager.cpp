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
#ifdef OHOS_BUILD_KEY_MOUSE
#include "constants_dinput.h"
#include "device_manager.h"
#include "dinput_manager.h"
#include "system_ability_definition.h"
#endif
#include "key_event_value_transformation.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceManager" };
constexpr int32_t INVALID_DEVICE_ID = -1;
constexpr int32_t SUPPORT_KEY = 1;
#ifdef OHOS_BUILD_KEY_MOUSE
const char *SPLIT_SYMBOL = "|";
#endif
// constexpr int32_t READ_CMD_BUFF_SIZE = 1024;

constexpr int32_t ABS_MT_TOUCH_MAJOR = 0x30;
constexpr int32_t ABS_MT_TOUCH_MINOR = 0x31;
constexpr int32_t ABS_MT_ORIENTATION = 0x34;
constexpr int32_t ABS_MT_POSITION_X  = 0x35;
constexpr int32_t ABS_MT_POSITION_Y = 0x36;
constexpr int32_t ABS_MT_PRESSURE = 0x3a;
constexpr int32_t ABS_MT_WIDTH_MAJOR = 0x32;
constexpr int32_t ABS_MT_WIDTH_MINOR = 0x33;

#ifdef OHOS_BUILD_KEY_MOUSE
const std::string BUNDLE_NAME = "ohos.multimodalinput.input";
const std::string DEVICE_ADD = "add";
const std::string DEVICE_REMOVE = "remove";
#endif

std::list<int32_t> axisType = {
    ABS_MT_TOUCH_MAJOR,
    ABS_MT_TOUCH_MINOR,
    ABS_MT_ORIENTATION,
    ABS_MT_POSITION_X,
    ABS_MT_POSITION_Y,
    ABS_MT_PRESSURE,
    ABS_MT_WIDTH_MAJOR,
    ABS_MT_WIDTH_MINOR,
};
} // namespace

#ifdef OHOS_BUILD_KEY_MOUSE
std::shared_ptr<InputDevice> InputDeviceManager::GetInputDevice(int32_t id) const
{
    CALL_LOG_ENTER;
    auto iter = inputDevice_.find(id);
    if (iter == inputDevice_.end()) {
        MMI_HILOGE("failed to search for the device");
        return nullptr;
    }
    if (IsRemote(iter->second) && !IsDistributedInput(iter->second)) {
        return nullptr;
    }
    std::shared_ptr<InputDevice> inputDevice = MakeInputDevice(iter->first, iter->second);
    return inputDevice;
}
#else
std::shared_ptr<InputDevice> InputDeviceManager::GetInputDevice(int32_t id) const
{
    CALL_LOG_ENTER;
    auto iter = inputDevice_.find(id);
    if (iter == inputDevice_.end()) {
        MMI_HILOGE("failed to search for the device");
        return nullptr;
    }

    std::shared_ptr<InputDevice> inputDevice = std::make_shared<InputDevice>();
    CHKPP(inputDevice);
    inputDevice->SetId(iter->first);
    inputDevice->SetType(static_cast<int32_t>(libinput_device_get_tags(iter->second)));
    auto name = libinput_device_get_name(iter->second);
    inputDevice->SetName((name == nullptr) ? ("null") : (name));
    inputDevice->SetBustype(libinput_device_get_id_bustype(iter->second));
    inputDevice->SetVersion(libinput_device_get_id_version(iter->second));
    inputDevice->SetProduct(libinput_device_get_id_product(iter->second));
    inputDevice->SetVendor(libinput_device_get_id_vendor(iter->second));
    auto phys = libinput_device_get_phys(iter->second);
    inputDevice->SetPhys((phys == nullptr) ? ("null") : (phys));
    auto uniq = libinput_device_get_uniq(iter->second);
    inputDevice->SetUniq((uniq == nullptr) ? ("null") : (uniq));

    InputDevice::AxisInfo axis;
    for (const auto &item : axisType) {
        auto min = libinput_device_get_axis_min(iter->second, item);
        if (min == -1) {
            MMI_HILOGW("The device does not support this axis");
            continue;
        }
        axis.SetAxisType(item);
        axis.SetMinimum(min);
        axis.SetMaximum(libinput_device_get_axis_max(iter->second, item));
        axis.SetFuzz(libinput_device_get_axis_fuzz(iter->second, item));
        axis.SetFlat(libinput_device_get_axis_flat(iter->second, item));
        axis.SetResolution(libinput_device_get_axis_resolution(iter->second, item));
        inputDevice->AddAxisInfo(axis);
    }
    return inputDevice;
}
#endif

#ifdef OHOS_BUILD_KEY_MOUSE
std::shared_ptr<InputDevice> InputDeviceManager::MakeInputDevice(int32_t id,
    struct libinput_device* libinputDevice) const
{
    std::shared_ptr<InputDevice> inputDevice = std::make_shared<InputDevice>();
    CHKPP(inputDevice);
    inputDevice->SetId(id);
    inputDevice->SetType(static_cast<int32_t>(libinput_device_get_tags(libinputDevice)));
    auto name = libinput_device_get_name(libinputDevice);
    inputDevice->SetName((name == nullptr) ? ("null") : (name));
    inputDevice->SetBustype(libinput_device_get_id_bustype(libinputDevice));
    inputDevice->SetVersion(libinput_device_get_id_version(libinputDevice));
    inputDevice->SetProduct(libinput_device_get_id_product(libinputDevice));
    inputDevice->SetVendor(libinput_device_get_id_vendor(libinputDevice));
    auto phys = libinput_device_get_phys(libinputDevice);
    inputDevice->SetPhys((phys == nullptr) ? ("null") : (phys));
    inputDevice->SetNetworkId((MakeNetworkId(inputDevice->GetPhys())));
    std::string::size_type pos = inputDevice->GetName().find(VIRTUAL_DEVICE_NAME);
    inputDevice->SetRemote(pos != inputDevice->GetName().npos);
    auto uniq = libinput_device_get_uniq(libinputDevice);
    inputDevice->SetUniq((uniq == nullptr) ? ("null") : (uniq));

    InputDevice::AxisInfo axis;
    for (const auto &item : axisType) {
        auto min = libinput_device_get_axis_min(libinputDevice, item);
        if (min == -1) {
            MMI_HILOGW("The device does not support this axis");
            continue;
        }
        axis.SetAxisType(item);
        axis.SetMinimum(min);
        axis.SetMaximum(libinput_device_get_axis_max(libinputDevice, item));
        axis.SetFuzz(libinput_device_get_axis_fuzz(libinputDevice, item));
        axis.SetFlat(libinput_device_get_axis_flat(libinputDevice, item));
        axis.SetResolution(libinput_device_get_axis_resolution(libinputDevice, item));
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
#endif

std::vector<int32_t> InputDeviceManager::GetInputDeviceIds() const
{
    CALL_LOG_ENTER;
    std::vector<int32_t> ids;
    for (const auto &item : inputDevice_) {
#ifdef OHOS_BUILD_KEY_MOUSE
        if (IsRemote(item.second) && !IsDistributedInput(item.second)) {
            continue;
        }
#endif // OHOS_BUILD_KEY_MOUSE
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

#ifdef OHOS_BUILD_KEY_MOUSE
void InputDeviceManager::OnInputDeviceAdded(struct libinput_device *inputDevice)
{
    CALL_LOG_ENTER;
    CHKPV(inputDevice);
    bool exsitPointerDevice = false;
    for (const auto& item : inputDevice_) {
        if (item.second == inputDevice) {
            MMI_HILOGI("the device already exists");
            return;
        }
        if (IsPointerDevice(static_cast<struct libinput_device *>(item.second))) {
            exsitPointerDevice = true;
        }
    }
    if (nextId_ == INT32_MAX) {
        MMI_HILOGE("the nextId_ exceeded the upper limit");
        return;
    }
    inputDevice_[nextId_] = inputDevice;
    bool isRemote = false;
    isRemote = IsRemote(inputDevice);
    if (!isRemote || IsDistributedInput(inputDevice)) {
        HandleDeviceChanged(DEVICE_ADD, nextId_);
    }
    ++nextId_;
    if (IsPointerDevice(inputDevice)) {
        bool needShowMouse = exsitPointerDevice || !isRemote;
        NotifyPointerDevice(true, needShowMouse);
    }
}
#else
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
    for (const auto &item : devMonitor_) {
        CHKPC(item.first);
        item.second("add", nextId_);
    }
    ++nextId_;

    if (IsPointerDevice(inputDevice)) {
        NotifyPointerDevice(true);
    }
}
#endif

#ifdef OHOS_BUILD_KEY_MOUSE
void InputDeviceManager::HandleDeviceChanged(std::string changedType, int32_t id)
{
    for (const auto &item : devMonitor_) {
        CHKPC(item.first);
        item.second(changedType, id);
    }
}
#endif
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
#ifdef OHOS_BUILD_KEY_MOUSE
    HandleDeviceChanged(DEVICE_REMOVE, deviceId);
#else
    for (const auto &item : devMonitor_) {
        CHKPC(item.first);
        item.second("remove", deviceId);
    }
#endif
    ScanPointerDevice();
}

#ifdef OHOS_BUILD_KEY_MOUSE
void InputDeviceManager::ScanPointerDevice()
{
    if (!HasPointerDevice()) {
        NotifyPointerDevice(false, true);
    }
}
#else
void InputDeviceManager::ScanPointerDevice()
{
    bool hasPointerDevice = false;
    for (auto it = inputDevice_.begin(); it != inputDevice_.end(); ++it) {
        if (IsPointerDevice(it->second)) {
            hasPointerDevice = true;
            break;
        }
    }
    if (!hasPointerDevice) {
        NotifyPointerDevice(false);
    }
}
#endif

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

#ifdef OHOS_BUILD_KEY_MOUSE
void InputDeviceManager::NotifyPointerDevice(bool hasPointerDevice, bool isPointerVisible)
{
    MMI_HILOGI("observers_ size:%{public}zu", observers_.size());
    for (auto observer = observers_.begin(); observer != observers_.end(); observer++) {
        (*observer)->UpdatePointerDevice(hasPointerDevice, isPointerVisible);
    }
}
#else
void InputDeviceManager::NotifyPointerDevice(bool hasPointerDevice)
{
    MMI_HILOGI("observers_ size:%{public}zu", observers_.size());
    for (auto observer = observers_.begin(); observer != observers_.end(); observer++) {
        (*observer)->UpdatePointerDevice(hasPointerDevice);
    }
}
#endif

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

#ifdef OHOS_BUILD_KEY_MOUSE
bool InputDeviceManager::IsRemote(struct libinput_device* inputDevice) const
{
    CHKPR(inputDevice, false);
    bool isRemote = false;
    std::string name = libinput_device_get_name(inputDevice);
    std::string::size_type pos = name.find(VIRTUAL_DEVICE_NAME);
    if (pos != name.npos) {
        isRemote = true;
    }
    MMI_HILOGD("isRemote = %{public}s", isRemote == true ? "true" : "false");
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
    uint32_t dinputType = 0;
    if (udevTags & EVDEV_UDEV_TAG_MOUSE) {
        dinputType = MOUSE_ABILITY;
    } else if (udevTags & EVDEV_UDEV_TAG_KEYBOARD) {
        dinputType = KEYBOARD_ABILITY;
    } else if (udevTags & EVDEV_UDEV_TAG_TOUCHPAD) {
        dinputType = TOUCHPAD_ABILITY;
    }
    return dinputType;
}

void InputDeviceManager::OnStartRemoteInputSucess(const std::string& deviceId, const uint32_t& inputTypes)
{
    NotifyDeviceChanged(deviceId, inputTypes, DEVICE_ADD);
}

void InputDeviceManager::OnStopRemoteInputSucess(const std::string& deviceId, const uint32_t& inputTypes)
{
    NotifyDeviceChanged(deviceId, inputTypes, DEVICE_REMOVE);
}

void InputDeviceManager::NotifyDeviceChanged(const std::string& deviceId,
    const uint32_t& inputTypes, std::string changedType)
{
    CALL_LOG_ENTER;
    for (const auto item : inputDevice_) {
        if (!IsRemote(item.second)) {
            continue;
        }
        if (IsPointerDevice(static_cast<struct libinput_device *>(item.second))) {
            if (changedType == DEVICE_ADD) {
                SetPointerVisible(getpid(), true);
            }
            if ((changedType == DEVICE_REMOVE) && !HasPointerDevice()) {
                SetPointerVisible(getpid(), false);
            }
        }
        enum evdev_device_udev_tags udevTags = libinput_device_get_tags(item.second);
        uint32_t dinputType = DeviceUdevTagsToDinputType(udevTags);
        if (dinputType & inputTypes) {
            auto phys = libinput_device_get_phys(item.second);
            std::string networkId = MakeNetworkId(phys);
            if (networkId.compare(deviceId) == 0) {
                HandleDeviceChanged(changedType, item.first);
            }
        }
    }
}

void InputDeviceManager::SetPointerVisible(int32_t pid, bool visible)
{
    MMI_HILOGW("observers_ size:%{public}zu", observers_.size());
    for (auto observer = observers_.begin(); observer != observers_.end(); observer++) {
        (*observer)->SetPointerVisible(pid, visible);
    }
}

int32_t InputDeviceManager::GetRemoteInputAbility(std::string deviceId, sptr<ICallDinput> ablitity)
{
    MMI_HILOGD("deviceId: %{public}s", GetAnonyString(deviceId).c_str());
    std::set<int32_t> remotInputAbility;
    for (const auto item : inputDevice_) {
        if (IsRemote(item.second)) {
            auto phys = libinput_device_get_phys(item.second);
            std::string networkId = MakeNetworkId(phys);
            if (networkId.compare(deviceId) == 0) {
                int32_t type = static_cast<int32_t>(libinput_device_get_tags(item.second));
                MMI_HILOGD("type: %{public}d", type);
                remotInputAbility.insert(type);
            }
        }
    }
    CHKPR(ablitity, ERROR_NULL_POINTER);
    return ablitity->HandleRemoteInputAbility(remotInputAbility);
}

void InputDeviceManager::OnDeviceOffline()
{
    for (const auto item : inputDevice_) {
        if (IsPointerDevice(item.second) && !IsRemote(item.second)) {
            NotifyPointerDevice(true, true);
            return;
        }
    }
}

void InputDeviceManager::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    MMI_HILOGD("systemAbilityId:%{public}d add!", systemAbilityId);
    if (DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID == systemAbilityId) {
        InitDeviceManager();
    }
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
    ret = DistributedHardware::DeviceManager::GetInstance()
        .RegisterDevStateCallback(BUNDLE_NAME, "", stateCallback_);
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
#endif // OHOS_BUILD_KEY_MOUSE
} // namespace MMI
} // namespace OHOS
