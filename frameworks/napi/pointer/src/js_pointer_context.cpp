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

#include "js_pointer_context.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "JsPointerContext" };
enum IconId {
    ANGLE = 0,
    HAND = 1,
    BACkGROUNDER = 2,
    BUSY = 3,
    CANDIDATE = 4,
    DIAGONALADJUSTMENT1 = 5,
    DIAGONALADJUSTMENT2 = 6,
    HORIZONTALTRIM = 7,
    VERTICALADJUSTMENT = 8,
    DISABLE = 9,
    HANDWRITTEN = 10,
    HELPCHOOSE = 11,
    MOVESTYLE = 12,
    PERSONALSELECTION = 13,
    PRECISIONSELECT = 14,
    SITESELECTION = 15,
    TEXTSELECTION = 16,
};
} // namespace

JsPointerContext::JsPointerContext() : mgr_(std::make_shared<JsPointerManager>()) {}

napi_value JsPointerContext::CreateInstance(napi_env env)
{
    CALL_DEBUG_ENTER;
    napi_value global = nullptr;
    CHKRP(env, napi_get_global(env, &global), GET_GLOBAL);

    constexpr char className[] = "JsPointerContext";
    napi_value jsClass = nullptr;
    napi_property_descriptor desc[] = {};
    napi_status status = napi_define_class(env, className, sizeof(className), JsPointerContext::CreateJsObject,
                                           nullptr, sizeof(desc) / sizeof(desc[0]), nullptr, &jsClass);
    CHKRP(env, status, DEFINE_CLASS);

    status = napi_set_named_property(env, global, "multimodalinput_pointer_class", jsClass);
    CHKRP(env, status, SET_NAMED_PROPERTY);

    napi_value jsInstance = nullptr;
    CHKRP(env, napi_new_instance(env, jsClass, 0, nullptr, &jsInstance), NEW_INSTANCE);
    CHKRP(env, napi_set_named_property(env, global, "multimodal_pointer", jsInstance), SET_NAMED_PROPERTY);

    JsPointerContext *jsContext = nullptr;
    CHKRP(env, napi_unwrap(env, jsInstance, (void**)&jsContext), UNWRAP);
    CHKPP(jsContext);
    CHKRP(env, napi_create_reference(env, jsInstance, 1, &(jsContext->contextRef_)), CREATE_REFERENCE);

    uint32_t refCount = 0;
    CHKRP(env, napi_reference_ref(env, jsContext->contextRef_, &refCount), REFERENCE_REF);
    return jsInstance;
}

napi_value JsPointerContext::CreateJsObject(napi_env env, napi_callback_info info)
{
    CALL_DEBUG_ENTER;
    napi_value thisVar = nullptr;
    void *data = nullptr;
    CHKRP(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data), GET_CB_INFO);

    JsPointerContext *jsContext = new (std::nothrow) JsPointerContext();
    CHKPP(jsContext);
    napi_status status = napi_wrap(env, thisVar, jsContext, [](napi_env env, void* data, void* hin) {
        MMI_HILOGI("jsvm ends");
        JsPointerContext *context = static_cast<JsPointerContext*>(data);
        delete context;
    }, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsContext;
        THROWERR(env, "Failed to wrap native instance");
        return nullptr;
    }
    return thisVar;
}

JsPointerContext* JsPointerContext::GetInstance(napi_env env)
{
    CALL_DEBUG_ENTER;
    napi_value global = nullptr;
    CHKRP(env, napi_get_global(env, &global), GET_GLOBAL);

    bool result = false;
    CHKRP(env, napi_has_named_property(env, global, "multimodal_pointer", &result), HAS_NAMED_PROPERTY);
    if (!result) {
        THROWERR(env, "multimodal_pointer was not found");
        return nullptr;
    }

    napi_value object = nullptr;
    CHKRP(env, napi_get_named_property(env, global, "multimodal_pointer", &object), SET_NAMED_PROPERTY);
    if (object == nullptr) {
        THROWERR(env, "object is nullptr");
        return nullptr;
    }

    JsPointerContext *instance = nullptr;
    CHKRP(env, napi_unwrap(env, object, (void**)&instance), UNWRAP);
    if (instance == nullptr) {
        THROWERR(env, "instance is nullptr");
        return nullptr;
    }
    return instance;
}

std::shared_ptr<JsPointerManager> JsPointerContext::GetJsPointerMgr() const
{
    return mgr_;
}

