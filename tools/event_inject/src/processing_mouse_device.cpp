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
#include "processing_mouse_device.h"

using namespace std;
using namespace OHOS::MMI;

namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "ProcessingMouseDevice" };
}

int32_t ProcessingMouseDevice::TransformJsonDataToInputData(const Json& fingerEventArrays,
                                                            InputEventArray& inputEventArray)
{
    MMI_LOGI("Enter TransformJsonDataForMouse function.");
    if (fingerEventArrays.empty()) {
        return RET_ERR;
    }
    Json inputData = fingerEventArrays.at("events");
    if (inputData.empty()) {
        MMI_LOGE("manage KeyBoard array faild, inputData is empty.");
        return RET_ERR;
    }
    vector<MouseEvent> mouseEventArray;
    if (AnalysisMouseEvent(inputData, mouseEventArray) == RET_ERR) {
        return RET_ERR;
    }
    TransformMouseEventToInputEvent(mouseEventArray, inputEventArray);
    MMI_LOGI("Leave TransformJsonDataForMouse function.");

    return RET_OK;
}

void ProcessingMouseDevice::TransformMouseEventToInputEvent(const std::vector<MouseEvent>& mouseEventArray,
                                                            InputEventArray& inputEventArray)
{
    for (MouseEvent mouseEvent : mouseEventArray) {
        if (mouseEvent.eventType == "KEY_EVENT_PRESS") {
            TransformKeyPressEvent(mouseEvent, inputEventArray);
        } else if (mouseEvent.eventType == "KEY_EVENT_RELEASE") {
            TransformKeyReleaseEvent(mouseEvent, inputEventArray);
        } else if (mouseEvent.eventType == "KEY_EVENT_CLICK") {
            TransformKeyClickEvent(mouseEvent, inputEventArray);
        } else if (mouseEvent.eventType == "MOUSE_EVENT_MOVE") {
            TransformMouseMoveEvent(mouseEvent, inputEventArray);
        } else if (mouseEvent.eventType == "MOUSE_EVENT_WHEEL") {
            TransformMouseWheelEvent(mouseEvent, inputEventArray);
        } else if (mouseEvent.eventType == "MOUSE_EVENT_HWHEEL") {
            TransformMouseHwheelEvent(mouseEvent, inputEventArray);
        } else {
            // nothing to do.
        }
    }
}

int32_t ProcessingMouseDevice::AnalysisMouseEvent(const Json& inputData,
    std::vector<MouseEvent>& mouseEventArray)
{
    MouseEvent mouseEvent = {};
    for (auto item : inputData) {
        mouseEvent = {};
        mouseEvent.eventType = item.at("eventType").get<std::string>();
        if ((item.find("keyValue")) != item.end()) {
            mouseEvent.keyValue = item.at("keyValue").get<int32_t>();
        }
        if ((item.find("blockTime")) != item.end()) {
            mouseEvent.blockTime = item.at("blockTime").get<int32_t>();
        }
        if ((item.find("xPos")) != item.end()) {
            mouseEvent.xPos = item.at("xPos").get<int32_t>();
        }
        if ((item.find("yPos")) != item.end()) {
            mouseEvent.yPos = item.at("yPos").get<int32_t>();
        }
        if ((item.find("distance")) != item.end()) {
            mouseEvent.distance = item.at("distance").get<int32_t>();
        }
        if ((item.find("direction")) != item.end()) {
            mouseEvent.direction = item.at("direction").get<std::string>();
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
    int32_t distance = mouseEvent.distance;
    if (mouseEvent.direction == "up") {
        distance = ~distance + 1;
    }
    SetRelWheel(inputEventArray, mouseEvent.blockTime, distance);
    SetSynReport(inputEventArray);
}

void ProcessingMouseDevice::TransformMouseHwheelEvent(const MouseEvent& mouseEvent,
    InputEventArray& inputEventArray)
{
    int32_t distance = mouseEvent.distance;
    if (mouseEvent.direction == "left") {
        distance = ~distance + 1;
    }
    SetRelWheel(inputEventArray, mouseEvent.blockTime, distance);
    SetSynReport(inputEventArray);
}
