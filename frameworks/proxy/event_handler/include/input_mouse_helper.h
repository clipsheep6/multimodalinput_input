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

#ifndef INPUT_MOUSE_HELPER_H
#define INPUT_MOUSE_HELPER_H

#include <mutex>

#include "nocopyable.h"

namespace OHOS {
namespace MMI {
class InputMouseHelper final {
public:
    InputMouseHelper() = default;
    DISALLOW_MOVE(InputMouseHelper);
    ~InputMouseHelper() = default;
    void MoveMouseEvent(int32_t offsetX, int32_t offsetY);
    int32_t SetPointerVisible(bool visible);
    bool IsPointerVisible();
    int32_t SetPointerSpeed(int32_t speed);
    int32_t GetPointerSpeed(int32_t &speed);
    int32_t SetPointerStyle(int32_t windowId, int32_t pointerStyle);
    int32_t GetPointerStyle(int32_t windowId, int32_t &pointerStyle);
    void SetPointerLocation(int32_t x, int32_t y);
    int32_t EnterCaptureMode(int32_t windowId);
    int32_t LeaveCaptureMode(int32_t windowId);
private:
    std::mutex mtx_;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_MOUSE_HELPER_H