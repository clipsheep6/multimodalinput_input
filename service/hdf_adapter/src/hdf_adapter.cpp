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

#include "hdf_adapter.h"
#include <array>
#include <linux/input.h>
#include "circle_stream_buffer.h"
#include "config_multimodal.h"
#include "error_multimodal.h"
#include "stack_dumper_helper.h"
#include "util_ex.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "HdfAdapter" };
inline constexpr uint32_t MMI_MAX_EVENT_PKG_NUM = 100;
inline constexpr uint32_t MMI_MAX_INPUT_DEVICE_COUNT = 32;
const std::string DEF_INPUT_SEAT = "seat0";
int32_t g_hdfAdapterWriteFd { -1 };
int32_t g_mmiServiceReadFd { -1 };
CircleStreamBuffer g_circBuf;
IInputInterface *g_inputInterface { nullptr };
InputEventCb g_eventCb;
InputHostCb g_hostCb;
std::mutex g_mutex;
std::map<uint32_t, int32_t> lastDevInfo;
} // namespace

void HdfDeviceStatusChanged(int32_t devIndex, int32_t devType, HdfInputEventDevStatus devStatus)
{
    CHK_PID_AND_TID();
    HdfInputEvent event;
    event.eventType = static_cast<uint32_t>(HdfInputEventType::DEV_NODE_ADD_RMV);
    event.devIndex = devIndex;
    event.devType = devType;
    event.devStatus = static_cast<uint32_t>(devStatus);
    event.time = GetSysClockTime();
    MMI_HILOGE("devIndex:%{public}u, devType:%{public}u, devStatus:%{public}u,time:%{public}llu",
        devIndex, event.devType, event.devStatus, event.time);
    auto ret = write(g_hdfAdapterWriteFd, &event, sizeof(event));
    if (ret == -1) {
        int saveErrno = errno;
        MMI_HILOGE("Write pipe fail, errno:%{public}d, %{public}s", saveErrno, strerror(saveErrno));
    }
}

static void HotPlugCallback(const InputHotPlugEvent *event)
{
    CHK_PID_AND_TID();
    CHKPV(event);
    auto devStatus = ((event->status == 0) ? HdfInputEventDevStatus::HDF_ADD_DEVICE : HdfInputEventDevStatus::HDF_RMV_DEVICE);
    HdfDeviceStatusChanged(event->devIndex, event->devType, devStatus);
}

static inline bool IsDupTouchBtnKey(const InputEventPackage &r, uint32_t devIndex)
{
    if (r.type == EV_KEY && r.code == BTN_TOUCH) {
        auto it = lastDevInfo.find(devIndex);
        if (it != lastDevInfo.end() && it->second == r.value) {
            return true;
        } else {
            lastDevInfo.emplace(devIndex, r.value);
        }
    }
    return false;
}

static inline void WriteToPipe(const InputEventPackage &r, uint32_t devIndex)
{
    HdfInputEvent event;
    event.eventType = static_cast<uint32_t>(HdfInputEventType::DEV_NODE_EVENT);
    event.devIndex = devIndex;
    event.type = r.type;
    event.value = r.value;
    event.time = r.timestamp;
    auto f = [&]() {
        auto ret = write(g_hdfAdapterWriteFd, &event, sizeof(HdfInputEvent));
        if (ret == -1) {
            int saveErrno = errno;
            MMI_HILOGE("Write pipe fail, errno:%{public}d, %{public}s", saveErrno, strerror(saveErrno));
        }
    };
    if ((r.type == 0) && (r.code == 0) && (r.value == 0)) {
        event.code = SYN_MT_REPORT;
        f();
    }
    event.code = r.code;
    f();
    MMI_HILOGD("devIndex:%{public}u,code:%{public}u,type:%{public}u,value:%{public}u,time:%{public}llu,",
        devIndex, r.code, r.type, r.value, r.timestamp);
}

