/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef X_KEY_EVENT_PROCESSOR_H
#define X_KEY_EVENT_PROCESSOR_H

#include <memory>
#include "libinput.h"
#include "singleton.h"

namespace OHOS {
namespace MMI {
#ifdef OHOS_BUILD_ENABLE_X_KEY

class XKeyEventProcessor {
    DECLARE_DELAYED_SINGLETON(XKeyEventProcessor);
public:
    DISALLOW_COPY_AND_MOVE(XKeyEventProcessor);
    void InterceptXKeyDown();
    void InterceptXKeyUp();
    bool IsXKeyEvent(struct libinput_event* event);
    int32_t HandleXKeyEvent(struct libinput_event* event);

private:
    int32_t AnalyseKeyEvent(struct libinput_event* event);
    int32_t HandleQuickAccessMenu(int32_t xKeyEventType);
    // 重置计数器
    void ResetCount();
    void StartXKeyIfNeeded(int32_t xKeyEventType);
    bool IsRemoveDelaySingleClick();
    void StartLongPressTimer();
    void StartSingleClickTimer();
    void RemoveTimer();

    int32_t singleClickTimerId_ { -1 };
    int32_t longPressTimerId_ { -1 };
    // 按压次数
    std::atomic<int32_t> pressCount_ { 0 };

    const std::string X_KEY_SOURCE_KEY { "fkey" };
    std::atomic_bool isStartedXKey_ { false };
    std::atomic_bool handledLongPress_ { false };
};
#define XKeyEventHdr ::OHOS::DelayedSingleton<XKeyEventProcessor>::GetInstance()
#endif // OHOS_BUILD_ENABLE_X_KEY
} // namespace MMI
} // namespace OHOS
#endif // X_KEY_EVENT_PROCESSOR_H