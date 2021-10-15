
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
#include "keyboard_event.h"
using namespace OHOS;
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    KeyBoardEvent keyBoardEventTmp;
    keyBoardEventTmp.Initialize(33, false, 33, true, false,
                                false, false, 54, 54, 54, "54", 54, 54, "54", 54, false, 0, 0);
    
    int32_t keycodeOne = size;
    keyBoardEventTmp.IsNoncharacterKeyPressed(keycodeOne);

    int32_t keycodeTwo = size;
    keyBoardEventTmp.IsNoncharacterKeyPressed(keycodeOne, keycodeTwo);
    
    int32_t keycodeThree = size;
    keyBoardEventTmp.IsNoncharacterKeyPressed(keycodeOne, keycodeTwo, keycodeThree);
    return 0;
}