static void EventPkgCallback(const InputEventPackage **pkgs, uint32_t count, uint32_t devIndex)
{
    CHK_PID_AND_TID();
    CHKPV(pkgs);
    uint32_t fixedCount = count;
    if (count > MMI_MAX_EVENT_PKG_NUM) {
        fixedCount = MMI_MAX_EVENT_PKG_NUM;
        MMI_HILOGE("Too big hdf event, count:%{public}d is beyond %{public}d", count, MMI_MAX_EVENT_PKG_NUM);
    }
    for (uint32_t i = 0; i < fixedCount; ++i) {
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

HdfAdapter::HdfAdapter()
{
    CHK_PID_AND_TID();
    g_eventCb.EventPkgCallback = EventPkgCallback;
    g_hostCb.HotPlugCallback = HotPlugCallback;
}

int32_t HdfAdapter::ScanInputDevice()
{
    CHK_PID_AND_TID();
    CALL_DEBUG_ENTER;
    CHKPR(g_inputInterface, RET_ERR);
    CHKPR(g_inputInterface->iInputManager, RET_ERR);
    CHKPR(g_inputInterface->iInputReporter, RET_ERR);
    std::array<InputDevDesc, MMI_MAX_INPUT_DEVICE_COUNT> devDescs;
    int32_t ret = g_inputInterface->iInputManager->ScanInputDevice(devDescs.data(), devDescs.size());
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("call ScanInputDevice failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    int32_t devCount = 0;
    for (const auto &i : devDescs) {
        if (i.devIndex != 0) {
            ++devCount;
            HdfDeviceStatusChanged(i.devIndex, i.devType, HdfInputEventDevStatus::HDF_ADD_DEVICE);
        }
    }
    MMI_HILOGI("Found %{public}d devices.", devCount);
    return RET_OK;
}

bool HdfAdapter::Init(HDFDeviceStatusEventCallback statusCallback, HDFDeviceInputEventCallback inputCallback)
{
    CALL_DEBUG_ENTER;
    CHKPF(statusCallback);
    CHKPF(inputCallback);
    statusCallback_ = statusCallback;
    inputCallback_ = inputCallback;

    int ret;
    do {
        int32_t fds[2] = {-1, -1};
        int32_t ret = pipe(fds);
        if (ret != 0) {
            int saveErrno = errno;
            MMI_HILOGE("create pipe error, errno: %{public}d, %{public}s", saveErrno, strerror(saveErrno));
            break;
        }
        g_mmiServiceReadFd = fds[0];
        g_hdfAdapterWriteFd = fds[1];
        MMI_HILOGD("connect hdf init, fds:(%{public}d, (%{public}d)", fds[0], fds[1]);
        MMI_HILOGE("connect hdf init, fds:(%{public}d, (%{public}d), g_mmiServiceReadFd:%{public}d, g_hdfAdapterWriteFd:%{public}d",
            fds[0], fds[1], g_mmiServiceReadFd, g_hdfAdapterWriteFd);

        ret = ConnectHDFInit();
        if (ret != RET_OK) {
            MMI_HILOGE("connect hdf init failed, ret:%{public}d", ret);
            break;
        }
        return true;
    } while (0);

    ret = DisconnectHDFInit();
    if (ret != RET_OK) {
        MMI_HILOGE("disconnect hdf init failed, ret:%{public}d", ret);
    }
    if (g_hdfAdapterWriteFd != -1) {
        ret = close(g_hdfAdapterWriteFd);
        if (ret != RET_OK) {
            MMI_HILOGE("disconnect hdf init failed, ret:%{public}d", ret);
        }
        g_hdfAdapterWriteFd = -1;
    }
    if (g_mmiServiceReadFd != -1) {
        ret = close(g_mmiServiceReadFd);
        if (ret != RET_OK) {
            MMI_HILOGE("disconnect hdf init failed, ret:%{public}d", ret);
        }
        g_mmiServiceReadFd = -1;
    }
    return false;
}

void HdfAdapter::DeInit()
{
    CALL_DEBUG_ENTER;
    auto ret = DisconnectHDFInit();
    if (ret != RET_OK) {
        MMI_HILOGE("disconnect hdf init failed, ret:%{public}d", ret);
        return;
    }
    MMI_HILOGI("disconnect hdf init success");
}

void HdfAdapter::Dump(int32_t fd, const std::vector<std::string> &args)
{
    mprintf(fd, "HDF adapter information:");
    // TODO:
}

int32_t HdfAdapter::GetInputFd() const
{
    return g_mmiServiceReadFd;
}

void HdfAdapter::EventDispatch(epoll_event &ev)
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
        MMI_HILOGE("fd(%{public}d) is invalid.", fd);
        return;
    }
    constexpr int32_t szBufSize = 1024;
    char szBuf[szBufSize] = {};
    auto retSize = read(fd, szBuf, szBufSize);
    if (retSize < 0) {
        errno_t errnoSaved = errno;
        MMI_HILOGE("read pipe failed, errno:%{public}d, %{public}s", errnoSaved, strerror(errnoSaved));
        return;
    } else if (retSize == 0) {
        MMI_HILOGD("pipe no data");
        return;
    }
    if (!g_circBuf.Write(szBuf, retSize)) {
        MMI_HILOGW("Write data failed. size:%{public}zu", retSize);
    }

    constexpr int32_t onceProcessLimit = 100;
    constexpr int32_t hdfEventSize = sizeof(HdfInputEvent);
    for (int32_t i = 0; i < onceProcessLimit; i++) {
        const int32_t unreadSize = g_circBuf.UnreadSize();
        if (unreadSize < hdfEventSize) {
            break;
        }
        char *buf = const_cast<char *>(g_circBuf.ReadBuf());
        CHKPB(buf);
        struct HdfInputEvent *event = reinterpret_cast<struct HdfInputEvent *>(buf);
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

int32_t HdfAdapter::ConnectHDFInit()
{
    CALL_DEBUG_ENTER;
    if (g_inputInterface != nullptr) {
        MMI_HILOGE("HDF has alread connected.");
        return RET_OK;
    }
    int32_t ret = GetInputInterface(&g_inputInterface);
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("Initialize failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    CHKPR(g_inputInterface, RET_ERR);
    CHKPR(g_inputInterface->iInputManager, RET_ERR);
    CHKPR(g_inputInterface->iInputReporter, RET_ERR);

    ret = g_inputInterface->iInputReporter->RegisterHotPlugCallback(&g_hostCb);
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("call RegisterHotPlugCallback fail, ret:%{public}d", ret);
        return RET_ERR;
    }
    MMI_HILOGI("call RegisterHotPlugCallback success");
    return RET_OK;
}

int32_t HdfAdapter::DisconnectHDFInit()
{
    CALL_DEBUG_ENTER;
    if (g_inputInterface == nullptr) {
        MMI_HILOGE("HDF has alread disconnected.");
        return RET_OK;
    }
    ReleaseInputInterface(g_inputInterface);
    g_inputInterface = nullptr;
    return RET_OK;
}

int32_t HdfAdapter::HandleDeviceAdd(HDFDeviceStatusEvent &event)
{
    CALL_DEBUG_ENTER;
    CHKPR(g_inputInterface, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputManager, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputController, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputReporter, ERROR_NULL_POINTER);

    const int32_t devIndex = event.devIndex;
    const int32_t devType = event.devType;
    int32_t ret;
    ret = g_inputInterface->iInputManager->OpenInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("OpenInputDevice failed, devIndex:%{public}d, ret:%{public}d", devIndex, ret);
        return RET_ERR;
    } else {
        MMI_HILOGD("OpenInputDevice success, devIndex:%{public}d", devIndex);
    }

    do {
        InputDeviceInfo *iDevInfo_ { nullptr };
        ret = g_inputInterface->iInputManager->GetInputDevice(devIndex, &iDevInfo_);
        if (ret != INPUT_SUCCESS) {
            MMI_HILOGE("GetInputDevice error, devIndex:%{public}d", devIndex);
            break;
        }
        if (iDevInfo_ == nullptr) {
            MMI_HILOGE("iDevInfo_ is nullptr, devIndex:%{public}d", devIndex);
            break;
        }
        event.devInfo = *iDevInfo_;
        ret = g_inputInterface->iInputReporter->RegisterReportCallback(devIndex, &g_eventCb);
        if (ret != RET_OK) {
            MMI_HILOGE("RegisterReportCallback fail,devindex:%{public}d, devType:%{public}d, ret:%{public}d", devIndex, devType, ret);
            break;;
        } else {
            MMI_HILOGE("RegisterReportCallback success,devindex:%{public}d, devType:%{public}d", devIndex, devType);
        }
        return RET_OK;
    } while (0);

    ret = g_inputInterface->iInputManager->CloseInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("CloseInputDevice failed, devIndex:%{public}d, ret:%{public}d", devIndex, ret);
    } else {
        MMI_HILOGD("CloseInputDevice success, devIndex:%{public}d", devIndex);
    }
    return RET_ERR;
}

int32_t HdfAdapter::HandleDeviceRmv(int32_t devIndex, int32_t devType)
{
    CALL_DEBUG_ENTER;
    CHKPR(g_inputInterface, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputManager, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputReporter, ERROR_NULL_POINTER);

    int32_t ret;
    ret = g_inputInterface->iInputManager->CloseInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("CloseInputDevice failed, devIndex:%{public}d, ret:%{public}d", devIndex, ret);
        return RET_ERR;
    } else {
        MMI_HILOGD("CloseInputDevice success, devIndex:%{public}d", devIndex);
    }
    return RET_OK;
}

