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
} // namespace
DistributedInputAdapter::DistributedInputAdapter()
{
    mouseListener_ = new (std::nothrow) MouseStateChangeCallbackImpl();
    CHKPV(mouseListener_);
    DistributedInputKit::RegisterEventListener(mouseListener_);
}

DistributedInputAdapter::~DistributedInputAdapter()
{
    std::lock_guard<std::mutex> guard(adapterLock_);
    DistributedInputKit::UnregisterEventListener(mouseListener_);
    mouseListener_ = nullptr;
    callbackMap_.clear();
}

bool DistributedInputAdapter::IsNeedFilterOut(const std::string &deviceId, const BusinessEvent &event)
{
    return DistributedInputKit::IsNeedFilterOut(deviceId, event);
}

int32_t DistributedInputAdapter::StartRemoteInput(const std::string &deviceId, const std::vector<std::string> &dhIds,
                                                  DInputCallback callback)
{
    sptr<IStartStopDInputsCallback> cb = new (std::nothrow) StartDInputCallbackDHIds();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallback(CallbackType::StartDInputCallbackDHIds, callback);
    return DistributedInputKit::StartRemoteInput(deviceId, dhIds, cb);
}

int32_t DistributedInputAdapter::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                  const uint32_t &inputTypes, DInputCallback callback)
{
    sptr<IStartDInputCallback> cb = new (std::nothrow) StartDInputCallback();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallback(CallbackType::StartDInputCallback, callback);
    return DistributedInputKit::StartRemoteInput(srcId, sinkId, inputTypes, cb);
}

int32_t DistributedInputAdapter::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                  const std::vector<std::string> &dhIds, DInputCallback callback)
{
    sptr<IStartStopDInputsCallback> cb = new (std::nothrow) StartDInputCallbackFds();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallback(CallbackType::StartDInputCallbackFds, callback);
    return DistributedInputKit::StartRemoteInput(srcId, sinkId, dhIds, cb);
}

int32_t DistributedInputAdapter::StopRemoteInput(const std::string &deviceId, const std::vector<std::string> &dhIds,
                                                 DInputCallback callback)
{
    sptr<IStartStopDInputsCallback> cb = new (std::nothrow) StopDInputCallbackDHIds();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallback(CallbackType::StopDInputCallbackDHIds, callback);
    return DistributedInputKit::StopRemoteInput(deviceId, dhIds, cb);
}

int32_t DistributedInputAdapter::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                 const uint32_t &inputTypes, DInputCallback callback)
{
    sptr<IStopDInputCallback> cb = new (std::nothrow) StopDInputCallback();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallback(CallbackType::StopDInputCallback, callback);
    return DistributedInputKit::StopRemoteInput(srcId, sinkId, inputTypes, cb);
}

int32_t DistributedInputAdapter::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                 const std::vector<std::string> &dhIds, DInputCallback callback)
{
    sptr<IStartStopDInputsCallback> cb = new (std::nothrow) StopDInputCallbackFds();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallback(CallbackType::StopDInputCallbackFds, callback);
    return DistributedInputKit::StopRemoteInput(srcId, sinkId, dhIds, cb);
}

int32_t DistributedInputAdapter::PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                    DInputCallback callback)
{
    SaveCallback(CallbackType::PrepareStartDInputCallbackSink, callback);
    sptr<IPrepareDInputCallback> cb = new (std::nothrow) PrepareStartDInputCallbackSink();
    CHKPR(cb, ERROR_NULL_POINTER);
    return DistributedInputKit::PrepareRemoteInput(srcId, sinkId, cb);
}

int32_t DistributedInputAdapter::UnPrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
                                                      DInputCallback callback)
{
    sptr<IUnprepareDInputCallback> cb = new (std::nothrow) UnPrepareStopDInputCallbackSink();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallback(CallbackType::UnPrepareStopDInputCallbackSink, callback);
    return DistributedInputKit::UnprepareRemoteInput(srcId, sinkId, cb);
}

int32_t DistributedInputAdapter::PrepareRemoteInput(const std::string &deviceId, DInputCallback callback)
{
    sptr<IPrepareDInputCallback> cb = new (std::nothrow) PrepareStartDInputCallback();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallback(CallbackType::PrepareStartDInputCallback, callback);
    return DistributedInputKit::PrepareRemoteInput(deviceId, cb);
}

int32_t DistributedInputAdapter::UnPrepareRemoteInput(const std::string &deviceId, DInputCallback callback)
{
    sptr<IUnprepareDInputCallback> cb = new (std::nothrow) UnPrepareStopDInputCallback();
    CHKPR(cb, ERROR_NULL_POINTER);
    SaveCallback(CallbackType::UnPrepareStopDInputCallback, callback);
    return DistributedInputKit::UnprepareRemoteInput(deviceId, cb);
}

int32_t DistributedInputAdapter::RegisterEventCallback(MouseStateChangeCallback callback)
{
    std::lock_guard<std::mutex> guard(adapterLock_);
    CHKPR(callback, RET_ERR);
    mouseStateChangeCallback_ = callback;
    return RET_OK;
}
int32_t DistributedInputAdapter::UnregisterEventCallback(MouseStateChangeCallback callback)
{
    std::lock_guard<std::mutex> guard(adapterLock_);
    CHKPR(callback, RET_ERR);
    mouseStateChangeCallback_ = nullptr;
    return RET_OK;
}

void DistributedInputAdapter::SaveCallback(CallbackType type, DInputCallback callback)
{
    std::lock_guard<std::mutex> guard(adapterLock_);
    CHKPR(callback, RET_ERR);
    callbackMap_[type] = callback;
    AddTimer(type);
}

