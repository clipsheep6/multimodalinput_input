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

#include <napi/native_api.h>
#include <napi/native_common.h>

#include <cstdint>
#include <cstddef>

#include "constants.h"
#include "cooperate_get_state.h"
#include "cooperate_messages.h"
#include "cooperate_monitor.h"
#include "mmi_log.h"
#include "util_napi.h"

namespace OHOS::MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "NativeRegisterModule" };
} // namespace

static void DeclareDeviceCooperateInterface(napi_env env, const napi_value &exports)
{
    napi_property_descriptor functions[] = {
        DECLARE_NAPI_STATIC_FUNCTION("start", CooperateStart::Start),
        DECLARE_NAPI_STATIC_FUNCTION("stop", CooperateStop::Stop),
        DECLARE_NAPI_STATIC_FUNCTION("getState", CooperateGetState::GetState),
    };
    CHKRV(env, napi_define_properties(env, exports,
        sizeof(functions) / sizeof(*functions), functions), DEFINE_PROPERTIES);
}

static napi_value EnumClassConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value args[1] = {};
    napi_value result = nullptr;
    void *data = nullptr;
    CHKRP(env, napi_get_cb_info(env, info, &argc, args, &result, &data), GET_CB_INFO);
    return result;
}

static napi_value CreateInt32(napi_env env, CooperateMessages msg)
{
    napi_value value = nullptr;
    CHKRP(env, napi_create_int32(env, static_cast<int32_t>(msg), &value), CREATE_INT32);
    return value;
}

static void DeclaerDeviceCooperateData(napi_env env, const napi_value &exports)
{
    napi_property_descriptor msg[] = {
        DECLARE_NAPI_STATIC_PROPERTY("MSG_COOPERATE_INFO_START",
            CreateInt32(env, CooperateMessages::MSG_COOPERATE_INFO_START)),
        DECLARE_NAPI_STATIC_PROPERTY("MSG_COOPERATE_INFO_SUCCESS",
            CreateInt32(env, CooperateMessages::MSG_COOPERATE_INFO_SUCCESS)),
        DECLARE_NAPI_STATIC_PROPERTY("MSG_COOPERATE_INFO_FAIL",
            CreateInt32(env, CooperateMessages::MSG_COOPERATE_INFO_FAIL)),
        DECLARE_NAPI_STATIC_PROPERTY("MSG_COOPERATE_STATE_ON",
            CreateInt32(env, CooperateMessages::MSG_COOPERATE_STATE_ON)),
        DECLARE_NAPI_STATIC_PROPERTY("MSG_COOPERATE_STATE_OFF",
            CreateInt32(env, CooperateMessages::MSG_COOPERATE_STATE_OFF)),
    };

    napi_value eventMsg = nullptr;
    CHKRV(env, napi_define_class(env, "EventMsg", NAPI_AUTO_LENGTH, EnumClassConstructor, nullptr,
        sizeof(msg) / sizeof(*msg), msg, &eventMsg), DEFINE_CLASS);
    CHKRV(env, napi_set_named_property(env, exports, "EventMsg", eventMsg), SET_NAMED_PROPERTY);
}

static napi_value RegisterDeviceCooperateInterface(napi_env env, napi_value exports)
{
    DeclareDeviceCooperateInterface(env, exports);
    DeclaerDeviceCooperateData(env, exports);
    return exports;
}

static napi_module mmiInputDeviceCooperateMoudle = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = RegisterDeviceCooperateInterface,
    .nm_modname = "multimodalInput.inputDeviceCooperate",
    .nm_priv = ((void *)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&mmiInputDeviceCooperateMoudle);
}
} // namespace OHOS::MMI
