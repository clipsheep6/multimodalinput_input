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

#ifndef I_INPUT_EVENT_HANDLER_PLUGIN_CONTEXT_H
#define I_INPUT_EVENT_HANDLER_PLUGIN_CONTEXT_H

#include <string>

#include "i_input_event_handler.h"

namespace OHOS {
namespace MMI {
enum class TimeStatFlag {
    BEGIN,
    END,
};

class IInputEventPluginContext
{
public:
    IInputEventPluginContext() = default;
    virtual ~IInputEventPluginContext() = default;
    virtual void SetEventHandler(std::shared_ptr<IInputEventHandler> handler) = 0;
    virtual std::shared_ptr<IInputEventHandler> GetEventHandler() = 0;
    virtual void BeginStat() = 0;
    virtual void EndStat() = 0;
    virtual void TimeStat(TimeStatFlag flag) = 0;
    virtual void OnReport(int32_t &max, int32_t &avg) = 0;
    std::string pluginName_;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INPUT_EVENT_HANDLER_PLUGIN_CONTEXT_H