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

#include "hdf_adapter.h"

#include <array>
#include <linux/input.h>

#include "circle_stream_buffer.h"
#include "config_multimodal.h"
#include "error_multimodal.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "HDFAdapter" };
int32_t g_hdfAdapterWriteFd { -1 };
int32_t g_mmiServiceReadFd { -1 };
CircleStreamBuffer g_circBuf;
IInputInterface *g_inputInterface { nullptr };
InputEventCb g_eventCb;
InputHostCb g_hostCb;
std::map<uint32_t, int32_t> g_lastDevInfo;
} // namespace

void HDFDeviceStatusChanged(int32_t devIndex, int32_t devType, HDFInputEventDevStatus devStatus)
{
    CHK_PID_AND_TID();
    if (devType != INDEV_TYPE_TOUCH) {
        MMI_HILOGW("devIndex:%{public}u, devType:%{public}u, devStatus:%{public}u", devIndex, devType, devStatus);
        return;
    }
    HDFInputEvent event;
    event.eventType = static_cast<uint32_t>(HDFInputEventType::DEV_NODE_ADD_RMV);
    event.devIndex = devIndex;
    event.devType = devType;
    event.devStatus = static_cast<uint32_t>(devStatus);
    event.time = GetSysClockTime();
    MMI_HILOGD("devIndex:%{public}u, devType:%{public}u, devStatus:%{public}u,time:%{public}" PRId64,
        devIndex, event.devType, event.devStatus, event.time);
    auto ret = write(g_hdfAdapterWriteFd, &event, sizeof(event));
    if (ret == -1) {
        int saveErrno = errno;
        MMI_HILOGE("Write pipe failed, fd:%{public}d, errno:%{public}d, %{public}s", 
            g_hdfAdapterWriteFd, saveErrno, strerror(saveErrno));
    }
}

static void HotPlugCallback(const InputHotPlugEvent *event)
{
    CHK_PID_AND_TID();
    CHKPV(event);
    auto devStatus = ((event->status == 0) ? HDFInputEventDevStatus::HDF_ADD_DEVICE : HDFInputEventDevStatus::HDF_RMV_DEVICE);
    HDFDeviceStatusChanged(event->devIndex, event->devType, devStatus);
}

static inline bool IsDupTouchBtnKey(const InputEventPackage &r, uint32_t devIndex)
{
    if (r.code == BTN_TOUCH && r.type == EV_KEY) {
        auto it = g_lastDevInfo.find(devIndex);
        if (it != g_lastDevInfo.end() && it->second == r.value) {
            return true;
        } else if (it == g_lastDevInfo.end()) {
            g_lastDevInfo.insert(std::pair(devIndex, r.value));
        } else {
            g_lastDevInfo[devIndex] = r.value;
        }
    }
    return false;
}

static inline void WriteToPipe(const InputEventPackage &r, uint32_t devIndex)
{
    HDFInputEvent event;
    event.eventType = static_cast<uint32_t>(HDFInputEventType::DEV_NODE_EVENT);
    event.devIndex = devIndex;
    event.type = r.type;
    event.value = r.value;
    event.time = r.timestamp;
    event.code = r.code;
    auto ret = write(g_hdfAdapterWriteFd, &event, sizeof(HDFInputEvent));
    if (ret == -1) {
        int saveErrno = errno;
        MMI_HILOGE("Write pipe failed, fd:%{public}d, errno:%{public}d, %{public}s",
        g_hdfAdapterWriteFd, saveErrno, strerror(saveErrno));
        return;
    }
    MMI_HILOGD("devIndex:%{public}u,code:%{public}u,type:%{public}u,value:%{public}u,time:%{public}" PRId64 ",",
        devIndex, r.code, r.type, r.value, r.timestamp);
}

static void EventPkgCallback(const InputEventPackage **pkgs, uint32_t count, uint32_t devIndex)
{
    CHK_PID_AND_TID();
    CHKPV(pkgs);
    constexpr uint32_t maxEventPkgNum = 100;  
    if (count > maxEventPkgNum) {
        MMI_HILOGE("Discard pkgs, too big hdf event pkg, count:%{public}u is beyond %{public}u", count, maxEventPkgNum);
        return;
    }
    for (uint32_t i = 0; i < count; ++i) {
        if (pkgs[i] == nullptr) {
            continue;
        }
        const InputEventPackage &r = *pkgs[i];
        if (IsDupTouchBtnKey(r, devIndex)) {
            continue;
        }
        WriteToPipe(r, devIndex);
    }
}

