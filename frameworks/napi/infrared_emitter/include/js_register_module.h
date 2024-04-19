/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

/****
*
*【需求描述】:
支持如下接口：
1、获取设备支持的红外频率数据：参考命名 getCarrierFrequencies
输入：调用此接口
处理：调用HDF红外服务的HDI接口，获取频率数据
输出：返回给调用方设备支持的外频率数据
2、设备是否有红外：参考命名 hasIrEmitter ()
输入：调用此接口
处理：判断HDF红外服务是否运行
输出：红外服务运行了返回true，否则返回false
3、发送红外信号：参考命名 transmit (int frequency, int[] pattern)
输入：调用此接口
处理：调用HDF红外服务的HDI接口，以频率 frequency，发送pattern红外码
*
*****/

#ifndef JS_INFRARED_MANAGER_H
#define JS_INFRARED_MANAGER_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "define_multimodal.h"
#include "nocopyable.h"
#include "utils/log.h"
#include "key_event.h"
#include "key_option.h"

namespace OHOS {
    namespace MMI {
       
    } // namespace MMI
} // namespace OHOS
#endif // JS_INFRARED_MANAGER_H