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

namespace OHOS {
namespace MMI {
struct MmiHdfEvent {
    int32_t type;
    int32_t code;
    int32_t value;
    int64_t time;
};

enum MmiHdfEventPacketType {
    HDF_NONE = 0,
    HDF_EVENT,
    HDF_ADD_DEVICE,
    HDF_RMV_DEVICE,
};

struct MmiHdfPacket {
    int32_t size { 0 };
    int32_t type { HDF_NONE };
};

struct MmiHdfDevDescPacket {
    static constexpr int32_t MAX_INPUT_DEVICE_COUNT = 256;
    MmiHdfPacket head;
    InputDevDesc descs[MAX_INPUT_DEVICE_COUNT];
};

struct MmiHdfEventPacket {
    static constexpr int32_t MAX_EVENT_PKG_NUM = 256;
    MmiHdfPacket head;
    MmiHdfEvent events[MAX_EVENT_PKG_NUM];;
};

class HdfAdapter {
    using HdfEventCallback = std::function<void(const MmiHdfEvent &event)>;
public:
    HdfAdapter();
    bool Init(HdfEventCallback callback, const std::string& seat_id = "seat0");
    void DeInit();
    int32_t GetInputFd() const;
    int32_t ScanInputDevice();
    void EventDispatch(struct epoll_event& ev);
    void OnEventHandler(const MmiHdfEvent &event);
    void OnEventCallback(const MmiHdfPacket &pkt);
private:
    int32_t ConnectHDFInit();
    int32_t DisconnectHDFInit();
    void HandleDeviceAdd(int32_t devIndex, int32_t devType);
    void HandleDeviceRmv(int32_t devIndex, int32_t devType);
    void HandleDeviceEvent(int32_t devIndex, int32_t type, int32_t code, int32_t value, int64_t timestamp);
private:
    HdfEventCallback callback_ = nullptr;
    std::string seat_id_;
};
} // namespace MMI
} // namespace OHOS
#endif // HDF_ADAPTER_H
