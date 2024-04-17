/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "js_register_module.h"
#include "input_manager.h"
#include <linux/input.h>

//#include "input_manager.h"
#include "mmi_log.h"
#include "napi_constants.h"
#include "util_napi_error.h"
#include "util_napi.h"

namespace OHOS {
namespace MMI {
namespace {
        constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JsInfraredRegister" }; 
}


bool CheckType(const napi_env& env, const napi_value& value, const napi_valuetype& type)
{
    napi_valuetype valuetype = napi_undefined;
    napi_typeof(env, value, &valuetype);
    if (valuetype != type) {
        return false;
    }
    return true;
}

bool IsArray(const napi_env& env, const napi_value& value)
{
    bool isArray = false;
    napi_status ret = napi_is_array(env, value, &isArray);
    if (ret != napi_ok) {
        return false;
    }
    return isArray;
}

bool ParseInt64(const napi_env& env, const napi_value& value, int64_t& result)
{
    if (!CheckType(env, value, napi_number)) {
        MMI_HILOGE("ParseInt64 type not number");
        return false;
    }
    if (napi_get_value_int64(env, value, &result) != napi_ok) {
        MMI_HILOGE("ParseInt64 cannot get value int64");
        return false;
    }
    return true;
}

bool ParsePatternArray(const napi_env& env, const napi_value& value, std::vector<int64_t>& result)
{
    uint32_t length = 0;
    if (!IsArray(env, value)) {
        MMI_HILOGE("  ParsePatternArray second para not array");
        return false;
    }
    napi_get_array_length(env, value, &length);
    // MMI_HILOGE("  ParsePatternArray lengtyh of  array; length:%{public}d", length);
    
    for (uint32_t i = 0; i < length; i++) {
        napi_value valueArray = nullptr;
        if (napi_get_element(env, value, i, &valueArray) != napi_ok) {
            MMI_HILOGE("ParsePatternArray  napi_get_element failed. index:%{public}d", i);
            return false; 
        }
        int64_t res = 0;        
                
        if (!ParseInt64(env, valueArray, res)) {
            MMI_HILOGE("  ParsePatternArray parse array fail. index:%{public}d", i);
            return false;
        }
        // MMI_HILOGE("ParsePatternArray parse array[%{public}d] succeed. value:%{public}" PRIu64, i, res);
        result.emplace_back(res);
    }
    return true;
};



bool ParseTransmitInfraredJSParam(const napi_env& env,  const napi_callback_info &info, int64_t & infraredFrequency,  
std::vector<int64_t> & vecPattern)
{
CALL_DEBUG_ENTER;
size_t argc = 2;
napi_value argv[2];
CHKRF(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);
// MMI_HILOGE(" result from ParseTransmitInfraredJSParam argc :%{public}d",   argc);
if ( argc != 2) {
    MMI_HILOGE("ParseTransmitInfraredJSParam Parameter number error");
    return false;
}

if (!CheckType(env, argv[0], napi_number)) {
    MMI_HILOGE("ParseTransmitInfraredJSParam  infraredFrequency parameter[0] type is invalid.  ");
    THROWERR_API9(env, COMMON_PARAMETER_ERROR, "infraredFrequency", "number");
    return false;
}
    
CHKRF(napi_get_value_int64(env, argv[0], &infraredFrequency), "get number64 value error");
// std::string context  =  std::to_string(infraredFrequency);
// MMI_HILOGE("ParseTransmitInfraredJSParam  infraredFrequency parameter[0]  .  :%{public}s", context.c_str());
if(!ParsePatternArray(env, argv[1], vecPattern)) {
    MMI_HILOGE("ParsePatternArray  parse pattern array fail." );
    return false;
}       
return true;    
} 

static void ThrowError(napi_env env, int32_t code, std::string operateType)
{
    int32_t errorCode = -code;
    MMI_HILOGE("Operate %{public}s  requst error. returnCode:%{public}d", operateType.c_str(), code);
    if (errorCode == COMMON_PERMISSION_CHECK_ERROR) {
        THROWERR_API9(env, COMMON_PERMISSION_CHECK_ERROR, "Infrared", "ohos.permission.INFRARED_EMITTER");
    } else {
        return;
    }
}

int64_t GetNamePropertyInt64(const napi_env &env, const napi_value &object, const std::string &name)
{
    int64_t value = 0;
    napi_value napiValue = {};
    napi_get_named_property(env, object, name.c_str(), &napiValue);
    napi_valuetype tmpType = napi_undefined;
    if (napi_typeof(env, napiValue, &tmpType) != napi_ok) {
        MMI_HILOGE("Call napi_typeof failed");
        return value;
    }
    if (tmpType != napi_number) {
        MMI_HILOGI("The value is not number");
        return value;
    }
    napi_get_value_int64(env, napiValue, &value);
    return value;
}



napi_value CreateInfraredFrequencyItem(napi_env env, const InfraredFrequency &infraredFrequency)
{
    napi_value result;
    napi_status status = napi_create_object(env, &result);
    CHKRP(status, CREATE_OBJECT);
    napi_value jsMax;
    CHKRP(napi_create_int64(env, infraredFrequency.max_, &jsMax), "napi_create_int64:max");
    CHKRP(napi_set_named_property(env, result, "max", jsMax), SET_NAMED_PROPERTY);
    napi_value jsMin;
    CHKRP(napi_create_int64(env, infraredFrequency.min_, &jsMin), "napi_create_int64:min");
    CHKRP(napi_set_named_property(env, result, "min", jsMin), SET_NAMED_PROPERTY);

    return result;

}  

static napi_value HasIrEmitter(napi_env env, napi_callback_info info)
{
    CALL_DEBUG_ENTER;
    napi_value result = nullptr;

    napi_status status = napi_get_boolean(env, true, &result);
    if (status != napi_ok) {
        THROWERR_API9(env, COMMON_PARAMETER_ERROR, "type", "boolean");
        return nullptr;
    }
    return result;
}

static napi_value GetInfraredFrequencies(napi_env env, napi_callback_info info)
{
    // MMI_HILOGI("***************************** GetInfraredFrequencies start para :  " );
    CALL_DEBUG_ENTER;
    napi_value result = nullptr;
    CHKRP(napi_create_array(env, &result), CREATE_ARRAY);
    std::vector<InfraredFrequency> requencys;
    int32_t ret = InputManager::GetInstance()->GetInfraredFrequencies(requencys);
        /********处理权限错误的问题 *******/
    if(0 > ret) {
        MMI_HILOGE("GetInfraredFrequencies requst error. Permission Error. returnCode:%{public}d", ret);
        ThrowError(env, ret, "GetInfraredFrequencies");
        return nullptr;    
    }
    if(ret != RET_OK) {
        if(COMMON_PERMISSION_CHECK_ERROR == ret) {            
        MMI_HILOGE("GetInfraredFrequencies  requst error. Permission Error. Positive returnCode:%{public}d", ret);    
            ThrowError(env, ret, "GetInfraredFrequencies");
             return nullptr;   
        }
        MMI_HILOGE("Parse GetInfraredFrequencies  requst error. returnCode: %{public}d", ret);
        return result;
    }
    int32_t size = requencys.size();
    std::string logPrint = "size:" + std::to_string(size) + ";\r\n";  
    MMI_HILOGI(" result from GetInfraredFrequencies :%{public}s " , logPrint.c_str());
    for(int32_t i = 0; i < size; i++)
    {
        InfraredFrequency frequencyItem = requencys[i];
        logPrint = std::to_string(i) + "max:" + std::to_string(frequencyItem.max_) + ";min:" 
        +  std::to_string(frequencyItem.min_) + ";\r\n";
        MMI_HILOGI("index:[%{}d]:%{public}s " ,i ,logPrint.c_str());
    } 
    
    CHKRP(napi_create_array(env, &result), CREATE_ARRAY); 
    for(size_t i = 0; i < requencys.size(); i++) {
        napi_value item = CreateInfraredFrequencyItem(env, requencys[i]);
        if(item == nullptr) {
            MMI_HILOGE("CreateInfraredFrequencyItem error");
            return nullptr;
        }
        //napi_set_element(env, result, i, item);
        CHKRP(napi_set_element(env, result, i, item), SET_ELEMENT);
    }
    /********* print debug log, delete this while finish test  ********/             
    return result;
}

static napi_value TransmitInfrared(napi_env env, napi_callback_info info) {
    MMI_HILOGI("***************************** js_register_module start para :  " );
    CALL_DEBUG_ENTER;
    napi_value result = nullptr;
    int64_t number =-1;
    std::vector<int64_t>  pattern;
    if(!ParseTransmitInfraredJSParam(env, info, number, pattern)) {
        MMI_HILOGE("Parse TransmitInfrared JSParam error");
        THROWERR_CUSTOM(env, COMMON_PARAMETER_ERROR, "Parse TransmitInfrared JSParam error");
        return nullptr;
    }

    int32_t size = static_cast<int32_t>(pattern.size());
    std::string context = "number:" + std::to_string(number) + "\r\n" + ";  size=" + std::to_string(size) + ";\r\n";
    MMI_HILOGI("js_register_module.TransmitInfrared para size :%{public}s " , context.c_str());
    for(int32_t i = 0; i < size; i++) {
        context =  std::to_string(i) + ":  pattern: " + std::to_string(pattern[i]) + ";\r\n";
        MMI_HILOGI("***************************** js_register_module TransmitInfrared para :%{public}s " , context.c_str());
    }            
    int32_t ret = InputManager::GetInstance()->TransmitInfrared(number, pattern);      
    /********处理权限错误的问题 *******/
    if(0 > ret) {
        MMI_HILOGE("TransmitInfrared  requst error. Permission Error. returnCode:%{public}d", ret);
        ThrowError(env, ret, "TransmitInfrared");
        return nullptr;    
    }         
    if(ret != RET_OK) {
        if(COMMON_PERMISSION_CHECK_ERROR == ret) {      
        MMI_HILOGE("TransmitInfrared  requst error. Permission Error. Positive returnCode:%{public}d", ret);      
            ThrowError(env, ret, "TransmitInfrared");
             return nullptr;   
        }
        MMI_HILOGE("TransmitInfrared  requst error. returnCode:%{public}d", ret);
        return nullptr;
    } 
    CHKRP(napi_create_int32(env, 0, &result), CREATE_INT32);
    return result;
}


EXTERN_C_START
static napi_value MmiInit(napi_env env, napi_value exports)
{
    CALL_DEBUG_ENTER;
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("hasIrEmitter", HasIrEmitter),
        DECLARE_NAPI_FUNCTION("getInfraredFrequencies", GetInfraredFrequencies),
        DECLARE_NAPI_FUNCTION("transmitInfrared", TransmitInfrared)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}
EXTERN_C_END

static napi_module infraredModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = MmiInit,
    .nm_modname = "multimodalInput.infraredEmitter",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&infraredModule);
}

}
}