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

#include "js_input_consumer_util.h"

#include "error_multimodal.h"
#include "mmi_log.h"
#include "napi_constants.h"
#include "util_napi_error.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JSInputConsumerUtil" };
constexpr size_t PRE_KEYS_SIZE = 4;
} // namespace

void JsInputConsumerUtil::SetNamedProperty(napi_env env,
    napi_value object, const std::string &name, int32_t value)
{
    MMI_HILOGD("Set named property %{public}s=%{public}d", name.c_str(), value);
    napi_value napiValue = nullptr;
    CHKRV(napi_create_int32(env, value, &napiValue), CREATE_INT32);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, object, name.c_str(), napiValue));
}

void JsInputConsumerUtil::SetNamedProperty(napi_env env,
    napi_value object, const std::string &name, std::string value)
{
    MMI_HILOGD("Set named property %{public}s=%{public}s", name.c_str(), value.c_str());
    napi_value napiValue = nullptr;
    CHKRV(napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &napiValue), CREATE_STRING_UTF8);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, object, name.c_str(), napiValue));
}

bool JsInputConsumerUtil::GetNamedPropertyBool(napi_env env,
    napi_value object, const std::string &name, bool &state)
{
    napi_value napiValue = nullptr;
    CHKRF(napi_get_named_property(env, object, name.c_str(), &napiValue), GET_NAMED_PROPERTY);
    if (!UtilNapi::TypeOf(env, napiValue, napi_boolean)) {
        MMI_HILOGE("The value is not bool");
        THROWERR_API9(env, COMMON_PARAMETER_ERROR, name.c_str(), "bool");
        return false;
    }
    CHKRF(napi_get_value_bool(env, napiValue, &state), GET_VALUE_BOOL);
    return true;
}

bool JsInputConsumerUtil::GetNamedPropertyInt32(napi_env env,
    napi_value object, const std::string &name, int32_t &result)
{
    napi_value napiValue = nullptr;
    CHKRF(napi_get_named_property(env, object, name.c_str(), &napiValue), GET_NAMED_PROPERTY);
    if (!UtilNapi::TypeOf(env, napiValue, napi_number)) {
        MMI_HILOGE("The value is not number");
        THROWERR_API9(env, COMMON_PARAMETER_ERROR, name.c_str(), "number");
        return false;
    }
    CHKRF(napi_get_value_int32(env, napiValue, &result), GET_VALUE_INT32);
    if (result < 0) {
        MMI_HILOGE("finalKey:%{public}d is less 0, can not process", result);
        std::string temp = name + "must be greater than or equal to 0";
        THROWERR_CUSTOM(env, COMMON_PARAMETER_ERROR, temp);
        return false;
    }
    return true;
}

bool JsInputConsumerUtil::GetPreKeys(napi_env env,
    napi_value object, const std::string &name, std::set<int32_t> &preKeys)
{
    napi_value receiveValue = nullptr;
    CHKRF(napi_get_named_property(env, object, name.c_str(), &receiveValue), GET_NAMED_PROPERTY);
    if (receiveValue == nullptr) {
        THROWERR_CUSTOM(env, COMMON_PARAMETER_ERROR, "preKeys not found");
        return false;
    }
    uint32_t arrayLength = 0;
    CHKRF(napi_get_array_length(env, receiveValue, &arrayLength), GET_ARRAY_LENGTH);
    for (uint32_t i = 0; i < arrayLength; ++i) {
        napi_value napiElement = nullptr;
        CHKRF(napi_get_element(env, receiveValue, i, &napiElement), GET_ELEMENT);
        if (!UtilNapi::TypeOf(env, napiElement, napi_number)) {
            MMI_HILOGE("PreKeys Wrong argument type, Number expected");
            THROWERR_CUSTOM(env, COMMON_PARAMETER_ERROR, "element of preKeys must be number");
            return false;
        }
        int32_t preKey = 0;
        CHKRF(napi_get_value_int32(env, napiElement, &preKey), GET_VALUE_INT32);
        if (preKey < 0) {
            MMI_HILOGE("preKey:%{public}d is less 0, can not process", preKey);
            THROWERR_CUSTOM(env, COMMON_PARAMETER_ERROR, "element of preKeys must be greater than or equal to 0");
            return false;
        }
        if (!preKeys.insert(preKey).second) {
            MMI_HILOGE("Params insert value failed");
            return false;
        }
    }
    if (preKeys.size() > PRE_KEYS_SIZE) {
        MMI_HILOGE("preKeys size invalid");
        THROWERR_CUSTOM(env, COMMON_PARAMETER_ERROR, "preKeys size invalid");
        return false;
    }
    return true;
}

