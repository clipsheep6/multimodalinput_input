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

#include "js_util.h"

#include "mmi_log.h"
#include "util_napi.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JsUtil" };
const std::string GET_REFERENCE = "napi_get_reference_value";
const std::string STRICT_EQUALS = "napi_strict_equals";
const std::string DELETE_REFERENCE = "napi_delete_reference";
const std::string CREATE_ARRAY = "napi_create_array";
const std::string CREATE_INT32 = "napi_create_int32";
const std::string SET_ELEMENT = "napi_set_element";
const std::string SET_NAMED_PROPERTY = "napi_set_named_property";
const std::string CREATE_STRING_UTF8 = "napi_create_string_utf8";
const std::string CREATE_OBJECT = "napi_create_object";

constexpr int32_t ABS_MT_TOUCH_MAJOR = 0x30;
constexpr int32_t ABS_MT_TOUCH_MINOR = 0x31;
constexpr int32_t ABS_MT_ORIENTATION = 0x34;
constexpr int32_t ABS_MT_POSITION_X  = 0x35;
constexpr int32_t ABS_MT_POSITION_Y = 0x36;
constexpr int32_t ABS_MT_PRESSURE = 0x3a;
constexpr int32_t ABS_MT_TOOL_X = 0x3c;
constexpr int32_t ABS_MT_TOOL_Y = 0x3d;
JsUtil::AxisType g_axisType[] = {
    {"touch_major", ABS_MT_TOUCH_MAJOR},
    {"touch_minor", ABS_MT_TOUCH_MINOR},
    {"orientation", ABS_MT_ORIENTATION},
    {"position_x", ABS_MT_POSITION_X},
    {"position_y", ABS_MT_POSITION_Y},
    {"pressure", ABS_MT_PRESSURE},
    {"tool_x", ABS_MT_TOOL_X},
    {"tool_y", ABS_MT_TOOL_Y},
};
} // namespace
int32_t JsUtil::GetInt32(uv_work_t *work)
{
    int32_t *uData = static_cast<int32_t*>(work->data);
    int32_t userData = *uData;
    delete uData;
    delete work;
    return userData;
}

bool JsUtil::IsHandleEquals(napi_env env, napi_value handle, napi_ref ref)
{
    napi_value handlerTemp = nullptr;
    CHKRB(env, napi_get_reference_value(env, ref, &handlerTemp), GET_REFERENCE);
    bool isEqual = false;
    CHKRB(env, napi_strict_equals(env, handle, handlerTemp, &isEqual), STRICT_EQUALS);
    return isEqual;
}

bool JsUtil::GetDeviceInfo(std::unique_ptr<CallbackInfo> &cbTemp, napi_value &object)
{
    CHKPF(cbTemp);
    CHKPF(cbTemp->env);
    napi_value id = nullptr;
    CHKRB(cbTemp->env, napi_create_int32(cbTemp->env, cbTemp->data.device->id, &id), CREATE_INT32);
    napi_value name = nullptr;
    CHKRB(cbTemp->env, napi_create_string_utf8(cbTemp->env, (cbTemp->data.device->name).c_str(),
        NAPI_AUTO_LENGTH, &name), CREATE_STRING_UTF8);
    CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, object, "id", id), SET_NAMED_PROPERTY);
    CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, object, "name", name), SET_NAMED_PROPERTY);
    napi_value busType = nullptr;
    CHKRB(cbTemp->env, napi_create_int32(cbTemp->env, cbTemp->data.device->busType, &busType), CREATE_INT32);
    CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, object, "busType", busType), SET_NAMED_PROPERTY);
    napi_value product = nullptr;
    CHKRB(cbTemp->env, napi_create_int32(cbTemp->env, cbTemp->data.device->product, &product), CREATE_INT32);
    CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, object, "product", product), SET_NAMED_PROPERTY);
    napi_value vendor = nullptr;
    CHKRB(cbTemp->env, napi_create_int32(cbTemp->env, cbTemp->data.device->vendor, &vendor), CREATE_INT32);
    CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, object, "vendor", vendor), SET_NAMED_PROPERTY);
    napi_value version = nullptr;
    CHKRB(cbTemp->env, napi_create_int32(cbTemp->env, cbTemp->data.device->version, &version), CREATE_INT32);
    CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, object, "version", version), SET_NAMED_PROPERTY);
    if (!GetDeviceAxisInfo(cbTemp, object)) {
        MMI_HILOGE("get device basic info failed");
        return false;
    }
    return true;
}

bool JsUtil::GetDeviceAxisInfo(std::unique_ptr<CallbackInfo> &cbTemp, napi_value &object)
{
    CHKPF(cbTemp);
    CHKPF(cbTemp->env);
    napi_value axisRanges = nullptr;
    CHKRB(cbTemp->env, napi_create_array(cbTemp->env, &axisRanges), CREATE_ARRAY);
    napi_value axisRange = nullptr;
    uint32_t i = 0;
    for (const auto &item : cbTemp->data.device->axis) {
        for (const auto &axisTemp : g_axisType) {
            if (item.axisType == axisTemp.axisType) {
                CHKRB(cbTemp->env, napi_create_object(cbTemp->env, &axisRange), CREATE_OBJECT);
                napi_value axisType = nullptr;
                CHKRB(cbTemp->env, napi_create_string_utf8(cbTemp->env, axisTemp.axisTypeName.c_str(),
                    NAPI_AUTO_LENGTH, &axisType), CREATE_STRING_UTF8);
                CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, axisRange, "axisType", axisType),
                    SET_NAMED_PROPERTY);
                napi_value min = nullptr;
                CHKRB(cbTemp->env, napi_create_int32(cbTemp->env, item.min, &min), CREATE_INT32);
                CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, axisRange, "min", min), SET_NAMED_PROPERTY);
                napi_value max = nullptr;
                CHKRB(cbTemp->env, napi_create_int32(cbTemp->env, item.max, &max), CREATE_INT32);
                CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, axisRange, "max", max), SET_NAMED_PROPERTY);
                napi_value fuzz = nullptr;
                CHKRB(cbTemp->env, napi_create_int32(cbTemp->env, item.fuzz, &fuzz), CREATE_INT32);
                CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, axisRange, "fuzz", fuzz), SET_NAMED_PROPERTY);
                napi_value flat = nullptr;
                CHKRB(cbTemp->env, napi_create_int32(cbTemp->env, item.flat, &flat), CREATE_INT32);
                CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, axisRange, "flat", flat), SET_NAMED_PROPERTY);
                napi_value resolution = nullptr;
                CHKRB(cbTemp->env, napi_create_int32(cbTemp->env, item.resolution, &resolution), CREATE_INT32);
                CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, axisRange, "resolution", resolution),
                    SET_NAMED_PROPERTY);
                CHKRB(cbTemp->env, napi_set_element(cbTemp->env, axisRanges, i, axisRange), SET_ELEMENT);
                ++i;
            }
        }
    }
    CHKRB(cbTemp->env, napi_set_named_property(cbTemp->env, object, "axisRanges", axisRanges), SET_NAMED_PROPERTY);
    return true;
}

JsUtil::CallbackInfo::CallbackInfo() {}

JsUtil::CallbackInfo::~CallbackInfo()
{
    CALL_LOG_ENTER;
    if (ref != nullptr && env != nullptr) {
        CHKRV(env, napi_delete_reference(env, ref), DELETE_REFERENCE);
        env = nullptr;
    }
}
} // namespace MMI
} // namespace OHOS