void HdfAdapter::OnEventHandler(const HdfInputEvent &event)
{
    CALL_DEBUG_ENTER;
    if (event.IsDevNodeAddRmvEvent()) {
        MMI_HILOGE("zpc:type:addrmv:eventType:%{public}u,devIndex:%{public}u,devType:%{public}u,devStatus:%{public}u, time:%{public}llu",
                    event.eventType, event.devIndex, event.devType, event.devStatus, event.time);
        HDFDeviceStatusEvent retEvent;
        retEvent.devIndex = event.devIndex;
        retEvent.time = event.time;
        retEvent.devType = event.devType;
        retEvent.devStatus = event.devStatus;
        if (event.IsDevAdd()) { // dev add
            auto ret = HandleDeviceAdd(retEvent);
            if (ret != RET_OK) {
                MMI_HILOGE("call HandleDeviceAdd fail, ret:%{public}d", ret);
                return;
            }
        } else { // dev remove
            auto ret = HandleDeviceRmv(event.devIndex, event.devType);
            if (ret != RET_OK) {
                MMI_HILOGE("call HandleDeviceRmv fail, ret:%{public}d", ret);
                return;
            }
        }
        CHKPV(statusCallback_);
        statusCallback_(retEvent);
        return;
    }

    CHKPV(inputCallback_);
    inputCallback_(event);
}

} // namespace MMI
} // namespace OHOS
