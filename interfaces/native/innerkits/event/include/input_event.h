/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

#include <functional>
#include <memory>
#include <mutex>

#include "nocopyable.h"
#include "parcel.h"

namespace OHOS {
namespace MMI {
class InputEvent {
public:
    /**
     * Unknown action. It is usually used as the initial value.
     *
     * @since 8
     */
    static constexpr int32_t ACTION_UNKNOWN = 0;

    /**
     * Cancel the action. It indicates that a consecutive input event is cancelled.
     *
     * @since 8
     */
    static constexpr int32_t ACTION_CANCEL = 1;

    /**
     * The actual type of the current input event is the basic type (InputEvent type).
     *
     * @since 8
     */
    static constexpr int32_t EVENT_TYPE_BASE = 0X00000000;

    /**
     * The actual type of the current input event is KeyEvent or its derived class.
     *
     * @since 8
     */
    static constexpr int32_t EVENT_TYPE_KEY = 0X00010000;

    /**
     * The actual type of the current input event is PointerEvent or its derived class.
     *
     * @since 8
     */
    static constexpr int32_t EVENT_TYPE_POINTER = 0X00020000;

    /**
     * The actual type of the current input event is AxisEvent or its derived class.
     *
     * @since 8
     */
    static constexpr int32_t EVENT_TYPE_AXIS = 0X00030000;

    /**
     * The multimodal input service sends input events to the interceptor and listener. This is the default value.
     *
     * @since 8
     */
    static constexpr uint32_t EVENT_FLAG_NONE = 0x00000000;

    /**
     * The multimodal input service does not intercept the input event.
     *
     * @since 8
     */
    static constexpr uint32_t EVENT_FLAG_NO_INTERCEPT = 0x00000001;

    /**
     * The multimodal input service does not listen for the input event.
     *
     * @since 8
     */
    static constexpr uint32_t EVENT_FLAG_NO_MONITOR = 0x00000002;

public:
    /**
     * Copy constructor function for InputEvent
     *
     * @since 8
     */
    InputEvent(const InputEvent& other);

    /**
     * Virtual destructor of InputEvent
     *
     * @since 8
     */
    virtual ~InputEvent();

    virtual InputEvent& operator=(const InputEvent& other) = delete;
    DISALLOW_MOVE(InputEvent);

    /**
     * Create InputEvent object
     *
     * @since 8
     */
    static std::shared_ptr<InputEvent> Create();

    /**
     * @brief Converts an input event type into a string.
     * @param Indicates the input event type.
     * @return Returns the string converted from the input event type.
     * @since 8
     */
    static const char* EventTypeToString(int32_t eventType);

   /**
    * @brief Resets an input event to the initial state.
    * @return void
    * @since 8
    */
    void Reset();

    /**
     * @brief Obtains the unique ID of an input event.
     * @return Returns the unique ID of the input event.
     * @since 8
     */
    int32_t GetId() const;

    /**
     * @brief Sets the unique ID of an input event.
     * @param id Indicates the unique ID.
     * @return void
     * @since 8
     */
    void SetId(int32_t id);

    /**
     * @brief Obtains the time when the action for this input event occurs.
     * @return Returns the time when the action for this input event occurs.
     * @since 8
     */
    int64_t GetActionTime() const;

    /**
     * @brief Sets the time when the action for this input event occurs.
     * @param actionTime Indicates the time when the action for this input event occurs.
     * @return void
     * @since 8
     */
    void SetActionTime(int64_t actionTime);

    /**
     * @brief Obtains the action for this input event.
     * @return Returns the action for this input event.
     * @since 8
     */
    int32_t GetAction() const;

    /**
     * @brief Sets the action for this input event.
     * @param action Indicates the action for the input event.
     * @return void
     * @since 8
     */
    void SetAction(int32_t action);

    /**
     * @brief Obtains the time when the action for the first input event in a series of related input events occurs.
     * @return Returns the time when the action for the first input event occurs.
     * @since 8
     */
    int64_t GetActionStartTime() const;

    /**
     * @brief Sets the time when the action for the first input event in a series of related input events occurs.
     * @param time Indicates the time when the action for the first input event occurs.
     * @return void
     * @since 8
     */
    void SetActionStartTime(int64_t time);

