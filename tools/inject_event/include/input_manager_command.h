/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef INJECT_EVENT_COMMAND_H
#define INJECT_EVENT_COMMAND_H

#include <regex>
#include <string>
#include <vector>

// #include "i_mouse_death_listener.h"
// #include "input_manager_service_interface.h"
// #include "mouse_death_listener_proxy.h"
// #include "mouse_death_listener_stub.h"
// #include "multimodal_input_errors.h"
// #include "multimodal_input_service_proxy.h"



namespace OHOS {

class InputManagerCommand {
public:
    int32_t ParseCommand(int argc, char *argv[]);
    int32_t ConnectService();
    void ShowUsage();
private:
    void InitializeMouseDeathStub();
    // sptr<IMultimodalInputService> service_ { nullptr };
    // sptr<IMouseDeathListener> listenerStub_ { nullptr };
};
} // namespace OHOS

#endif // INJECT_EVENT_COMMAND_H

