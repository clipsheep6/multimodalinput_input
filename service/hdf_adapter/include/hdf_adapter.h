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

#ifndef HDF_ADAPTER_H
#define HDF_ADAPTER_H

#include <unistd.h>
#include <functional>
#include <string>
#include <sys/epoll.h>

#include "input_type.h"
#include "input_manager.h"

#include "define_multimodal.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
enum class HDFInputEventType : uint32_t {
    DEV_NODE_EVENT = 0,
    DEV_NODE_ADD_RMV = 1,
};

enum class HDFInputEventDevStatus : uint32_t {
    HDF_ADD_DEVICE = 1,
    HDF_RMV_DEVICE = 2,
};

#pragma pack(1)
struct HDFInputEvent {
    bool IsDevNodeAddRmvEvent() const { return (eventType == (static_cast<uint32_t>(HDFInputEventType::DEV_NODE_ADD_RMV))); }
    bool IsDevAdd() const { return (devStatus == (static_cast<uint32_t>(HDFInputEventDevStatus::HDF_ADD_DEVICE))); }
    uint32_t eventType; // 0 设备节点事件, 1 设备添加删除事件
    uint32_t devIndex;    
    uint64_t time; 
    union {
        struct {
            uint16_t type;
            uint16_t code;
        };
        uint32_t devType;
    };
    union {
        uint32_t value;
        uint32_t devStatus;
    };  
};

struct HDFDeviceStatusEvent {
    uint32_t devIndex;
    uint64_t time;
    uint32_t devType;
    uint32_t devStatus;
    InputDeviceInfo devInfo;
};

using HDFDeviceInputEvent = HDFInputEvent;
#pragma pack()

class HDFAdapter final {
    using HDFDeviceStatusEventCallback = std::function<void(const HDFDeviceStatusEvent &event)>;
    using HDFDeviceInputEventCallback = std::function<void(const HDFDeviceInputEvent &event)>;
public:
    HDFAdapter();
    ~HDFAdapter() = default;
    bool Init(HDFDeviceStatusEventCallback statusCallback, HDFDeviceInputEventCallback inputCallback);
    void Uninit();
    int32_t GetInputFd() const;
    int32_t ScanInputDevice();
    void EventDispatch(epoll_event &ev);
private:
    int32_t ConnectHDFService();
    int32_t DisconnectHDFService();
    int32_t HandleDeviceAdd(HDFDeviceStatusEvent &event);
    int32_t HandleDeviceRmv(int32_t devIndex, int32_t devType);
    void OnEventHandler(const HDFInputEvent &event);
private:
    HDFDeviceStatusEventCallback statusCallback_;
    HDFDeviceInputEventCallback inputCallback_;    
};
} // namespace MMI
} // namespace OHOS
#endif // HDF_ADAPTER_H
