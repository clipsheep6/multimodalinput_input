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
#include "dfx_hisysevent.h"
#include "input_device_cooperate_sm.h"
#include "input_device_cooperate_util.h"


namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "InputDeviceManager"};
} // namespace

InputDeviceManager::InputDeviceManager() {}
InputDeviceManager::~InputDeviceManager() {}

std::vector<int32_t> InputDeviceManager::GetInputDeviceIds() const
{
    CALL_DEBUG_ENTER;
    std::vector<int32_t> ids;
    ids.push_back(1);
    return ids;
}

bool InputDeviceManager::IsPointerDevice(struct libinput_device* device) const
{
    return true;
}

std::vector<std::string> InputDeviceManager::GetCooperateDhids(const std::string &dhid)
{
    std::vector<std::string> dhids;
    dhids.push_back("1");
    dhids.push_back("2");
    return dhids;
}

std::vector<std::string> InputDeviceManager::GetCooperateDhids(int32_t deviceId)
{
    std::vector<std::string> dhids;
    dhids.push_back("1");
    dhids.push_back("2");
    return dhids;
}
std::string InputDeviceManager::GetOriginNetworkId(int32_t id)
{
    return "networkId";
}

std::string InputDeviceManager::GetOriginNetworkId(const std::string &dhid)
{
    if (dhid.empty()) {
        return "";
    }
    return "networkId";
}

std::string InputDeviceManager::GetDhid(int32_t deviceId) const
{
    return "dhid";
}
} // namespace MMI
} // namespace OHOS
