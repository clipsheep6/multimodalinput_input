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

namespace OHOS {
namespace MMI {
struct MmiHdfEvent {
    int32_t type;
    int32_t code;
    int32_t value;
    int64_t time;
};

class HdfAdapter {
    using HdfEventCallback = std::function<void(void *event)>;
public:
    int32_t Init();
    int32_t DeInit();
    int32_t GetInputFd() const;
    void EventDispatch(struct epoll_event& ev);
    void OnEventHandler(const MmiHdfEvent &data);
    void OnEventCallBack(void *data);
private:
    int32_t pipes_[2] = { -1, -1 };
    HdfEventCallback callback_ = nullptr;
};
} // namespace MMI
} // namespace OHOS
#endif // HDF_ADAPTER_H
