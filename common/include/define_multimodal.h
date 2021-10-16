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
#ifndef OHOS_DEFINE_MULTIMODAL_H
#define OHOS_DEFINE_MULTIMODAL_H

#include "log.h"

#ifndef RET_OK
    #define RET_OK (0)
#endif

#ifndef RET_ERR
    #define RET_ERR (-1)
#endif

#ifndef LINEINFO
#define LINEINFO __FILE__, __LINE__
#endif

#if defined(__GNUC__) && __GNUC__ >= 4
    #define WL_EXPORT __attribute__ ((visibility("default")))
#else
    #define WL_EXPORT
#endif

#define USE_CLMAP
#define USE_CLSET

#ifdef USE_CLMAP
    #define MAKEPAIR    std::make_pair
    #define PAIR        std::pair
    #define CLMAP       std::map
    #define CLMULTIMAP  std::multimap
#else
    #define CLMAP       std::unordered_map
    #define CLMULTIMAP  std::unordered_multimap
#endif

#ifdef USE_CLSET
    #define CLSET       std::set
    #define CLMULTISET  std::multiset
#else
    #define CLSET       std::unordered_set
    #define CLMULTISET  std::unordered_multiset
#endif

#define IdsList     std::vector<int32_t>
#define StringList  std::vector<std::string>
#define StringSet   CLSET<std::string>
#define StringMap   CLMAP<std::string, std::string>

#ifdef DEBUG_CODE_TEST
#define CK(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("%{public}s(%{public}d): CK(%{public}s) errCode:%{public}d", __FILE__, __LINE__, #msg, ec); \
    }

#define CHK(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("%{public}s(%{public}d): CHK(%{public}s) errCode:%{public}d", __FILE__, __LINE__, #msg, ec); \
        return; \
    }

#define CHKF(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("%{public}s(%{public}d): CHKF(%{public}s) errCode:%{public}d", __FILE__, __LINE__, #msg, ec); \
        return 0; \
    }

#define CHKC(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("%{public}s(%{public}d): CHKC(%{public}s) errCode:%{public}d", __FILE__, __LINE__, #msg, ec); \
        continue; \
    }

#define CHKB(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("%{public}s(%{public}d): CHKB(%{public}s) errCode:%{public}d", __FILE__, __LINE__, #msg, ec); \
        break; \
    }

#define CHKR(msg, ec, r) \
    if (!(msg)) { \
        MMI_LOGE("%{public}s(%{public}d): CHKR(%{public}s) errCode:%{public}d", __FILE__, __LINE__, #msg, ec); \
        return r; \
    }
#define CHK_SEC_RET_EQ(ret, expectValue) \
    if ((ret) == (expectValue)) { \
        MMI_LOGE("%{public}s(%{public}d): SEC_RET_EQ: ret:%{public}d", __FILE__, __LINE__, ret); \
    }
#define CHK_SEC_RET_NE(ret, expectValue) \
    if ((ret) != (expectValue)) { \
        MMI_LOGE("%{public}s(%{public}d): SEC_RET_NE: ret:%{public}d", __FILE__, __LINE__, ret); \
    }
#else
#define CK(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("CK(%{public}s) errCode:%{public}d", #msg, ec); \
    }

#define CHK(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("CHK(%{public}s) errCode:%{public}d", #msg, ec); \
        return; \
    }

#define CHKF(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("CHKF(%{public}s) errCode:%{public}d", #msg, ec); \
        return 0; \
    }

#define CHKC(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("CHKC(%{public}s) errCode:%{public}d", #msg, ec); \
        continue; \
    }

#define CHKB(msg, ec) \
    if (!(msg)) { \
        MMI_LOGE("CHKB(%{public}s) errCode:%{public}d", #msg, ec); \
        break; \
    }

#define CHKR(msg, ec, r) \
    if (!(msg)) { \
        MMI_LOGE("CHKR(%{public}s) errCode:%{public}d", #msg, ec); \
        return r; \
    }

#define CHK_SEC_RET_EQ(ret, expectValue) \
    if ((ret) == (expectValue)) { \
        MMI_LOGE("SEC_RET_EQ: ret:%{public}d", ret); \
    }
#define CHK_SEC_RET_NE(ret, expectValue) \
    if ((ret) != (expectValue)) { \
        MMI_LOGE("SEC_RET_NE: ret:%{public}d", ret); \
    }
#endif

#define CHK_TYPE(a, b) \
    std::is_same<typename std::decay<a>::type, b>::value

#define LOCKGUARD(a) \
    std::lock_guard<std::mutex> ll(a)

#define UNIQUELOCK(a) \
    std::unique_lock<std::mutex> ll(a)

#define ARR_SIZE(a) (sizeof((a)) / sizeof((a[0])))
#define MAX_(t) ((std::numeric_limits<t>::max)())
#endif
