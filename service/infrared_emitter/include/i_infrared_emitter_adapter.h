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

#ifndef I_INFRARED_EMITTER_ADAPTER_H
#define I_INFRARED_EMITTER_ADAPTER_H

#include "hdi_base.h"

namespace OHOS {
namespace HDI {
namespace Consumerir {
namespace V1_0 {
struct ConsumerIrFreqRange {
    int32_t min { 0 };
    int32_t max { 0 };
};
} // namespace V1_0
} // namespace Consumerir
} // namespace HDI

namespace MMI {
class IInfraredEmitterAdapter  {
public:
    IInfraredEmitterAdapter() = default;
    virtual ~IInfraredEmitterAdapter() = default;

    virtual int32_t Transmit(int32_t carrierFreq, const std::vector<int32_t>& pattern, bool& ret) = 0;
    virtual int32_t GetCarrierFreqs(bool& ret,
        std::vector<OHOS::HDI::Consumerir::V1_0::ConsumerIrFreqRange>& range) = 0;
};
} // namespace MMI
} // namespace OHOS
#endif // I_INFRARED_EMITTER_ADAPTER_H