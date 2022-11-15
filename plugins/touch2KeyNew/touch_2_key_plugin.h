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

#ifndef TOUCH_2_KEY_PLUGIN_H
#define TOUCH_2_KEY_PLUGIN_H

namespace OHOS {
namespace MMI {
#include "i_input_event_handler_plugin.h"
#include "touch_2_key_new_handler.h"
class Touch2KeyPlugin : public IPlugin
{
public:
    virtual bool Init(IInputEventPluginContext *context)
    {
        context_ = context;
        context_->SetEventHandler(new Touch2KeyNewHandler(context));
    }
    virtual void Uninit()
    {
        delete;
    }
private:
    IInputEventPluginContext &context_;
};

//the class factories
extern "C" IPlugin* create() {
    return new Touch2KeyPlugin();
}
extern "C" void Release(IPlugin* p) {
    delete p;
}

} // namespace MMI
} // namespace OHOS
#endif // TOUCH_2_KEY_NEW_HANDLER_H