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

#include "distributed_input_kit.h"
#include "define_multimodal.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
int32_t DistributedInputKit::PrepareRemoteInput(const std::string &deviceId, sptr<IPrepareDInputCallback> callback)
{
    if (deviceId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([deviceId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResult(deviceId, 0);
    });
    t.join();
    return RET_OK;
}

int32_t DistributedInputKit::UnprepareRemoteInput(const std::string &deviceId, sptr<IUnprepareDInputCallback> callback)
{
    if (deviceId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([deviceId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResult(deviceId, 0);
    });
    t.join();
    return RET_OK;
}
int32_t DistributedInputKit::StartRemoteInput(
    const std::string& deviceId, const uint32_t& inputTypes, sptr<IStartDInputCallback> callback)
{
    if (deviceId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([deviceId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResult(deviceId, 0, 0);
    });
    t.join();
    return RET_OK;
}

int32_t DistributedInputKit::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
                                             const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    if (srcId == "" || sinkId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([srcId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResult(srcId, 0, 0);
    });
    t.join();
    return RET_OK;
}

bool DistributedInputKit::IsNeedFilterOut(const std::string &deviceId, const BusinessEvent &event)
{
    if (deviceId == "") {
        return false;
    }
    return true;
}

DInputServerType DistributedInputKit::IsStartDistributedInput(const uint32_t &inputType)
{
    return  DInputServerType::SOURCE_SERVER_TYPE;
}

int32_t DistributedInputKit::PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                sptr<IPrepareDInputCallback> callback)
{
    if (srcId == "" || sinkId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([srcId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResult(srcId, 0);
    });
    t.join();
    return RET_OK;
}

int32_t DistributedInputKit::UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                  sptr<IUnprepareDInputCallback> callback)
{
    if (srcId == "" || sinkId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([srcId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResult(srcId, 0);
    });
    t.join();
    return RET_OK;
}
int32_t DistributedInputKit::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    if (srcId == "" || sinkId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([srcId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResult(srcId, 0, 0);
    });
    t.join();
    return RET_OK;
}

int32_t DistributedInputKit::StopRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
                                             sptr<IStartStopDInputsCallback> callback)
{
    if (sinkId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([sinkId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResultDhids(sinkId, 0);
    });
    t.join();
    return RET_OK;
}
int32_t DistributedInputKit::StartRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<IStartStopDInputsCallback> callback)
{
    if (sinkId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([sinkId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResultDhids(sinkId, 0);
    });
    t.join();
    return RET_OK;
}

int32_t DistributedInputKit::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
                                             const std::vector<std::string> &dhIds,
                                             sptr<IStartStopDInputsCallback> callback)
{
    if (srcId == "" || sinkId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([srcId, sinkId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResultDhids(srcId, 0);
    });
    t.join();
    return RET_OK;
}
int32_t DistributedInputKit::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    if (srcId == "" || sinkId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([srcId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResultDhids(srcId, 0);
    });
    t.join();
    return RET_OK;
}

int32_t DistributedInputKit::StopRemoteInput(
    const std::string& deviceId, const uint32_t& inputTypes, sptr<IStopDInputCallback> callback)
{
    if (deviceId == "" || callback == nullptr) {
        return RET_ERR;
    }
    std::thread t = std::thread([deviceId, callback] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        callback->OnResult(deviceId, 0, 0);
    });
    t.join();
    return RET_OK;
}

int32_t DistributedInputKit::RegisterStartStopResultCallback(const sptr<IStartStopResultCallback> callback)
{
    return RET_OK;
}

int32_t DistributedInputKit::RegisterInputNodeListener(sptr<InputNodeListener> listener)
{
    return RET_OK;
}

int32_t DistributedInputKit::UnRegisterInputNodeListener(sptr<InputNodeListener> listener)
{
    return RET_OK;
}

int32_t DistributedInputKit::RegisterSimulationEventListener(sptr<ISimulationEventListener> listener)
{
    return RET_OK;
}

int32_t DistributedInputKit::UnregisterSimulationEventListener(sptr<ISimulationEventListener> listener)
{
    return RET_OK;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS