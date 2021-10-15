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

#include "multimodal_input_connect_def_parcel.h"

namespace OHOS {
namespace MMI {
#define RETURN_IF_FALSE(expr) \
    if (!(expr)) { \
        return false; \
    }

#define RELEASE_IF_FALSE(expr, obj) \
    if (!(expr)) { \
        delete (obj); \
        (obj) = nullptr; \
        return (obj); \
    }

bool ConnectDefReqParcel::Marshalling(Parcel& out) const
{
    RETURN_IF_FALSE(out.WriteInt32(data.moduleId));
    RETURN_IF_FALSE(out.WriteString(data.clientName));
    return true;
}

ConnectDefReqParcel *ConnectDefReqParcel::Unmarshalling(Parcel& in)
{
    auto* request = new (std::nothrow) ConnectDefReqParcel();
    RELEASE_IF_FALSE(request != nullptr, request);
    RELEASE_IF_FALSE(in.ReadInt32(request->data.moduleId), request);
    request->data.clientName = in.ReadString();
    return request;
}

bool ConnectDefRespParcel::Marshalling(Parcel &out) const
{
    RETURN_IF_FALSE(out.WriteInt32(data.returnCode));
    RETURN_IF_FALSE(out.WriteInt32(data.allocedSocketId));
    return true;
}

ConnectDefRespParcel *ConnectDefRespParcel::Unmarshalling(Parcel &in)
{
    auto *response = new (std::nothrow) ConnectDefRespParcel();
    RELEASE_IF_FALSE(response != nullptr, response);
    RELEASE_IF_FALSE(in.ReadInt32(response->data.returnCode), response);
    RELEASE_IF_FALSE(in.ReadInt32(response->data.allocedSocketId), response);
    return response;
}
} // namespace MMI
} // namespace OHOS
