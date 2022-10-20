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

#include "device_collector.h"
#include "input_context.h"
#include "touch_screen_handler.h"

namespace OHOS {
namespace MMI {
std::shared_ptr<IDeviceCollector> IDeviceCollector::CreateInstance(IInputContext* context) 
{
    return DeviceCollector::CreateInstance(context);
}

std::shared_ptr<IInputContext> IInputContext::CreateInstance() 
{
    return InputContext::CreateInstance();
}

std::unique_ptr<ITouchScreenHandler> ITouchScreenHandler::CreateInstance(const IInputContext* context)
{
    return TouchScreenHandler::CreateInstance(context);
}
} // namespace MMI
} // namespace OHOS