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

#ifndef KNUCKLE_GLOW_TRACE_SYSTEM_H
#define KNUCKLE_GLOW_TRACE_SYSTEM_H

#include <vector>
#include "knuckle_glow_point.h"
#include "Knukle_divergent_point.h"
#include "image/bitmap.h"
#include "draw/canvas.h"
#include "include/core/SkPath.h"
#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_recording_canvas.h"
#else
#include "recording/recording_canvas.h"
#endif

namespace OHOS {
namespace MMI {
class KnuckleGlowTraceSystem {
public:
    KnuckleGlowTraceSystem(int pointsize, const OHOS::Rosen::Drawing::Bitmap &shadow, int maxDivergenceNum);
    ~KnuckleGlowTraceSystem();
    void SetTrackColor(int trackColor);
    void Clear();
    void Update();
    void Draw(Rosen::Drawing::RecordingCanvas* canvas);
    void ResetDivergentPoints(double pointx, double pointY);
    void AddGlowPoints(Rosen::Drawing::Path path, long timeInteval);

private:
    std::vector<std::shared_ptr<KnukleGlowPoint>> mGlowPoints_;
    std::vector<std::shared_ptr<KnukleDivergentPoint>> mDivergentPoints_;
    int mMaxDivergenceNum_;
};
} // namespace MMI
} // namespace OHOS
#endif // KNUCKLE_GLOW_TRACE_SYSTEM_H
