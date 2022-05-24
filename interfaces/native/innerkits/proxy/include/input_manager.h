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

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <map>
#include <memory>
#include <list>

#include "event_handler.h"
#include "nocopyable.h"

#include "display_info.h"
#include "error_multimodal.h"
#include "i_input_event_consumer.h"
#include "key_option.h"

namespace OHOS {
namespace MMI {
class InputManager {
public:
    /**
     * @brief Obtains an <b>InputManager</b> instance.
     * @return Returns the pointer to the <b>InputManager</b> instance.
     * @since 9
     */
    static InputManager *GetInstance();
    virtual ~InputManager() = default;

    /**
     * @brief Updates the screen and window information.
     * @param physicalDisplays Indicates the physical screen information.
     * @param logicalDisplays Indicates the logical screen information, which includes the window information.
     * @since 9
     */
    void UpdateDisplayInfo(const std::vector<PhysicalDisplayInfo> &physicalDisplays,
        const std::vector<LogicalDisplayInfo> &logicalDisplays);
        
    /**
     * @brief Sets a globally unique input event filter.
     * @param filter Indicates the input event filter to set. When an input event occurs, this filter is
     * called and returns a value indicating whether to continue processing the input event.If the filter
     * returns <b>true</b>, the processing of the input event ends. If the filter returns <b>false</b>,
     * the processing of the input event continues.
     * @return return Returns a value greater than or equal to <b>0</b> if the input event filter is added
     * successfully; returns a value less than <b>0</b> otherwise.
     * @since 9
     */
    int32_t AddInputEventFilter(std::function<bool(std::shared_ptr<PointerEvent>)> filter);

    /**
     * @brief Sets a consumer for the window input event of the current process.
     * @param inputEventConsumer Indicates the consumer to set. The window input event of the current process
     * will be called back to the consumer object for processing.
     * @since 9
     */
    void SetWindowInputEventConsumer(std::shared_ptr<IInputEventConsumer> inputEventConsumer);

    /**
     * @brief Sets a window input event consumer that runs on the specified thread.
     * @param inputEventConsumer Indicates the consumer to set.
     * @param eventHandler Indicates the thread running the consumer.
     * @since 9
     */
    void SetWindowInputEventConsumer(std::shared_ptr<IInputEventConsumer> inputEventConsumer,
        std::shared_ptr<AppExecFwk::EventHandler> eventHandler);

    /**
     * @brief Subscribes to the key input event that meets a specific condition. When such an event occurs,
     * the <b>callback</b> specified is invoked to process the event.
     * @param keyOption Indicates the condition of the key input event.
     * @param callback Indicates the callback.
     * @return Returns the subscription ID, which uniquely identifies a subscription in the process.
     * If the value is greater than or equal to <b>0</b>,
     * the subscription is successful. Otherwise, the subscription fails.
     * @since 9
     */
    int32_t SubscribeKeyEvent(std::shared_ptr<KeyOption> keyOption,
        std::function<void(std::shared_ptr<KeyEvent>)> callback);

    /**
     * @brief Unsubscribes from a key input event.
     * @param subscriberId Indicates the subscription ID, which is the return value of <b>SubscribeKeyEvent</b>.
     * @return void
     * @since 9
     */
    void UnsubscribeKeyEvent(int32_t subscriberId);

    /**
     * @brief Adds an input event monitor. After such a monitor is added,
     * an input event is copied and distributed to the monitor while being distributed to the original target.
     * @param monitor Indicates the input event monitor. After an input event is generated,
     * the functions of the monitor object will be called.
     * @return Returns the monitor ID, which uniquely identifies a monitor in the process.
     * If the value is greater than or equal to <b>0</b>, the monitor is successfully added. Otherwise,
     * the monitor fails to be added.
     * @since 9
     */
    int32_t AddMonitor(std::function<void(std::shared_ptr<KeyEvent>)> monitor);

    /**
     * @brief Adds an input event monitor. After such a monitor is added,
     * an input event is copied and distributed to the monitor while being distributed to the original target.
     * @param monitor Indicates the input event monitor. After an input event is generated,
     * the functions of the monitor object will be called.
     * @return Returns the monitor ID, which uniquely identifies a monitor in the process.
     * If the value is greater than or equal to <b>0</b>, the monitor is successfully added. Otherwise,
     * the monitor fails to be added.
     * @since 9
     */
    int32_t AddMonitor(std::function<void(std::shared_ptr<PointerEvent>)> monitor);

    /**
     * @brief Adds an input event monitor. After such a monitor is added,
     * an input event is copied and distributed to the monitor while being distributed to the original target.
     * @param monitor Indicates the input event monitor. After an input event is generated,
     * the functions of the monitor object will be called.
     * @return Returns the monitor ID, which uniquely identifies a monitor in the process.
     * If the value is greater than or equal to <b>0</b>, the monitor is successfully added. Otherwise,
     * the monitor fails to be added.
     * @since 9
     */
    int32_t AddMonitor(std::shared_ptr<IInputEventConsumer> monitor);

    /**
     * @brief Removes a monitor.
     * @param monitorId Indicates the monitor ID, which is the return value of <b>AddMonitor</b>.
     * @return void
     * @since 9
     */
    void RemoveMonitor(int32_t monitorId);

    /**
     * @brief Marks that a monitor has consumed a touchscreen input event. After being consumed,
     * the touchscreen input event will not be distributed to the original target.
     * @param monitorId Indicates the monitor ID.
     * @param eventId Indicates the ID of the consumed touchscreen input event.
     * @return void
     * @since 9
     */
    void MarkConsumed(int32_t monitorId, int32_t eventId);

    /**
     * @brief Moves the cursor to the specified position.
     * @param offsetX Indicates the offset on the X axis.
     * @param offsetY Indicates the offset on the Y axis.
     * @return void
     * @since 9
     */
    void MoveMouse(int32_t offsetX, int32_t offsetY);

    /**
     * @brief Adds an input event interceptor. After such an interceptor is added,
     * an input event will be distributed to the interceptor instead of the original target and monitor.
     * @param interceptor Indicates the input event interceptor. After an input event is generated,
     * the functions of the interceptor object will be called.
     * @return Returns the interceptor ID, which uniquely identifies an interceptor in the process.
     * If the value is greater than or equal to <b>0</b>,the interceptor is successfully added. Otherwise,
     * the interceptor fails to be added.
     * @since 9
     */
    int32_t AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptor);
    int32_t AddInterceptor(std::function<void(std::shared_ptr<KeyEvent>)> interceptor);

    /**
     * @brief Removes an interceptor.
     * @param interceptorId Indicates the interceptor ID, which is the return value of <b>AddInterceptor</b>.
     * @return void
     * @since 9
     */
    void RemoveInterceptor(int32_t interceptorId);

    /**
     * @brief Simulates a key input event. This event will be distributed and
     * processed in the same way as the event reported by the input device.
     * @param keyEvent Indicates the key input event to simulate.
     * @return void
     * @since 9
     */
    void SimulateInputEvent(std::shared_ptr<KeyEvent> keyEvent);

    /**
     * @brief Simulates a touchpad input event, touchscreen input event, or mouse device input event.
     * This event will be distributed and processed in the same way as the event reported by the input device.
     * @param pointerEvent Indicates the touchpad input event, touchscreen input event,
     * or mouse device input event to simulate.
     * @return void
     * @since 9
     */
    void SimulateInputEvent(std::shared_ptr<PointerEvent> pointerEvent);

    /**
     * @brief Checks whether the specified key codes of an input device are supported.
     * @param deviceId ID of the input device.
     * @param keyCodes Key codes of the input device.
     * @return Returns a result indicating whether the specified key codes are supported.
     * @since 9
     */
    void SupportKeys(int32_t deviceId, std::vector<int32_t> keyCodes,
        std::function<void(std::vector<bool>&)> callback);