HDFAdapter::HDFAdapter()
{
    CHK_PID_AND_TID();
    g_eventCb.EventPkgCallback = EventPkgCallback;
    g_hostCb.HotPlugCallback = HotPlugCallback;
}

int32_t HDFAdapter::ScanInputDevice()
{
    CHK_PID_AND_TID();
    CALL_DEBUG_ENTER;
    CHKPR(g_inputInterface, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputManager, ERROR_NULL_POINTER);
    constexpr uint32_t maxDevices = 32;
    std::array<InputDevDesc, maxDevices> devDescs;
    int32_t ret = g_inputInterface->iInputManager->ScanInputDevice(devDescs.data(), devDescs.size());
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("call ScanInputDevice failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    int32_t devCount = 0;
    for (const auto &i : devDescs) {
        if (i.devIndex != 0) {
            ++devCount;
            HDFDeviceStatusChanged(i.devIndex, i.devType, HDFInputEventDevStatus::HDF_ADD_DEVICE);
        }
    }
    MMI_HILOGI("Found %{public}d devices.", devCount);
    return RET_OK;
}

bool HDFAdapter::Init(HDFDeviceStatusEventCallback statusCallback, HDFDeviceInputEventCallback inputCallback)
{
    CALL_DEBUG_ENTER;
    CHKPF(statusCallback);
    CHKPF(inputCallback);
    statusCallback_ = statusCallback;
    inputCallback_ = inputCallback;

    do {
        int32_t fds[2] = {-1, -1};
        auto ret = pipe(fds);
        if (ret != 0) {
            int saveErrno = errno;
            MMI_HILOGE("Create pipe error, errno: %{public}d, %{public}s", saveErrno, strerror(saveErrno));
            break;
        }
        g_mmiServiceReadFd = fds[0];
        g_hdfAdapterWriteFd = fds[1];
        MMI_HILOGI("Connect hdf init, fds:(read:%{public}d, write:%{public}d)", fds[0], fds[1]);

        ret = ConnectHDFService();
        if (ret != RET_OK) {
            MMI_HILOGE("Connect hdf init failed, ret:%{public}d", ret);
            break;
        }
        return true;
    } while (0);

    Uninit();
    return false;
}

void HDFAdapter::Uninit()
{
    CALL_DEBUG_ENTER;
    auto ret = DisconnectHDFService();
    if (ret != RET_OK) {
        MMI_HILOGE("Disconnect hdf init failed, ret:%{public}d", ret);
    }
    if (g_hdfAdapterWriteFd != -1) {
        ret = close(g_hdfAdapterWriteFd);
        if (ret != RET_OK) {
            MMI_HILOGE("Close fd failed, write fd:%{public}d, ret:%{public}d, errno:%{public}d", g_hdfAdapterWriteFd, ret, errno);
        }
        g_hdfAdapterWriteFd = -1;
    }
    if (g_mmiServiceReadFd != -1) {
        ret = close(g_mmiServiceReadFd);
        if (ret != RET_OK) {
            MMI_HILOGE("Close fd failed, read fd:%{public}d, ret:%{public}d, errno:%{public}d", g_mmiServiceReadFd, ret, errno);
        }
        g_mmiServiceReadFd = -1;
    }
    MMI_HILOGI("Disconnect hdf init success");
}

int32_t HDFAdapter::GetInputFd() const
{
    return g_mmiServiceReadFd;
}

void HDFAdapter::EventDispatch(epoll_event &ev)
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
    constexpr int32_t hdfEventSize = sizeof(HDFInputEvent);
    for (int32_t i = 0; i < onceProcessLimit; ++i) {
        const int32_t unreadSize = g_circBuf.UnreadSize();
        if (unreadSize < hdfEventSize) {
            break;
        }
        char *buf = const_cast<char *>(g_circBuf.ReadBuf());
        CHKPB(buf);
        HDFInputEvent *event = reinterpret_cast<HDFInputEvent *>(buf);
        CHKPB(event);
        if (!g_circBuf.SeekReadPos(hdfEventSize)) {
            MMI_HILOGW("Set read position error, and this error cannot be recovered, and the buffer will be reset."
                " hdfEventSize:%{public}d unreadSize:%{public}d", hdfEventSize, unreadSize);
            g_circBuf.Reset();
            break;
        }
        OnEventHandler(*event);
        if (g_circBuf.IsEmpty()) {
            g_circBuf.Reset();
            break;
        }
    }
}

