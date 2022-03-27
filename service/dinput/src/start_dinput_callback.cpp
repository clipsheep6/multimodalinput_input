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

#include "start_dinput_callback.h"
#include "define_multimodal.h"

namespace OHOS {
namespace MMI {
namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "StartDInputCallback" };
}    

StartDInputCallback::StartDInputCallback(const int32_t& taskId, SessionPtr& sess)
{
    taskId_ = taskId;
    sess_ = sess;
}


void StartDInputCallback::OnResult(const std::string& deviceId, const int32_t& status)
{ 
    MMI_LOGI("StartDInputCallback::OnResult");
    NetPacket pkt2(MmiMessageId::INPUT_START_REMOTE);
    CHK(pkt2.Write(taskId_), STREAM_BUF_WRITE_FAIL);
    CHK(pkt2.Write(status), STREAM_BUF_WRITE_FAIL);
    if (!sess_->SendMsg(pkt2)) {
        MMI_LOGE("StartDInputCallback::OnResult Sending failed!\n");
    }
    MMI_LOGD("StartDInputCallback::OnResult end");
}
}  // namespace MMI
}  // namespace OHOS
