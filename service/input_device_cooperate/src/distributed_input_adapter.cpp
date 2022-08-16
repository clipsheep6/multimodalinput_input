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
#include <mutex>

#include "error_multimodal.h"
#include "timer_manager.h"

namespace OHOS {
namespace MMI {
using namespace DistributedHardware::DistributedInput;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "DistributedInputAdapter"};
namespace {
constexpr int32_t DEFAULT_DELAY_TIME = 4000;
constexpr int32_t RETRY_TIME = 2;
std::mutex adapterLock;
} // namespace
DistributedInputAdapter::DistributedInputAdapter()
{
    Init();
}

DistributedInputAdapter::~DistributedInputAdapter()
{
    Release();
}

bool DistributedInputAdapter::IsNeedFilterOut(const std::string &deviceId, const BusinessEvent &event)
{
    return DistributedInputKit::IsNeedFilterOut(deviceId, event);
}

int32_t DistributedInputAdapter::StartRemoteInput(const std::string &deviceId, const std::vector<std::string> &dhIds,
                                                  DICallback callback)
{
    SaveCallbackFunc(CallbackType::StartDInputCallbackDHIds, callback);
    sptr<IStartStopDInputVectorCallback> cb = new (std::nothrow) StartDInputCallbackDHIds();
    CHKPR(cb, ERROR_NULL_POINTER);
    return DistributedInputKit::StartRemoteInput(deviceId, dhIds, cb);
}

int32_t DistributedInputAdapter::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                  const uint32_t &inputTypes, DICallback callback)
{
    sptr<IStartDInputCallback> cb = new (std::nothrow) StartDInputCallback();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallbackFunc(CallbackType::StartDInputCallback, callback);
    return DistributedInputKit::StartRemoteInput(srcId, sinkId, inputTypes, cb);
}

int32_t DistributedInputAdapter::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                  const std::vector<std::string> &dhIds, DICallback callback)
{
    sptr<IStartStopDInputVectorCallback> cb = new (std::nothrow) StartDInputCallbackFds();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallbackFunc(CallbackType::StartDInputCallbackFds, callback);
    return DistributedInputKit::StartRemoteInput(srcId, sinkId, dhIds, cb);
}

int32_t DistributedInputAdapter::StopRemoteInput(const std::string &deviceId, const std::vector<std::string> &dhIds,
                                                 DICallback callback)
{
    sptr<IStartStopDInputVectorCallback> cb = new (std::nothrow) StopDInputCallbackDHIds();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallbackFunc(CallbackType::StopDInputCallbackDHIds, callback);
    return DistributedInputKit::StopRemoteInput(deviceId, dhIds, cb);
}

int32_t DistributedInputAdapter::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                 const uint32_t &inputTypes, DICallback callback)
{
    sptr<IStopDInputCallback> cb = new (std::nothrow) StopDInputCallback();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallbackFunc(CallbackType::StopDInputCallback, callback);
    return DistributedInputKit::StopRemoteInput(srcId, sinkId, inputTypes, cb);
}

int32_t DistributedInputAdapter::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                 const std::vector<std::string> &dhIds, DICallback callback)
{
    sptr<IStartStopDInputVectorCallback> cb = new (std::nothrow) StopDInputCallbackFds();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallbackFunc(CallbackType::StopDInputCallbackFds, callback);
    return DistributedInputKit::StopRemoteInput(srcId, sinkId, dhIds, cb);
}

int32_t DistributedInputAdapter::PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                    DICallback callback)
{
    SaveCallbackFunc(CallbackType::PrepareStartDInputCallbackSink, callback);
    sptr<IPrepareDInputCallback> cb = new (std::nothrow) PrepareStartDInputCallbackSink();
    CHKPR(cb, ERROR_NULL_POINTER);
    return DistributedInputKit::PrepareRemoteInput(srcId, sinkId, cb);
}

int32_t DistributedInputAdapter::UnPrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                      DICallback callback)
{
    SaveCallbackFunc(CallbackType::UnPrepareStopDInputCallbackSink, callback);
    sptr<IUnprepareDInputCallback> cb = new (std::nothrow) UnPrepareStopDInputCallbackSink();
    CHKPR(cb, ERROR_NULL_POINTER);
    return DistributedInputKit::UnprepareRemoteInput(srcId, sinkId, cb);
}

int32_t DistributedInputAdapter::PrepareRemoteInput(const std::string &deviceId, DICallback callback)
{
    SaveCallbackFunc(CallbackType::PrepareStartDInputCallback, callback);
    sptr<IPrepareDInputCallback> cb = new (std::nothrow) PrepareStartDInputCallback();
    CHKPR(cb, ERROR_NULL_POINTER);
    return DistributedInputKit::PrepareRemoteInput(deviceId, cb);
}

