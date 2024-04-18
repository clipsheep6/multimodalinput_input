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

#include "Knukle_divergent_point.h"

#include <ctime>
#include <iostream>
#include <random>
#include "define_multimodal.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KnukleDivergentPoint" };
constexpr double PI = 3.14159265358979323846;
Rosen::Drawing::Pen KnukleDivergentPoint::sTracePaint;
double KnukleDivergentPoint::BASIC_LIFESPAN = 15;
double KnukleDivergentPoint::BASIC_GRAVITY_Y = 0.5;
double KnukleDivergentPoint::DEFAULT_LIFESPAN = -1;
float KnukleDivergentPoint::DOUBLE = 2.0f;
int KnukleDivergentPoint::TRACE_COLOR = 255;
int KnukleDivergentPoint::DEFAULT_SIZE = 80;
int KnukleDivergentPoint::DEFAULT_SIZE_OFFSET = 20;
int KnukleDivergentPoint::DEFAULT_SPEED = 8;
int KnukleDivergentPoint::DEFAULT_SPEED_OFFSET = 8;

KnukleDivergentPoint::KnukleDivergentPoint(const OHOS::Rosen::Drawing::Bitmap &shadow)
{
    CALL_DEBUG_ENTER;
    mTraceShadow_ = shadow;
    srand((unsigned)time(nullptr));
    float newSize = rand()%DEFAULT_SIZE + DEFAULT_SIZE_OFFSET;
    float proportion = 0;
    int width = shadow.GetWidth();
    if (width!= 0) {
        proportion = newSize / width;
    }
    mTraceMatrix_.PostTranslate(-width / DOUBLE, -shadow.GetHeight() /DOUBLE);
    mTraceMatrix_.PostScale(proportion, proportion);
}

KnukleDivergentPoint::~KnukleDivergentPoint() {};

void KnukleDivergentPoint::Update()
{
    CALL_DEBUG_ENTER;
    if (IsEnded()) {
        return;
    }
    mLifespan_--;
    mPointX_ += mMoveVelocityX_;
    mPointY_ += mMoveVelocityY_;
    mMoveVelocityY_ += BASIC_GRAVITY_Y;
}

void KnukleDivergentPoint::Clear()
{
    CALL_DEBUG_ENTER;
    mLifespan_ = DEFAULT_LIFESPAN;
}

void KnukleDivergentPoint::Draw(Rosen::Drawing::RecordingCanvas* canvas)
{
    CALL_DEBUG_ENTER;
    if (canvas == nullptr) {
        return;
    }

    if (IsEnded() || mPointX_ <= 0 || mPointY_ <= 0) {
        return;
    }

    sTracePaint.SetAlpha((int) (TRACE_COLOR * (mLifespan_/BASIC_LIFESPAN)));
    Rosen::Drawing::Matrix matrixs = Rosen::Drawing::Matrix(mTraceMatrix_);
    matrixs.PostTranslate((float) mPointX_,(float) mPointY_);

    Rosen::Drawing::Paint paint;
    canvas->AttachPaint(paint);
    canvas->SetMatrix(mTraceMatrix_);
    MMI_HILOGE("DrawBitmap");
    canvas->DrawBitmap(mTraceShadow_, mPointX_, mPointY_);
    // canvas.DrawBitmap(mTraceShadow_, matrixs, sTracePaint);
}

void KnukleDivergentPoint::Reset(double pointX, double pointY)
{
    CALL_DEBUG_ENTER;
    mPointX_ = pointX;
    mPointY_ = pointY;
    mLifespan_ = BASIC_LIFESPAN;

    // 随机double
    std::default_random_engine e(time(nullptr));
    std::uniform_real_distribution<double> u(0.0, 1.0);
    double baseVelocity = u(e) * DOUBLE * PI;

    srand((unsigned)time(nullptr));
    double moveSpeed = rand() % DEFAULT_SPEED + DEFAULT_SPEED_OFFSET;
    mMoveVelocityX_ = std::cos(baseVelocity) * moveSpeed;
    mMoveVelocityY_ = std::sin(baseVelocity) * moveSpeed;
}

bool KnukleDivergentPoint::IsEnded()
{
    CALL_DEBUG_ENTER;
    return mLifespan_ < 0;
}
}
}
