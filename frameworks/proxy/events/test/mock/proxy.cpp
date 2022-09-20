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

#include "mock_multimodal_input_connect_proxy.h"

#include "mmi_log.h"
#include "errors.h"
#include "message_parcel.h"
#include "message_option.h"
#include "mmi_log.h"
#include "multimodal_input_connect_def_parcel.h"
#include "multimodal_input_connect_define.h"
#include "string_ex.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MockMultimodalInputConnectProxy" };
static bool state_ = true;
} // namespace


void MultimodalInputConnectProxy::SetMIState(bool state)
{
    state_ = state;
}

int32_t MultimodalInputConnectProxy::AllocSocketFd(const std::string &programName,
    const int32_t moduleType, int32_t &socketFd, int32_t &tokenType)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::AddInputEventFilter(sptr<IEventFilter> filter)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SetPointerVisible(bool visible)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::IsPointerVisible(bool &visible)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SetPointerSpeed(int32_t speed)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::GetPointerSpeed(int32_t &speed)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SetPointerStyle(int32_t windowId, int32_t pointerStyle)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::GetPointerStyle(int32_t windowId, int32_t &pointerStyle)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::RegisterDevListener()
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::UnregisterDevListener()
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SupportKeys(int32_t userData, int32_t deviceId, std::vector<int32_t> &keys)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::GetDeviceIds(int32_t userData)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::GetDevice(int32_t userData, int32_t deviceId)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::GetKeyboardType(int32_t userData, int32_t deviceId)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::AddInputHandler(InputHandlerType handlerType,
    HandleEventType eventType)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::RemoveInputHandler(InputHandlerType handlerType, HandleEventType eventType)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::MarkEventConsumed(int32_t eventId)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::MoveMouseEvent(int32_t offsetX, int32_t offsetY)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::InjectKeyEvent(const std::shared_ptr<KeyEvent> keyEvent)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SubscribeKeyEvent(int32_t subscribeId, const std::shared_ptr<KeyOption> keyOption)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::UnsubscribeKeyEvent(int32_t subscribeId)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::InjectPointerEvent(const std::shared_ptr<PointerEvent> pointerEvent)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SetAnrObserver()
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SetInputDevice(const std::string& dhid, const std::string& screenId)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::RegisterCooperateListener()//a
{
    bool ret = state_;
    if(ret == false){
        return RET_ERR;
    }
    return RET_OK;
}


int32_t MultimodalInputConnectProxy::UnregisterCooperateListener()//a
{
    bool ret = state_;
    if(ret == false){
        return RET_ERR;
    }
    return RET_OK;
}


int32_t MultimodalInputConnectProxy::EnableInputDeviceCooperate(int32_t userData, bool enabled)//a
{
    bool ret = state_;
    if(ret == false){
        return RET_ERR;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::StartInputDeviceCooperate(int32_t userData, const std::string &sinkDeviceId,
    int32_t srcInputDeviceId)
{
    bool ret = state_;
    if(sinkDeviceId == "" || ret == false){
        return RET_ERR;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::StopDeviceCooperate(int32_t userData)
{
    bool ret = state_;
    if(ret == false){
        return RET_ERR;
    }
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::GetInputDeviceCooperateState(int32_t userData, const std::string &deviceId)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::StartRemoteCooperate(const std::string& localDeviceId)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::StartRemoteCooperateResult(bool isSuccess,
    const std::string& startDhid, int32_t xPercent, int32_t yPercent)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::StopRemoteCooperate()
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::StopRemoteCooperateResult(bool isSuccess)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::StartCooperateOtherResult(const std::string& srcNetworkId)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::GetFunctionKeyState(int32_t funcKey, bool &state)
{
    return RET_OK;
}

int32_t MultimodalInputConnectProxy::SetFunctionKeyState(int32_t funcKey, bool enable)
{
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
