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

#include "processing_pen_device.h"

using namespace OHOS::MMI;

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "ProcessingPenDevice" };
} // namespace

int32_t ProcessingPenDevice::TransformJsonDataToInputData(const cJSON* penEventArrays,
    InputEventArray& inputEventArray)
{
    CALL_LOG_ENTER;
    cJSON* inputData = cJSON_GetObjectItemCaseSensitive(penEventArrays, "events");
    CHKPR(inputData, RET_ERR);
    std::vector<PenEvent> penEventArray;
    if (AnalysisPenPadEvent(inputData, penEventArray) == RET_ERR) {
        MMI_LOGE("AnalysisPenPadEvent error.");
        return RET_ERR;
    }
    TransformPenEventToInputEvent(penEventArray, inputEventArray);
    return RET_OK;
}

void ProcessingPenDevice::TransformPenEventToInputEvent(const std::vector<PenEvent>& penEventArray,
                                                        InputEventArray& inputEventArray)
{
    SetPenApproachPadEvent(penEventArray[0], inputEventArray);
    for (const auto &item : penEventArray) {
        SetPenSlidePadEvent(item, inputEventArray);
    }
    uint64_t lastEventIndex = penEventArray.size() - 1;
    SetPenLeavePadEvent(penEventArray[lastEventIndex], inputEventArray);
}

void ProcessingPenDevice::SetPenApproachPadEvent(const PenEvent& penEvent, InputEventArray& inputEventArray)
{
    SetEvAbsX(inputEventArray, 0, penEvent.xPos);
    SetEvAbsY(inputEventArray, 0, penEvent.yPos);
    SetAbsTiltX(inputEventArray, 0, penEvent.tiltX);
    SetAbsTiltY(inputEventArray, 0, penEvent.tiltY);
    SetEvAbsZ(inputEventArray, 0, EV_ABS_Z_DEFAULT_VALUE);
    SetAbsDistance(inputEventArray, 0, penEvent.distance);
    if (penEvent.eventType == "PEN_TOUCH") {
        SetBtnPen(inputEventArray, 0, 1);
    } else if (penEvent.eventType == "RUBBER_TOUCH") {
        SetBtnRubber(inputEventArray, 0, 1);
    } else {
        // nothing to do.
    }

    SetMscSerial(inputEventArray, 0);
    SetAbsMisc(inputEventArray, 0, EV_ABS_MISC_DEFAULT_VALUE);
    SetSynReport(inputEventArray);
}

void ProcessingPenDevice::SetPenSlidePadEvent(const PenEvent& penEvent, InputEventArray& inputEventArray)
{
    if (penEvent.eventType == "PEN_KEY") {
        SetBtnStylus(inputEventArray, 0, static_cast<uint16_t>(penEvent.keyValue), penEvent.keyStatus);
        return;
    }
    if (penEvent.distance == 0) {
        SetMscSerial(inputEventArray, 0);
        SetSynReport(inputEventArray, 0);
        return;
    }
    SetEvAbsX(inputEventArray, 0, penEvent.xPos);
    SetEvAbsY(inputEventArray, 0, penEvent.yPos);
    static int32_t previousPressure = 0;
    if (penEvent.pressure > 0) {
        if (previousPressure == 0) {
            SetAbsPressure(inputEventArray, 0, penEvent.pressure);
            SetBtnTouch(inputEventArray, 0, 1);
        } else if (previousPressure > 0) {
            SetAbsPressure(inputEventArray, 0, penEvent.pressure);
        } else {
            // nothing to do.
        }
    } else if ((penEvent.pressure == 0) && (previousPressure > 0)) {
        SetAbsPressure(inputEventArray, 0, penEvent.pressure);
        SetBtnTouch(inputEventArray, 0, 0);
    } else {
        // nothing to do.
    }
    previousPressure = penEvent.pressure;
    SetAbsDistance(inputEventArray, 0, penEvent.distance);
    SetAbsTiltX(inputEventArray, 0, penEvent.tiltX);
    SetAbsTiltY(inputEventArray, 0, penEvent.tiltY);
    SetMscSerial(inputEventArray, 0);
    SetSynReport(inputEventArray);
}

