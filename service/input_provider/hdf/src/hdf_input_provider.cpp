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

#include "hdf_input_provider.h"

#include <array>
#include <unistd.h>

#include "circle_stream_buffer.h"

#include "define_multimodal.h"
#include "error_multimodal.h"
#include "hdf_input_device.h"
#include "i_event_queue.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "HDFInputProvider" };
InputHostCb g_hostCbNew;
std::shared_ptr<IInputContext> g_InputContext;
std::map<uint32_t, int32_t> lastDevInfoMap_;
CircleStreamBuffer g_circBuf;
IInputInterface *g_InputInterface { nullptr };
InputEventCb g_eventCb;
} // namespace

HDFInputProvider::HDFInputProvider()
{
    CHK_PID_AND_TID();
    g_hostCbNew.HotPlugCallback = HotPlugCallback;
    g_eventCb.EventPkgCallback = EventPkgCallback;
}

void HDFInputProvider::HotPlugCallback(const InputHotPlugEvent *event)
{
    CHK_PID_AND_TID();
    CHKPV(event);
    if (event->devType != INDEV_TYPE_TOUCH) {
        MMI_HILOGW("devType:%{public}u", event->devType);
        return;
    }
    HDFDeviceStatusChanged(event->devIndex, event->devType,
    event->status == 0 ? HDFEventDevStatus::HDF_ADD_DEVICE: HDFEventDevStatus::HDF_RMV_DEVICE);
}

void HDFInputProvider::HDFDeviceStatusChanged(int32_t devIndex, int32_t devType, HDFEventDevStatus devStatus)
{
    CHK_PID_AND_TID();
    if (devType != INDEV_TYPE_TOUCH) {
        MMI_HILOGW("devType:%{public}u", devType);
        return;
    }
    EventData eventData;
    eventData.type = 1;
    eventData.data.type = devType;
    eventData.data.code = devIndex;
    eventData.data.value = (devStatus == HDFEventDevStatus::HDF_ADD_DEVICE ? 1 : 0);
    CHKPV(g_InputContext);
    auto queueManager = g_InputContext->GetEventQueueManager();
    CHKPV(queueManager);
    auto queue = queueManager->GetDefaultQueue(1);
    CHKPV(queue);
    queue->SendEvent(eventData);
}

void HDFInputProvider::EventPkgCallback(const InputEventPackage **pkgs, uint32_t count, uint32_t devIndex)
{
    CHK_PID_AND_TID();
    CHKPV(pkgs);
    constexpr uint32_t maxEventPkgNum = 100; 
    if (count > maxEventPkgNum) {
        MMI_HILOGE("Discard pkgs, too big hdf event pkg, count:%{public}u is beyond %{public}u", count, maxEventPkgNum);
        return;
    }
    CHKPV(g_InputContext);
    auto deviceManager = g_InputContext->GetInputDeviceManager();
    CHKPV(deviceManager);
    auto device = deviceManager->GetHdfInputDevice(devIndex);
    CHKPV(device);
    auto queueManager = g_InputContext->GetEventQueueManager();
    CHKPV(queueManager);
    auto queue = queueManager->GetDefaultQueue(1);
    CHKPV(queue);
    auto eventHandlerMgr = g_InputContext->GetEventHandlerManager();
    CHKPV(eventHandlerMgr);
    auto eventHandler = device->GetEventHandler();
    CHKPV(eventHandler);
    eventHandler->SetEventHandlerManager(eventHandlerMgr);
    for (uint32_t i = 0; i < count; i++) {
        if (pkgs[i] == nullptr) {
            continue;
        }
        const InputEventPackage &r = *pkgs[i];
        if (IsDupTouchBtnKey(r, devIndex)) {
            continue;
        }
        EventData eventData;
        eventData.type = 0;
        eventData.data.type = r.type;
        eventData.data.code = r.code;
        eventData.data.value = r.value;
        eventData.size = sizeof(*pkgs[i]);
        queue->RegisterEventHandler(eventHandler);
        queue->SendEvent(eventData);
    }
} 

bool HDFInputProvider::IsDupTouchBtnKey(const InputEventPackage &r, uint32_t devIndex)
{
    if (r.code == BTN_TOUCH && r.type == EV_KEY) {
        auto it = lastDevInfoMap_.find(devIndex);
        if (it != lastDevInfoMap_.end() && it->second == r.value) {
            return true;
        }
        lastDevInfoMap_.emplace(devIndex, r.value);
    }
    return false;
}