napi_value JsPointerContext::SetPointerVisible(napi_env env, napi_callback_info info)
{
    CALL_DEBUG_ENTER;
    size_t argc = 2;
    napi_value argv[2];
    CHKRP(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);
    if (argc < 1 || argc > 2) {
        THROWERR(env, "the number of parameters is not as expected");
        return nullptr;
    }
    if (!JsCommon::TypeOf(env, argv[0], napi_boolean)) {
        THROWERR(env, "The first parameter type is wrong");
        return nullptr;
    }
    bool visible = true;
    CHKRP(env, napi_get_value_bool(env, argv[0], &visible), GET_BOOL);

    JsPointerContext *jsPointer = JsPointerContext::GetInstance(env);
    auto jsPointerMgr = jsPointer->GetJsPointerMgr();
    if (argc == 1) {
        return jsPointerMgr->SetPointerVisible(env, visible);
    }
    if (!JsCommon::TypeOf(env, argv[1], napi_function)) {
        THROWERR(env, "The second parameter type is wrong");
        return nullptr;
    }
    return jsPointerMgr->SetPointerVisible(env, visible, argv[1]);
}

napi_value JsPointerContext::IsPointerVisible(napi_env env, napi_callback_info info)
{
    CALL_DEBUG_ENTER;
    size_t argc = 1;
    napi_value argv[1];
    CHKRP(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);
    if (argc > 1) {
        THROWERR(env, "the number of parameters is not as expected");
        return nullptr;
    }

    JsPointerContext *jsPointer = JsPointerContext::GetInstance(env);
    auto jsPointerMgr = jsPointer->GetJsPointerMgr();
    if (argc == 0) {
        return jsPointerMgr->IsPointerVisible(env);
    }
    if (!JsCommon::TypeOf(env, argv[0], napi_function)) {
        THROWERR(env, "The first parameter type is wrong");
        return nullptr;
    }

    return jsPointerMgr->IsPointerVisible(env, argv[0]);
}

napi_value JsPointerContext::SetPointerStyle(napi_env env, napi_callback_info info)
{
    CALL_DEBUG_ENTER;
    size_t argc = 3;
    napi_value argv[3];
    CHKRP(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);
    if (argc < 1 || argc > 3) {
        THROWERR(env, "The number of parameters is not as expected");
        return nullptr;
    }

    if (!JsCommon::TypeOf(env, argv[0], napi_number)) {
        THROWERR(env, "The first parameter type is wrong");
        return nullptr;
    }
    int32_t windowid = 0;
    CHKRP(env, napi_get_value_int32(env, argv[0], &windowid), GET_INT32);

    if (!JsCommon::TypeOf(env, argv[1], napi_number)) {
        THROWERR(env, "The second parameter type is wrong");
        return nullptr;
    }
    int32_t pointerStyle = 0;
    CHKRP(env, napi_get_value_int32(env, argv[1], &pointerStyle), GET_INT32);

    JsPointerContext *jsPointer = JsPointerContext::GetInstance(env);
    auto jsmouseMgr = jsPointer->GetJsPointerMgr();
    if (argc == 2) {
        return jsmouseMgr->SetPointerStyle(env, windowid, pointerStyle);
    }
    if (!JsCommon::TypeOf(env, argv[2], napi_function)) {
        THROWERR(env, "The third parameter type is wrong");
        return nullptr;
    }
    return jsmouseMgr->SetPointerStyle(env, windowid, pointerStyle, argv[2]);
}

napi_value JsPointerContext::GetPointerStyle(napi_env env, napi_callback_info info)
{
    CALL_DEBUG_ENTER;
    size_t argc = 2;
    napi_value argv[2];
    CHKRP(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr), GET_CB_INFO);
    if (argc < 1 || argc > 2) {
        THROWERR(env, "The number of parameters is not as expected");
        return nullptr;
    }

    if (!JsCommon::TypeOf(env, argv[0], napi_number)) {
        THROWERR(env, "The first parameter type is wrong");
        return nullptr;
    }
    int32_t windowid = 0;
    CHKRP(env, napi_get_value_int32(env, argv[0], &windowid), GET_INT32);

    JsPointerContext *jsPointer = JsPointerContext::GetInstance(env);
    auto jsmouseMgr = jsPointer->GetJsPointerMgr();
    if (argc == 1) {
        return jsmouseMgr->GetPointerStyle(env, windowid);
    }
    if (!JsCommon::TypeOf(env, argv[1], napi_function)) {
        THROWERR(env, "The second parameter type is wrong");
        return nullptr;
    }
    return jsmouseMgr->GetPointerStyle(env, windowid, argv[1]);
}

napi_value JsPointerContext::CreatePointerStyle(napi_env env, napi_value exports)
{
    CALL_DEBUG_ENTER;
    napi_value angle = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::ANGLE, &angle), CREATE_INT32);
    napi_value hand = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::HAND, &hand), CREATE_INT32);
    napi_value backgrounder = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::BACkGROUNDER, &backgrounder), CREATE_INT32);
    napi_value busy = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::BUSY, &busy), CREATE_INT32);
    napi_value candidate = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::CANDIDATE, &candidate), CREATE_INT32);
    napi_value diagonaladjustment1 = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::DIAGONALADJUSTMENT1, &diagonaladjustment1), CREATE_INT32);
    napi_value diagonaladjustment2 = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::DIAGONALADJUSTMENT2, &diagonaladjustment2), CREATE_INT32);
    napi_value horizontaltrim = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::HORIZONTALTRIM, &horizontaltrim), CREATE_INT32);
    napi_value verticaladjustment = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::VERTICALADJUSTMENT, &verticaladjustment), CREATE_INT32);
    napi_value disable = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::DISABLE, &disable), CREATE_INT32);
    napi_value handwritten = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::HANDWRITTEN, &handwritten), CREATE_INT32);
    napi_value helpchoose = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::HELPCHOOSE, &helpchoose), CREATE_INT32);
    napi_value move = nullptr; 
    CHKRP(env, napi_create_int32(env, IconId::MOVESTYLE, &move), CREATE_INT32);
    napi_value personalselection = nullptr; 
    CHKRP(env, napi_create_int32(env, IconId::PERSONALSELECTION, &personalselection), CREATE_INT32);
    napi_value precisionselect = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::PRECISIONSELECT, &precisionselect), CREATE_INT32);
    napi_value siteselection = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::SITESELECTION, &siteselection), CREATE_INT32);
    napi_value textselection = nullptr;
    CHKRP(env, napi_create_int32(env, IconId::TEXTSELECTION, &textselection), CREATE_INT32);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("ANGLE", angle),
        DECLARE_NAPI_STATIC_PROPERTY("HAND", hand),
        DECLARE_NAPI_STATIC_PROPERTY("BACkGROUNDER", busy),
        DECLARE_NAPI_STATIC_PROPERTY("CANDIDATE", candidate),
        DECLARE_NAPI_STATIC_PROPERTY("DIAGONALADJUSTMENT1", diagonaladjustment1),
        DECLARE_NAPI_STATIC_PROPERTY("DIAGONALADJUSTMENT2", diagonaladjustment2),
        DECLARE_NAPI_STATIC_PROPERTY("HORIZONTALTRIM", horizontaltrim),
        DECLARE_NAPI_STATIC_PROPERTY("VERTICALADJUSTMENT", verticaladjustment),
        DECLARE_NAPI_STATIC_PROPERTY("DISABLE", disable),
        DECLARE_NAPI_STATIC_PROPERTY("HANDWRITTEN", handwritten),
        DECLARE_NAPI_STATIC_PROPERTY("HELPCHOOSE", helpchoose),
        DECLARE_NAPI_STATIC_PROPERTY("MOVE", move),
        DECLARE_NAPI_STATIC_PROPERTY("PERSONALSELECTION", personalselection),
        DECLARE_NAPI_STATIC_PROPERTY("PRECISIONSELECT", precisionselect),
        DECLARE_NAPI_STATIC_PROPERTY("SITESELECTION", siteselection),
        DECLARE_NAPI_STATIC_PROPERTY("TEXTSELECTION", textselection),
    };
    napi_value result = nullptr;
    CHKRP(env, napi_define_class(env, "Iconid", NAPI_AUTO_LENGTH, EnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result), DEFINE_CLASS);
    CHKRP(env, napi_set_named_property(env, exports, "Iconid", result), SET_NAMED_PROPERTY);
    return exports;
}

napi_value JsPointerContext::EnumConstructor(napi_env env, napi_callback_info info)
{
    CALL_DEBUG_ENTER;
    size_t argc = 0;
    napi_value args[1] = {0};
    napi_value ret = nullptr;
    void *data = nullptr;
    CHKRP(env, napi_get_cb_info(env, info, &argc, args, &ret, &data), GET_CB_INFO);
    return ret;
}
napi_value JsPointerContext::Export(napi_env env, napi_value exports)
{
    CALL_DEBUG_ENTER;
    auto instance = CreateInstance(env);
    if (instance == nullptr) {
        THROWERR(env, "failed to create instance");
        return nullptr;
    }
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_FUNCTION("setPointerVisible", SetPointerVisible),
        DECLARE_NAPI_STATIC_FUNCTION("isPointerVisible", IsPointerVisible),
        DECLARE_NAPI_STATIC_FUNCTION("setPointerStyle", SetPointerStyle),
        DECLARE_NAPI_STATIC_FUNCTION("getPointerStyle", GetPointerStyle),
    };
    CHKRP(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc), DEFINE_PROPERTIES);
    if(CreatePointerStyle(env, exports) == nullptr) {
        THROWERR(env, "Failed to create pointer style");
        return nullptr;
    }
    return exports;
}
} // namespace MMI
} // namespace OHOS