void DistributedInputAdapter::AddTimer(const CallbackType &type)
{
    MMI_HILOGD("AddTimer type:%{public}d", type);
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
        return;
    }
    watchingMap_[type].timerId = timerId;
    watchingMap_[type].times = 0;
    return;
}

int32_t DistributedInputAdapter::RemoveTimer(const CallbackType &type)
{
    MMI_HILOGD("RemoveTimer type:%{public}d", type);
    if ((callbackMap_.find(type) == callbackMap_.end()) || (watchingMap_.find(type) == watchingMap_.end())) {
        MMI_HILOGE("callback or watching do not exist");
        return RET_ERR;
    }
    TimerMgr->RemoveTimer(watchingMap_[type].timerId);
    watchingMap_.erase(type);
    return RET_OK;
}

void DistributedInputAdapter::ProcessCallbackFromDinput(CallbackType type, int32_t status)
{
    CALL_INFO_TRACE;
    std::lock_guard<std::mutex> guard(adapterLock_);
    RemoveTimer(type);
    auto it = callbackMap_.find(type);
    if (it == callbackMap_.end()) {
        MMI_HILOGI("dinput callback not exist");
        return;
    }
    it->second(status == RET_OK);
    callbackMap_.erase(CallbackType::type);
}

void DistributedInputAdapter::StartDInputCallback::OnResult(const std::string &devId, const uint32_t &inputTypes,
                                                            const int32_t &status)
{
    DistributedAdapter->ProcessCallbackFromDinput(CallbackType::StartDInputCallback, status);
}

void DistributedInputAdapter::StopDInputCallback::OnResult(const std::string &devId, const uint32_t &inputTypes,
                                                           const int32_t &status)
{
    DistributedAdapter->ProcessCallbackFromDinput(CallbackType::StopDInputCallback, status);
}

void DistributedInputAdapter::StartDInputCallbackDHIds::OnResultFds(const std::string &srcId,
    const std::string &sinkId, const int32_t &status)
{
    MMI_HILOGI("Fds result status:%{public}d", status);
    std::lock_guard<std::mutex> guard(adapterLock_);
    DistributedAdapter->RemoveTimer(CallbackType::StartDInputCallbackDHIds);
}

void DistributedInputAdapter::StartDInputCallbackDHIds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->ProcessCallbackFromDinput(CallbackType::StartDInputCallbackDHIds, status);
}

void DistributedInputAdapter::StopDInputCallbackDHIds::OnResultFds(const std::string &srcId, const std::string &sinkId,
                                                                   const int32_t &status)
{
    MMI_HILOGI("Fds Result srcId:%{public}s, sinkId:%{public}s, status:%{public}d", srcId.c_str(), sinkId.c_str(),
               status);
    std::lock_guard<std::mutex> guard(adapterLock_);
    DistributedAdapter->RemoveTimer(CallbackType::StopDInputCallbackDHIds);
}

void DistributedInputAdapter::StopDInputCallbackDHIds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->ProcessCallbackFromDinput(CallbackType::StopDInputCallbackDHIds, status);
}

void DistributedInputAdapter::StartDInputCallbackFds::OnResultFds(const std::string &srcId, const std::string &sinkId,
                                                                  const int32_t &status)
{
    MMI_HILOGI("On result for fds status:%{public}d", status);
    std::lock_guard<std::mutex> guard(adapterLock_);
    DistributedAdapter->RemoveTimer(CallbackType::StartDInputCallbackFds);
}

void DistributedInputAdapter::StartDInputCallbackFds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->ProcessCallbackFromDinput(CallbackType::StartDInputCallbackFds, status);
}

void DistributedInputAdapter::StopDInputCallbackFds::OnResultFds(const std::string &srcId, const std::string &sinkId,
                                                                 const int32_t &status)
{
    MMI_HILOGI("Stop dinput callback for fds status:%{public}d", status);
    std::lock_guard<std::mutex> guard(adapterLock_);
    DistributedAdapter->RemoveTimer(CallbackType::StopDInputCallbackFds);
}

void DistributedInputAdapter::StopDInputCallbackFds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->ProcessCallbackFromDinput(CallbackType::StopDInputCallbackFds, status);
}

void DistributedInputAdapter::PrepareStartDInputCallback::OnResult(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->ProcessCallbackFromDinput(CallbackType::PrepareStartDInputCallback, status);
}

void DistributedInputAdapter::UnPrepareStopDInputCallback::OnResult(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->ProcessCallbackFromDinput(CallbackType::UnPrepareStopDInputCallback, status);
}

void DistributedInputAdapter::PrepareStartDInputCallbackSink::OnResult(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->ProcessCallbackFromDinput(CallbackType::PrepareStartDInputCallbackSink, status);
}

void DistributedInputAdapter::UnPrepareStopDInputCallbackSink::OnResult(const std::string &devId, const int32_t &status)
{
    DistributedAdapter->ProcessCallbackFromDinput(CallbackType::UnPrepareStopDInputCallbackSink, status);
}

int32_t DistributedInputAdapter::MouseStateChangeCallbackImpl::OnMouseDownEvent(uint32_t type, uint32_t code,
                                                                                int32_t value)
{
    std::lock_guard<std::mutex> guard(adapterLock_);
    CHKPR(DistributedAdapter->mouseStateChangeCallback_, RET_ERR);
    DistributedAdapter->mouseStateChangeCallback_(type, code, value);
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS