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

void OHOS::MMI::HdfAdapter::OnHDFHotPlugCallback(const HotPlugEvent *event)
{
    CHKPV(event);
    MMI_HILOGD("status:%{public}u,index:%{public}u,type:%{public}u", event->status, event->devIndex, event->devType);
    if (hdfAdapterWriteFd_ == -1) {
        MMI_HILOGE("hdfAdapterWriteFd_ is invalid.");
        return;
    }

    MmiHdfDevDescPacket pkt;
    pkt.head.size = sizeof(pkt.desc[0]);
    pkt.head.type = (event->status ? HDF_RMV_DEVICE : HDF_ADD_DEVICE);
    pkt.descs[0].devIndex = event->devIndex;
    pkt.descs[0].devType = event->devType;
    auto ret = write(hdfAdapterWriteFd_, &pkt, sizeof(pkt.head) + pkt.head.size);
    if (ret == -1) {
        int saveErrno = errno;
        MMI_HILOGE("Write pipe fail, errno:%{public}d, %{public}s", saveErrno, strerror(saveErrno));
    }
}

void OHOS::MMI::HdfAdapter::OnHDFEventCallback(const KeyEventHandler **pkgs, uint32_t count, uint32_t devIndex)
{
    static constexpr uint16_t byteSize = 8;
    CHKPV(pkgs);    
    uint32_t fixedCount = std::min(count, MmiHdfEventPacket::MAX_EVENT_PKG_NUM);
    MmiHdfEventPacket pkt;
    pkt.head.size = sizeof(MmiHdfEventPacket::events[0]) * fixedCount;
    pkt.head.type = HDF_EVENT;
    for (uint32_t i = 0; i < fixedCount; ++i) {
        pkt.events[i].code = pkgs[i]->code;
        pkt.events[i].type = (pkgs[i]->type) | static_cast<uint16_t>(devIndex << byteSize);
        pkt.events[i].value = pkgs[i]->value;
        pkt.events[i].input_event_sec = (pkgs[i]->timestamp) / (USEC_PER_SEC);
        pkt.events[i].input_event_usec = (pkgs[i]->timestamp) % (USEC_PER_SEC);
    }
    auto ret = write(hdfAdapterWriteFd_, &pkt, sizeof(pkt.head) + pkt.head.size);
    if (ret == -1) {
        int saveErrno = errno;
        MMI_HILOGE("Write pipe fail, errno:%{public}d, %{public}s", saveErrno, strerror(saveErrno));
    } 
}

int32_t OHOS::MMI::HdfAdapter::ScanInputDevice()
{
    CALL_DEBUG_ENTER;
    CHKPR(inputInterface_, RET_ERR);
    CHKPR(inputInterface_->iInputManager, RET_ERR);
    CHKPR(inputInterface_->iInputReporter, RET_ERR);

    InputDevDesc mountDevIndex[TOTAL_INPUT_DEVICE_COUNT] = {};
    int32_t ret = inputInterface_->iInputManager->ScanInputDevice(mountDevIndex, MmiHdfDevDescPacket::MAX_INPUT_DEVICE_COUNT);
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("call ScanInputDevice failed, ret:%{public}d", ret);
        return RET_ERR;
    }

    MmiHdfDevDescPacket pkt = {};
    int32_t devCount = 0;
    for (int32_t i = 0; i < MmiHdfDevDescPacket::MAX_INPUT_DEVICE_COUNT; i++) {
        if (mountDevIndex[i].devIndex != 0) {
            pkt.descs[devCount] = mountDevIndex[i];
            ++devCount;
        }
    }
    pkt.head.size = devCount * sizeof(InputDevDesc);
    ptk.head.type = MmiHdfEventPacketType::HDF_ADD_DEVICE;
    auto ret = write(hdfAdapterWriteFd_, &pkt, sizeof(pkt.head) + pkt.head.size);
    if (ret == -1) {
        int saveErrno = errno;
        MMI_HILOGE("Write pipe fail, errno:%{public}d, %{public}s", saveErrno, strerror(saveErrno));
        return RET_ERR;
    }
    return RET_OK;
}

int32_t OHOS::MMI::HdfAdapter::Init(HdfEventCallback callback, const std::string &seat_id = "seat0")
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
        ret = ConnectHDFInit();
        if (ret != RET_OK) {
            MMI_HILOGE("connect hdf init failed, ret:%{public}d", ret);
            break;
        }

        int32_t fds[2] = {-1, -1};
        int32_t ret = pipe(fds);
        if (ret != 0) {
            int saveErrno = errno;
            MMI_HILOGE("create pipe error, errno: %{public}d, %{public}s", saveErrno, strerror(saveErrno));       
            break;
        }
        hdfAdapterWriteFd_ = fds[0];
        mmiServiceReadFd_ = fds[1];
        return RET_OK;
    } while (0);
    
    ret = DisconnectHDFInit();
    if (ret != RET_OK) {
        MMI_HILOGE("disconnect hdf init failed, ret:%{public}d", ret);
    }
    if (hdfAdapterWriteFd_ != -1) {
        ret = close(hdfAdapterWriteFd_);
        if (ret != RET_OK) {
            MMI_HILOGE("disconnect hdf init failed, ret:%{public}d", ret);
        }
        hdfAdapterWriteFd_ = -1;
    }
    if (mmiServiceReadFd_ != -1) {
        ret = close(mmiServiceReadFd_);
        if (ret != RET_OK) {
            MMI_HILOGE("disconnect hdf init failed, ret:%{public}d", ret);
        }
        mmiServiceReadFd_ = -1;
    }
    return RET_ERR;
}

