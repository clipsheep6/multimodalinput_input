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

#include <algorithm>
#include <inttypes.h>
#include "input_manager.h"
#include "js_register_util.h"
#include "js_register_module.h"
#include "key_event_pre.h"

namespace OHOS {
namespace MMI {
namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JSRegisterMoudle" };
}

const uint32_t EVENT_NAME_LEN = 64;
const uint32_t ARGC_NUM = 3;
const uint32_t ARGV_FIRST = 0;
const uint32_t ARGV_SECOND = 1;
const uint32_t ARGV_THIRD = 2;
const uint32_t PRE_KEYS_SIZE = 4;
Callbacks callbacks = {};

int32_t GetEventInfo(napi_env env, napi_callback_info info, KeyEventMonitorInfo* event,
    std::shared_ptr<KeyOption> keyOption)
{
    MMI_LOGD("enter");
    size_t argc = ARGC_NUM;
    napi_value argv[ARGC_NUM] = { 0 };
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok) {
        napi_throw_error(env, nullptr, "Get param failed");
        return ERROR_CODE;
    }
    NAPI_ASSERT(env, argc == ARGC_NUM, "requires 3 parameter");
    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, argv[ARGV_FIRST], &valueType) != napi_ok) {
        napi_throw_error(env, nullptr, "Get type of first param failed");
        return ERROR_CODE;
    }
    NAPI_ASSERT(env, valueType == napi_string, "parameter1 is not napi_string");
    if (napi_typeof(env, argv[ARGV_SECOND], &valueType) != napi_ok) {
        napi_throw_error(env, nullptr, "Get type of second param failed");
        return ERROR_CODE;
    }
    NAPI_ASSERT(env, valueType == napi_object, "parameter2 is not napi_object");
    if (napi_typeof(env, argv[ARGV_THIRD], &valueType) != napi_ok) {
        napi_throw_error(env, nullptr, "Get type of third param failed");
        return ERROR_CODE;
    }
    NAPI_ASSERT(env, valueType == napi_function, "parameter2 is not napi_function");
    char eventName[EVENT_NAME_LEN] = { 0 };
    size_t typeLen = 0;
    if (napi_get_value_string_utf8(env, argv[ARGV_FIRST], eventName, NAPI_AUTO_LENGTH, &typeLen) != napi_ok) {
        napi_throw_error(env, nullptr, "Get value of first param failed");
        return ERROR_CODE;
    }
    event->name = eventName;
    napi_value receiceValue = nullptr;
    if (napi_get_named_property(env, argv[ARGV_SECOND], "preKeys", &receiceValue) != napi_ok) {
        napi_throw_error(env, nullptr, "Get preKeys failed");
        return ERROR_CODE;
    }
    std::vector<int32_t> preKeys = GetIntArray(receiceValue, env);
    MMI_LOGD("PreKeys size:%{public}d", static_cast<int32_t>(preKeys.size()));
    std::vector<int32_t> sortPrekeys = preKeys;
    sort(sortPrekeys.begin(), sortPrekeys.end());
    keyOption->SetPreKeys(preKeys);

    std::string subKeyNames = "";
    for (int32_t i = 0; i < sortPrekeys.size(); i++){
        subKeyNames += std::to_string(sortPrekeys[i]);
        subKeyNames += ",";
        MMI_LOGD("PreKeys = %{public}d", preKeys[i]);
    }

    int32_t finalKey = GetNamedPropertyInt32(env, argv[ARGV_SECOND], "finalKey");
    subKeyNames += std::to_string(finalKey);
    subKeyNames += ",";
    keyOption->SetFinalKey(finalKey);
    MMI_LOGD("FinalKey = %{public}d", finalKey);

    bool isFinalKeyDown = GetNamedPropertyBool(env, argv[ARGV_SECOND], "isFinalKeyDown");
    subKeyNames += std::to_string(isFinalKeyDown);
    keyOption->SetFinalKeyDown(isFinalKeyDown);
    event->eventType = subKeyNames;
    MMI_LOGD("IsFinalKeyDown: %{public}d", (isFinalKeyDown == true?1:0));
    MMI_LOGD("SubKeyNames: %{public}s", subKeyNames.c_str());

    int32_t finalKeyDownDuriation = GetNamedPropertyInt32(env, argv[ARGV_SECOND], "finalKeyDownDuration");
    if (napi_get_value_int32(env, receiceValue, &finalKeyDownDuriation) != napi_ok) {
        napi_throw_error(env, nullptr, "FinalKeyDownDuriation get value failed");
        return ERROR_CODE;
    }
    keyOption->SetFinalKeyDownDuration(finalKeyDownDuriation);
    MMI_LOGD("FinalKeyDownDuriation: %{public}d", finalKeyDownDuriation);

    if (napi_create_reference(env, argv[ARGV_THIRD], 1, &event->callback[0]) != napi_ok) {
        napi_throw_error(env, nullptr, "Event create reference failed");
        return ERROR_CODE;
    }
    MMI_LOGD("end");
    return SUCCESS_CODE;
}

