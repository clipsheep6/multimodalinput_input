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

#ifndef OHOS_MULTIMODAL_EVENTS_H
#define OHOS_MULTIMODAL_EVENTS_H

#include <string>
#include "refbase.h"

namespace OHOS {
namespace MMI {
enum SourceDevice {
    /**
    * Indicates that there is no input device. It is usually used as the default value.
    *
    * @since 1
    */
    UNSUPPORTED_DEVICE = -1,

    /**
    * Indicates a touch panel (TP) input event.
    *
    * @since 1
    */
    TOUCH_PANEL = 0,

    /**
    * Indicates a keyboard input event.
    *
    * @since 1
    */
    KEYBOARD = 1,

    /**
    * Indicates a mouse input event.
    *
    * @since 1
    */
    MOUSE = 2,

    /**
    * Indicates a stylus input event.
    *
    * @since 1
    */
    STYLUS = 3,

    /**
    * Indicates a builtin key input event.
    *
    * @since 1
    */
    BUILTIN_KEY = 4,

    /**
    * Indicates the event generated by a rotation component.
    *
    * @since 3
    */
    ROTATION = 5,

    /**
    * Indicates the event generated by Speech component.
    *
    * @since 3
    */
    SPEECH = 6,

    /**
    * Indicates a joystick input event.
    *
    * @since 3
    */
    JOYSTICK = 7,

    /**
    * Indicates a tablet_pad input event.
    *
    * @since 3
    */
    TOUCH_PAD = 8,

    /**
    * Indicates a knuckle input event.
    *
    * @since 3
    */
    KNUCKLE = 9,

    /**
    * Indicates a track ball input event.
    *
    * @since 3
    */
    TRACKBALL = 10
};

enum HighLevel {
    /**
    * Indicates the default value of the standard high-level event supported by the OS system.
    *
    * @since 1
    */
    DEFAULT_TYPE = -1
};

enum AbstractEvent {
    /**
    * Indicates the mute key.
    * <p>The mute key is used to mute or unmute a device.
    *
    * @hide
    * @since 1
    */
    MUTE = 91,

    /**
    * Indicates an abstract up motion event, which concerns the user intent, rather than the
    * actual input.
    *
    * @hide
    * @since 1
    */
    NAVIGATION_UP = 280,

    /**
    * Indicates an abstract down motion event, which concerns the user intent, rather than the
    * actual input.
    *
    * @hide
    * @since 1
    */
    NAVIGATION_DOWN = 281,

    /**
    * Indicates an abstract left motion event, which concerns the user intent, rather than the
    * actual input.
    *
    * @hide
    * @since 1
    */
    NAVIGATION_LEFT = 282,

    /**
    * Indicates an abstract right motion event, which concerns the user intent, rather than the
    * actual input.
    *
    * @hide
    * @since 1
    */
    NAVIGATION_RIGHT = 283
};

enum DayNightMode {
    /**
    * Indicates the day mode.
    *
    * @hide
    * @since 1
    */
    DAY_MODE = 5,

    /**
    * Indicates the night mode.
    *
    * @hide
    * @since 1
    */
    NIGHT_MODE = 4
};

enum MultimodalEventType {
    NONE_EVENT = 0,
    MULTIMODAL_EVENT = 1,
    KEY_EVENT = 2,
    BUILTIN_KEY_EVENT = 3,
    KEYBOARD_EVENT = 4,
    MANIPULATION_EVENT = 5,
    COMPOSITE_EVENT = 6,
    TOUCH_EVENT = 7,
    STYLUS_EVENT = 8,
    MOUSE_EVENT = 9,
    ROTATION_EVENT = 10,
    SPEECH_EVENT = 11
};

class MultimodalEvent : public RefBase {
public:
    virtual ~MultimodalEvent();
    /**
    * initialize the object.
    *
    * @return void
    * @since 1
    */
    void Initialize(int32_t windowId, int32_t highLevelEvent, const std::string& uuid, int32_t sourceType,
                    uint64_t occurredTime, const std::string& deviceId, int32_t inputDeviceId,  bool isHighLevelEvent,
                    uint16_t deviceUdevTags = 0, bool isIntercepted = true);

