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

#include "distributed_input_adapter.h"

#include <algorithm>
#include <map>

namespace OHOS {
namespace MMI {
using namespace DistributedHardware::DistributedInput;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "DistributedInputAdapter"};
DistributedInputAdapter::DistributedInputAdapter()
{
    Init();
}

DistributedInputAdapter::~DistributedInputAdapter()
{
    UnInit();
}

bool DistributedInputAdapter::IsNeedFilterOut(const std::string &deviceId, const BusinessEvent &event)
{
    return DistributedInputKit::IsNeedFilterOut(deviceId, event);
}

int32_t DistributedInputAdapter::StartRemoteInput(const std::string &deviceId, const std::vector<std::string> &dhIds,
                                                  DICallback callback)
{
    SaveCallbackFunc(CallbackType::StartDInputCallbackDHIds, callback);
    sptr<IStartStopDInputVectorCallback> cb = new StartDInputCallbackDHIds();
    return DistributedInputKit::StartRemoteInput(deviceId, dhIds, cb);
}

int32_t DistributedInputAdapter::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                  const uint32_t &inputTypes, DICallback callback)
{
    sptr<IStartDInputCallback> cb = new StartDInputCallback();
    SaveCallbackFunc(CallbackType::StartDInputCallback, callback);
    return DistributedInputKit::StartRemoteInput(srcId, sinkId, inputTypes, cb);
}

int32_t DistributedInputAdapter::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                  const std::vector<std::string> &dhIds, DICallback callback)
{
    sptr<IStartStopDInputVectorCallback> cb = new StartDInputCallbackFds();
    SaveCallbackFunc(CallbackType::StartDInputCallbackFds, callback);
    return DistributedInputKit::StartRemoteInput(srcId, sinkId, dhIds, cb);
}

int32_t DistributedInputAdapter::StopRemoteInput(const std::string &deviceId, const std::vector<std::string> &dhIds,
                                                 DICallback callback)
{
    sptr<IStartStopDInputVectorCallback> cb = new StopDInputCallbackDHIds();
    SaveCallbackFunc(CallbackType::StopDInputCallbackDHIds, callback);
    return DistributedInputKit::StopRemoteInput(deviceId, dhIds, cb);
}

int32_t DistributedInputAdapter::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                 const uint32_t &inputTypes, DICallback callback)
{
    sptr<IStopDInputCallback> cb = new StopDInputCallback();
    SaveCallbackFunc(CallbackType::StopDInputCallback, callback);
    return DistributedInputKit::StopRemoteInput(srcId, sinkId, inputTypes, cb);
}

int32_t DistributedInputAdapter::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                 const std::vector<std::string> &dhIds, DICallback callback)
{
    sptr<IStartStopDInputVectorCallback> cb = new StopDInputCallbackFds();
    SaveCallbackFunc(CallbackType::StopDInputCallbackFds, callback);
    return DistributedInputKit::StopRemoteInput(srcId, sinkId, dhIds, cb);
}

int32_t DistributedInputAdapter::PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                    DICallback callback)
{
    SaveCallbackFunc(CallbackType::PrepareStartDInputCallbackSink, callback);
    sptr<IPrepareDInputCallback> cb = new PrepareStartDInputCallbackSink();
    return DistributedInputKit::PrepareRemoteInput(srcId, sinkId, cb);
}

int32_t DistributedInputAdapter::UnPrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                      DICallback callback)
{
    SaveCallbackFunc(CallbackType::UnPrepareStopDInputCallbackSink, callback);
    sptr<IUnprepareDInputCallback> cb = new UnPrepareStopDInputCallbackSink();
    return DistributedInputKit::UnprepareRemoteInput(srcId, sinkId, cb);
}

int32_t DistributedInputAdapter::PrepareRemoteInput(const std::string &deviceId, DICallback callback)
{
    SaveCallbackFunc(CallbackType::PrepareStartDInputCallback, callback);
    sptr<IPrepareDInputCallback> cb = new PrepareStartDInputCallback();
    return DistributedInputKit::PrepareRemoteInput(deviceId, cb);
}

int32_t DistributedInputAdapter::UnPrepareRemoteInput(const std::string &deviceId, DICallback callback)
{
    SaveCallbackFunc(CallbackType::UnPrepareStopDInputCallback, callback);
    sptr<IUnprepareDInputCallback> cb = new UnPrepareStopDInputCallback();
    return DistributedInputKit::UnprepareRemoteInput(deviceId, cb);
}

