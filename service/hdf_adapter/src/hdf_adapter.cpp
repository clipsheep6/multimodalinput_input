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
#include <linux/input.h>
#include "config_multimodal.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "HdfAdapter" };
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
} // namespace

static void HotPlugCallback(const InputHotPlugEvent *event)
{
    CHKPV(event);
    HdfDeviceStatusChanged(event->devIndex, event->devType, (event->status ? HDF_RMV_DEVICE : HDF_ADD_DEVICE));
}

static void EventPkgCallback(const InputEventPackage **pkgs, uint32_t count, uint32_t devIndex)
{
    static constexpr uint16_t byteSize = 8;
    CHKPV(pkgs);
    uint32_t fixedCount = count;
    if (count > MmiHdfEventPacket::MAX_EVENT_PKG_NUM) {
        fixedCount = MmiHdfEventPacket::MAX_EVENT_PKG_NUM;
        MMI_HILOGE("Too big hdf event, count:%{public}d is beyond %{public}d", count, MmiHdfEventPacket::MAX_EVENT_PKG_NUM);
    }
    for (uint32_t i = 0; i < fixedCount; ++i) {
        input_event event;
        event.code = pkgs[i]->code;
        event.type = (pkgs[i]->type) | static_cast<uint16_t>(devIndex << byteSize);
        event.value = pkgs[i]->value;
        InputEventSetTime(event, pkgs[i]->timestamp);
        auto ret = write(g_hdfAdapterWriteFd, &event, sizeof(input_event);
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

void HdfAdapter::HdfDeviceStatusChanged(int32_t devIndex, int32_t devType, int32_t statusType)
{
    input_event event;
    event.code = -1;
    event.type = (statusType << 16) | (devIndex << 8);
    event.value = devType;
    InputEventSetTime(event, GetSysClockTime());
    auto ret = write(g_hdfAdapterWriteFd, &event, sizeof(event));
    if (ret == -1) {
        int saveErrno = errno;
        MMI_HILOGE("Write pipe fail, errno:%{public}d, %{public}s", saveErrno, strerror(saveErrno));
    }
}

int32_t HdfAdapter::ScanInputDevice()
{
    CALL_DEBUG_ENTER;
    CHKPR(g_inputInterface, RET_ERR);
    CHKPR(g_inputInterface->iInputManager, RET_ERR);
    CHKPR(g_inputInterface->iInputReporter, RET_ERR);
    std::array<InputDevDesc, MmiHdfDevDescPacket::MAX_INPUT_DEVICE_COUNT> devDescs;
    int32_t ret = g_inputInterface->iInputManager->ScanInputDevice(devDescs.data(), devDescs.size);
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("call ScanInputDevice failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    int32_t devCount = 0;
    for (const auto &i : devDescs) {
        if (i.devIndex != 0) {
            ++devCount;
            HdfDeviceStatusChanged(i.devIndex, i.devType, HDF_ADD_DEVICE);
        }
    }
    MMI_HILOGI("Found %{public}d devices.", devCount);    
    return RET_OK;
}

bool HdfAdapter::Init(HdfEventCallback callback, const std::string &seat_id)
{
    CALL_DEBUG_ENTER;
    CHKPF(callback);
    callback_ = callback;
    seat_id_ = seat_id;
    if (seat_id_.empty()) {
        seat_id_ = DEF_SEAT_ID;
    }

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
    mprintf(fd, "uds_server: count=%d", sessionsMap_.size());    
}

int32_t HdfAdapter::GetInputFd() const
{
    return g_mmiServiceReadFd;
}

void HdfAdapter::EventDispatch(struct epoll_event &ev)
{
    CALL_DEBUG_ENTER;
    CHKPV(ev.data.ptr);
    if ((ev.events & EPOLLERR) || (ev.events & EPOLLHUP)) {
        MMI_HILOGF("Epoll unrecoverable error");
        free(ev.data.ptr);
        ev.data.ptr = nullptr;
        return;
    }
    auto data = static_cast<const MmiHdfPacket *>(ev.data.ptr);
    CHKPV(data);
    OnEventCallback(*data);
}

void HdfAdapter::OnEventCallback(const MmiHdfPacket &pkt)
{
    std::ostringstream oss;
    oss << std::dec << "pkt.size: " << pkt.size
        << "(" << pkt.size << ","
        << pkt.type << ",";
    
    eventRecords_.push_back(oss.str());
    //CALL_DEBUG_ENTER;
    #if 0
    auto pktType = pkt.type;
    switch(pktType) {
        case HDF_ADD_DEVICE:
        {
            const MmiHdfDevDescPacket &descPkt = static_cast<const MmiHdfDevDescPacket>(pkt);
            for (auto &i : descPkt.descs) {
                HandleDeviceAdd(i.devIndex, i.devType);
            }
        }        
        break;
        case HDF_RMV_DEVICE:
        {
            const MmiHdfDevDescPacket &descPkt = static_cast<const MmiHdfDevDescPacket>(pkt);
            for (auto &i : pkt.descs) {
                HandleDeviceRmv(i.devIndex, i.devType);
            }
        }        
        break;
        case HDF_EVENT:
        {
            const MmiHdfEventPacket &eventPkt = static_cast<const MmiHdfEventPacket>(pkt);
            for (auto &i : eventPkt.events) {
                HandleDeviceEvent(i.devIndex, i.devType);
            }
        }        
        break;
        default:
        {
            MMI_HILOGE("unknown type:%{public}d", pktType);
        }
        break;
    }
    #endif
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

void HdfAdapter::HandleDeviceAdd(int32_t devIndex, int32_t devType)
{
    CALL_DEBUG_ENTER;
    CHKPV(g_inputInterface);
    CHKPV(g_inputInterface->iInputManager);
    CHKPV(g_inputInterface->iInputReporter);

    int32_t ret;
    ret = g_inputInterface->iInputManager->OpenInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("OpenInputDevice failed, devIndex:%{public}d, ret:%{public}d", devIndex, ret);
        return;
    } else {
        MMI_HILOGD("OpenInputDevice success, devIndex:%{public}d", devIndex);
    }

    do {
        ret = g_inputInterface->iInputReporter->RegisterReportCallback(devIndex, &g_eventCb);
        if (ret != RET_OK) {
            MMI_HILOGE("RegisterReportCallback fail,devindex:%{public}d, devType:%{public}d, ret:%{public}d", devIndex, devType, ret);
            return;
        } else {
            MMI_HILOGE("RegisterReportCallback success,devindex:%{public}d, devType:%{public}d", devIndex, devType);
        }
        input_event event {.type = HDF_ADD_DEVICE, .code = devIndex, .value = devType, .time = 0};
        callback_(event);
        return;
    } while (0);

    ret = g_inputInterface->iInputManager->CloseInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("CloseInputDevice failed, devIndex:%{public}d, ret:%{public}d", devIndex, ret);
        return;
    } else {
        MMI_HILOGD("CloseInputDevice success, devIndex:%{public}d", devIndex);
    }
}

void HdfAdapter::HandleDeviceRmv(int32_t devIndex, int32_t devType)
{
    CALL_DEBUG_ENTER;
    CHKPV(g_inputInterface);
    CHKPV(g_inputInterface->iInputManager);
    CHKPV(g_inputInterface->iInputReporter);

    int32_t ret;
    ret = g_inputInterface->iInputManager->CloseInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("CloseInputDevice failed, devIndex:%{public}d, ret:%{public}d", devIndex, ret);
        return;
    } else {
        MMI_HILOGD("CloseInputDevice success, devIndex:%{public}d", devIndex);
    }    
}

void HdfAdapter::HandleDeviceEvent(int32_t devIndex, int32_t type, int32_t code, int32_t value, int64_t timestamp)
{
    CALL_DEBUG_ENTER;    
}

void HdfAdapter::OnEventHandler(const input_event &event)
{
    CALL_DEBUG_ENTER;
    CHKPV(callback_);
    callback_(event);
}

} // namespace MMI
} // namespace OHOS
