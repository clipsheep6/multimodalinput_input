/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FINGERSENSE_MANAGER_H
#define FINGERSENSE_MANAGER_H

#include "nocopyable.h"
#include "singleton.h"

namespace OHOS {
namespace MMI {
struct TouchType {
    int id;
    float x, y;                     // screen coordinates (px)
    float touch_major, touch_minor; // widths (px)
    float pressure;                 // 0..1
    float orientation;              // -90..90
    float tool_major;
    float tool_minor;
    int touch_kind;
};

// Knuckle Type
enum KnuckleType {
    FS_TOUCH_KIND_PAD = 0,
    FS_TOUCH_KIND_TIP,
    FS_TOUCH_KIND_NAIL,
    FS_TOUCH_KIND_KNUCKLE,
    FS_TOUCH_KIND_ALT_HAND_1,
    FS_TOUCH_KIND_ALT_HAND_2,
    FS_TOUCH_KIND_BRUSH_1,
    FS_TOUCH_KIND_BRUSH_2,
    FS_TOUCH_KIND_STYLUS_1,
    FS_TOUCH_KIND_STYLUS_2,
    FS_TOUCH_KIND_ERASER,
    FS_TOUCH_KIND_UNKNOWN,
    FS_TOUCH_KIND_BEZEL,
    FS_NUM_TOUCH_KIND
};

class FingersenseManager final {
    DECLARE_DELAYED_SINGLETON(FingersenseManager);
public:
    DISALLOW_COPY_AND_MOVE(FingersenseManager);
    void Init();
    void KnuckleTouchUp(TouchType *rawTouch);
    void KnuckleClassifyTouch(TouchType *rawTouch);
    void enableFingersense();
    void disableFingerSense();
    void SetUpKnuckle();
    void SetCurrentToolType(struct TouchType touchType, int32_t &toolType);

private:
    void *knuckleHandle_;
    // Handle to the interface in libfingersense.so
    typedef void (*KNUCKLE_TOUCH_UP_HANDLE)(struct TouchType *rawTouch);
    typedef void (*KNUCKLE_SWITCH_MANAGER)();
    KNUCKLE_TOUCH_UP_HANDLE knuckleTouchUpHandle_ = nullptr;
    KNUCKLE_TOUCH_UP_HANDLE KnuckleClassifyTouchHandle_ = nullptr;
    KNUCKLE_SWITCH_MANAGER knuckleOnResumeHandle_ = nullptr;
    KNUCKLE_SWITCH_MANAGER knuckleOnPauseHandle_ = nullptr;
};

#define FingersenseMgr ::OHOS::DelayedSingleton<FingersenseManager>::GetInstance()
} // namespace MMI
} // namespace OHOS
#endif // FINGERSENSE_MANAGER_H