    /**
    * initialize the object.
    *
    * @return void
    * @since 1
    */
    void Initialize(const MultimodalEvent& multimodalEvent);

    /**
     * Checks whether the current event is the same as the event with the
     * specified UUID.
     * @param id Specifies UUID of the event to be checked.
     * @return Returns {@code true} if the current event is the same as
     * the event with the specified UUID; returns {@code false} otherwise.
     * @since 1
     */
    bool IsSameEvent(const std::string& id);

    /**
    * Checks whether a high-level event can be generated with the current event.
    *
    * @return Returns {@code true} if a high-level event can be generated with
    * the current event; returns {@code false} otherwise.
    * @hide
    * @since 1
    */
    bool IsHighLevelInput() const;

    /**
    * Obtains the high-level event generated with the current event. The event
    * type is mainly used for triggering of a callback.
    *
    * @return Returns a high-level event if one has been generated; returns
    * {@link #DEFAULT_TYPE} otherwise.
    * @hide
    * @since 1
    */
    int32_t GetHighLevelEvent() const;

    /**
     * Obtains the type of the input device that generates the current event.
     *
     * @return Returns the type of the input device that generates the event.
     * The return values are as follows:{@link #UNSUPPORTED_DEVICE}:no input device.
     * Generally,this is the default value.
     * {@link #TOUCH_PANEL}: touch panel
     * {@link #KEYBOARD}: keyboard
     * {@link #MOUSE}: mouse
     * {@link #STYLUS}: stylus
     * {@link #BUILTIN_KEY}: built-in key
     * {@link #ROTATION}: rotation component
     * {@link #SPEECH}: speech component
     * @since 3
    */
    int32_t GetSourceDevice() const;

    /**
    * Obtains the ID of the bearing device for the input device that
    * generates the current event.
    * <p>For example, if two mouse devices are connected to the same device,
    * this device is the bearing device of the two mouse devices.
    *
    * @return Returns the ID of the bearing device for the input device that
    * generates the current event; returns {@code null} if there is no
    * input device.
    * @see #getInputDeviceId()
    * @since 1
    */
    std::string GetDeviceId() const;

    /**
     * Obtains the ID of the input device that generates the current event.
     *
     * <p>An input device is identified by a unique ID. For example, when
     * two mouse devices generate an event respectively, the device ID in
     * the generated events are different. This allows your application to
     * use the device ID to identify the actual input device.
     * @return Returns the ID of the input device that generates the
     * current event; returns{@code -1} if there is no input device.
     * @see #getDeviceId()
     * @since 1
     */
    int32_t GetInputDeviceId() const;

    /**
     * Obtains the time when the current event is generated.
     *
     * @return Returns the time (in ms) when the current event is generated.
     * @since 1
     */
    uint64_t GetOccurredTime() const;

    /**
     * Obtains the device tags.
     *
     * @return Returns the device tags when the current event is generated.
     * @since 1
     */
    uint16_t GetDeviceUdevTags() const;

    int32_t GetEventType() const;

    int32_t GetWindowID() const;

    std::string GetUuid() const;

    bool IsIntercepted() const;
    bool marshalling();
    bool unmarshalling();
private:
    int32_t DeviceTypeTransform(int32_t sourceType) const;

private:
    int32_t mWindowId_ = 0;
    std::string mUuid_ = "";
    std::string mDeviceId_ = "";
    int32_t mHighLevelEvent_ = 0;
    int32_t mSourceDevice_ = 0;
    uint64_t mOccurredTime_ = 0;
    int32_t mInputDeviceId_ = 0;
    bool mIsHighLevelEvent_ = false;
    uint16_t mDeviceUdevTags_ = 0;
    int32_t mEventType_ = 0;
    bool isIntercepted_ = true;
};
using MultimodalEventPtr = sptr<MultimodalEvent>;
}
}
#endif