int32_t HDFAdapter::ConnectHDFService()
{
    CALL_INFO_TRACE;
    if (g_inputInterface != nullptr) {
        MMI_HILOGW("HDF has alread connected.");
        return RET_OK;
    }
    int32_t ret = GetInputInterface(&g_inputInterface);
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("Initialize failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    CHKPR(g_inputInterface, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputManager, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputController, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputReporter, ERROR_NULL_POINTER);

    ret = g_inputInterface->iInputReporter->RegisterHotPlugCallback(&g_hostCb);
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("Call RegisterHotPlugCallback fail, ret:%{public}d", ret);
        return RET_ERR;
    }
    MMI_HILOGI("Call RegisterHotPlugCallback success");
    return RET_OK;
}

int32_t HDFAdapter::DisconnectHDFService()
{
    CALL_INFO_TRACE;
    if (g_inputInterface == nullptr) {
        MMI_HILOGE("HDF has alread disconnected.");
        return RET_OK;
    }
    ReleaseInputInterface(g_inputInterface);
    g_inputInterface = nullptr;
    return RET_OK;
}

int32_t HDFAdapter::HandleDeviceAdd(HDFDeviceStatusEvent &event)
{
    CALL_DEBUG_ENTER;
    CHKPR(g_inputInterface, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputManager, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputController, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputReporter, ERROR_NULL_POINTER);

    const uint32_t devIndex = event.devIndex;
    const uint32_t devType = event.devType;
    auto ret = g_inputInterface->iInputManager->OpenInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("OpenInputDevice failed, devIndex:%{public}u, ret:%{public}u", devIndex, ret);
        return RET_ERR;
    } else {
        MMI_HILOGI("OpenInputDevice success, devIndex:%{public}u", devIndex);
    }

    do {
        InputDeviceInfo *devInfo { nullptr };
        ret = g_inputInterface->iInputManager->GetInputDevice(devIndex, &devInfo);
        if (ret != INPUT_SUCCESS) {
            MMI_HILOGE("GetInputDevice failed, devIndex:%{public}u, ret:%{public}u", devIndex, ret);
            break;
        }
        if (devInfo == nullptr) {
            MMI_HILOGE("devInfo is nullptr, devIndex:%{public}u", devIndex);
            break;
        }
        event.devInfo = *devInfo;
        ret = g_inputInterface->iInputReporter->RegisterReportCallback(devIndex, &g_eventCb);
        if (ret != RET_OK) {
            MMI_HILOGE("RegisterReportCallback fail,devindex:%{public}u, devType:%{public}u, ret:%{public}u", devIndex, devType, ret);
            break;
        }
        MMI_HILOGI("RegisterReportCallback success,devindex:%{public}u, devType:%{public}u", devIndex, devType);
        return RET_OK;
    } while (0);

    ret = g_inputInterface->iInputManager->CloseInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("CloseInputDevice failed, devIndex:%{public}u, ret:%{public}d", devIndex, ret);
    } else {
        MMI_HILOGI("CloseInputDevice success, devIndex:%{public}u", devIndex);
    }
    return RET_ERR;
}

int32_t HDFAdapter::HandleDeviceRmv(int32_t devIndex, int32_t devType)
{
    CALL_DEBUG_ENTER;
    CHKPR(g_inputInterface, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputManager, ERROR_NULL_POINTER);

    auto ret = g_inputInterface->iInputManager->CloseInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("CloseInputDevice failed, devIndex:%{public}d, ret:%{public}d", devIndex, ret);
        return RET_ERR;
    }
    MMI_HILOGD("CloseInputDevice success, devIndex:%{public}d", devIndex);
    return RET_OK;
}

void HDFAdapter::OnEventHandler(const HDFInputEvent &event)
{
    CALL_DEBUG_ENTER;
    if (!event.IsDevNodeAddRmvEvent()) {
        CHKPV(inputCallback_);
        inputCallback_(event);
        return;
    }

    HDFDeviceStatusEvent tmpEvent;
    tmpEvent.devIndex = event.devIndex;
    tmpEvent.time = event.time;
    tmpEvent.devType = event.devType;
    tmpEvent.devStatus = event.devStatus;
    if (event.IsDevAdd()) {
        auto ret = HandleDeviceAdd(tmpEvent);
        if (ret != RET_OK) {
            MMI_HILOGE("Call HandleDeviceAdd fail, ret:%{public}d", ret);
            return;
        }
    } else {
        auto ret = HandleDeviceRmv(event.devIndex, event.devType);
        if (ret != RET_OK) {
            MMI_HILOGE("Call HandleDeviceRmv fail, ret:%{public}d", ret);
            return;
        }
    }
    CHKPV(statusCallback_);
    statusCallback_(tmpEvent);
}
} // namespace MMI
} // namespace OHOS
