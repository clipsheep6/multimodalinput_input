/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef LIBINPUT_MOCK_H
#define LIBINPUT_MOCK_H

#include <gmock/gmock.h>

#include "libinput_interface.h"

namespace OHOS {
namespace MMI {
class LibinputInterfaceMock : public LibinputInterface {
public:
    LibinputInterfaceMock() = default;
    virtual ~LibinputInterfaceMock() = default;

    MOCK_METHOD(enum libinput_event_type, GetEventType, (struct libinput_event *));
    MOCK_METHOD(struct libinput_device *, GetDevice, (struct libinput_event *));
    MOCK_METHOD(uint64_t, GetSensorTime, (struct libinput_event *));
    MOCK_METHOD(struct libinput_event_touch *, GetTouchEvent, (struct libinput_event *));
    MOCK_METHOD(struct libinput_event_gesture *, GetGestureEvent, (struct libinput_event *));
    MOCK_METHOD(uint64_t, TouchEventGetTime, (struct libinput_event_touch *));
    MOCK_METHOD(double, TouchEventGetPressure, (struct libinput_event_touch *));
    MOCK_METHOD(uint32_t, GestureEventGetTime, (struct libinput_event_gesture *));
    MOCK_METHOD(int, GestureEventGetFingerCount, (struct libinput_event_gesture *));
    MOCK_METHOD(int, GestureEventGetDevCoordsX, (struct libinput_event_gesture *, uint32_t));
    MOCK_METHOD(int, GestureEventGetDevCoordsY, (struct libinput_event_gesture *, uint32_t));
};
} // namespace MMI
} // namespace OHOS
#endif // LIBINPUT_MOCK_H
