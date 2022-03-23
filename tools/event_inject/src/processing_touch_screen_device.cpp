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

#include "processing_touch_screen_device.h"

using namespace OHOS::MMI;

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "ProcessingTouchScreenDevice" };
} // namespace

int32_t ProcessingTouchScreenDevice::TransformJsonDataToInputData(const cJSON* touchScreenEventArrays,
                                                                  InputEventArray& inputEventArray)
{
    CALL_LOG_ENTER;
    cJSON* inputData = cJSON_GetObjectItemCaseSensitive(touchScreenEventArrays, "events");
    CHKPR(inputData, RET_ERR);
    TouchScreenInputEvents touchScreenInputEvents = {};
    AnalysisTouchScreenDate(inputData, touchScreenInputEvents);
    if (touchScreenInputEvents.eventArray.size() < 1) {
        return RET_ERR;
    }
    TouchScreenInputEvent pressEvents = touchScreenInputEvents.eventArray[0];
    AnalysisTouchScreenPressData(inputEventArray, pressEvents);
    for (uint64_t i = 1; i < static_cast<uint64_t>(touchScreenInputEvents.eventNumber); i++) {
        AnalysisTouchScreenMoveData(inputEventArray, touchScreenInputEvents.eventArray[i]);
    }
    uint64_t releaseEventIndex = static_cast<uint64_t>(touchScreenInputEvents.eventNumber) - 1;
    TouchScreenInputEvent releaseEvents = touchScreenInputEvents.eventArray[releaseEventIndex];
    AnalysisTouchScreenReleaseData(inputEventArray, releaseEvents);
    return RET_OK;
}

int32_t ProcessingTouchScreenDevice::TransformJsonDataSingleTouchScreen(const cJSON* touchScreenEventArrays,
    InputEventArray& inputEventArray)
{
    CALL_LOG_ENTER;
    cJSON* inputData = cJSON_GetObjectItemCaseSensitive(touchScreenEventArrays, "singleEvent");
    CHKPR(inputData, RET_ERR);
    std::vector<TouchSingleEventData> touchSingleEventDatas;
    AnalysisSingleTouchScreenDate(inputData, touchSingleEventDatas);
    for (const auto &item : touchSingleEventDatas) {
        AnalysisTouchScreenToInputData(inputEventArray, item);
    }
    return RET_OK;
}

void ProcessingTouchScreenDevice::AnalysisTouchScreenDate(const cJSON* inputData,
                                                          TouchScreenInputEvents& touchScreenInputEvents)
{
    TouchScreenCoordinates touchScreenCoordinates = {};
    TouchScreenInputEvent touchScreenInputEvent = {};
    for (int32_t i = 0; i < cJSON_GetArraySize(inputData); i++) {
        cJSON* inputDataI = cJSON_GetArrayItem(inputData, i);
        CHKPV(inputDataI);
        for (int32_t j = 0; j < cJSON_GetArraySize(inputDataI); j++) {
            cJSON* inputDataJ = cJSON_GetArrayItem(inputDataI, j);
            CHKPV(inputDataJ);
            cJSON* xPos = cJSON_GetArrayItem(inputData, 0);
            CHKPV(xPos);
            cJSON* yPos = cJSON_GetArrayItem(inputData, 1);
            CHKPV(yPos);
            touchScreenCoordinates.xPos = xPos->valueint;
            touchScreenCoordinates.yPos = yPos->valueint;
            touchScreenInputEvent.events.push_back(touchScreenCoordinates);
            touchScreenInputEvent.groupNumber = j + 1;
        }
        touchScreenInputEvents.eventNumber = cJSON_GetArraySize(inputData);
        touchScreenInputEvents.eventArray.push_back(touchScreenInputEvent);
        touchScreenInputEvent.events.clear();
    }
}

void ProcessingTouchScreenDevice::AnalysisSingleTouchScreenDate(const cJSON* inputData,
    std::vector<TouchSingleEventData>& touchSingleEventDatas)
{
    TouchSingleEventData touchSingleEventData = {};
    for (int32_t i = 0; i < cJSON_GetArraySize(inputData); i++) {
        touchSingleEventData = {};
        cJSON* event = cJSON_GetArrayItem(inputData, i);
        CHKPV(event);
        cJSON* eventType = cJSON_GetObjectItemCaseSensitive(event, "eventType");
        CHKPV(eventType);
        touchSingleEventData.eventType = eventType->valuestring;
        cJSON* trackingId = cJSON_GetObjectItemCaseSensitive(event, "trackingId");
        if (trackingId) {
            touchSingleEventData.trackingId = trackingId->valueint;
        }
        if (touchSingleEventData.eventType != "release") {
            cJSON* xPos = cJSON_GetObjectItemCaseSensitive(event, "xPos");
            if (xPos) {
                touchSingleEventData.xPos = xPos->valueint;
            }
            cJSON* yPos = cJSON_GetObjectItemCaseSensitive(event, "yPos");
            if (yPos) {
                touchSingleEventData.yPos = yPos->valueint;
            }
        }
        cJSON* blockTime = cJSON_GetObjectItemCaseSensitive(event, "blockTime");
        if (blockTime) {
            touchSingleEventData.blockTime = blockTime->valueint;
        }
        cJSON* reportType = cJSON_GetObjectItemCaseSensitive(event, "reportType");
        if (reportType) {
            touchSingleEventData.reportType = reportType->valuestring;
        }
        touchSingleEventDatas.push_back(touchSingleEventData);
    }
}

