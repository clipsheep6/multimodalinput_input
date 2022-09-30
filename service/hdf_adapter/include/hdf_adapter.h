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
    bool Init(HdfEventCallback callback);
    void DeInit();
    void Dump(int32_t fd, const std::vector<std::string> &args);
    int32_t GetInputFd() const;
    int32_t ScanInputDevice();
    void EventDispatch(epoll_event& ev);
    void OnEventHandler(const input_event &event);
private:
    int32_t ConnectHDFInit();
    int32_t DisconnectHDFInit();
    int32_t HandleDeviceAdd(int32_t devIndex, int32_t devType);
    int32_t HandleDeviceRmv(int32_t devIndex, int32_t devType);
private:
    HdfEventCallback callback_ = nullptr;
    std::vector<std::string> eventRecords_;
};
} // namespace MMI
} // namespace OHOS
#endif // HDF_ADAPTER_H
