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

#include "manage_inject_device.h"

#include <chrono>
#include <thread>
#include "cJSON.h"

using namespace OHOS::MMI;

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "ManageInjectDevice" };
} // namespace

int32_t ManageInjectDevice::TransformJsonData(const cJSON* configData)
{
    CALL_LOG_ENTER;
    for (int32_t i = 0; i < cJSON_GetArraySize(configData); i++) {
        cJSON* Arrayjson = cJSON_GetArrayItem(configData, i);
        CHKPR(Arrayjson, RET_ERR);
        InputEventArray inputEventArray = {};
        cJSON* deviceName = cJSON_GetObjectItemCaseSensitive(Arrayjson, "deviceName");
        if (deviceName) {
            inputEventArray.deviceName = deviceName->valuestring;
        }
        uint16_t index = 0;
        cJSON* devIndex = cJSON_GetObjectItemCaseSensitive(Arrayjson, "devIndex");
        if (devIndex) {
            index = devIndex->valueint;
        }
        std::string deviceNode;
        if (getDeviceNodeObject_.GetDeviceNodeName(inputEventArray.deviceName, index, deviceNode) == RET_ERR) {
            MMI_LOGE("fail get device:%{public}s node", inputEventArray.deviceName.c_str());
            return RET_ERR;
        }
        inputEventArray.target = deviceNode;
        auto devicePtr = GetDeviceObject::CreateDeviceObject(inputEventArray.deviceName);
        CHKPR(devicePtr, RET_ERR);
        int32_t ret = devicePtr->TransformJsonDataToInputData(Arrayjson, inputEventArray);
        if (devicePtr != nullptr) {
            delete devicePtr;
            devicePtr = nullptr;
        }
        if (ret == RET_ERR) {
            MMI_LOGE("fail read json file");
            return ret;
        }
        ret = SendEvent(inputEventArray);
        if (ret == RET_ERR) {
            MMI_LOGE("SendEvent fail");
            return ret;
        }
    }
    return RET_OK;
}

int32_t ManageInjectDevice::SendEvent(const InputEventArray& inputEventArray)
{
    return SendEventToDeviveNode(inputEventArray);
}

int32_t ManageInjectDevice::SendEventToDeviveNode(const InputEventArray& inputEventArray)
{
    CALL_LOG_ENTER;
    std::string deviceNode = inputEventArray.target;
    if (deviceNode.empty()) {
        MMI_LOGE("device node:%{public}s is not exit", deviceNode.c_str());
        return RET_ERR;
    }
    char realPath[PATH_MAX] = {};
    if (realpath(deviceNode.c_str(), realPath) == nullptr) {
        MMI_LOGE("path is error, path:%{public}s", deviceNode.c_str());
        return RET_ERR;
    }
    int32_t fd = open(realPath, O_RDWR);
    if (fd < 0) {
        MMI_LOGE("open device node:%{public}s faild", deviceNode.c_str());
        return RET_ERR;
    }
    for (const auto &item : inputEventArray.events) {
        write(fd, &item.event, sizeof(item.event));
        int64_t blockTime = (item.blockTime == 0) ? INJECT_SLEEP_TIMES : item.blockTime;
        std::this_thread::sleep_for(std::chrono::milliseconds(blockTime));
    }
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
    return RET_OK;
}