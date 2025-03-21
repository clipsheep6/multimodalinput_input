/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <system_ability_definition.h>
#include "setting_datashare.h"
#include "touchpad_settings_handler.h"
#include "mmi_log.h"
#include "account_manager.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "TouchpadSettingsObserver"

#define TP_CHECK_FALSE_RETURN(cond, ret, fmt, ...)  \
    if (!(cond)) {                                  \
        MMI_HILOGE(fmt, ##__VA_ARGS__);             \
        return ret;                                 \
    }

namespace OHOS {
namespace MMI {
namespace {
const std::string g_pressureKey {"settings.trackpad.press_level"};
const std::string g_vibrationKey {"settings.trackpad.shock_level"};
const std::string g_touchpadSwitchesKey {"settings.trackpad.touchpad_switches"};
const std::string g_knuckleSwitchesKey {"settings.trackpad.touchpad_switches"};
const std::string g_datashareBaseUri =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_";
const std::string g_libthpPath {"/system/lib64/libthp_extra_innerapi.z.so"};
const std::map<std::string, int> g_keyToCmd = {
    {g_pressureKey, 103}, // pressure cmd 103
    {g_vibrationKey, 104}, // vibration cmd 104
    {g_touchpadSwitchesKey, 108}, // touchpad switches cmd 108
    {g_knuckleSwitchesKey, 109} // knuckle switches cmd 109
};
const std::map<std::string, std::string> g_defaultValue = {
    {g_pressureKey, "2"}, // default pressure value 2
    {g_vibrationKey, "2"}, // default vibration value 2
    {g_touchpadSwitchesKey, "1"}, // tdefault touchpad on
    {g_knuckleSwitchesKey, "0"} // default knuckle off
};
} // namespace

std::mutex TouchpadSettingsObserver::mutex_;
std::shared_ptr<TouchpadSettingsObserver> TouchpadSettingsObserver::instance_ = nullptr;

TouchpadSettingsObserver::TouchpadSettingsObserver() {}

TouchpadSettingsObserver::~TouchpadSettingsObserver() {}

std::shared_ptr<TouchpadSettingsObserver> TouchpadSettingsObserver::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<TouchpadSettingsObserver>();
        }
    }
    return instance_;
}

void TouchpadSettingsObserver::RegisterUpdateFunc()
{
    updateFunc_ = nullptr;
    const std::string datashareUri = datashareUri_;
    const std::string libthpPath = g_libthpPath;
    const std::map<std::string, int> keyToCmd = g_keyToCmd;
    const std::map<std::string, std::string> defaultValue = g_defaultValue;

    SettingObserver::UpdateFunc UpdateFunc =
        [datashareUri, libthpPath, keyToCmd, defaultValue](const std::string& key) {
        MMI_HILOGI("Touchpad settings change:%{public}s", key.c_str());
        typedef const char* (*ThpExtraRunCommandFunc)(const char* command, const char* parameters);
        const char* (*ThpExtraRunCommand)(const char* command, const char* parameters) {};
        std::string value;

        auto ret = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID).GetStringValue(key, value, datashareUri);
        if (ret != 0) {
            auto valueIter = defaultValue.find(key);
            if (valueIter == defaultValue.end()) {
                MMI_HILOGE("Invalid value key");
                return;
            }
            value = valueIter->second;
        }
        MMI_HILOGI("Get value: %{public}s", value.c_str());

        auto cmdIter = keyToCmd.find(key);
        if (cmdIter == keyToCmd.end()) {
            MMI_HILOGE("Invalid command key");
            return;
        }

        void *handle = nullptr;
        handle = dlopen(libthpPath.c_str(), RTLD_LAZY);
        if (handle == nullptr) {
            MMI_HILOGE("Handle is null");
            return;
        }

        ThpExtraRunCommand = reinterpret_cast<ThpExtraRunCommandFunc>(dlsym(handle, "ThpExtraRunCommand"));
        if (ThpExtraRunCommand == nullptr) {
            MMI_HILOGE("ThpExtraRunCommand is null");
            dlclose(handle);
            return;
        }
        const std::string param = std::string("#").append(std::to_string(cmdIter->second)).append("#").append(value);
        ThpExtraRunCommand("THP_TouchpadStatusChange", param.c_str());
        dlclose(handle);
    };
    MMI_HILOGI("Update function register end");
    updateFunc_ = UpdateFunc;
    return;
}

