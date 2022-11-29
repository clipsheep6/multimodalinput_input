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

#include "js_input_consumer_context.h"

#include "error_multimodal.h"
#include "input_manager.h"
#include "js_input_consumer_util.h"
#include "mmi_log.h"
#include "napi_constants.h"
#include "util_napi_error.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JSInputConsumerContext" };
constexpr size_t EVENT_NAME_LEN = 64;
} // namespace

Callbacks JsInputConsumerContext::callbacks = {};

std::string JsInputConsumerContext::GetSubKeyNames(const std::shared_ptr<KeyOption> &keyOption)
{
    CHKPS(keyOption);
    std::string subKeyNames = "";
    std::set<int32_t> preKeys { keyOption->GetPreKeys() };
    for (const auto &preKey : preKeys) {
        subKeyNames += std::to_string(preKey);
        subKeyNames += ",";
    }
    subKeyNames += std::to_string(keyOption->GetFinalKey());
    subKeyNames += ",";
    subKeyNames += std::to_string(keyOption->IsFinalKeyDown());
    subKeyNames += ",";
    subKeyNames += std::to_string(keyOption->GetFinalKeyDownDuration());
    MMI_HILOGD("SubKey Names:%{public}s", subKeyNames.c_str());
    return subKeyNames;
}

std::shared_ptr<KeyOption> JsInputConsumerContext::CreateKeyOption(napi_env env, napi_value object)
{
    auto keyOption = std::make_shared<KeyOption>();
    std::set<int32_t> preKeys;
    if (!JsInputConsumerUtil::GetPreKeys(env, object, "preKeys", preKeys)) {
        MMI_HILOGE("Get preKeys failed");
        return nullptr;
    }
    keyOption->SetPreKeys(preKeys);

    int32_t finalKey = 0;
    if (!JsInputConsumerUtil::GetNamedPropertyInt32(env, object, "finalKey", finalKey)) {
        MMI_HILOGE("Get NamedProperty failed");
        return nullptr;
    }
    keyOption->SetFinalKey(finalKey);

    bool isFinalKeyDown;
    if (!JsInputConsumerUtil::GetNamedPropertyBool(env, object, "isFinalKeyDown", isFinalKeyDown)) {
        MMI_HILOGE("GetNamedPropertyBool failed");
        return nullptr;
    }
    keyOption->SetFinalKeyDown(isFinalKeyDown);

    int32_t finalKeyDownDuration = 0;
    if (!JsInputConsumerUtil::GetNamedPropertyInt32(env, object, "finalKeyDownDuration", finalKeyDownDuration)) {
        MMI_HILOGE("Get NamedProperty failed");
        return nullptr;
    }
    keyOption->SetFinalKeyDownDuration(finalKeyDownDuration);
    return keyOption;
}

int32_t JsInputConsumerContext::GetSubEventInfo(napi_env env, size_t argc,
    napi_value* argv, std::shared_ptr<SubscribeInfo> &event)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    CHKPR(argv, ERROR_NULL_POINTER);

    if (!UtilNapi::TypeOf(env, argv[0], napi_string)) {
        THROWERR_API9(env, COMMON_PARAMETER_ERROR, "type", "string");
        MMI_HILOGE("The first parameter is not string");
        return RET_ERR;
    }
    char SubType[EVENT_NAME_LEN] = { 0 };
    size_t typeLen = 0;
    CHKRR(napi_get_value_string_utf8(env, argv[0], SubType, EVENT_NAME_LEN - 1, &typeLen),
        GET_VALUE_STRING_UTF8, RET_ERR);
    if (std::string(SubType) != SUBSCRIBE_TYPE) {
        MMI_HILOGE("Type is not key");
        THROWERR_CUSTOM(env, COMMON_PARAMETER_ERROR, "type must be key");
        return RET_ERR;
    }

    if (!UtilNapi::TypeOf(env, argv[1], napi_object)) {
        THROWERR_API9(env, COMMON_PARAMETER_ERROR, "keyOptions", "object");
        MMI_HILOGE("The second parameter is not napi_object");
        return RET_ERR;
    }
    event->keyOption = CreateKeyOption(env, argv[1]);
    if (event->keyOption == nullptr) {
        MMI_HILOGE("Failed to create a keyOption");
        return RET_ERR;
    }
    event->eventType = GetSubKeyNames(event->keyOption);
    if (argc == ARGC_THREE) {
        if (!UtilNapi::TypeOf(env, argv[2], napi_function)) {
            MMI_HILOGE("The third parameter is not napi_function");
            THROWERR_API9(env, COMMON_PARAMETER_ERROR, "callback", "function");
            return RET_ERR;
        }
        const uint32_t refCount = 1;
        CHKRR(napi_create_reference(env, argv[2], refCount, &event->callback), CREATE_REFERENCE, RET_ERR);
    }
    return RET_OK;
}