int32_t HDFInputProvider::Enable()
{
    g_InputContext = GetInputContext();
    int32_t ret = RegisterHotPlug();
    if (ret != RET_OK) {
        MMI_HILOGE("RegisterHotPlug failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    ret = ScanInputDevice();
    if (ret != RET_OK) {
        MMI_HILOGE("ScanInputDevice failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t HDFInputProvider::Disable()
{
    CALL_INFO_TRACE;
    if (g_InputInterface == nullptr) {
        MMI_HILOGE("HDF has alread disconnected.");
        return RET_OK;
    }
    ReleaseInputInterface(g_InputInterface);
    g_InputInterface = nullptr;
    return RET_OK;
}

std::string HDFInputProvider::GetName()
{
    return "HDFInputProvider";
}

int32_t HDFInputProvider::RegisterHotPlug()
{
    if (g_InputInterface != nullptr) {
        MMI_HILOGW("HDF has alread connected.");
        return RET_OK;
    }
    int32_t ret = GetInputInterface(&g_InputInterface);
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("Initialize failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    CHKPR(g_InputInterface, ERROR_NULL_POINTER);
    CHKPR(g_InputInterface->iInputReporter, ERROR_NULL_POINTER);
    ret = g_InputInterface->iInputReporter->RegisterHotPlugCallback(&g_hostCbNew);
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("Call RegisterHotPlugCallback fail, ret:%{public}d", ret);
        return RET_ERR;
    }
    MMI_HILOGI("Call RegisterHotPlugCallback success");
    return RET_OK;
}

int32_t HDFInputProvider::ScanInputDevice()
{
    CHKPR(g_InputInterface, ERROR_NULL_POINTER);
    CHKPR(g_InputInterface->iInputManager, ERROR_NULL_POINTER);
    constexpr uint32_t maxDevices = 32;
    std::array<InputDevDesc, maxDevices> devDescs;
    int32_t ret = g_InputInterface->iInputManager->ScanInputDevice(devDescs.data(), devDescs.size());
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("call ScanInputDevice failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    int32_t devCount = 0;
    for (const auto &i : devDescs) {
        if (i.devIndex != 0) {
            ++devCount;
            HDFDeviceStatusChanged(i.devIndex, i.devType, HDFEventDevStatus::HDF_ADD_DEVICE);
        }
    }
    MMI_HILOGI("Found %{public}d devices.", devCount);
    return RET_OK;
}

void HDFInputProvider::EventDispatch(epoll_event &ev)
{
    CALL_DEBUG_ENTER;
    CHKPV(ev.data.ptr);
    if ((ev.events & EPOLLERR) || (ev.events & EPOLLHUP)) {
        MMI_HILOGF("Epoll unrecoverable error");
        free(ev.data.ptr);
        ev.data.ptr = nullptr;
        return;
    }
    int32_t fd = *static_cast<int32_t *>(ev.data.ptr);
    if (fd < 0) {
        MMI_HILOGE("Fd:%{public}d is invalid", fd);
        return;
    }
    constexpr int32_t szBufSize = 1024;
    char szBuf[szBufSize] = {};
    auto retSize = read(fd, szBuf, szBufSize);
    if (retSize < 0) {
        MMI_HILOGE("Read pipe failed, errno:%{public}d, %{public}s", errno, strerror(errno));
        return;
    } else if (retSize == 0) {
        MMI_HILOGD("Pipe no data");
        return;
    }
    if (!g_circBuf.Write(szBuf, retSize)) {
        MMI_HILOGW("Write data failed. size:%{public}zu", retSize);
    }
    constexpr int32_t onceProcessLimit = 100;
    constexpr int32_t hdfEventSize = sizeof(EventData);
    for (int32_t i = 0; i < onceProcessLimit; ++i) {
        const int32_t unreadSize = g_circBuf.UnreadSize();
        if (unreadSize < hdfEventSize) {
            break;
        }
        char *buf = const_cast<char *>(g_circBuf.ReadBuf());
        CHKPB(buf);
        EventData *event = reinterpret_cast<EventData*>(buf);
        CHKPB(event);
        if (!g_circBuf.SeekReadPos(hdfEventSize)) {
            MMI_HILOGW("Set read position error, and this error cannot be recovered, and the buffer will be reset."
                " hdfEventSize:%{public}d unreadSize:%{public}d", hdfEventSize, unreadSize);
            g_circBuf.Reset();
            break;
        }
        auto ev = event->data;
        if (event->type == 1) {
            CHKPB(g_InputContext);
            auto inputDeviceManager = g_InputContext->GetInputDeviceManager();
            CHKPB(inputDeviceManager);
            if (ev.value == 1) {
                auto dev = std::make_shared<HDFInputDevice>(ev.code, g_InputInterface, g_eventCb);
                CHKPC(dev);
                inputDeviceManager->AddInputDevice(dev);
            } else {
                auto dev = inputDeviceManager->GetHdfInputDevice(ev.code);
                CHKPV(dev);
                inputDeviceManager->RemoveInputDevice(dev);
            }
        } else {
            auto iEventHandler = event->handler;
            CHKPB(iEventHandler);         
            iEventHandler->HandleEvent(ev);
        }
        if (g_circBuf.IsEmpty()) {
            g_circBuf.Reset();
            break;
        }
    }
}
} // namespace MMI
} // namespace OHOS