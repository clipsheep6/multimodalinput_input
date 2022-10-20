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
#include "input_type.h"
#include "define_multimodal.h"
#include "input_manager.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
enum class HdfInputEventType : uint32_t {
    DEV_NODE_EVENT = 0,
    DEV_NODE_ADD_RMV = 1,
};

enum class HdfInputEventDevStatus : int32_t {
    HDF_ADD_DEVICE = 1,
    HDF_RMV_DEVICE = 2,
};

#pragma pack(1)
struct HdfInputEvent {
    bool IsDevNodeAddRmvEvent() const { return (eventType == (static_cast<uint32_t>(HdfInputEventType::DEV_NODE_ADD_RMV))); }
    bool IsDevAdd() const { return (devStatus == (static_cast<uint32_t>(HdfInputEventDevStatus::HDF_ADD_DEVICE))); }
    uint32_t eventType; // 0 设备节点事件, 1 设备添加删除事件
    uint32_t devIndex;    
    uint64_t time; 
    union {
        union {
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

using HDFDeviceInputEvent = HdfInputEvent;
#pragma pack()
class HdfAdapter {
    using HDFDeviceStatusEventCallback = std::function<void(const HDFDeviceStatusEvent &event)>;
    using HDFDeviceInputEventCallback = std::function<void(const HDFDeviceInputEvent &event)>;
public:
    HdfAdapter();
    ~HdfAdapter() = default;
    bool Init(HDFDeviceStatusEventCallback statusCallback, HDFDeviceInputEventCallback inputCallback);
    void DeInit();
    void Dump(int32_t fd, const std::vector<std::string> &args);
    int32_t GetInputFd() const;
    int32_t ScanInputDevice();
    void EventDispatch(epoll_event& ev);
    void OnEventHandler(const HdfInputEvent &event);
private:
    int32_t ConnectHDFInit();
    int32_t DisconnectHDFInit();
    int32_t HandleDeviceAdd(HDFDeviceStatusEvent &retEvent);
    int32_t HandleDeviceRmv(int32_t devIndex, int32_t devType);
private:
    HDFDeviceStatusEventCallback statusCallback_;
    HDFDeviceInputEventCallback inputCallback_;
    std::vector<std::string> eventRecords_;
};
} // namespace MMI
} // namespace OHOS
#endif // HDF_ADAPTER_H
