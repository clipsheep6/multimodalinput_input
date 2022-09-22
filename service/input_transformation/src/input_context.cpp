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

#include "input_context.h"

#include <iostream>
#include <cstring>

#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputContext" };
};
std::unique_ptr<InputContext> InputContext::CreateInstance()
{
    MMI_HILOGD("Enter");

    auto inputContext = std::unique_ptr<InputContext>(new InputContext());
    auto retCode = InitInstance(inputContext.get());
    if (retCode != 0) {
        MMI_HILOGE("Leave");
        return nullptr;
    }
    MMI_HILOGD("Leave");
    return inputContext;
}

int32_t InputContext::InitInstance(InputContext* inputContext) {
    MMI_HILOGD("Enter");
    auto seatManager = ISeatManager::CreateInstance(inputContext);
    auto retCode = inputContext->SetSeatManager(seatManager);
    if (retCode == -1) {
        MMI_HILOGE("Leave, SetSeatManager Failed");
        return -1;
    }

    auto deviceManager = IDeviceManager::CreateInstance(inputContext);
    retCode = inputContext->SetDeviceManager(deviceManager);
    if (retCode == -1) {
        MMI_HILOGE("Leave, SetDeviceManager Failed");
        return -1;
    }
    MMI_HILOGD("Leave");
    return 0;
}

const std::unique_ptr<IDeviceManager>& InputContext::GetInputDeviceManager() const {
    return inputDeviceManager_;
}

const std::unique_ptr<ISeatManager>& InputContext::GetSeatManager() const {
    return seatManager_;
}

int32_t InputContext::SetDeviceManager(std::unique_ptr<IDeviceManager>& inputDeviceManager) {
    if (!inputDeviceManager) {
        return -1;
    }

    inputDeviceManager_ = std::move(inputDeviceManager);
    return 0;
}

int32_t InputContext::SetSeatManager(std::unique_ptr<ISeatManager>& seatManager) {
    if (!seatManager) {
        return -1;
    }

    seatManager_ = std::move(seatManager);
    return 0;
}
} // namespace MMI
} // namespace OHOS