static bool MatchCombinationkeys(KeyEventMonitorInfo* monitorInfo, std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent){
    MMI_LOGD("enter");
    auto keyOption = monitorInfo->keyOption;
    std::vector<int32_t> infoPreKeys = keyOption->GetPreKeys();
    std::vector<KeyEvent::KeyItem> keyEventKeyItems = keyEvent->GetKeyItems();
    int32_t infoFinalKey = keyOption->GetFinalKey();
    int32_t keyEventFinalKey = keyEvent->GetKeyCode();
    MMI_LOGD("InfoFinalKey:%{public}d, keyEventFinalKey:%{public}d", infoFinalKey, keyEventFinalKey);
    if (infoFinalKey != keyEventFinalKey || keyEventKeyItems.size() > 4) {
        MMI_LOGD("%{public}d", __LINE__);
        return false;
    }
    int infoSize = 0;
    auto it = infoPreKeys.begin();
    while(it != infoPreKeys.end()) {
        if (*it >= 0) {
            infoSize++;
        }
        it++;
    }
    int kevEventSize = 0;
    for (auto it = keyEventKeyItems.begin(); it != keyEventKeyItems.end(); it++) {
        if (it->GetKeyCode() == keyEventFinalKey) {
            continue;
        }
        auto iter = find(infoPreKeys.begin(), infoPreKeys.end(), it->GetKeyCode());
        if (iter == infoPreKeys.end()) {
            MMI_LOGE("No keyCode in preKeys");
            return false;
        }
        kevEventSize++;
    }
    MMI_LOGD("kevEventSize:%{public}d, infoSize:%{public}d", kevEventSize, infoSize);
    return kevEventSize == infoSize;
}

static void SubKeyEventCallback(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent)
{
    MMI_LOGD("enter");
    auto iter = callbacks.begin();
    while (iter != callbacks.end()) {
        auto &list = iter->second;
        iter++;
        auto infoIter = list.begin();
        MMI_LOGD("list size:%{public}d", static_cast<int32_t>(list.size()));
        while(infoIter != list.end()) {
            auto monitorInfo = *infoIter;
            if (MatchCombinationkeys(monitorInfo, keyEvent)) {
                monitorInfo->keyEvent = keyEvent;
                monitorInfo->status = 1;
                EmitAsyncCallbackWork(monitorInfo);
            }
            infoIter++;
        }
    }
}

bool CheckPara(const std::shared_ptr<KeyOption> keyOption)
{
    std::vector<int32_t> preKeys = keyOption->GetPreKeys();
    if (preKeys.size() > PRE_KEYS_SIZE) {
        MMI_LOGE("preKeys size is bigger than 4, can not process");
        return false;
    } 
    std::vector<int32_t> checkRepeat;
    for (int32_t i = 0; i < preKeys.size(); ++i) {
        if (preKeys[i] < 0) {
            MMI_LOGE("preKey:%{public}d is less 0, can not process", preKeys[i]);
            return false;
        }
        if (std::find(checkRepeat.begin(), checkRepeat.end(), preKeys[i]) != checkRepeat.end()){
            MMI_LOGE("preKey is repeat, can not process");
            return false;
        }
        checkRepeat.push_back(preKeys[i]);
    }
    return true;
}

static napi_value JsOn(napi_env env, napi_callback_info info)
{
    MMI_LOGD("enter");
    KeyEventMonitorInfo *event = new KeyEventMonitorInfo {
        .env = env,
        .asyncWork = nullptr,
    };
    auto keyOption = std::shared_ptr<KeyOption>(new KeyOption());
    if (GetEventInfo(env, info, event, keyOption) < 0 || !CheckPara(keyOption)) {
        delete event;
        event = nullptr;
        MMI_LOGE("GetEventInfo failed");
        return nullptr;
    }

    event->keyOption = keyOption;
    int32_t preSubscribeId = -1;
    if (AddEventCallback(env, callbacks, event, preSubscribeId) < 0 ) {
        delete event;
        event = nullptr;
        MMI_LOGE("AddEventCallback failed");
        return nullptr;
    }

    if (preSubscribeId < 0) {
        MMI_LOGD("eventType = %{public}s", event->eventType.c_str());
        MMI_LOGD("eventName = %{public}s", event->name.c_str());
        int32_t subscribeId = -1;
        subscribeId = InputManager::GetInstance()->SubscribeKeyEvent(keyOption, SubKeyEventCallback);
        if (subscribeId < 0) {
            MMI_LOGD("subscribeId invalid: %{public}d", subscribeId);
            event->status = -1;
            EmitAsyncCallbackWork(event);
            return nullptr;
        }
        MMI_LOGD("SubscribeId: %{public}d", subscribeId);
        event->subscribeId = subscribeId;
    } else {
        event->subscribeId = preSubscribeId;
    }
    return nullptr;
}

static napi_value JsOff(napi_env env, napi_callback_info info)
{
    MMI_LOGD("enter");
    KeyEventMonitorInfo *event = new KeyEventMonitorInfo
    {
        .env = env,
        .asyncWork = nullptr,
    };
    auto keyOption = std::shared_ptr<KeyOption>(new KeyOption());
    if (GetEventInfo(env, info, event, keyOption) < 0) {
        MMI_LOGE("GetEventInfo failed");
        return nullptr;
    }
    int32_t subscribeId = -1;
    if (DelEventCallback(env, callbacks, event, subscribeId) < 0) {
        delete event;
        event = nullptr;
        MMI_LOGE("DelEventCallback failed");
        return result;
    }
    MMI_LOGD("SubscribeId: %{public}d", subscribeId);
    if (subscribeId > 0) {
        InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId);
    }
    event->status = 0;
    EmitAsyncCallbackWork(event);
    return nullptr;
}

EXTERN_C_START
static napi_value MmiInit(napi_env env, napi_value exports)
{
    MMI_LOGD("enter");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("on", JsOn),
        DECLARE_NAPI_FUNCTION("off", JsOff),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}
EXTERN_C_END

static napi_module mmiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = MmiInit,
    .nm_modname = "inputConsumer",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&mmiModule);
}
}
}