void OHOS::MMI::HdfAdapter::DeInit()
{
    CALL_DEBUG_ENTER;
    auto ret = DisconnectHDFInit();    
    if (ret != RET_OK) {
        MMI_HILOGE("disconnect hdf init failed, ret:%{public}d", ret);
        return;
    }
    MMI_HILOGI("disconnect hdf init success");
}

void OHOS::MMI::HdfAdapter::EventDispatch(struct epoll_event &ev)
{
    CALL_DEBUG_ENTER;
    CHKPV(ev.data.ptr);
    if ((ev.events & EPOLLERR) || (ev.events & EPOLLHUP)) {
        MMI_HILOGF("Epoll unrecoverable error,"
                   "The service must be restarted. fd:%{public}d",
                   fd);
        free(ev.data.ptr);
        ev.data.ptr = nullptr;
        return;
    }
    auto data = static_cast<const MmiHdfPacket *>(ev.data.ptr);
    CHKPV(data);
    OnEventCallback(*data);
}

void OHOS::MMI::HdfAdapter::OnEventCallback(const MmiHdfPacket &pkt)
{
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
}

int32_t OHOS::MMI::HdfAdapter::ConnectHDFInit()
{
    CALL_DEBUG_ENTER();
    if (inputInterface_ != nullptr) {
        MMI_HILOGE("HDF has alread connected.");
        return RET_OK;
    }
    int32_t ret = GetInputInterface(&inputInterface_);
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("Initialize failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    CHKPR(inputInterface_, RET_ERR);
    CHKPR(inputInterface_->iInputManager, RET_ERR);
    CHKPR(inputInterface_->iInputReporter, RET_ERR);

    auto ret = inputInterface_->iInputReporter->RegisterHotPlugCallback(&HdfAdapter::OnHDFHotPlugCallback);
    if (ret != INPUT_SUCCESS) {
        MMI_HILOGE("call RegisterHotPlugCallback fail, ret:%{public}d", ret);
        return RET_ERR;
    }
    MMI_HILOGI("call RegisterHotPlugCallback success");
    return RET_OK;
}

int32_t OHOS::MMI::HdfAdapter::DisconnectHDFInit()
{
    CALL_DEBUG_ENTER;
    if (inputInterface_ == nullptr) {
        MMI_HILOGE("HDF has alread disconnected.");
        return RET_OK;
    }
    int32_t ret = ReleaseInputInterface(inputInterface_);
    if (ret != 0) {
        MMI_HILOGE("Initialize failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    inputInterface_ = nullptr;
    return RET_OK;
}

void OHOS::MMI::HdfAdapter::HandleDeviceAdd(int32_t devIndex, int32_t devType)
{
    CALL_DEBUG_ENTER;
    CHKPR(inputInterface_, RET_ERR);
    CHKPR(inputInterface_->iInputManager, RET_ERR);
    CHKPR(inputInterface_->iInputReporter, RET_ERR);

    int32_t ret;
    ret = inputInterface_->iInputManager->OpenInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("OpenInputDevice failed, devIndex:%{public}d, ret:%{public}d", devIndex, ret);
        return;
    } else {
        MMI_HILOGD("OpenInputDevice success, devIndex:%{public}d", devIndex);
    }

    do {
        ret = inputInterface_->iInputReporter->RegisterReportCallback(devIndex, &HdfAdapter::OnHDFEventCallback);
        if (ret != RET_OK) {
            MMI_HILOGE("RegisterReportCallback fail,devindex:%{public}d, devType:%{public}d, ret:%{public}d", devIndex, devType, ret);
            return;
        } else {
            MMI_HILOGE("RegisterReportCallback success,devindex:%{public}d, devType:%{public}d", devIndex, devType);
        }
        MmiHdfEvent event {.type = HDF_ADD_DEVICE, .code = devIndex, .value = devType, .time = 0};
        callback_(event);
        return;
    } while (0);

    ret = inputInterface_->iInputManager->CloseInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("CloseInputDevice failed, devIndex:%{public}d, ret:%{public}d", devIndex, ret);
        return RET_ERR;
    } else {
        MMI_HILOGD("CloseInputDevice success, devIndex:%{public}d", devIndex);
    }
}

void OHOS::MMI::HdfAdapter::HandleDeviceRmv(int32_t devIndex, int32_t devType)
{
    CALL_DEBUG_ENTER;
    CHKPR(inputInterface_, RET_ERR);
    CHKPR(inputInterface_->iInputManager, RET_ERR);
    CHKPR(inputInterface_->iInputReporter, RET_ERR);

    int32_t ret;
    ret = inputInterface_->iInputManager->CloseInputDevice(devIndex);
    if (ret != RET_OK) {
        MMI_HILOGE("CloseInputDevice failed, devIndex:%{public}d, ret:%{public}d", devIndex, ret);
        return RET_ERR;
    } else {
        MMI_HILOGD("CloseInputDevice success, devIndex:%{public}d", devIndex);
    }    
}

void OHOS::MMI::HdfAdapter::HandleDeviceEvent(int32_t devIndex, int32_t type, int32_t code, int32_t value, int64_t timestamp)
{
    
}

void OHOS::MMI::OnEventHandler(const MmiHdfEvent &data)
{
    CALL_DEBUG_ENTER;
    CHKPV(callback_);
    callback_(data);
}
