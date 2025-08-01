/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "inject_notice_manager.h"

#include "ability_manager_client.h"
#ifdef OHOS_BUILD_ENABLE_DFX_RADAR
#include "dfx_hisysevent.h"
#endif // OHOS_BUILD_ENABLE_DFX_RADAR
#include "mmi_log.h"

#undef MMI_LOG_DOMAIN
#define MMI_LOG_DOMAIN MMI_LOG_SERVER
#undef MMI_LOG_TAG
#define MMI_LOG_TAG "InjectNoticeManage"

namespace OHOS {
namespace MMI {
namespace {
constexpr int32_t INVALID_USERID { -1 };
constexpr int32_t MESSAGE_PARCEL_KEY_NOTICE_SEND { 0 };
constexpr int32_t MESSAGE_PARCEL_KEY_NOTICE_CLOSE { 1 };
const std::u16string INJECT_NOTICE_INTERFACE_TOKEN { u"ohos.multimodalinput.IInjectNotice" };
}

InjectNoticeManager::InjectNoticeManager() : connectionCallback_(new (std::nothrow) InjectNoticeConnection()) {}

InjectNoticeManager::~InjectNoticeManager()
{
    connectionCallback_ = nullptr;
}

bool InjectNoticeManager::StartNoticeAbility()
{
    CALL_DEBUG_ENTER;
    if (isStartSrv_) {
        MMI_HILOGW("The injectNoticeAbility has start");
        return true;
    }
    auto client = AAFwk::AbilityManagerClient::GetInstance();
    if (client == nullptr) {
        return false;
    }
    AAFwk::Want want;
    want.SetElementName("com.ohos.powerdialog", "InjectNoticeAbility");
    auto begin = std::chrono::high_resolution_clock::now();
    int32_t result = client->StartAbility(want);
    auto durationMS = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - begin).count();
#ifdef OHOS_BUILD_ENABLE_DFX_RADAR
    DfxHisysevent::ReportApiCallTimes(ApiDurationStatistics::Api::ABILITY_MGR_CLIENT_START_ABILITY, durationMS);
#endif // OHOS_BUILD_ENABLE_DFX_RADAR
    if (result != 0) {
        MMI_HILOGW("Start injectNoticeAbility failed, result:%{public}d", result);
        return false;
    }
    isStartSrv_ = true;
    MMI_HILOGI("Start injectNoticeAbility success");
    return true;
}

bool InjectNoticeManager::ConnectNoticeSrv()
{
    CALL_DEBUG_ENTER;
    CHKPF(connectionCallback_);
    if (connectionCallback_->IsConnected()) {
        MMI_HILOGD("InjectNoticeAbility has connected");
        return true;
    }
    auto abilityMgr = AAFwk::AbilityManagerClient::GetInstance();
    CHKPF(abilityMgr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.powerdialog", "InjectNoticeAbility");
    auto begin = std::chrono::high_resolution_clock::now();
    ErrCode result = abilityMgr->ConnectAbility(want, connectionCallback_, INVALID_USERID);
    auto durationMS = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - begin).count();
#ifdef OHOS_BUILD_ENABLE_DFX_RADAR
    DfxHisysevent::ReportApiCallTimes(ApiDurationStatistics::Api::ABILITY_MGR_CONNECT_ABILITY, durationMS);
#endif // OHOS_BUILD_ENABLE_DFX_RADAR
    if (result != ERR_OK) {
        MMI_HILOGW("Connect InjectNoticeAbility failed, result:%{public}d", result);
        return false;
    }
    MMI_HILOGI("Connect InjectNoticeAbility success");
    return true;
}

bool InjectNoticeManager::IsAbilityStart() const
{
    return isStartSrv_;
}

sptr<InjectNoticeManager::InjectNoticeConnection> InjectNoticeManager::GetConnection() const
{
    return connectionCallback_;
}

void InjectNoticeManager::InjectNoticeConnection::OnAbilityConnectDone(const AppExecFwk::ElementName& element,
    const sptr<IRemoteObject>& remoteObject, int resultCode)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> lock(mutex_);
    CHKPV(remoteObject);
    if (remoteObject_ == nullptr) {
        remoteObject_ = remoteObject;
    }
    isConnected_ = true;
}

void InjectNoticeManager::InjectNoticeConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName& element,
    int resultCode)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> lock(mutex_);
    isConnected_ = false;
    remoteObject_ = nullptr;
}

bool InjectNoticeManager::InjectNoticeConnection::SendNotice(const InjectNoticeInfo& noticeInfo)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(INJECT_NOTICE_INTERFACE_TOKEN);
    data.WriteInt32(noticeInfo.pid);
    int32_t cmdCode = MESSAGE_PARCEL_KEY_NOTICE_SEND;
    std::lock_guard<std::mutex> lock(mutex_);
    CHKPF(remoteObject_);
    MMI_HILOGD("Requst send notice begin");
    int32_t ret = remoteObject_->SendRequest(cmdCode, data, reply, option);
    if (ret != ERR_OK) {
        MMI_HILOGW("Requst send notice failed:%{public}d", ret);
        return false;
    }
    MMI_HILOGI("Requst send notice ok");
    return true;
}

bool InjectNoticeManager::InjectNoticeConnection::CancelNotice(const InjectNoticeInfo& noticeInfo)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(INJECT_NOTICE_INTERFACE_TOKEN);
    data.WriteInt32(noticeInfo.pid);
    int32_t cmdCode = MESSAGE_PARCEL_KEY_NOTICE_CLOSE;
    std::lock_guard<std::mutex> lock(mutex_);
    CHKPF(remoteObject_);
    MMI_HILOGD("Requst send close notice begin");
    int32_t ret = remoteObject_->SendRequest(cmdCode, data, reply, option);
    if (ret != ERR_OK) {
        MMI_HILOGW("Requst send close notice failed:%{public}d", ret);
        return false;
    }
    MMI_HILOGI("Requst send close notice ok");
    return true;
}

bool InjectNoticeManager::InjectNoticeConnection::IsConnected() const
{
    return isConnected_;
}
} // namespace MMI
} // namespace OHOS