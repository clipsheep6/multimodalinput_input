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

#ifndef PLUGIN_CONTEXT_H
#define PLUGIN_CONTEXT_H

#include <map>

#include "i_input_event_handler_plugin_context.h"

namespace OHOS {
namespace MMI {
    enum class TimeStatFlag {
        BEGIN,
        END,
    };
class PluginContext : public IInputEventHandlerPluginContext
{
public:
    PluginContext() = default;
    virtual ~PluginContext() = default;
    virtual void SetEventHandler(std::shared_ptr<IInputEventHandler> handler);
    virtual std::shared_ptr<IInputEventHandler> GetEventHandler();
    virtual void BeginStat();
    virtual void EndStat();
    virtual void TimeStat(TimeStatFlag flag);
    virtual void OnReport(int32_t &max, int32_t &avg);
private:
    std::shared_ptr<IInputEventHandler> handler_ { nullptr };
    int32_t max_ { 0 };
    int32_t raw_ { 0 };
    int64_t sum_ { 0 };
    int32_t times_ { 0 };
};
} // namespace MMI
} // namespace OHOS
#endif // PLUGIN_CONTEXT_H