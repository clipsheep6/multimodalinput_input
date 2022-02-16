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

#ifndef ROTATION_EVENT_H
#define ROTATION_EVENT_H

#include "multimodal_event.h"
#include "nocopyable.h"

namespace OHOS {
/**
 * Defines events generated by rotation components, for example, a digital
 * watch crown on a smart watch.
 * @see MultimodalEvent
 * @since 1
 */
class RotationEvent : public MMI::MultimodalEvent {
public:
    RotationEvent() = default;
    DISALLOW_COPY_AND_MOVE(RotationEvent);
    virtual ~RotationEvent();

    /**
    * initialize the object.
    *
    * @return void
    * @since 1
    */
    void Initialize(int32_t windowId, float rotationValue, int32_t highLevelEvent, const std::string& uuid,
                    int32_t sourceType, int32_t occurredTime, const std::string& deviceId, int32_t inputDeviceId,
                    bool isHighLevelEvent, uint16_t deviceUdevTags = 0);
    /**
    * initialize the object.
    *
    * @return void
    * @since 1
    */
    void Initialize(RotationEvent& rotationEvent);

    /**
     * Obtains the rotation value generated by the rotation component.
     *
     * <p>The value definition may vary depending on the component.
     * For example, this value of some components may indicate the angle of
     * rotation, whereas that of other components may indicate the number
     * of circles that have been rotated.
     * @return Returns the generated rotation value.
     * @since 1
     */
    virtual float GetRotationValue() const;

private:
    float mRotationValue_ = 0;
};
} // namespace OHOS
#endif // ROTATION_EVENT_H