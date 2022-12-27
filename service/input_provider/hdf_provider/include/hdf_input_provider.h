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

#ifndef HDF_INPUT_PROVIDER
#define HDF_INPUT_PROVIDER

#include <map>
#include <memory>

#include "circle_stream_buffer.h"
#include "input_manager.h"

#include "i_input_provider.h"

namespace OHOS {
namespace MMI {

enum class HDFEventDevStatus : uint32_t {
    HDF_ADD_DEVICE = 1,
    HDF_RMV_DEVICE = 2,
};

enum class HDFEventType : uint32_t {
    DEV_NODE_EVENT = 0,
    DEV_NODE_ADD_RMV = 1,
};

class HDFInputProvider : public IInputProvider {
public:
    HDFInputProvider(int32_t queueId);
    virtual ~HDFInputProvider() = default;
    virtual int32_t Enable() override;
    virtual int32_t Disable() override;
    virtual std::string GetName() override;
    virtual void EventDispatch(epoll_event &ev) override;

private:
    static void HotPlugCallback(const InputHotPlugEvent *event);
    static void EventPkgCallback(const InputEventPackage **pkgs, uint32_t count, uint32_t devIndex);
    static void HDFDeviceStatusChanged(int32_t devIndex, int32_t devType, HDFEventDevStatus devStatus);
    static bool IsDupTouchBtnKey(const InputEventPackage &r, uint32_t devIndex);
    int32_t RegisterHotPlug();
    int32_t ScanInputDevice();

private:
    InputHostCb hostcb_;
    InputEventCb eventcb_;
    CircleStreamBuffer circbuf_;
};
} // namespace MMI
} // namespace OHOS
#endif // HDF_INPUT_PROVIDER