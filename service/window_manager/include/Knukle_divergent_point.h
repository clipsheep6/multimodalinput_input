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
#ifndef KNUCKLE_GIVERGENT_POINT_H
#define KNUCKLE_GIVERGENT_POINT_H

#include "image/bitmap.h"
#include "draw/canvas.h"
#include "utils/matrix.h"
#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_recording_canvas.h"
#else
#include "recording/recording_canvas.h"
#endif
namespace OHOS {
namespace MMI {
class KnukleDivergentPoint {
public:
    KnukleDivergentPoint(const OHOS::Rosen::Drawing::Bitmap &shadow);
    ~KnukleDivergentPoint();
    void Update();
    void Clear();
    void Draw(Rosen::Drawing::RecordingCanvas* canvas);
    void Reset(double pointX, double pointY);
    bool IsEnded();

private:
    static Rosen::Drawing::Pen sTracePaint;
    static double BASIC_LIFESPAN;
    static double BASIC_GRAVITY_Y;
    static double DEFAULT_LIFESPAN;
    static float DOUBLE;
    static int TRACE_COLOR;
    static int DEFAULT_SIZE;
    static int DEFAULT_SIZE_OFFSET;
    static int DEFAULT_SPEED;
    static int DEFAULT_SPEED_OFFSET;
    // static double PI;

    double mMoveVelocityX_;
    double mMoveVelocityY_;
    double mPointX_;
    double mPointY_;
    double mLifespan_ = DEFAULT_LIFESPAN;
    Rosen::Drawing::Matrix mTraceMatrix_;
    OHOS::Rosen::Drawing::Bitmap mTraceShadow_;
};
} // namespace MMI
} // namespace OHOS
#endif // KNUCKLE_GIVERGENT_POINT_H
