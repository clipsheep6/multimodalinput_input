/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "processing_mouse_device.h"

using namespace OHOS::MMI;

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "ProcessingMouseDevice" };
} // namespace

int32_t ProcessingMouseDevice::TransformJsonDataToInputData(const cJSON* fingerEventArrays,
                                                            InputEventArray& inputEventArray)
{
    CALL_LOG_ENTER;
    cJSON* inputData = cJSON_GetObjectItemCaseSensitive(fingerEventArrays, "events");
    CHKPR(inputData, RET_ERR);
    std::vector<MouseEvent> mouseEventArray;
    if (AnalysisMouseEvent(inputData, mouseEventArray) == RET_ERR) {
        return RET_ERR;
    }
    TransformMouseEventToInputEvent(mouseEventArray, inputEventArray);
    return RET_OK;
}

void ProcessingMouseDevice::TransformMouseEventToInputEvent(const std::vector<MouseEvent>& mouseEventArray,
                                                            InputEventArray& inputEventArray)
{
    for (const auto &item : mouseEventArray) {
        if (item.eventType == "KEY_EVENT_PRESS") {
            TransformKeyPressEvent(item, inputEventArray);
        } else if (item.eventType == "KEY_EVENT_RELEASE") {
            TransformKeyReleaseEvent(item, inputEventArray);
        } else if (item.eventType == "KEY_EVENT_CLICK") {
            TransformKeyClickEvent(item, inputEventArray);
        } else if (item.eventType == "MOUSE_EVENT_MOVE") {
            TransformMouseMoveEvent(item, inputEventArray);
        } else if (item.eventType == "MOUSE_EVENT_WHEEL") {
            TransformMouseWheelEvent(item, inputEventArray);
        } else if (item.eventType == "MOUSE_EVENT_HWHEEL") {
            TransformMouseHwheelEvent(item, inputEventArray);
        } else {
            MMI_HILOGW("json file format error");
        }
    }
}

int32_t ProcessingMouseDevice::AnalysisMouseEvent(const cJSON* inputData,
    std::vector<MouseEvent>& mouseEventArray)
{
    MouseEvent mouseEvent = {};
    for (int32_t i = 0; i < cJSON_GetArraySize(inputData); i++) {
        mouseEvent = {};
        cJSON* event = cJSON_GetArrayItem(inputData, i);
        CHKPR(event, RET_ERR);
        cJSON* eventType = cJSON_GetObjectItemCaseSensitive(event, "eventType");
        if (eventType) {
            mouseEvent.eventType = eventType->valuestring;
        }
        cJSON* blockTime = cJSON_GetObjectItemCaseSensitive(event, "blockTime");
        if (blockTime) {
            mouseEvent.blockTime = blockTime->valueint;
        }
        cJSON* xPos = cJSON_GetObjectItemCaseSensitive(event, "xPos");
        if (xPos) {
            mouseEvent.xPos = xPos->valueint;
        }
        cJSON* yPos = cJSON_GetObjectItemCaseSensitive(event, "yPos");
        if (yPos) {
            mouseEvent.yPos = yPos->valueint;
        }
        cJSON* direction = cJSON_GetObjectItemCaseSensitive(event, "direction");
        if (direction) {
            mouseEvent.direction = direction->valuestring;
        }
        cJSON* distance = cJSON_GetObjectItemCaseSensitive(event, "distance");
        if (distance) {
            mouseEvent.distance = distance->valueint;
        }
        mouseEventArray.push_back(mouseEvent);
    }
    return RET_OK;
}

void ProcessingMouseDevice::TransformKeyPressEvent(const MouseEvent& mouseEvent,
    InputEventArray& inputEventArray)
{
    uint16_t keyValue = static_cast<uint16_t>(mouseEvent.keyValue);
    SetKeyPressEvent(inputEventArray, mouseEvent.blockTime, keyValue);
    SetSynReport(inputEventArray);
}

void ProcessingMouseDevice::TransformKeyReleaseEvent(const MouseEvent& mouseEvent,
    InputEventArray& inputEventArray)
{
    uint16_t keyValue = static_cast<uint16_t>(mouseEvent.keyValue);
    SetKeyReleaseEvent(inputEventArray, mouseEvent.blockTime, keyValue);
    SetSynReport(inputEventArray);
}

void ProcessingMouseDevice::TransformKeyClickEvent(const MouseEvent& mouseEvent,
                                                   InputEventArray& inputEventArray)
{
    uint16_t keyValue = static_cast<uint16_t>(mouseEvent.keyValue);
    SetKeyPressEvent(inputEventArray, mouseEvent.blockTime, keyValue);
    SetSynReport(inputEventArray);
    SetKeyReleaseEvent(inputEventArray, mouseEvent.blockTime, keyValue);
    SetSynReport(inputEventArray);
}

void ProcessingMouseDevice::TransformMouseMoveEvent(const MouseEvent& mouseEvent,
    InputEventArray& inputEventArray)
{
    SetRelX(inputEventArray, mouseEvent.blockTime, mouseEvent.xPos);
    SetSynReport(inputEventArray);
    SetRelY(inputEventArray, mouseEvent.blockTime, mouseEvent.yPos);
    SetSynReport(inputEventArray);
}

void ProcessingMouseDevice::TransformMouseWheelEvent(const MouseEvent& mouseEvent,
    InputEventArray& inputEventArray)
{
    uint32_t distance = static_cast<uint32_t>(mouseEvent.distance);
    if (mouseEvent.direction == "up") {
        distance = ~distance + 1;
    }
    SetRelWheel(inputEventArray, mouseEvent.blockTime, static_cast<int32_t>(distance));
    SetSynReport(inputEventArray);
}

void ProcessingMouseDevice::TransformMouseHwheelEvent(const MouseEvent& mouseEvent,
    InputEventArray& inputEventArray)
{
    uint32_t distance = static_cast<uint32_t>(mouseEvent.distance);
    if (mouseEvent.direction == "left") {
        distance = ~distance + 1;
    }
    SetRelWheel(inputEventArray, mouseEvent.blockTime, static_cast<int32_t>(distance));
    SetSynReport(inputEventArray);
}
