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

#ifndef I_TOUCH_SCREEN_EVENT_HANDLER
#define I_TOUCH_SCREEN_EVENT_HANDLER

#include <memory>
#include <string>

namespace OHOS {
namespace MMI {
class TouchScreenEventHandler : public ITouchScreenEventHandler {
public:
    virtual ~TouchScreenEventHandler() = default;
    DISALLOW_COPY_AND_MOVE(TouchScreenEventHandler);
    virtual void HandleAbsEvent(int32_t code, int32_t value) override;
    virtual void HandleKeyEvent(int32_t code, int32_t value) override;
    virtual void HandleSyncEvent() override;
protected:
    static std::shared_ptr<TouchScreenEventHandler> CreateInstance();
    TouchScreenHandler();
};
} // namespace MMI
} // namespace OHOS
#endif // I_TOUCH_SCREEN_EVENT_HANDLER