void JsInputConsumerContext::SubKeyEventCallback(std::shared_ptr<KeyEvent> keyEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(keyEvent);
    for (auto iter = callbacks.begin(); iter != callbacks.end(); ++iter) {
        auto &infoList = iter->second;
        MMI_HILOGD("Info list size:%{public}zu, SubKey Names:%{public}s", infoList.size(), (iter->first).c_str());
        for (const auto &info : infoList) {
            if (JsInputConsumerUtil::MatchCombinationKeys(info, keyEvent)) {
                info->keyEvent = keyEvent;
                JsInputConsumerUtil::EmitAsyncCallbackWork(info);
            }
        }
    }
}

napi_value JsInputConsumerContext::JsOn(napi_env env, napi_callback_info info)
{
    CALL_DEBUG_ENTER;
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = { nullptr };
    CHKRP(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);
    if (argc < ARGC_THREE) {
        MMI_HILOGE("Parameter number error");
        THROWERR_CUSTOM(env, COMMON_PARAMETER_ERROR, "parameter number error");
        return nullptr;
    }
    auto event = std::make_shared<SubscribeInfo>(env);
    if (GetSubEventInfo(env, argc, argv, event) != RET_OK) {
        MMI_HILOGE("GetEventInfo failed");
        return nullptr;
    }
    int32_t preSubscribeId = JsInputConsumerUtil::GetPreSubscribeId(callbacks, event);
    if (preSubscribeId < 0) {
        int32_t subscribeId = INVALID_SUBID;
        subscribeId = InputMgr->SubscribeKeyEvent(event->keyOption, SubKeyEventCallback);
        if (subscribeId < 0) {
            MMI_HILOGE("SubscribeId invalid:%{public}d", subscribeId);
            napi_delete_reference(env, event->callback);
            return nullptr;
        }
        MMI_HILOGD("SubscribeId:%{public}d", subscribeId);
        event->subscribeId = subscribeId;
    } else {
        event->subscribeId = preSubscribeId;
    }
    if (JsInputConsumerUtil::AddEventCallback(env, event, callbacks) != RET_OK) {
        MMI_HILOGE("AddEventCallback failed");
    }
    return nullptr;
}

napi_value JsInputConsumerContext::JsOff(napi_env env, napi_callback_info info)
{
    CALL_DEBUG_ENTER;
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = { nullptr };
    CHKRP(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);
    if (argc < ARGC_TWO) {
        MMI_HILOGE("Parameter number error");
        THROWERR_CUSTOM(env, COMMON_PARAMETER_ERROR, "parameter number error");
        return nullptr;
    }
    auto event = std::make_shared<SubscribeInfo>(env);
    if (GetSubEventInfo(env, argc, argv, event) != RET_OK) {
        MMI_HILOGE("Get SubEventInfo failed");
        return nullptr;
    }
    int32_t subscribeId = INVALID_SUBID;
    if (JsInputConsumerUtil::DelEventCallback(env, event, callbacks, subscribeId) != RET_OK) {
        MMI_HILOGE("DelEventCallback failed");
        return nullptr;
    }
    if (subscribeId > INVALID_SUBID) {
        InputMgr->UnsubscribeKeyEvent(subscribeId);
        MMI_HILOGD("UnsubscribeId:%{public}d", subscribeId);
    }
    if (event->callback != nullptr) {
        CHKRP(napi_delete_reference(env, event->callback), DELETE_REFERENCE);
    }
    return nullptr;
}

napi_value JsInputConsumerContext::Init(napi_env env, napi_value exports)
{
    CALL_DEBUG_ENTER;
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("on", JsOn),
        DECLARE_NAPI_FUNCTION("off", JsOff),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

} // namespace MMI
} // namespace OHOS