    /**
     * @brief Sets whether the pointer icon is visible.
     * @param visible Whether the pointer icon is visible. The value <b>true</b> indicates that
     * the pointer icon is visible, and the value <b>false</b> indicates the opposite.
     * @return void
     * @since 9
     */
    int32_t SetPointerVisible(bool visible);

    /**
     * @brief Checks whether the pointer icon is visible.
     * @return Returns <b>true</b> if the pointer icon is visible; returns <b>false</b> otherwise.
     * @since 9
     */
    bool IsPointerVisible();
#ifdef OHOS_BUILD_KEY_MOUSE
    /**
     * @brief 设置鼠标光标的位置.
     * @param x x 坐标
     * @param y y 坐标
     * @return 如果设置成功，则返回大于或等于 <b>0</b> 的值
     * 否则返回小于 <b>0</b> 的值
     */
    int32_t SetPointerLocation(int32_t x, int32_t y);
    /**
     * @brief 取得远端输入能力.
     * @param deviceId 远端的deviceId
     * @param remoteTypes 返回远端输入能力
     * @return 如果取得成功，则返回大于或等于 <b>0</b> 的值
     * 否则返回小于 <b>0</b> 的值
     * @since 9
     */
    int32_t GetRemoteInputAbility(std::string deviceId, std::function<void(std::set<int32_t>)> remoteTypes);

    /**
     * @brief 准备分布式.
     * @param deviceId 准备分布式的那台设备的ID
     * @param callback 准备分布式的回调，如果准备分布式执行完了，此回调被调用
     * 如果准备分布式成功，则返回大于或等于 <b>0</b> 的值
     * 否则返回小于 <b>0</b> 的值。
     * @return 如果准备分布式被成功调用，则返回大于或等于 <b>0</b> 的值
     * 否则返回小于 <b>0</b> 的值
     */
    int32_t PrepareRemoteInput(const std::string& deviceId, std::function<void(int32_t)> callback);

    /**
     * @brief 取消准备分布式.
     * @param deviceId 取消准备分布式的那台设备的ID
     * @param callback 取消准备分布式的回调，如果取消准备分布式执行完了，此回调被调用
     * 如果取消准备分布式成功，则返回大于或等于 <b>0</b> 的值
     * 否则返回小于 <b>0</b> 的值。
     * @return 如果取消准备分布式被成功调用，则返回大于或等于 <b>0</b> 的值
     * 否则返回小于 <b>0</b> 的值
     */
    int32_t UnprepareRemoteInput(const std::string& deviceId, std::function<void(int32_t)> callback);

    /**
     * @brief 开始分布式.
     * @param deviceId 开始分布式的那台设备的ID
     * @param callback 开始分布式的回调，如果开始分布式执行完了，此回调被调用
     * 如果开始分布式成功，则返回大于或等于 <b>0</b> 的值
     * 否则返回小于 <b>0</b> 的值。
     * @return 如果取消准备分布式被成功调用，则返回大于或等于 <b>0</b> 的值
     * 否则返回小于 <b>0</b> 的值
     */
    int32_t StartRemoteInput(const std::string& deviceId, uint32_t inputAbility, std::function<void(int32_t)> callback);

    /**
     * @brief 取消分布式.
     * @param deviceId 取消分布式的那台设备的ID
     * @param callback 取消分布式的回调，如果取消分布式执行完了，此回调被调用
     * 如果取消分布式成功，则返回大于或等于 <b>0</b> 的值
     * 否则返回小于 <b>0</b> 的值。
     * @return 如果取消分布式被成功调用，则返回大于或等于 <b>0</b> 的值
     * 否则返回小于 <b>0</b> 的值
     */
    int32_t StopRemoteInput(const std::string& deviceId, uint32_t inputAbility, std::function<void(int32_t)> callback);
#endif

private:
    InputManager() = default;
    DISALLOW_COPY_AND_MOVE(InputManager);
    static InputManager *instance_;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_MANAGER_H