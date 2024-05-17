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

#ifndef INFRARED_EMMITTER_DEF_H
#define INFRARED_EMMITTER_DEF_H

#include <stdint.h>
#include <vector>
#include "hdi_base.h"

namespace OHOS {
namespace HDI {
namespace Consumerir {
namespace V1_0 {
using namespace OHOS::HDI;
struct ConsumerIrFreqRange {
    int64_t max_ { 0 };
    int64_t min_ { 0 };
} _attribute_((aligned(8)));
class ConsumerIr: public HdiBase {
public:
    DECLARE_HDI_DESCRIPROT(u"ohos.hdi.consumerir.v1_0.ConsumerIr");
    virtual ~ConsumerIr() = default;

    static sptr<OHOS::HDI::Consumerir::V1_0::ConsumerIr> Get(bool isStub = false);
    static sptr<OHOS::HDI::Consumerir::V1_0::ConsumerIr> Get(const std::string &serviceName, bool isStub = false);

    virtual int32_t Transmit(int32_t carrierFreq, const std::vector<int32_t> &pattern, bool &ret) = 0;

    virtual int32_t GetCarrierFreqs(bool &ret, std::vector<OHOS::HDI::Consumerir::V1_0::ConsumerIrFreqRange> &range) = 0;

    virtual int32_t GetVersion(uint32_t &majorVer, uint32_t &minorVer) = 0;

    virtual bool IsProxy();

    virtual const std::u16string GetDesc();
};
}
}
}
}
#endif // INFRARED_EMMITTER_DEF_H