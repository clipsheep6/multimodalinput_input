/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "mouse_event.h"
using namespace OHOS;
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    MouseEvent mouseEvent;
    int32_t windowId = 0;
    int32_t action = 10;
    int32_t actionButton = 10;
    int32_t pressedButtons = 5;
    const MmiPoint mmiPoint = {};
    float xOffset = 0.0f;
    float yOffset = 0.0f;
    float cursorDelta = 0.0f;
    float scrollingDelta = 0.0f;
    int32_t highLevelEvent = 0;
    const std::string uuid = "a";
    int32_t sourceType = 0;
    int32_t occurredTime = 0;
    const std::string deviceId = "b";
    int32_t inputDeviceId = 0;
    bool isHighLevelEvent = true;
    uint16_t deviceUdevTags = 0;
    EventJoyStickAxis eventJoyStickInfo;
    eventJoyStickInfo.abs_rx.standardValue = 0.15f;
    mouseEvent.Initialize(windowId, action, actionButton, pressedButtons, mmiPoint, xOffset, yOffset, cursorDelta,
                          scrollingDelta, highLevelEvent, uuid, sourceType, occurredTime, deviceId, inputDeviceId,
                          isHighLevelEvent, deviceUdevTags, eventJoyStickInfo);
    int32_t axis = size;
    mouseEvent.GetAxisValue(axis);
    return 0;
}
