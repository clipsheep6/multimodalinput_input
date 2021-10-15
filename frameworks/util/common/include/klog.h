/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef OHOS_KLOG_H
#define OHOS_KLOG_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

//#ifdef OHOS_BUILD_MMI_DEBUG

void kMsgLog(const char *fileName, int line, const char *kLevel, const char *fmt, ...);

#define KMSG_FILE_NAME   (strrchr((__FILE__), '/') ? strrchr((__FILE__), '/') + 1 : (__FILE__))
#define KMSG_LOGD(fmt, ...) kMsgLog((KMSG_FILE_NAME), (__LINE__), "<7>", fmt"\n", ##__VA_ARGS__)
#define KMSG_LOGI(fmt, ...) kMsgLog((KMSG_FILE_NAME), (__LINE__), "<6>", fmt"\n", ##__VA_ARGS__)
#define KMSG_LOGW(fmt, ...) kMsgLog((KMSG_FILE_NAME), (__LINE__), "<4>", fmt"\n", ##__VA_ARGS__)
#define KMSG_LOGE(fmt, ...) kMsgLog((KMSG_FILE_NAME), (__LINE__), "<3>", fmt"\n", ##__VA_ARGS__)
#define KMSG_LOGF(fmt, ...) kMsgLog((KMSG_FILE_NAME), (__LINE__), "<3>", fmt"\n", ##__VA_ARGS__)

//#else
//
//#define KMSG_LOGD(fmt, ...)
//#define KMSG_LOGI(fmt, ...)
//#define KMSG_LOGW(fmt, ...)
//#define KMSG_LOGE(fmt, ...)
//#define KMSG_LOGF(fmt, ...)
//
//#endif // OHOS_BUILD_MMI_DEBUG

#ifdef __cplusplus
}
#endif

#endif // OHOS_KLOG_H
