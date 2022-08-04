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

#ifndef COOPERATE_MESSAGES_H
#define COOPERATE_MESSAGES_H

namespace OHOS {
namespace MMI {
enum class CooperateMessages {
    MSG_COOPERATE_OPEN_SUCCESS = 100,
    MSG_COOPERATE_OPEN_FAIL = 101,
    MSG_COOPERATE_INFO_START = 200,
    MSG_COOPERATE_INFO_SUCCESS = 201,
    MSG_COOPERATE_INFO_FAIL = 202,
    MSG_COOPERATE_CLOSE = 300,
    MSG_COOPERATE_CLOSE_SUCCESS = 301,
    MSG_COOPERATE_STOP = 400,
    MSG_COOPERATE_STOP_SUCCESS = 401,
    MSG_COOPERATE_STATE_ON = 500,
    MSG_COOPERATE_STATE_OFF = 501,
};
} // namespace MMI
} // namespace OHOS
#endif // COOPERATE_MESSAGES_H