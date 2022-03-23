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

#include "processing_keyboard_device.h"

using namespace OHOS::MMI;

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "ProcessingKeyboardDevice" };
} // namespace

int32_t ProcessingKeyboardDevice::TransformJsonDataToInputData(const cJSON* fingerEventArrays,
    InputEventArray& inputEventArray)
{
    CALL_LOG_ENTER;
    cJSON* inputData = cJSON_GetObjectItemCaseSensitive(fingerEventArrays, "events");
    if (inputData == nullptr) {
        MMI_LOGE("manage finger array faild, inputData is empty.");
        return RET_ERR;
    }
    std::vector<KeyBoardEvent> keyBoardEventArray;
    if (AnalysisKeyBoardEvent(inputData, keyBoardEventArray) == RET_ERR) {
        return RET_ERR;
    }
    TransformKeyBoardEventToInputEvent(keyBoardEventArray, inputEventArray);
    return RET_OK;
}

void ProcessingKeyboardDevice::TransformKeyBoardEventToInputEvent(const std::vector<KeyBoardEvent>& keyBoardEventArray,
                                                                  InputEventArray& inputEventArray)
{
    for (const auto &item : keyBoardEventArray) {
        if (item.eventType == "KEY_EVENT_PRESS") {
            TransformKeyPressEvent(item, inputEventArray);
        } else if (item.eventType == "KEY_EVENT_RELEASE") {
            TransformKeyReleaseEvent(item, inputEventArray);
        } else if (item.eventType == "KEY_EVENT_CLICK") {
            TransformKeyClickEvent(item, inputEventArray);
        } else if (item.eventType == "KEY_EVENT_LONG_PRESS") {
            TransformKeyLongPressEvent(item, inputEventArray);
        } else {
            MMI_LOGW("json file format error");
        }
    }
}

int32_t ProcessingKeyboardDevice::AnalysisKeyBoardEvent(const cJSON* inputData,
                                                        std::vector<KeyBoardEvent>& keyBoardEventArray)
{
    KeyBoardEvent keyBoardEvent = {};
    for (int32_t i = 0; i < cJSON_GetArraySize(inputData); i++) {
        keyBoardEvent = {};
        cJSON* event = cJSON_GetArrayItem(inputData, i);
        if (event) {
            cJSON* eventType = cJSON_GetObjectItemCaseSensitive(event, "eventType");
            if (eventType) {
                keyBoardEvent.eventType = eventType->valuestring;
            }
            cJSON* keyValue = cJSON_GetObjectItemCaseSensitive(event, "keyValue");
            if (keyValue) {
                keyBoardEvent.keyValue = keyValue->valueint;
            }
            cJSON* blockTime = cJSON_GetObjectItemCaseSensitive(event, "blockTime");
            if (blockTime) {
                keyBoardEvent.blockTime = blockTime->valueint;
            }
        }
        keyBoardEventArray.push_back(keyBoardEvent);
    }
    return RET_OK;
}

void ProcessingKeyboardDevice::TransformKeyPressEvent(const KeyBoardEvent& keyBoardEvent,
                                                      InputEventArray& inputEventArray)
{
    uint16_t keyValue = static_cast<uint16_t>(keyBoardEvent.keyValue);
    SetKeyPressEvent(inputEventArray, keyBoardEvent.blockTime, keyValue);
    SetSynReport(inputEventArray);
}

void ProcessingKeyboardDevice::TransformKeyLongPressEvent(const KeyBoardEvent& keyBoardEvent,
                                                          InputEventArray& inputEventArray)
{
    uint16_t keyValue = static_cast<uint16_t>(keyBoardEvent.keyValue);
    SetKeyPressEvent(inputEventArray, EVENT_REPROT_TIMES, keyValue);
    SetSynReport(inputEventArray);
    int32_t keyEventNum = (keyBoardEvent.blockTime / EVENT_REPROT_COUNTS) + 1;
    int32_t count = 0;
    while (count++ < keyEventNum) {
        SetKeyLongPressEvent(inputEventArray, EVENT_REPROT_TIMES, keyValue);
        SetSynConfigReport(inputEventArray, EVENT_REPROT_TIMES);
    }
    SetKeyReleaseEvent(inputEventArray, EVENT_REPROT_TIMES, keyValue);
    SetSynReport(inputEventArray);
}

void ProcessingKeyboardDevice::TransformKeyReleaseEvent(const KeyBoardEvent& keyBoardEvent,
                                                        InputEventArray& inputEventArray)
{
    uint16_t keyValue = static_cast<uint16_t>(keyBoardEvent.keyValue);
    SetKeyReleaseEvent(inputEventArray, keyBoardEvent.blockTime, keyValue);
    SetSynReport(inputEventArray);
}

void ProcessingKeyboardDevice::TransformKeyClickEvent(const KeyBoardEvent& keyBoardEvent,
                                                      InputEventArray& inputEventArray)
{
    uint16_t keyValue = static_cast<uint16_t>(keyBoardEvent.keyValue);
    SetKeyPressEvent(inputEventArray, keyBoardEvent.blockTime, keyValue);
    SetSynReport(inputEventArray);
    SetKeyReleaseEvent(inputEventArray, keyBoardEvent.blockTime, keyValue);
    SetSynReport(inputEventArray);
}