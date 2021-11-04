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
#include <codecvt>
#include "mmi_token.h"
#include "mmi_client.h"
#include "error_multimodal.h"
#include "multimodal_event_handler.h"
#include "touch_event_handler.h"
#include "proto.h"
#include "log.h"

using namespace OHOS::MMI;
using namespace OHOS;

class AppTouchEventHandle : public TouchEventHandler {
public:
    AppTouchEventHandle() { 
        type_ = MmiMessageId::TOUCH_EVENT_BEGIN; 
    }
    ~AppTouchEventHandle() {}
    virtual bool OnTouch(const TouchEvent& touchEvent) override
    {
        return true;
    }
};

template<class T>
StandEventPtr CreateEvent()
{
    return StandEventPtr(new T());
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::string u8String = "\nTest!\n";
    auto wsConvert = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{};
    auto u16String = wsConvert.from_bytes(u8String);
    auto remoteObject = MMIToken::Create(u16String);
    
    int32_t windowId = size;
     
    auto appTouch = CreateEvent<AppTouchEventHandle>();
    MMIEventHdl.RegisterStandardizedEventHandle(remoteObject, windowId, appTouch);
    MMIEventHdl.UnregisterStandardizedEventHandle(remoteObject, windowId, appTouch);

    return 0;
}
