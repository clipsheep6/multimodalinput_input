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

#ifndef HDF_ADAPTER_H
#define HDF_ADAPTER_H

#include <unistd.h>
#include <functional>
#include <string>
#include <sys/epoll.h>
#include "define_multimodal.h"
#include "input_manager.h"
#include "mmi_log.h"

struct input_event;

namespace OHOS {
namespace MMI {
class HdfAdapter {
    using HdfEventCallback = std::function<void(const input_event &event)>;
public:
    HdfAdapter();
    ~HdfAdapter() = default;
    bool Init(HdfEventCallback callback, const std::string& seat_id = "seat0");
    void DeInit();
    void Dump(int32_t fd, const std::vector<std::string> &args);
    int32_t GetInputFd() const;
    int32_t ScanInputDevice();
    void EventDispatch(epoll_event& ev);
    void OnEventHandler(const input_event &event);
    void OnEventCallback(const input_event &event);
private:
    int32_t ConnectHDFInit();
    int32_t DisconnectHDFInit();
private:
    void HandleDeviceStatusChanged(int32_t devIndex, int32_t devType, int32_t status);
    void HandleEvent(int32_t devIndex, int32_t evType, int32_t code, int32_t value, int64_t time);
    void HandleDeviceAdd(int32_t devIndex, int32_t devType);
    void HandleDeviceRmv(int32_t devIndex, int32_t devType);
    void HandleDeviceEvent(int32_t devIndex, int32_t type, int32_t code, int32_t value, int64_t timestamp);
private:
    HdfEventCallback callback_ = nullptr;
    std::string seat_id_;
    std::vector<std::string> eventRecords_;
};
} // namespace MMI
} // namespace OHOS
#endif // HDF_ADAPTER_H