void ProcessingPenDevice::SetPenLeavePadEvent(const PenEvent& penEvent, InputEventArray& inputEventArray)
{
    SetEvAbsX(inputEventArray, 0);
    SetEvAbsY(inputEventArray, 0);
    SetAbsTiltX(inputEventArray, 0);
    SetAbsTiltY(inputEventArray, 0);
    SetEvAbsZ(inputEventArray, 0);
    SetAbsDistance(inputEventArray, 0, 0);
    if (penEvent.eventType == "PEN_TOUCH") {
        SetBtnPen(inputEventArray, 0, 0);
    } else if (penEvent.eventType == "RUBBER_TOUCH") {
        SetBtnRubber(inputEventArray, 0, 0);
    } else {
        // nothing to do.
    }

    SetMscSerial(inputEventArray, 0);
    SetAbsMisc(inputEventArray, 0, 0);
    SetSynReport(inputEventArray);
}

int32_t ProcessingPenDevice::AnalysisPenPadEvent(const cJSON* inputData, std::vector<PenEvent>& penEventArray)
{
    uint64_t endEventIndex = cJSON_GetArraySize(inputData);
    if (AnalysisPenApproachPadEvent(cJSON_GetArrayItem(inputData, 0), penEventArray) == RET_ERR) {
        MMI_LOGE("manage finger array faild");
        return RET_ERR;
    }
    for (uint64_t i = 1; i < endEventIndex; i++) {
        if (AnalysisPenSlidePadEvent(cJSON_GetArrayItem(inputData, i), penEventArray) == RET_ERR) {
            MMI_LOGE("manage finger array faild");
            return RET_ERR;
        }
    }
    if (AnalysisPenLeavePadEvent(cJSON_GetArrayItem(inputData, (endEventIndex - 1)), penEventArray) == RET_ERR) {
        MMI_LOGE("manage finger array faild");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t ProcessingPenDevice::AnalysisPenApproachPadEvent(const cJSON* event, std::vector<PenEvent>& penEventArray)
{
    PenEvent penEvent = {};
    cJSON* eventType = cJSON_GetObjectItemCaseSensitive(event, "eventType");
    CHKPR(eventType, RET_ERR);
    penEvent.eventType = eventType->valuestring;
    if ((penEvent.eventType != "RUBBER_TOUCH") && (penEvent.eventType != "PEN_TOUCH")) {
        MMI_LOGE("Enter the correct event type in the configuration file.");
        return RET_ERR;
    }
    cJSON* xPos = cJSON_GetObjectItemCaseSensitive(event, "xPos");
    CHKPR(xPos, RET_ERR);
    cJSON* yPos = cJSON_GetObjectItemCaseSensitive(event, "yPos");
    CHKPR(yPos, RET_ERR);
    cJSON* tiltX = cJSON_GetObjectItemCaseSensitive(event, "tiltX");
    CHKPR(tiltX, RET_ERR);
    cJSON* tiltY = cJSON_GetObjectItemCaseSensitive(event, "tiltY");
    CHKPR(tiltY, RET_ERR);
    cJSON* pressure = cJSON_GetObjectItemCaseSensitive(event, "pressure");
    CHKPR(pressure, RET_ERR);
    cJSON* distance = cJSON_GetObjectItemCaseSensitive(event, "distance");
    CHKPR(distance, RET_ERR);
    penEvent.xPos = xPos->valueint;
    penEvent.yPos = yPos->valueint;
    penEvent.tiltX = tiltX->valueint;
    penEvent.tiltY = tiltY->valueint;
    penEvent.pressure = pressure->valueint;
    penEvent.distance = distance->valueint;
    penEventArray.push_back(penEvent);
    return RET_OK;
}

int32_t ProcessingPenDevice::AnalysisPenSlidePadEvent(const cJSON* event, std::vector<PenEvent>& penEventArray)
{
    PenEvent penEvent = {};
    cJSON* eventType = cJSON_GetObjectItemCaseSensitive(event, "eventType");
    CHKPR(eventType, RET_ERR);
    penEvent.eventType = eventType->valuestring;
    if (penEvent.eventType == "PEN_KEY") {
        cJSON* keyValue = cJSON_GetObjectItemCaseSensitive(event, "keyValue");
        CHKPR(keyValue, RET_ERR);
        cJSON* keyStatus = cJSON_GetObjectItemCaseSensitive(event, "keyStatus");
        CHKPR(keyStatus, RET_ERR);
        penEvent.keyValue = keyValue->valueint;
        penEvent.keyStatus = keyStatus->valueint;
    }
    if ((penEvent.eventType == "PEN_TOUCH") || (penEvent.eventType == "RUBBER_TOUCH")) {
        cJSON* xPos = cJSON_GetObjectItemCaseSensitive(event, "xPos");
        CHKPR(xPos, RET_ERR);
        cJSON* yPos = cJSON_GetObjectItemCaseSensitive(event, "yPos");
        CHKPR(yPos, RET_ERR);
        cJSON* tiltX = cJSON_GetObjectItemCaseSensitive(event, "tiltX");
        CHKPR(tiltX, RET_ERR);
        cJSON* tiltY = cJSON_GetObjectItemCaseSensitive(event, "tiltY");
        CHKPR(tiltY, RET_ERR);
        cJSON* pressure = cJSON_GetObjectItemCaseSensitive(event, "pressure");
        CHKPR(pressure, RET_ERR);
        cJSON* distance = cJSON_GetObjectItemCaseSensitive(event, "distance");
        CHKPR(distance, RET_ERR);
        penEvent.xPos = xPos->valueint;
        penEvent.yPos = yPos->valueint;
        penEvent.tiltX = tiltX->valueint;
        penEvent.tiltY = tiltY->valueint;
        penEvent.pressure = pressure->valueint;
        penEvent.distance = distance->valueint;
    }
    penEventArray.push_back(penEvent);

    return RET_OK;
}

int32_t ProcessingPenDevice::AnalysisPenLeavePadEvent(const cJSON* event, std::vector<PenEvent>& penEventArray)
{
    PenEvent penEvent = {};
    cJSON* eventType = cJSON_GetObjectItemCaseSensitive(event, "eventType");
    CHKPR(eventType, RET_ERR);
    penEvent.eventType = eventType->valuestring;
    if ((penEvent.eventType != "RUBBER_TOUCH") && (penEvent.eventType != "PEN_TOUCH")) {
        MMI_LOGE("Enter the correct event type in the configuration file.");
        return RET_ERR;
    }
    cJSON* xPos = cJSON_GetObjectItemCaseSensitive(event, "xPos");
    CHKPR(xPos, RET_ERR);
    cJSON* yPos = cJSON_GetObjectItemCaseSensitive(event, "yPos");
    CHKPR(yPos, RET_ERR);
    cJSON* tiltX = cJSON_GetObjectItemCaseSensitive(event, "tiltX");
    CHKPR(tiltX, RET_ERR);
    cJSON* tiltY = cJSON_GetObjectItemCaseSensitive(event, "tiltY");
    CHKPR(tiltY, RET_ERR);
    cJSON* pressure = cJSON_GetObjectItemCaseSensitive(event, "pressure");
    CHKPR(pressure, RET_ERR);
    cJSON* distance = cJSON_GetObjectItemCaseSensitive(event, "distance");
    CHKPR(distance, RET_ERR);
    penEvent.xPos = xPos->valueint;
    penEvent.yPos = yPos->valueint;
    penEvent.tiltX = tiltX->valueint;
    penEvent.tiltY = tiltY->valueint;
    penEvent.pressure = pressure->valueint;
    penEvent.distance = distance->valueint;
    penEventArray.push_back(penEvent);
    return RET_OK;
}