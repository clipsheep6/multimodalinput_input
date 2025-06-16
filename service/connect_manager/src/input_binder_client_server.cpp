/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "input_binder_client_server.h"
#include "input_binder_client_server_impl.h"
#include "mmi_log.h"
#include "input_manager.h"

#undef MMI_LOG_TAG
#define MMI_LOG_TAG "InputBinderClientServer"

namespace OHOS {
namespace MMI {

InputBinderClientServer::InputBinderClientServer()
{
    InitClientSrv();
}

InputBinderClientServer::~InputBinderClientServer() {}

void InputBinderClientServer::InitClientSrv()
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> clientLock(clientSrvMutex_);
    if (clientSrvStub_ == nullptr) {
        clientSrvStub_ = sptr<InputBinderClientServerImpl>::MakeSptr();
        CHKPV(clientSrvStub_);
    }
}

sptr<IRemoteObject> InputBinderClientServer::GetClientSrv()
{
    std::lock_guard<std::mutex> clientLock(clientSrvMutex_);
    CHKPP(clientSrvStub_);
    return clientSrvStub_->AsObject();
}
} // namespace MMI
} // namespace OHOS