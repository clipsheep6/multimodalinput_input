/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "mmi_sign_helper.h"
#include <random>
#include "openssl/evp.h"
#include "openssl/hmac.h"
#include "securec.h"
#include "error_multimodal.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MmiSignHelper" };
} // namespace

static std::array<uint8_t, 128> GetRandomKey()
{
    std::array<uint8_t, 128> key;
    std::random_device rd;
    for (auto i = 0; i < 128; i+=4) {
        uint32_t a = static_cast<uint32_t>(rd());
        errno_t ret = memcpy_s(&key[i], 4, &a, 4);
        if (ret != EOK) {
            MMI_HILOGE("memcpy_s fail, ret = %{public}d", ret);
        }
    }
    return key;
}

MmiSignHelper::MmiSignHelper() : hmacKey_(GetRandomKey()) {}

std::array<uint8_t, 32> MmiSignHelper::Sign(const uint8_t *data, size_t size) const
{
    std::array<uint8_t, 32> hash;
    uint hashLen = 0;
    uint8_t *result = HMAC(EVP_sha256(), hmacKey_.data(), hmacKey_.size(), data, size, hash.data(), &hashLen);
    if (result == nullptr) {
        MMI_HILOGE("fail to sign the data with HMAC");
        return INVALID_HMAC;
    }
    if (hashLen != hash.size()) {
        MMI_HILOGE("HMAC-256 has unexpected length");
        return INVALID_HMAC;
    }

    return hash;
}

std::string MmiSignHelper::ToString(const std::array<uint8_t, 32> & a)
{
    std::ostringstream oss;
    for (auto &i : a) {
        char buf[3] = {};
        sprintf(buf, "%02x", i);
        oss << buf;
    }
    return oss.str();
}
} // namespace MMI
} // namespace OHOS
