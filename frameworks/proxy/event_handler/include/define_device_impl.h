/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef DEFINE_DEVICE_IMPL_H
#define DEFINE_DEVICE_IMPL_H

#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    #include "input_device_impl.h"
#else
    #include "i_input_device_impl.h"
#endif // OHOS_BUILD_ENABLE_INTERCEPTOR

namespace OHOS {
namespace MMI {
#ifdef OHOS_BUILD_DEVICE_MANAGER_API
    #define InputDevImpl OHOS::MMI::InputDeviceImpl::GetInstance()
#else
    #define InputDevImpl OHOS::MMI::IInputDeviceImpl::GetInstance()
#endif
} // namespace MMI
} // namespace OHOS
#endif // DEFINE_DEVICE_IMPL_H