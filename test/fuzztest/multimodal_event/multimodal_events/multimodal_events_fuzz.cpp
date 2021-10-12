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
#include "multimodal_event.h"
using namespace OHOS;
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    MultimodalEvent multiModalEvent;
    
    int32_t windowId = 1;
    int32_t highLevelEvent = 1;
    std::string strUuid = "1";
    int32_t sourceType = 1;
    int32_t occurredTime = 1;
    std::string deviceId = "1";
    int32_t inputDeviceId = 1;
    bool isHighLevelEvent = false;
    multiModalEvent.Initialize(windowId, highLevelEvent, strUuid, sourceType, occurredTime,
                               deviceId, inputDeviceId, isHighLevelEvent);
           
    std::string id = std::to_string(*data);
    multiModalEvent.IsSameEvent(id);
    return 0;
}
