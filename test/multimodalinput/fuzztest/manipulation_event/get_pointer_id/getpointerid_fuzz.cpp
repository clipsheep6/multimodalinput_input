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
#include "manipulation_event.h"
using namespace OHOS;
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    ManipulationEvent manipulationEventTest;
    const int32_t pos = 2;
    int32_t windowId = 0;
    int32_t startTime = 100;
    int32_t operationState = 10;
    int32_t pointerCount = 3;
    fingerInfos fingersInfos[FINGER_NUM] = {};
    int32_t highLevelEvent = 0;
    const std::string uuid = "a";
    int32_t sourceType = 0;
    uint64_t occurredTime = 0;
    const std::string deviceId = "1";
    int32_t inputDeviceId = 0;
    bool isHighLevelEvent = true;
    uint16_t deviceUdevTags = 0;
    fingersInfos[pos].mPointerId = 3;
    manipulationEventTest.Initialize(windowId, startTime, operationState, pointerCount, fingersInfos, highLevelEvent,
                                     uuid, sourceType, occurredTime, deviceId, inputDeviceId, isHighLevelEvent,
                                     deviceUdevTags);
                    
    int32_t index = size;  
    manipulationEventTest.GetPointerId(index);
    
    return 0;
}