int32_t DistributedInputAdapter::UnPrepareRemoteInput(const std::string &deviceId, DICallback callback)
{
    SaveCallbackFunc(CallbackType::UnPrepareStopDInputCallback, callback);
    sptr<IUnprepareDInputCallback> cb = new (std::nothrow) UnPrepareStopDInputCallback();
    CHKPR(cb, ERROR_NULL_POINTER);
    return DistributedInputKit::UnprepareRemoteInput(deviceId, cb);
}

int32_t DistributedInputAdapter::RegisterEventCallback(MouseStateChangeCallback callback)
{
    std::lock_guard<std::mutex> lock(adapterLock);
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
    mouseListener_ = new (std::nothrow) MouseStateChangeCallbackImpl();
    CHKPV(mouseListener_);
    DistributedInputKit::RegisterEventListener(mouseListener_);
}

void DistributedInputAdapter::Release()
{
    DistributedInputKit::UnregisterEventListener(mouseListener_);
    mouseListener_ = nullptr;
    callbackMap_.clear();
}

int32_t DistributedInputAdapter::SaveCallbackFunc(CallbackType type, DICallback callback)
{
    std::lock_guard<std::mutex> lock(adapterLock);
    CHKPR(callback, RET_ERR);
    callbackMap_[type] = callback;
    return AddWatch(type);
}

int32_t DistributedInputAdapter::AddWatch(const CallbackType &type)
{
    MMI_HILOGD("AddWatch type:%{public}d", type);
    int32_t timerId = TimerMgr->AddTimer(DEFAULT_DELAY_TIME, RETRY_TIME, [this, type]() {
        if ((callbackMap_.find(type) == callbackMap_.end()) || (watchingMap_.find(type) == watchingMap_.end())) {
            MMI_HILOGE("callback or watching is not exist");
            return;
        }
        if (watchingMap_[type].times == 0) {
            MMI_HILOGI("It will be retry to call callback next time");
            watchingMap_[type].times++;
            return;
        }
        callbackMap_[type](false);
        callbackMap_.erase(type);
    });
    if (timerId < 0) {
        MMI_HILOGE("Add timer failed timeId:%{public}d", timerId);
        return RET_ERR;
    }
    watchingMap_[type].timerId = timerId;
    watchingMap_[type].times = 0;
    return RET_OK;
}

int32_t DistributedInputAdapter::RemoveWatch(const CallbackType &type)
{
    MMI_HILOGD("RemoveWatch type:%{public}d", type);
    if ((callbackMap_.find(type) == callbackMap_.end()) || (watchingMap_.find(type) == watchingMap_.end())) {
        MMI_HILOGE("callback or watching do not exist");
        return RET_ERR;
    }
    TimerMgr->RemoveTimer(watchingMap_[type].timerId);
    watchingMap_.erase(type);
    return RET_OK;
}

void DistributedInputAdapter::StartDInputCallback::OnResult(const std::string &devId, const uint32_t &inputTypes,
                                                            const int32_t &status)
{
    DistributedAdapter->RemoveWatch(CallbackType::StartDInputCallback);
    if (DistributedAdapter->callbackMap_.find(CallbackType::StartDInputCallback) ==
        DistributedAdapter->callbackMap_.end()) {
        MMI_HILOGI("No Callback for StartDInput");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StartDInputCallback](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StartDInputCallback);
}

void DistributedInputAdapter::StopDInputCallback::OnResult(const std::string &devId, const uint32_t &inputTypes,
                                                           const int32_t &status)
{
    DistributedAdapter->RemoveWatch(CallbackType::StopDInputCallback);
    if (DistributedAdapter->callbackMap_.find(CallbackType::StopDInputCallback) ==
        DistributedAdapter->callbackMap_.end()) {
        MMI_HILOGI("No Callback StopDInput");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StopDInputCallback](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StopDInputCallback);
}

void DistributedInputAdapter::StartDInputCallbackDHIds::OnResultFds(const std::string &srcId,
    const std::string &sinkId, const int32_t &status)
{
    MMI_HILOGI("Fds Result srcId:%{public}s, sinkId:%{public}s, status:%{public}d", srcId.c_str(), sinkId.c_str(),
               status);
    DistributedAdapter->RemoveWatch(CallbackType::StartDInputCallbackDHIds);
}

void DistributedInputAdapter::StartDInputCallbackDHIds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    MMI_HILOGI("Start Distributed Input Callback Results status : %{public}d", status);
    DistributedAdapter->RemoveWatch(CallbackType::StartDInputCallbackDHIds);
    if (DistributedAdapter->callbackMap_.find(CallbackType::StartDInputCallbackDHIds) ==
        DistributedAdapter->callbackMap_.end()) {
        MMI_HILOGI("No Start Distributed Input Callback for DHIds");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StartDInputCallbackDHIds](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StartDInputCallbackDHIds);
}

