/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OH_INPUT_INTERCEPTOR_H
#define OH_INPUT_INTERCEPTOR_H

#include <mutex>

#include "i_input_event_consumer.h"
#include "nocopyable.h"

namespace OHOS {
namespace MMI {
class OHInputInterceptor final : public IInputEventConsumer, public std::enable_shared_from_this<OHInputInterceptor> {
public:
    OHInputInterceptor() = default;
    DISALLOW_COPY_AND_MOVE(OHInputInterceptor);
    ~OHInputInterceptor() override = default;

    int32_t Start();
    void Stop();
    void SetCallback(std::function<void(std::shared_ptr<PointerEvent>)> callback);
    void OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent) const override;
private:
    std::function<void(std::shared_ptr<PointerEvent>)> callback_;
    int32_t interceptorId_ { -1 }; 
    mutable std::mutex mutex_;  
};
}
}
#endif