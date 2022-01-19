/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "i_multimodal_input_connect_stub.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "error_multimodal.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "multimodal_input_connect_define.h"
#include "string_ex.h"

namespace OHOS {
namespace MMI {
    namespace {
        static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
            LOG_CORE, MMI_LOG_DOMAIN, "IMultimodalInputConnectStub"
        };
    }
int32_t IMultimodalInputConnectStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    MMI_LOG_T("enter, code: %{public}d", code);

    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != IMultimodalInputConnect::GetDescriptor()) {
        MMI_LOG_E("get unexpect descriptor: %{public}s", Str16ToStr8(descriptor).c_str());
        return ERR_INVALID_STATE;
    }

    switch (code) {
        case static_cast<uint32_t>(IMultimodalInputConnect::ALLOC_SOCKET_FD):
            return HandleAllocSocketFd(data, reply);
        case static_cast<uint32_t>(IMultimodalInputConnect::SET_EVENT_POINTER_FILTER):
            return StubAddInputEventFilter(data, reply);
        default:
            MMI_LOG_E("unknown code: %{public}u, go switch defaut", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

bool IMultimodalInputConnectStub::IsAuthorizedCalling() const
{
    int callingUid = IPCSkeleton::GetCallingUid();
    MMI_LOGIK("Calling uid: %{public}d", callingUid);
    return true;
}

int32_t IMultimodalInputConnectStub::GetCallingUid() const
{
    return IPCSkeleton::GetCallingUid();
}

int32_t IMultimodalInputConnectStub::GetCallingPid() const
{
    return IPCSkeleton::GetCallingPid();
}

int32_t IMultimodalInputConnectStub::StubAddInputEventFilter(MessageParcel& data, MessageParcel& reply)
{
    MMI_LOG_T("enter");
    int32_t ret = RET_OK;

    do {
        const int32_t uid = GetCallingUid();
        if (uid != SYSTEM_UID && uid != ROOT_UID) {
            MMI_LOG_E("uid is not root or system");
            ret = SASERVICE_PERMISSION_FAIL;
            break;
        }

        sptr<IRemoteObject> client = data.ReadRemoteObject();
        if (client == nullptr) {
            MMI_LOG_E("the mouse client value is nullptr");
            ret = ERR_INVALID_VALUE;
            break;
        }

        sptr<IEventFilter> filter = iface_cast<IEventFilter>(client);
        if (filter == nullptr) {
            MMI_LOG_E("filter is nullptr");
            ret = ERROR_NULL_POINTER;
            break;
        }

        MMI_LOG_T("filter iface_cast succeeded");

        ret = AddInputEventFilter(filter);
    } while (0);
    
    if (!reply.WriteInt32(ret)) {
        MMI_LOG_E("WriteInt32(%{public}d) fail", ret);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    MMI_LOG_T("leave, ret = %{public}d", ret);
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS