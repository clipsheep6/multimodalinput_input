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
#ifndef PROTO_H
#define PROTO_H
#include <sys/types.h>

enum class MmiMessageId : int32_t {
    INVALID,
    ADD_INPUT_DEVICE_LISTENER,
    REMOVE_INPUT_DEVICE_LISTENER,
    INPUT_DEVICE_KEYBOARD_TYPE,

    BEGIN = 1001,
    REGISTER_APP_INFO,
    REGISTER_MSG_HANDLER,
    UNREGISTER_MSG_HANDLER,
    CHECK_REPLY_MESSAGE,
    ADD_KEY_EVENT_INTERCEPTOR,
    KEY_EVENT_INTERCEPTOR,
    REMOVE_KEY_EVENT_INTERCEPTOR,
    ADD_TOUCH_EVENT_INTERCEPTOR,
    TOUCH_EVENT_INTERCEPTOR,
    REMOVE_TOUCH_EVENT_INTERCEPTOR,
    ADD_POINTER_INTERCEPTOR,
    REMOVE_POINTER_INTERCEPTOR,
    POINTER_EVENT_INTERCEPTOR,
    ADD_INPUT_EVENT_MONITOR,
    REMOVE_INPUT_EVENT_MONITOR,
    ADD_INPUT_HANDLER,
    REMOVE_INPUT_HANDLER,
    MARK_CONSUMED,
    ADD_INPUT_EVENT_TOUCHPAD_MONITOR,
    REMOVE_INPUT_EVENT_TOUCHPAD_MONITOR,
    NOTICE_ANR,
    MARK_PROCESS,
    ON_SUBSCRIBE_KEY,
    ON_KEY_EVENT,
    ON_POINTER_EVENT,
    REPORT_KEY_EVENT,
    REPORT_POINTER_EVENT,
    ON_DEVICE_ADDED,
    ON_DEVICE_REMOVED,

#ifdef OHOS_BUILD_ENABLE_COOPERATE
    COOPERATION_ADD_LISTENER,
    COOPERATION_MESSAGE,
    COOPERATION_GET_STATE,
#endif // OHOS_BUILD_ENABLE_COOPERATE
};

enum TokenType : int32_t {
    TOKEN_INVALID = -1,
    TOKEN_HAP = 0,
    TOKEN_NATIVE,
    TOKEN_SHELL,
};

enum ANREventType {
    ANR_DISPATCH,
    ANR_MONITOR,
};
#endif // PROTO_H