bool JsInputConsumerUtil::IsMatchKeyAction(bool isFinalKeydown, int32_t keyAction)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGD("isFinalKeydown:%{public}d,keyAction:%{public}d", isFinalKeydown, keyAction);
    if ((isFinalKeydown && keyAction == KeyEvent::KEY_ACTION_DOWN) ||
        (!isFinalKeydown && keyAction == KeyEvent::KEY_ACTION_UP)) {
        return true;
    }
    MMI_HILOGE("isFinalKeydown not matched with keyAction");
    return false;
}

bool JsInputConsumerUtil::MatchCombinationKeys(const std::shared_ptr<SubscribeInfo> &event,
    const std::shared_ptr<KeyEvent> &keyEvent)
{
    CALL_DEBUG_ENTER;
    CHKPF(event);
    CHKPF(keyEvent);
    auto keyOption = event->keyOption;
    CHKPF(keyOption);
    std::vector<KeyEvent::KeyItem> items = keyEvent->GetKeyItems();
    int32_t infoFinalKey = keyOption->GetFinalKey();
    int32_t keyEventFinalKey = keyEvent->GetKeyCode();
    bool isFinalKeydown = keyOption->IsFinalKeyDown();
    MMI_HILOGD("infoFinalKey:%{public}d,keyEventFinalKey:%{public}d", infoFinalKey, keyEventFinalKey);
    if (infoFinalKey != keyEventFinalKey || items.size() > PRE_KEYS_SIZE ||
        !IsMatchKeyAction(isFinalKeydown, keyEvent->GetKeyAction())) {
        MMI_HILOGE("Param invalid");
        return false;
    }
    std::set<int32_t> infoPreKeys = keyOption->GetPreKeys();
    int32_t infoSize = 0;
    for (const auto &preKey : infoPreKeys) {
        if (preKey >= 0) {
            infoSize++;
        }
    }
    int32_t count = 0;
    for (const auto &item : items) {
        if (item.GetKeyCode() == keyEventFinalKey) {
            continue;
        }
        auto iter = find(infoPreKeys.begin(), infoPreKeys.end(), item.GetKeyCode());
        if (iter == infoPreKeys.end()) {
            MMI_HILOGW("No keyCode in preKeys");
            return false;
        }
        count++;
    }
    MMI_HILOGD("kevEventSize:%{public}d,infoSize:%{public}d", count, infoSize);
    auto keyItem = keyEvent->GetKeyItem();
    CHKPF(keyItem);
    auto upTime = keyEvent->GetActionTime();
    auto downTime = keyItem->GetDownTime();
    auto curDurationTime = keyOption->GetFinalKeyDownDuration();
    if (curDurationTime > 0 && (upTime - downTime >= (static_cast<int64_t>(curDurationTime) * 1000))) {
        MMI_HILOGE("Skip, upTime - downTime >= duration");
        return false;
    }
    return count == infoSize;
}

int32_t JsInputConsumerUtil::GetPreSubscribeId(const Callbacks &callbacks,
    const std::shared_ptr<SubscribeInfo> &event)
{
    CHKPR(event, INVALID_SUBID);
    auto it = callbacks.find(event->eventType);
    if (it == callbacks.end() || it->second.empty()) {
        MMI_HILOGW("The callbacks is empty");
        return INVALID_SUBID;
    }
    CHKPR(it->second.front(), INVALID_SUBID);
    return it->second.front()->subscribeId;
}

int32_t JsInputConsumerUtil::AddEventCallback(napi_env env,
    const std::shared_ptr<SubscribeInfo> &event, Callbacks &callbacks)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    if (callbacks.find(event->eventType) == callbacks.end()) {
        MMI_HILOGD("No callback in %{public}s", event->eventType.c_str());
        callbacks[event->eventType] = {};
    }
    napi_value handler1 = nullptr;
    CHKRR(napi_get_reference_value(env, event->callback, &handler1), GET_REFERENCE_VALUE, RET_ERR);
    auto iter = callbacks.find(event->eventType);
    for (const auto &info : iter->second) {
        napi_value handler2 = nullptr;
        CHKRR(napi_get_reference_value(env, info->callback, &handler2), GET_REFERENCE_VALUE, RET_ERR);
        bool isEqual = false;
        CHKRR(napi_strict_equals(env, handler1, handler2, &isEqual), STRICT_EQUALS, RET_ERR);
        if (isEqual) {
            MMI_HILOGW("Callback already exist");
            return RET_OK;
        }
    }
    iter->second.push_back(event);
    return RET_OK;
}

