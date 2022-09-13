/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_I_DISTRIBUTED_DEVICE_PROFILE_H
#define OHOS_I_DISTRIBUTED_DEVICE_PROFILE_H

#include <list>
#include "idistributed_device_profile.h"
#include "stdint-intn.h"

namespace OHOS {
namespace DeviceProfile {
class MockIDistributedDeviceProfile {
public:
    enum {
        PUT_DEVICE_PROFILE = 1,
        GET_DEVICE_PROFILE = 2,
        DELETE_DEVICE_PROFILE = 3,
        SUBSCRIBE_PROFILE_EVENT = 4,
        UNSUBSCRIBE_PROFILE_EVENT = 5,
        SYNC_DEVICE_PROFILE = 6
    };
    int32_t serviceId;
};
} // namespace DeviceProfile
} // namespace OHOS
#endif // OHOS_I_DISTRIBUTED_DEVICE_PROFILE_H