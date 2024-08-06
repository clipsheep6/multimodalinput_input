/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef TOUCH_GESTURE_DETECTOR_H
#define TOUCH_GESTURE_DETECTOR_H

#include <map>

#include "pointer_event.h"

namespace OHOS {
namespace MMI {
using AdapterType = uint32_t;
constexpr AdapterType SwipeAdapterType {0x0};
constexpr AdapterType PinchAdapterType {0x1};
enum class GetureType {
    ACTION_UNKNOW,
    ACTION_SWIPE_DOWN,
    ACTION_SWIPE_UP,
    ACTION_SWIPE_LEFT,
    ACTION_SWIPE_RIGHT,
    ACTION_PINCH_CLOSED,
    ACTION_PINCH_OPENED
};

struct Point {
    Point() {}
    Point(int32_t px, int32_t py) : x(px), y(py) {}
    Point(int32_t px, int32_t py, int64_t pt)
        : x(px), y(py), time(pt) {}
    int32_t x {0};
    int32_t y {0};
    int64_t time {0};
};

class TouchGestureDetector final {
public:
    class GestureListener {
    public:
        GestureListener() = default;
        virtual ~GestureListener() = default;
        virtual bool OnGestureEvent(std::shared_ptr<PointerEvent> event, GetureType mode) = 0;
    };

    TouchGestureDetector(AdapterType type, std::shared_ptr<GestureListener> listener)
        : getureType_(type), listener_(listener) {}
    bool OnTouchEvent(std::shared_ptr<PointerEvent> event);

private:
    enum class SlideState {
        DIRECTION_UNKNOW,
        DIRECTION_DOWN,
        DIRECTION_UP,
        DIRECTION_LEFT,
        DIRECTION_RIGHT
    };
    void ReleaseData();
    void HandleDownEvent(std::shared_ptr<PointerEvent> event);
    void HandleMoveEvent(std::shared_ptr<PointerEvent> event);
    void HandleUpEvent(std::shared_ptr<PointerEvent> event);
    bool NotifyGestureEvent(std::shared_ptr<PointerEvent> event, GetureType mode);
    bool WhetherDiscardTouchEvent(std::shared_ptr<PointerEvent> event);

    Point CalcGravityCenter(std::map<int32_t, Point> map);
    double CalcTwoPointsDistance(Point p1, Point p2);
    void CalcAndStoreDistance(std::map<int32_t, Point> map);
    int32_t CalcMultiFingerMovement(std::map<int32_t, Point> map);
    void HandlePinchMoveEvent(std::shared_ptr<PointerEvent> event);
    GetureType JudgeOperationMode(std::map<int32_t, Point> movePoint);
    bool AntiJitter(std::shared_ptr<PointerEvent> event, GetureType mode);

    bool HandleFingerDown();
    int64_t GetMaxDownInterval();
    float GetMaxFingerSpacing();
    GetureType ChangeToGetureType(SlideState state);
    SlideState GetSlidingDirection(double angle);
    void HandleSwipeMoveEvent(std::shared_ptr<PointerEvent> event);
    bool IsFingerMove(float startX, float startY, float endX, float endY);
    double GetAngle(float startX, float startY, float endX, float endY);
    SlideState ClacFingerMoveDirection(std::shared_ptr<PointerEvent> event);

private:
    AdapterType getureType_ {-1};
    bool isRecognized_ {false};
    bool gestureEnable_ {true};
    bool isFingerReady_ {false};
    bool haveLastDistance_ {false};
    int32_t gestureDisplayId_ {INT32_MAX};
    int32_t continuousCloseCount_ {0};
    int32_t continuousOpenCount_ {0};
    std::map<int32_t, Point> downPoint_;
    std::map<int32_t, double> lastDistance_;
    std::shared_ptr<GestureListener> listener_ {nullptr};
};
} // namespace MMI
} // namespace OHOS
#endif // TOUCH_GESTURE_DETECTOR_H