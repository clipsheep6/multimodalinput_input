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

#include "input_mouse_helper.h"

#include "input_connect_manager.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputMouseHelper" };
} // namespace

void InputMouseHelper::MoveMouseEvent(int32_t offsetX, int32_t offsetY)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    int32_t ret = MultimodalInputConnMgr->MoveMouseEvent(offsetX, offsetY);
    if (ret != 0) {
        MMI_HILOGE("Move mouse event failed, ret:%{public}d", ret);
        return;
    }
}

int32_t InputMouseHelper::SetPointerVisible(bool visible)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    int32_t ret = MultimodalInputConnMgr->SetPointerVisible(visible);
    if (ret != RET_OK) {
        MMI_HILOGE("Set pointer visible failed, ret:%{public}d", ret);
    }
    return ret;
}

bool InputMouseHelper::IsPointerVisible()
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    bool visible;
    int32_t ret = MultimodalInputConnMgr->IsPointerVisible(visible);
    if (ret != 0) {
        MMI_HILOGE("Get pointer visible failed, ret:%{public}d", ret);
    }
    return visible;
}

int32_t InputMouseHelper::SetPointerSpeed(int32_t speed)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    int32_t ret = MultimodalInputConnMgr->SetPointerSpeed(speed);
    if (ret != RET_OK) {
        MMI_HILOGE("Failed to set pointer speed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t InputMouseHelper::GetPointerSpeed(int32_t &speed)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    int32_t ret = MultimodalInputConnMgr->GetPointerSpeed(speed);
    if (ret != RET_OK) {
        MMI_HILOGE("Get pointer speed failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t InputMouseHelper::SetPointerStyle(int32_t windowId, int32_t pointerStyle)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (windowId < 0 || pointerStyle < 0) {
        MMI_HILOGE("The param is invalid");
        return RET_ERR;
    }
    int32_t ret = MultimodalInputConnMgr->SetPointerStyle(windowId, pointerStyle);
    if (ret != RET_OK) {
        MMI_HILOGE("Set pointer style failed, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}

int32_t InputMouseHelper::GetPointerStyle(int32_t windowId, int32_t &pointerStyle)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (windowId < 0) {
        MMI_HILOGE("The param is invalid");
        return RET_ERR;
    }
    int32_t ret = MultimodalInputConnMgr->GetPointerStyle(windowId, pointerStyle);
    if (ret != RET_OK) {
        MMI_HILOGE("Get pointer style failed, ret:%{public}d", ret);
        return ret;
    }
    return RET_OK;
}
} // namespace MMI
} // namespace OHOS
