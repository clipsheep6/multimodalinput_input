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
#ifndef ID_FACTORY_H
#define ID_FACTORY_H

#include <set>

namespace OHOS {
namespace MMI {
template<typename T>
class IdFactroy {
public:
    IdFactroy() : IdFactroy(1) {}
    explicit IdFactroy(T seed) : seed_(seed) {}
    virtual ~IdFactroy() = default;

    T GetId()
    {
        if (ids_.empty()) {
            return seed_++;
        }
        T id = *ids_.begin();
        ids_.erase(ids_.begin());
        return id;
    }
    void ReleaseId(T id)
    {
        ids_.insert(id);
    }

private:
    T seed_ = 0;
    std::set<T> ids_;
};
} // namespace MMI
} // namespace OHOS
#endif // ID_FACTORY_H