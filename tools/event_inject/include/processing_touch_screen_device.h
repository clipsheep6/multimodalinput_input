/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PROCESSING_TOUCH_SCREEN_DEVICE_H
#define PROCESSING_TOUCH_SCREEN_DEVICE_H

#include "device_base.h"
#include "msg_head.h"

namespace OHOS {
namespace MMI {
class ProcessingTouchScreenDevice : public DeviceBase {
    struct TouchSingleEventData {
        int32_t trackingId { 0 };
        int32_t xPos { 0 };
        int32_t yPos { 0 };
        int64_t blockTime { 0 };
        std::string eventType;
        std::string reportType;
    };

    struct TouchScreenCoordinates {
        int32_t xPos { 0 };
        int32_t yPos { 0 };
    };

    struct TouchScreenInputEvent {
        uint32_t groupNumber { 0 };
        std::vector<TouchScreenCoordinates> events;
    };

    struct TouchScreenInputEvents {
        uint32_t eventNumber { 0 };
        std::vector<TouchScreenInputEvent> eventArray;
    };
public:
    ProcessingTouchScreenDevice() = default;
    ~ProcessingTouchScreenDevice() = default;
    DISALLOW_COPY_AND_MOVE(ProcessingTouchScreenDevice);
    int32_t TransformJsonDataSingleTouchScreen(const DeviceItem &inputEventArrays, InputEventArray &inputEventArray);
    int32_t TransformJsonDataToInputData(const DeviceItem &inputEventArrays, InputEventArray &inputEventArray);
private:
    void AnalysisTouchScreenDate(const std::vector<DeviceEvent> &inputData,
        TouchScreenInputEvents &touchScreenInputEvents);
    void AnalysisSingleTouchScreenDate(const std::vector<DeviceEvent> &inputData,
                                       std::vector<TouchSingleEventData> &touchSingleEventDatas);
    void AnalysisTouchScreenPressData(InputEventArray &inputEventArray,
        const TouchScreenInputEvent &touchScreenInputEvent);
    void AnalysisTouchScreenMoveData(InputEventArray &inputEventArray,
        const TouchScreenInputEvent &touchScreenInputEvent);
    void AnalysisTouchScreenReleaseData(InputEventArray &inputEventArray,
        const TouchScreenInputEvent &touchScreenInputEvent);
    void AnalysisTouchScreenPressData(InputEventArray &inputEventArray,
        const TouchSingleEventData &touchSingleEventData);
    void AnalysisTouchScreenMoveData(InputEventArray &inputEventArray,
        const TouchSingleEventData &touchSingleEventData);
    void AnalysisTouchScreenReleaseData(InputEventArray &inputEventArray,
        const TouchSingleEventData &touchSingleEventData);
    void AnalysisTouchScreenToInputData(InputEventArray &inputEventArray,
                                        const TouchSingleEventData &touchSingleEventData);
};
} // namespace MMI
} // namespace OHOS
#endif // PROCESSING_TOUCH_SCREEN_DEVICE_H