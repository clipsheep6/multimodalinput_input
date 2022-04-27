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

#include "multimodal_input_connect_stub.h"

#include <sys/types.h>
#include <sys/socket.h>

#include "string_ex.h"

#include "error_multimodal.h"
#include "mmi_log.h"
#include "multimodal_input_connect_define.h"
#include "time_cost_chk.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MultimodalInputConnectStub" };
} // namespace

int32_t MultimodalInputConnectStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    CALL_LOG_ENTER;
    int32_t pid = GetCallingPid();
    TimeCostChk chk("IPC-RemoteRequest", "overtime 300(us)", MAX_OVER_TIME, pid,
        static_cast<int64_t>(code));
    uint64_t tid = GetThisThreadId();
    MMI_HILOGD("RemoteRequest recv code:%{public}d tid:%{public}" PRId64 " pid:%{public}d", code, tid, pid);

    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != IMultimodalInputConnect::GetDescriptor()) {
        MMI_HILOGE("get unexpect descriptor:%{public}s", Str16ToStr8(descriptor).c_str());
        return ERR_INVALID_STATE;
    }
    if (!IsRunning()) {
        MMI_HILOGE("service is not running. code:%{public}u, go switch defaut", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    if (!rMsgHandler_.ChkKey(code)) {
        MMI_HILOGE("unknown code:%{public}u ids:(%{public}s), go switch defaut", code,
            rMsgHandler_.GetDebugInfo().c_str());
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    int32_t uid = GetCallingUid();
    if (!entrustTasks_.PostSyncTask(std::bind(&RemoteMsgHandler::OnMsgHandler, &rMsgHandler_, std::placeholders::_1,
        uid, pid, tid, code, std::ref(data), std::ref(reply)))) {
        MMI_HILOGE("post task failed code:%{public}u, go switch defaut", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NO_ERROR;
}
} // namespace MMI
} // namespace OHOS