int32_t DistributedInputAdapter::RegisterStartStopCallback(StartStopResultCallback callback)
{
    CHKPR(callback, RET_ERR);
    startStopCallback_ = callback;
    return RET_OK;
}

int32_t DistributedInputAdapter::RegisterEventCallback(MouseStateChangeCallback callback)
{
    CHKPR(callback, RET_ERR);
    mouseStateChangeCallback_ = callback;
    return RET_OK;
}
int32_t DistributedInputAdapter::UnregisterEventCallback(MouseStateChangeCallback callback)
{
    CHKPR(callback, RET_ERR);
    mouseStateChangeCallback_ = nullptr;
    return RET_OK;
}

void DistributedInputAdapter::Init()
{
    sptr<IStartStopResultCallback> callback = new StartStopResCallback();
    DistributedInputKit::RegisterStartStopResultCallback(callback);
    mouseListener_ = new MouseStateChangeCallbackImpl();
    DistributedInputKit::RegisterEventListener(mouseListener_);
}

void DistributedInputAdapter::UnInit()
{
    DistributedInputKit::UnregisterEventListener(mouseListener_);
    mouseListener_ = nullptr;
    callbackMap_.clear();
}

void DistributedInputAdapter::SaveCallbackFunc(CallbackType type, DICallback callback)
{
    CHKPV(callback);
    DistributedAdapter->callbackMap_[type] = callback;
}

// Callback For old version
void DistributedInputAdapter::StartDInputCallback::OnResult(const std::string &devId, const uint32_t &inputTypes,
                                                            const int32_t &status)
{
    if (DistributedAdapter->callbackMap_.count(CallbackType::StartDInputCallback) == 0) {
        MMI_HILOGI("No StartDInputCallback");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StartDInputCallback](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StartDInputCallback);
}

void DistributedInputAdapter::StopDInputCallback::OnResult(const std::string &devId, const uint32_t &inputTypes,
                                                           const int32_t &status)
{
    if (DistributedAdapter->callbackMap_.count(CallbackType::StopDInputCallback) == 0) {
        MMI_HILOGI("No StopDInputCallback");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StopDInputCallback](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StopDInputCallback);
}

// Callback For DHIDS
void DistributedInputAdapter::StartDInputCallbackDHIds::OnResultFds(const std::string &srcId, const std::string &sinkId,
                                                                    const int32_t &status)
{
    MMI_HILOGI("OnResultFds srcId:%{public}s sinkId:%{public}s status:%{public}d", srcId.c_str(), sinkId.c_str(),
               status);
}

void DistributedInputAdapter::StartDInputCallbackDHIds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    MMI_HILOGI("StartDInputCallbackDHIds::OnResultDhids status : %{public}d", status);
    if (DistributedAdapter->callbackMap_.count(CallbackType::StartDInputCallbackDHIds) == 0) {
        MMI_HILOGI("No StartDInputCallbackDHIds");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StartDInputCallbackDHIds](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StartDInputCallbackDHIds);
}

void DistributedInputAdapter::StopDInputCallbackDHIds::OnResultFds(const std::string &srcId, const std::string &sinkId,
                                                                   const int32_t &status)
{
    MMI_HILOGI("OnResultFds srcId:%{public}s sinkId:%{public}s status:%{public}d", srcId.c_str(), sinkId.c_str(),
               status);
}

void DistributedInputAdapter::StopDInputCallbackDHIds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    MMI_HILOGI("StopDInputCallbackDHIds::OnResultDhids status : %{public}d", status);
    if (DistributedAdapter->callbackMap_.count(CallbackType::StopDInputCallbackDHIds) == 0) {
        MMI_HILOGI("No StopDInputCallbackDHIds");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StopDInputCallbackDHIds](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StopDInputCallbackDHIds);
}

// Callback For FDS
void DistributedInputAdapter::StartDInputCallbackFds::OnResultFds(const std::string &srcId, const std::string &sinkId,
                                                                  const int32_t &status)
{
    MMI_HILOGI("OnResultFds srcId:%{public}s sinkId:%{public}s status:%{public}d", srcId.c_str(), sinkId.c_str(),
               status);
}

void DistributedInputAdapter::StartDInputCallbackFds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    MMI_HILOGI("StartDInputCallbackFds::OnResultDhids status : %{public}d", status);
    if (DistributedAdapter->callbackMap_.count(CallbackType::StartDInputCallbackFds) == 0) {
        MMI_HILOGI("No StartDInputCallbackFds");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StartDInputCallbackFds](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StartDInputCallbackFds);
}

void DistributedInputAdapter::StopDInputCallbackFds::OnResultFds(const std::string &srcId, const std::string &sinkId,
                                                                 const int32_t &status)
{
    MMI_HILOGI("OnResultFds srcId:%{public}s sinkId:%{public}s status:%{public}d", srcId.c_str(), sinkId.c_str(),
               status);
}

void DistributedInputAdapter::StopDInputCallbackFds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    if (DistributedAdapter->callbackMap_.count(CallbackType::StopDInputCallbackFds) == 0) {
        MMI_HILOGI("No StopDInputCallbackFds");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StopDInputCallbackFds](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StopDInputCallbackFds);
}

// Callabck For Prepare this device
void DistributedInputAdapter::PrepareStartDInputCallback::OnResult(const std::string &devId, const int32_t &status)
{
    if (DistributedAdapter->callbackMap_.count(CallbackType::PrepareStartDInputCallback) == 0) {
        MMI_HILOGI("No PrepareStartDInputCallback");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::PrepareStartDInputCallback](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::PrepareStartDInputCallback);
}

void DistributedInputAdapter::UnPrepareStopDInputCallback::OnResult(const std::string &devId, const int32_t &status)
{
    if (DistributedAdapter->callbackMap_.count(CallbackType::UnPrepareStopDInputCallback) == 0) {
        MMI_HILOGI("No UnPrepareStopDInputCallback");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::UnPrepareStopDInputCallback](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::UnPrepareStopDInputCallback);
}

void DistributedInputAdapter::PrepareStartDInputCallbackSink::OnResult(const std::string &devId, const int32_t &status)
{
    if (DistributedAdapter->callbackMap_.count(CallbackType::PrepareStartDInputCallbackSink) == 0) {
        MMI_HILOGI("No PrepareStartDInputCallbackSink");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::PrepareStartDInputCallbackSink](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::PrepareStartDInputCallbackSink);
}

void DistributedInputAdapter::UnPrepareStopDInputCallbackSink::OnResult(const std::string &devId, const int32_t &status)
{
    CHKPV(DistributedAdapter->callbackMap_[CallbackType::UnPrepareStopDInputCallbackSink]);
    if (DistributedAdapter->callbackMap_.count(CallbackType::UnPrepareStopDInputCallbackSink) == 0) {
        MMI_HILOGI("No UnPrepareStopDInputCallbackSink");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::UnPrepareStopDInputCallbackSink](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::UnPrepareStopDInputCallbackSink);
}

void DistributedInputAdapter::StartStopResCallback::OnStart(const std::string &srcId, const std::string &sinkId,
                                                            std::vector<FdDhidDeviceData> &devData)
{
    MMI_HILOGI("srcId: %{public}s, sinkId: %{public}s,", srcId.c_str(), sinkId.c_str());
    for (auto item : devData) {
        MMI_HILOGI("dhid: %{public}s, fd: %{public}d,", item.dhid_.c_str(), item.fd_);
    }
    StartStopResultData data;
    data.srcId = srcId;
    data.sinkId = sinkId;
    data.isStart = true;
    std::for_each(devData.begin(), devData.end(),
                  [&data](FdDhidDeviceData &dev) { data.deviceDataMap[dev.fd_] = dev.dhid_; });

    CHKPV(DistributedAdapter->startStopCallback_);
    DistributedAdapter->startStopCallback_(data);
}
void DistributedInputAdapter::StartStopResCallback::OnStop(const std::string &srcId, const std::string &sinkId,
                                                           std::vector<FdDhidDeviceData> &devData)
{
    StartStopResultData data;
    data.srcId = srcId;
    data.sinkId = sinkId;
    data.isStart = false;
    std::for_each(devData.begin(), devData.end(),
                  [&data](FdDhidDeviceData &dev) { data.deviceDataMap[dev.fd_] = dev.dhid_; });

    CHKPV(DistributedAdapter->startStopCallback_);
    DistributedAdapter->startStopCallback_(data);
}

int32_t DistributedInputAdapter::MouseStateChangeCallbackImpl::OnMouseDownEvent(uint32_t type, uint32_t code,
                                                                                int32_t value)
{
    CHKPR(DistributedAdapter->mouseStateChangeCallback_, RET_OK);
    DistributedAdapter->mouseStateChangeCallback_(type, code, value);
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS