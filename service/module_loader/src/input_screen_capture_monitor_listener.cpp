/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "input_screen_capture_monitor_listener.h"
#include "mmi_service.h"

namespace OHOS {
namespace MMI {
#ifdef OHOS_BUILD_ENABLE_MONITOR
void InputScreenCaptureMonitorListener::OnScreenCaptureStarted(int32_t pid)
{
    OHOS::DelayedSingleton<MMIService>::GetInstance()->SaveScreenCapturePid(pid);
}

void InputScreenCaptureMonitorListener::OnScreenCaptureFinished(int32_t pid)
{
    OHOS::DelayedSingleton<MMIService>::GetInstance()->RemoveScreenCaptureMonitor(pid);
}
#endif
}
}