    /**
     * @brief Obtains the unique ID of the device that generates this input event.
     * @return Returns the unique ID of the device.
     * @since 8
     */
    int32_t GetDeviceId() const;

    /**
     * @brief Sets the unique ID of the device that generates this input event.
     * @param deviceId Indicates the unique ID of the device.
     * @return void
     * @since 8
     */
    void SetDeviceId(int32_t deviceId);

    /**
     * @brief Obtains the ID of the target display for an input event.
     * @return Returns the ID of the target display.
     * @since 8
     */
    int32_t GetTargetDisplayId() const;

    /**
     * @brief Sets the ID of the target screen for an input event.
     * @param displayId Indicates the ID of the target display.
     * @return void
     * @since 8
     */
    void SetTargetDisplayId(int32_t displayId);

    /**
     * @brief Obtains the ID of the target window for an input event.
     * @return Returns the ID of the target window.
     * @since 8
     */
    int32_t GetTargetWindowId() const;

    /**
     * @brief Sets the ID of the target window for an input event.
     * @param windowId Indicates the ID of the target window.
     * @return void
     * @since 8
     */
    void SetTargetWindowId(int32_t windowId);

    /**
     * @brief Obtains the ID of the agent window for an input event.
     * @return Returns the ID of the agent window.
     * @since 8
     */
    int32_t GetAgentWindowId() const;

    /**
     * @brief Sets the ID of the agent window for an input event.
     * @param windowId Indicates the ID of the agent window.
     * @return void
     * @since 8
     */
    void SetAgentWindowId(int32_t windowId);

    /**
     * @brief Obtains the type of the this input event.
     * @return Returns the type of the this input event.
     * @since 8
     */
    int32_t GetEventType() const;

    /**
     * @brief Obtains all flags of an input event.
     * @return Returns the flags of the input event.
     * @since 8
     */
    uint32_t GetFlag() const;

    /**
     * @brief Checks whether a flag has been set for an input event.
     * @param flag Indicates the flag of the input event.
     * @return Returns <b>true</b> if a flag has been set; returns <b>false</b> otherwise.
     * @since 8
     */
    bool HasFlag(uint32_t flag);

    /**
     * @brief Adds a flag for an input event.
     * @param flag Indicates the flag of the input event.
     * @return void
     * @since 8
     */
    void AddFlag(uint32_t flag);

    /**
     * @brief Clears all flags of an input event.
     * @return void
     * @since 8
     */
    void ClearFlag();

    /**
     * @brief Assigns a new ID for an input event.
     * @return void
     * @since 8
     */
    void AssignNewId();

    /**
     * @brief Marks an input event as completed.
     * This method can only be called once.
     * @return void
     * @since 8
     */
    void MarkProcessed();

    /**
     * @brief Sets the callback invoked when an input event is processed.
     * This method is not available for third-party applications.
     * @return void
     * @since 8
     */
    void SetProcessedCallback(std::function<void(int32_t)> callback);

public:
    /**
     * @brief Writes data to a <b>Parcel</b> obejct.
     * @param out Indicates the object into which data will be written.
     * @return Returns <b>true</b> if the data is successfully written; returns <b>false</b> otherwise.
     * @since 8
     */
    bool WriteToParcel(Parcel &out) const;

    /**
     * @brief Reads data from a <b>Parcel</b> obejct.
     * @param in Indicates the object from which data will be read.
     * @return Returns <b>true</b> if the data is successfully read; returns <b>false</b> otherwise.
     * @since 8
     */
    bool ReadFromParcel(Parcel &in);

protected:
    /**
     * @brief Constructs an input event object by using the specified input event type. Generally, this method 
     * is used to construct a base class object when constructing a derived class object.
     * @since 8
     */
    explicit InputEvent(int32_t eventType);

private:
    int32_t eventType_;
    int32_t id_;
    int64_t actionTime_;
    int32_t action_;
    int64_t actionStartTime_;
    int32_t deviceId_;
    int32_t targetDisplayId_;
    int32_t targetWindowId_;
    int32_t agentWindowId_;
    uint32_t bitwise_;
    std::function<void(int32_t)> processedCallback_;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_EVENT_H