int32_t JsInputConsumerUtil::DelEventCallback(napi_env env,
    const std::shared_ptr<SubscribeInfo> &event, Callbacks &callbacks, int32_t &subscribeId)
{
    CALL_DEBUG_ENTER;
    CHKPR(event, ERROR_NULL_POINTER);
    if (callbacks.count(event->eventType) <= 0) {
        MMI_HILOGE("Callback doesn't exists");
        return RET_ERR;
    }
    auto &infos = callbacks[event->eventType];
    MMI_HILOGD("EventType:%{public}s,keySubInfos:%{public}zu", event->eventType.c_str(), infos.size());
    napi_value handler1 = nullptr;
    if (event->callback != nullptr) {
        CHKRR(napi_get_reference_value(env, event->callback, &handler1), GET_REFERENCE_VALUE, RET_ERR);
    }
    bool isEquals = false;
    for (auto iter = infos.begin(); iter != infos.end();) {
        if (*iter == nullptr) {
            infos.erase(iter++);
            continue;
        }
        if (handler1 != nullptr) {
            napi_value handler2 = nullptr;
            CHKRR(napi_get_reference_value(env, (*iter)->callback, &handler2), GET_REFERENCE_VALUE, RET_ERR);
            CHKRR(napi_strict_equals(env, handler1, handler2, &isEquals), STRICT_EQUALS, RET_ERR);
        }
        if (napi_delete_reference(env, (*iter)->callback) != napi_ok) {
            MMI_HILOGE("Delete reference failed");
            napi_throw_error(env, nullptr, "Delete reference failed");
            return RET_ERR;
        }
        auto info = *iter;
        infos.erase(iter++);
        if (infos.empty()) {
            subscribeId = info->subscribeId;
        }
        info = nullptr;
        MMI_HILOGD("Callback has deleted, size:%{public}zu", infos.size());
        if (isEquals) {
            break;
        }
    }
    MMI_HILOGD("Callback size:%{public}zu", infos.size());
    return RET_OK;
}

void JsInputConsumerUtil::AsyncWorkFn(napi_env env, const std::shared_ptr<SubscribeInfo> &event, napi_value &result)
{
    CHKPV(event);
    CHKPV(event->keyOption);
    CHKRV(napi_create_object(env, &result), CREATE_OBJECT);
    napi_value arr = nullptr;
    CHKRV(napi_create_array(env, &arr), CREATE_ARRAY);
    std::set<int32_t> preKeys = event->keyOption->GetPreKeys();
    int32_t i = 0;
    napi_value value = nullptr;
    for (const auto &preKey : preKeys) {
        CHKRV(napi_create_int32(env, preKey, &value), CREATE_INT32);
        CHKRV(napi_set_element(env, arr, i, value), SET_ELEMENT);
        ++i;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "preKeys", arr));
    SetNamedProperty(env, result, "finalKey", event->keyOption->GetFinalKey());
    SetNamedProperty(env, result, "isFinalKeyDown", event->keyOption->IsFinalKeyDown());
    SetNamedProperty(env, result, "finalKeyDownDuration", event->keyOption->GetFinalKeyDownDuration());
}

struct SubscribeInfoWorker {
    napi_env env { nullptr };
    std::shared_ptr<SubscribeInfo> reportEvent { nullptr };
};

void JsInputConsumerUtil::UvQueueWorkAsyncCallback(uv_work_t *work, int32_t status)
{
    CALL_DEBUG_ENTER;
    (void)status;
    CHKPV(work);
    if (work->data == nullptr) {
        MMI_HILOGE("Check data is null");
        delete work;
        return;
    }
    auto dataWorker = static_cast<SubscribeInfoWorker *>(work->data);
    auto event = dataWorker->reportEvent;
    napi_env env = dataWorker->env;
    delete work;
    delete dataWorker;
    CHKPV(event);

    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        MMI_HILOGE("Scope is nullptr");
        return;
    }
    napi_value callback = nullptr;
    CHKRV_SCOPE(env, napi_get_reference_value(env, event->callback, &callback), GET_REFERENCE_VALUE, scope);
    napi_value result = nullptr;
    AsyncWorkFn(env, event, result);
    napi_value callResult = nullptr;
    CHKRV_SCOPE(env, napi_call_function(env, nullptr, callback, 1, &result, &callResult), CALL_FUNCTION, scope);
    napi_close_handle_scope(env, scope);
}

void JsInputConsumerUtil::EmitAsyncCallbackWork(const std::shared_ptr<SubscribeInfo> &reportEvent)
{
    CALL_DEBUG_ENTER;
    CHKPV(reportEvent);
    uv_loop_s *loop = nullptr;
    CHKRV(napi_get_uv_event_loop(reportEvent->env, &loop), GET_UV_EVENT_LOOP);
    uv_work_t *work = new (std::nothrow) uv_work_t;
    CHKPV(work);
    auto dataWorker = new (std::nothrow) SubscribeInfoWorker();
    if (dataWorker == nullptr) {
        MMI_HILOGE("dataWorker is nullptr");
        delete work;
        return;
    }

    dataWorker->env = reportEvent->env;
    dataWorker->reportEvent = reportEvent;
    work->data = static_cast<void *>(dataWorker);

    int32_t ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvQueueWorkAsyncCallback);
    if (ret != 0) {
        delete dataWorker;
        delete work;
    }
}
} // namespace MMI
} // namespace OHOS