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

#ifndef MOCK_DISTRIBUTED_DEVICE_PROFILE_CLIENT
#define MOCK_DISTRIBUTED_DEVICE_PROFILE_CLIENT

#include "distributed_device_profile_client.h"

namespace OHOS {
namespace DeviceProfile {
class MockDistributedDeviceProfileClient {
public:
    static void SetDPState(bool state);
};
//#define MDDeviceProfileClient MockDistributedDeviceProfileClient::GetInstance()
} // namespace DeviceProfile
} // namespace OHOS
#endif // MOCK_DISTRIBUTED_DEVICE_PROFILE_CLIENT
