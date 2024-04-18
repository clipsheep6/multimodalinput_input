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

#include "knuckle_glow_point.h"

#include "include/core/SkColorFilter.h"
#include "define_multimodal.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {

constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "KnukleGlowPoint" };
constexpr int32_t SEC_TO_NANOSEC = 1000000000;
constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;

int KnukleGlowPoint::TRACE_COLOR = 255;
float KnukleGlowPoint::BASIC_SIZE = 100.0f;
float KnukleGlowPoint::DOUBLE = 2.0f;
double KnukleGlowPoint::BASIC_LIFESPAN = 400;
double KnukleGlowPoint::DEFAULT_LIFESPAN = -1;

KnukleGlowPoint::KnukleGlowPoint(const OHOS::Rosen::Drawing::Bitmap &shadow)
{
    CALL_DEBUG_ENTER;
    mTraceShadow_ = shadow;
}

KnukleGlowPoint::~KnukleGlowPoint() {};

int64_t KnukleGlowPoint::GetNanoTime()
{
    CALL_DEBUG_ENTER;
    struct timespec time = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &time);
    return static_cast<int64_t>(time.tv_sec) * SEC_TO_NANOSEC + time.tv_nsec;
}

void KnukleGlowPoint::Update()
{
    CALL_DEBUG_ENTER;
    if (IsEnded()) {
        return;
    }
    long currentTime = GetNanoTime() / NANOSECOND_TO_MILLISECOND;
    long timeInterval = currentTime - mLastUpdateTimeMillis_;
    if (timeInterval < 0) {
        timeInterval = 0;
    }

    mLastUpdateTimeMillis_ = currentTime;
    MMI_HILOGE("mLifespan_:%{public}f  timeInterval:%{public}ld", mLifespan_, timeInterval);
    mLifespan_ -= timeInterval;
    mTraceSize_ = (float)((mLifespan_ / BASIC_LIFESPAN) * BASIC_SIZE);
    UpdateMatrix();
    mGlowPaint_.SetAlpha((int) (TRACE_COLOR * (mLifespan_ / BASIC_LIFESPAN)));
}

void KnukleGlowPoint::Draw(Rosen::Drawing::RecordingCanvas* canvas)
{
    CALL_DEBUG_ENTER;
    if (canvas == nullptr) {
        MMI_HILOGE("canvas null");
        return;
    }
    if (IsEnded()) {
        MMI_HILOGE("IsEnded");
    }
    MMI_HILOGE("mPointX_:%{public}f  mPointY_:%{public}f", mPointX_, mPointY_);
    if (IsEnded() || mPointX_ <= 0 || mPointY_ <= 0) {
        return;
    }
    canvas->AttachPaint(mGlowPaint_);
    canvas->SetMatrix(mTraceMatrix_);
    MMI_HILOGE("DrawBitmap");
    canvas->DrawBitmap(mTraceShadow_, mPointX_, mPointY_);
}

void KnukleGlowPoint::SetTrackColor(int color)
{
    CALL_DEBUG_ENTER;
}

void KnukleGlowPoint::Reset(double pointx, double pointy, float lifespanoffset)
{
    CALL_DEBUG_ENTER;
    mPointX_ = pointx;
    mPointY_ = pointy;
    mLifespan_ = BASIC_LIFESPAN - lifespanoffset;
    mTraceSize_ = BASIC_SIZE;
    mLastUpdateTimeMillis_ = GetNanoTime() / NANOSECOND_TO_MILLISECOND;
}

bool KnukleGlowPoint::IsEnded()
{
    CALL_DEBUG_ENTER;
    MMI_HILOGE("mLifespan_:%{public}f", mLifespan_);
    return mLifespan_ < 0;
}

void KnukleGlowPoint::UpdateMatrix()
{
    CALL_DEBUG_ENTER;
    mTraceMatrix_.Reset();
    mTraceMatrix_.PostTranslate(-mTraceShadow_.GetWidth() / DOUBLE, -mTraceShadow_.GetHeight() / DOUBLE);
    
    float proportion = mTraceSize_ / (float) mTraceShadow_.GetWidth();
    mTraceMatrix_.PostScale(proportion, proportion);
    mTraceMatrix_.PostTranslate((float)mPointX_, (float)mPointY_);
}
}
}
