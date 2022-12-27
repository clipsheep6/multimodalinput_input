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

#include "touch_2_key_handler.h"
#include "i_input_event_handler_plugin.h"

namespace OHOS {
namespace MMI {
class Touch2KeyPlugin : public IPlugin
{
public:
    Touch2KeyPlugin() = default;
    DISALLOW_COPY_AND_MOVE(Touch2KeyPlugin);
    virtual ~Touch2KeyPlugin() = default;
    virtual bool Init(IInputEventPluginContext *context);
    virtual void Uninit() {}
private:
    IInputEventPluginContext *context_;
};

extern "C" IPlugin* create() {
    return new Touch2KeyPlugin();
}

extern "C" void Release(IPlugin* p) {
    delete p;
    p = nullptr;
}
} // namespace MMI
} // namespace OHOS
#endif // TOUCH_2_KEY_PLUGIN_H