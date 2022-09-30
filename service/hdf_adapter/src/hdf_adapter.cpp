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
#include "config_multimodal.h"
#include "error_multimodal.h"
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
IInputInterface *g_inputInterface { nullptr };
InputEventCb g_eventCb;
InputHostCb g_hostCb;
// sptr<IInputCallback> g_inputEventCallback = nullptr;
// sptr<IInputCallback> g_hotplugEventCallback = nullptr;
std::mutex g_mutex;
void InputEventSetTime(struct input_event &e, int64_t time)
{
    e.input_event_sec = time / 1000000;
	e.input_event_usec = time % 1000000;
}
inline uint64_t GetTime(const struct input_event &event)
{
    return event.input_event_sec * 1000000 + event.input_event_usec;
}
} // namespace

void HdfDeviceStatusChanged(int32_t devIndex, int32_t devType, HDFDevicePlugType statusType)
{
    input_event event;
    event.code = -1;
    event.type = (static_cast<int32_t>(statusType) << 16) | (devIndex << 8);
    event.value = devType;
    InputEventSetTime(event, GetSysClockTime());
    auto ret = write(g_hdfAdapterWriteFd, &event, sizeof(event));
    if (ret == -1) {
        int saveErrno = errno;
        MMI_HILOGE("Write pipe fail, errno:%{public}d, %{public}s", saveErrno, strerror(saveErrno));
    }
}

static void HotPlugCallback(const InputHotPlugEvent *event)
{
    CHKPV(event);
    auto plugType = (event->status ? HDFDevicePlugType::HDF_RMV_DEVICE : HDFDevicePlugType::HDF_ADD_DEVICE);
    HdfDeviceStatusChanged(event->devIndex, event->devType, plugType);
}

static void EventPkgCallback(const InputEventPackage **pkgs, uint32_t count, uint32_t devIndex)
{
    static constexpr uint16_t byteSize = 8;
    CHKPV(pkgs);
    uint32_t fixedCount = count;
    if (count > MMI_MAX_EVENT_PKG_NUM) {
        fixedCount = MMI_MAX_EVENT_PKG_NUM;
        MMI_HILOGE("Too big hdf event, count:%{public}d is beyond %{public}d", count, MMI_MAX_EVENT_PKG_NUM);
    }
    for (uint32_t i = 0; i < fixedCount; ++i) {
        input_event event;
        event.code = pkgs[i]->code;
        event.type = (pkgs[i]->type) | static_cast<uint16_t>(devIndex << byteSize);
        event.value = pkgs[i]->value;
        InputEventSetTime(event, pkgs[i]->timestamp);
        auto ret = write(g_hdfAdapterWriteFd, &event, sizeof(input_event));
        if (ret == -1) {
            int saveErrno = errno;
            MMI_HILOGE("Write pipe fail, errno:%{public}d, %{public}s", saveErrno, strerror(saveErrno));
        }
    }     
}

HdfAdapter::HdfAdapter()
{
    g_eventCb.EventPkgCallback = EventPkgCallback;
    g_hostCb.HotPlugCallback = HotPlugCallback;
}

int32_t HdfAdapter::ScanInputDevice()
{
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
            HdfDeviceStatusChanged(i.devIndex, i.devType, HDFDevicePlugType::HDF_ADD_DEVICE);
        }
    }
    MMI_HILOGI("Found %{public}d devices.", devCount);    
    return RET_OK;
}

bool HdfAdapter::Init(HdfEventCallback callback)
{
    CALL_DEBUG_ENTER;
    CHKPF(callback);
    callback_ = callback;

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
    auto data = static_cast<const input_event *>(ev.data.ptr);
    CHKPV(data);
    OnEventHandler(*data);
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

int32_t HdfAdapter::HandleDeviceAdd(int32_t devIndex, int32_t devType)
{
    CALL_DEBUG_ENTER;
    CHKPR(g_inputInterface, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputManager, ERROR_NULL_POINTER);
    CHKPR(g_inputInterface->iInputReporter, ERROR_NULL_POINTER);

    int32_t ret;
    ret = g_inputInterface->iInputManager->OpenInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("OpenInputDevice failed, devIndex:%{public}d, ret:%{public}d", devIndex, ret);
        return RET_ERR;
    } else {
        MMI_HILOGD("OpenInputDevice success, devIndex:%{public}d", devIndex);
    }

    do {
        ret = g_inputInterface->iInputReporter->RegisterReportCallback(devIndex, &g_eventCb);
        if (ret != RET_OK) {
            MMI_HILOGE("RegisterReportCallback fail,devindex:%{public}d, devType:%{public}d, ret:%{public}d", devIndex, devType, ret);
            break;;
        } else {
            MMI_HILOGE("RegisterReportCallback success,devindex:%{public}d, devType:%{public}d", devIndex, devType);
        }
        HdfDeviceStatusChanged(devIndex, devType, HDFDevicePlugType::HDF_ADD_DEVICE);
        // input_event event {.type = HDF_ADD_DEVICE, .code = devIndex, .value = devType, .time = 0};
        // callback_(event);
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

void HdfAdapter::OnEventHandler(const input_event &event)
{
    CALL_DEBUG_ENTER;
    CHKPV(callback_);
    /*
        31 - 24 | 23 - 16    | 15 - 8   | 7 - 0      |
        保留    | 热插拨     | devIndex | ev_xx type |
        rev     | plugStatus | devIndex | evType     |
     */
    int32_t devStatus = ((event.type | 0xff0000) >> 16);
    int32_t devIndex = ((event.type | 0xff00) >> 8);
    int32_t devType = (event.type | 0xff);
    if (devStatus == 1) { // dev add
        auto ret = HandleDeviceAdd(devIndex, devType);
        if (ret != RET_OK) {
            MMI_HILOGE("call HandleDeviceAdd fail, ret:%{public}d, type:%{public}d, code:%{public}d, value:%{public}d, time:%{public}lld",
                ret, event.type, event.code, event.value, GetTime(event));
            return;
        }
    } else if (devStatus == 2) { // dev remove
        auto ret = HandleDeviceRmv(devIndex, devType);
        if (ret != RET_OK) {
            MMI_HILOGE("call HandleDeviceRmv fail, ret:%{public}d, type:%{public}d, code:%{public}d, value:%{public}d, time:%{public}lld",
                ret, event.type, event.code, event.value, GetTime(event));
            return;
        }
    }
    callback_(event);
}

} // namespace MMI
} // namespace OHOS