void ProcessingTouchScreenDevice::AnalysisTouchScreenPressData(InputEventArray& inputEventArray,
                                                               const TouchScreenInputEvent& touchScreenInputEvent)
{
    int32_t xPos = 0;
    int32_t yPos = 0;
    MMI_LOGE(" touchScreenInputEvent.groupNumber:%{public}d",  touchScreenInputEvent.groupNumber);
    for (uint32_t i = 0; i < touchScreenInputEvent.groupNumber; i++) {
        xPos = touchScreenInputEvent.events[i].xPos;
        yPos = touchScreenInputEvent.events[i].yPos;
        SetPositionX(inputEventArray, 0, xPos);
        SetPositionY(inputEventArray, 0, yPos);
        SetTrackingId(inputEventArray, 0, static_cast<int32_t>(i + 1));
        SetBtnTouch(inputEventArray, 0, 1);
        SetSynMtReport(inputEventArray, 0);
    }
    SetSynReport(inputEventArray);
}

void ProcessingTouchScreenDevice::AnalysisTouchScreenMoveData(InputEventArray& inputEventArray,
                                                              const TouchScreenInputEvent& touchScreenInputEvent)
{
    int32_t xPos = 0;
    int32_t yPos = 0;
    for (uint32_t i = 0; i < touchScreenInputEvent.groupNumber; i++) {
        xPos = touchScreenInputEvent.events[i].xPos;
        yPos = touchScreenInputEvent.events[i].yPos;
        SetPositionX(inputEventArray, 0, xPos);
        SetPositionY(inputEventArray, 0, yPos);
        SetTrackingId(inputEventArray, 0, static_cast<int32_t>(i + 1));
        SetSynMtReport(inputEventArray, 0);
    }
    SetSynReport(inputEventArray);
}

void ProcessingTouchScreenDevice::AnalysisTouchScreenReleaseData(InputEventArray& inputEventArray,
                                                                 const TouchScreenInputEvent& touchScreenInputEvent)
{
    for (uint32_t i = 0; i < touchScreenInputEvent.groupNumber; i++) {
        SetTrackingId(inputEventArray, 0, static_cast<int32_t>(i + 1));
        SetBtnTouch(inputEventArray, 0, 0);
        SetSynMtReport(inputEventArray, 0);
    }
    SetSynReport(inputEventArray);
    SetSynMtReport(inputEventArray, 0);
    SetSynReport(inputEventArray);
}

void ProcessingTouchScreenDevice::AnalysisTouchScreenToInputData(InputEventArray& inputEventArray,
                                                                 const TouchSingleEventData& touchSingleEventData)
{
    if (touchSingleEventData.eventType == "press") {
        AnalysisTouchScreenPressData(inputEventArray, touchSingleEventData);
    } else if (touchSingleEventData.eventType == "move") {
        AnalysisTouchScreenMoveData(inputEventArray, touchSingleEventData);
    } else if (touchSingleEventData.eventType == "release") {
        AnalysisTouchScreenReleaseData(inputEventArray, touchSingleEventData);
    }
}

void ProcessingTouchScreenDevice::AnalysisTouchScreenPressData(InputEventArray& inputEventArray,
                                                               const TouchSingleEventData& touchSingleEventData)
{
    SetPositionX(inputEventArray, 0, touchSingleEventData.xPos);
    SetPositionY(inputEventArray, 0, touchSingleEventData.yPos);
    SetTrackingId(inputEventArray, 0, touchSingleEventData.trackingId);
    SetBtnTouch(inputEventArray, 0, 1);
    if (touchSingleEventData.reportType == "mtReport") {
        SetSynMtReport(inputEventArray, 0);
    } else if (touchSingleEventData.reportType == "synReport") {
        SetSynMtReport(inputEventArray, 0);
        SetSynReport(inputEventArray, touchSingleEventData.blockTime);
    }
}

void ProcessingTouchScreenDevice::AnalysisTouchScreenMoveData(InputEventArray& inputEventArray,
                                                              const TouchSingleEventData& touchSingleEventData)
{
    SetPositionX(inputEventArray, 0, touchSingleEventData.xPos);
    SetPositionY(inputEventArray, 0, touchSingleEventData.yPos);
    SetTrackingId(inputEventArray, 0, touchSingleEventData.trackingId);
    if (touchSingleEventData.reportType == "mtReport") {
        SetSynMtReport(inputEventArray, 0);
    } else if (touchSingleEventData.reportType == "synReport") {
        SetSynMtReport(inputEventArray, 0);
        SetSynReport(inputEventArray, touchSingleEventData.blockTime);
    }
}

void ProcessingTouchScreenDevice::AnalysisTouchScreenReleaseData(InputEventArray& inputEventArray,
                                                                 const TouchSingleEventData& touchSingleEventData)
{
    SetTrackingId(inputEventArray, 0, touchSingleEventData.trackingId);
    SetBtnTouch(inputEventArray, 0, 0);
    if (touchSingleEventData.reportType == "mtReport") {
        SetSynMtReport(inputEventArray, 0);
    } else if (touchSingleEventData.reportType == "synReport") {
        SetSynMtReport(inputEventArray, 0);
        SetSynReport(inputEventArray);
        SetSynMtReport(inputEventArray, 0);
        SetSynReport(inputEventArray, touchSingleEventData.blockTime);
    }
}