bool TouchpadSettingsObserver::RegisterTpObserver(const int32_t accountId)
{
    if (hasRegistered_) { return false; }
    std::lock_guard<std::mutex> lock(lock_);
    ErrCode ret = 0;

    currentAccountId_ = accountId;
    TP_CHECK_FALSE_RETURN(currentAccountId_ >= 0, false, "Get account info fail");

    datashareUri_ = g_datashareBaseUri + std::to_string(currentAccountId_) + "?Proxy=true";
    RegisterUpdateFunc();
    TP_CHECK_FALSE_RETURN(updateFunc_ != nullptr, false, "Update function is null");

    if (pressureObserver_ == nullptr) {
        pressureObserver_ = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .CreateObserver(g_pressureKey, updateFunc_);
        TP_CHECK_FALSE_RETURN(pressureObserver_ != nullptr, false, "PressureObserver fail");
        ret = ret || SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .RegisterObserver(pressureObserver_, datashareUri_);
    }
    if (vibrationObserver_ == nullptr) {
        vibrationObserver_ = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .CreateObserver(g_vibrationKey, updateFunc_);
        TP_CHECK_FALSE_RETURN(vibrationObserver_ != nullptr, false, "VibrationObserver fail");
        ret = ret || SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .RegisterObserver(vibrationObserver_, datashareUri_);
    }
    if (touchpadSwitchesObserver_ == nullptr) {
        touchpadSwitchesObserver_ = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .CreateObserver(g_touchpadSwitchesKey, updateFunc_);
        TP_CHECK_FALSE_RETURN(touchpadSwitchesObserver_ != nullptr, false, "TouchpadSwitchesObserver fail");
        ret = ret || SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .RegisterObserver(touchpadSwitchesObserver_, datashareUri_);
    }
    if (knuckleSwitchesObserver_ == nullptr) {
        knuckleSwitchesObserver_ = SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .CreateObserver(g_knuckleSwitchesKey, updateFunc_);
        TP_CHECK_FALSE_RETURN(knuckleSwitchesObserver_ != nullptr, false, "KnuckleSwitchesObserver fail");
        ret = ret || SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .RegisterObserver(knuckleSwitchesObserver_, datashareUri_);
    }
    if (ret) {
        pressureObserver_ = nullptr;
        vibrationObserver_ = nullptr;
        touchpadSwitchesObserver_ = nullptr;
        knuckleSwitchesObserver_ = nullptr;
        MMI_HILOGE("Register setting observer failed, ret = %{public}d", ret);
        return false;
    }
    MMI_HILOGI("Register touchpad observer end");
    hasRegistered_ = true;
    return true;
}

bool TouchpadSettingsObserver::UnregisterTpObserver(const int32_t accountId)
{
    if (!hasRegistered_ || accountId == currentAccountId_) {
        return false;
    }
    std::lock_guard<std::mutex> lock { lock_ };
    ErrCode ret = 0;

    if (pressureObserver_ != nullptr) {
        ret = ret || SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .UnregisterObserver(pressureObserver_, datashareUri_);
        TP_CHECK_FALSE_RETURN(ret == 0, false, "Unregister pressureObserver fail");
        pressureObserver_ = nullptr;
    }
    if (vibrationObserver_ != nullptr) {
        ret = ret || SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .UnregisterObserver(vibrationObserver_, datashareUri_);
        TP_CHECK_FALSE_RETURN(ret == 0, false, "Unregister vibrationObserver fail");
        vibrationObserver_ = nullptr;
    }
    if (touchpadSwitchesObserver_ != nullptr) {
        ret = ret || SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .UnregisterObserver(touchpadSwitchesObserver_, datashareUri_);
        TP_CHECK_FALSE_RETURN(ret == 0, false, "Unregister touchpadSwitchesObserver fail");
        touchpadSwitchesObserver_ = nullptr;
    }
    if (knuckleSwitchesObserver_ != nullptr) {
        ret = ret || SettingDataShare::GetInstance(MULTIMODAL_INPUT_SERVICE_ID)
            .UnregisterObserver(knuckleSwitchesObserver_, datashareUri_);
        TP_CHECK_FALSE_RETURN(ret == 0, false, "Unregister knuckleSwitchesObserver fail");
        knuckleSwitchesObserver_ = nullptr;
    }

    hasRegistered_ = false;
    MMI_HILOGI("Unregister touchpad observer");
    return true;
}

void TouchpadSettingsObserver::SyncTouchpadSettingsData()
{
    if (!hasRegistered_ && !RegisterTpObserver(ACCOUNT_MGR->GetCurrentAccountSetting().GetAccountId())) {
        return;
    }
    std::lock_guard<std::mutex> lock { lock_ };
    if (updateFunc_ == nullptr) {
        return;
    }
    updateFunc_(g_pressureKey);
    updateFunc_(g_vibrationKey);
    updateFunc_(g_touchpadSwitchesKey);
    updateFunc_(g_knuckleSwitchesKey);
    MMI_HILOGI("Sync touchpad settings end");
}
} // namespace MMI
} // namespace OHOS