void DistributedInputAdapter::StopDInputCallbackDHIds::OnResultFds(const std::string &srcId, const std::string &sinkId,
                                                                   const int32_t &status)
{
    MMI_HILOGI("Fds Result srcId:%{public}s, sinkId:%{public}s, status:%{public}d", srcId.c_str(), sinkId.c_str(),
               status);
    DistributedAdapter->RemoveWatch(CallbackType::StopDInputCallbackDHIds);
}

void DistributedInputAdapter::StopDInputCallbackDHIds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    MMI_HILOGI("Stop Distributed Input Callback for DHIds status : %{public}d", status);
    DistributedAdapter->RemoveWatch(CallbackType::StopDInputCallbackDHIds);
    if (DistributedAdapter->callbackMap_.find(CallbackType::StopDInputCallbackDHIds) ==
        DistributedAdapter->callbackMap_.end()) {
        MMI_HILOGI("No Stop Distributed Input Callback for DHIds");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StopDInputCallbackDHIds](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StopDInputCallbackDHIds);
}

void DistributedInputAdapter::StartDInputCallbackFds::OnResultFds(const std::string &srcId, const std::string &sinkId,
                                                                  const int32_t &status)
{
    MMI_HILOGI("On result for fds srcId:%{public}s, sinkId:%{public}s, status:%{public}d", srcId.c_str(),
               sinkId.c_str(), status);
    DistributedAdapter->RemoveWatch(CallbackType::StartDInputCallbackFds);
}

void DistributedInputAdapter::StartDInputCallbackFds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    MMI_HILOGI("Start Distributed InputCallback for Dhids status : %{public}d", status);
    DistributedAdapter->RemoveWatch(CallbackType::StartDInputCallbackFds);
    if (DistributedAdapter->callbackMap_.find(CallbackType::StartDInputCallbackFds) ==
        DistributedAdapter->callbackMap_.end()) {
        MMI_HILOGI("No StartDInputCallbackFds");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StartDInputCallbackFds](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StartDInputCallbackFds);
}

void DistributedInputAdapter::StopDInputCallbackFds::OnResultFds(const std::string &srcId, const std::string &sinkId,
                                                                 const int32_t &status)
{
    MMI_HILOGI("Stop dinput callback for fds srcId:%{public}s, sinkId:%{public}s, status:%{public}d", srcId.c_str(),
               sinkId.c_str(), status);
    DistributedAdapter->RemoveWatch(CallbackType::StopDInputCallbackFds);
}

void DistributedInputAdapter::StopDInputCallbackFds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->RemoveWatch(CallbackType::StopDInputCallbackFds);
    if (DistributedAdapter->callbackMap_.find(CallbackType::StopDInputCallbackFds) ==
        DistributedAdapter->callbackMap_.end()) {
        MMI_HILOGI("No Stop DInput Callback for Fds");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::StopDInputCallbackFds](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::StopDInputCallbackFds);
}

void DistributedInputAdapter::PrepareStartDInputCallback::OnResult(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->RemoveWatch(CallbackType::PrepareStartDInputCallback);
    if (DistributedAdapter->callbackMap_.find(CallbackType::PrepareStartDInputCallback) ==
        DistributedAdapter->callbackMap_.end()) {
        MMI_HILOGI("No Prepare Start DInput Callback");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::PrepareStartDInputCallback](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::PrepareStartDInputCallback);
}

void DistributedInputAdapter::UnPrepareStopDInputCallback::OnResult(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->RemoveWatch(CallbackType::UnPrepareStopDInputCallback);
    if (DistributedAdapter->callbackMap_.find(CallbackType::UnPrepareStopDInputCallback) ==
        DistributedAdapter->callbackMap_.end()) {
        MMI_HILOGI("No UnPrepare Stop DInput Callback");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::UnPrepareStopDInputCallback](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::UnPrepareStopDInputCallback);
}

void DistributedInputAdapter::PrepareStartDInputCallbackSink::OnResult(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->RemoveWatch(CallbackType::PrepareStartDInputCallbackSink);
    if (DistributedAdapter->callbackMap_.find(CallbackType::PrepareStartDInputCallbackSink) ==
        DistributedAdapter->callbackMap_.end()) {
        MMI_HILOGI("No Prepare Start DInput Callback Sink");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::PrepareStartDInputCallbackSink](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::PrepareStartDInputCallbackSink);
}

void DistributedInputAdapter::UnPrepareStopDInputCallbackSink::OnResult(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->RemoveWatch(CallbackType::UnPrepareStopDInputCallbackSink);
    if (DistributedAdapter->callbackMap_.find(CallbackType::UnPrepareStopDInputCallbackSink) ==
        DistributedAdapter->callbackMap_.end()) {
        MMI_HILOGI("No UnPrepare Stop DInputCallback Sink");
        return;
    }
    DistributedAdapter->callbackMap_[CallbackType::UnPrepareStopDInputCallbackSink](status == RET_OK);
    DistributedAdapter->callbackMap_.erase(CallbackType::UnPrepareStopDInputCallbackSink);
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