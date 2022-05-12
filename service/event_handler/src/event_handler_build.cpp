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

#include "event_handler_build.h"


int32_t EventHandlerBuild::Build()
{
    // 判断 /etc/input_handler_config.json
    if (!ParseConfigFile()) {
        return RET_ERR;
    }
}

int32_t EventHandlerBuild::ParseConfigFile()
{
}

std::shared_ptr<IInputEventHandler> EventHandlerBuild::BuildChainKey()
{
    auto keyEventHandler_ = std::make_shared<IInputEventHandler>();
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    if (IsConfigContainKey) {
        for (auto & info : keyInfos) {
            if (info.name == "KeyEventHandler") {
                keyEventHandler_->AddConstructHandler<KeyEventHandler>(info.priority);
            } else if (info.name == "IInterceptorManagerGlobal") {
                keyEventHandler_->AddInstanceHandler<IInterceptorManagerGlobal>(info.priority);
            }
        }
    } else {
        keyEventHandler_->AddConstructHandler<KeyEventHandler>(1);
        keyEventHandler_->AddInstanceHandler<IInterceptorManagerGlobal>(1);
        keyEventHandler_->AddInstanceHandler<IKeyCommandManager>(1);
        keyEventHandler_->AddConstructHandler<KeyEventSubscriber>(1);
        keyEventHandler_->AddConstructHandler<InputHandlerManagerGlobal>(1);
        keyEventHandler_->AddConstructHandler<EventDispatch>(1);
    }
#endif // OHOS_BUILD_ENABLE_KEYBOARD
    return keyEventHandler_;
}

std::shared_ptr<IInputEventHandler> EventHandlerBuild::BuildChainPointer()
{

}
std::shared_ptr<IInputEventHandler> EventHandlerBuild::BuildChainTouch()
{

}