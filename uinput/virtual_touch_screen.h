/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef VIRTUALTOUCHSCREEN_H
#define VIRTUALTOUCHSCREEN_H

#include <cstdint>

#include "virtual_device.h"

class VirtualTouchScreen : public VirtualDevice {
public:
    VirtualTouchScreen(const uint32_t maxX, const uint32_t maxY);
    virtual ~VirtualTouchScreen() = default;

protected:
    const std::vector<uint32_t>& GetEventTypes() const override;
    const std::vector<uint32_t>& GetKeys() const override;
    const std::vector<uint32_t>& GetProperties() const override;
    const std::vector<uint32_t>& GetAbs() const override;
};


#endif  // VIRTUALTOUCHSCREEN_H
