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
#ifndef KEY_EVENT_SUBSCRIBER_H
#define KEY_EVENT_SUBSCRIBER_H

#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <thread>

#include "i_input_event_handler.h"
#include "key_event.h"
#include "key_option.h"
#include "uds_server.h"

namespace OHOS {
namespace MMI {
class KeyEventSubscriber :  public IInputEventHandler {
public:
    KeyEventSubscriber() = default;
    ~KeyEventSubscriber() = default;
    DISALLOW_COPY_AND_MOVE(KeyEventSubscriber);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    void HandleKeyEvent(std::shared_ptr<KeyEvent> keyEvent) override;
#endif // OHOS_BUILD_ENABLE_KEYBOARD
#ifdef OHOS_BUILD_ENABLE_POINTER
    void HandlePointerEvent(std::shared_ptr<PointerEvent> pointerEvent) override;
#endif // OHOS_BUILD_ENABLE_POINTER
#ifdef OHOS_BUILD_ENABLE_TOUCH
    void HandleTouchEvent(std::shared_ptr<PointerEvent> pointerEvent) override;
#endif // OHOS_BUILD_ENABLE_TOUCH
    int32_t SubscribeKeyEvent(SessionPtr sess, int32_t subscribeId,
            const std::shared_ptr<KeyOption> keyOption);
    int32_t UnSubscribeKeyEvent(SessionPtr sess, int32_t subscribeId);
    bool OnSubscribeKeyEvent(std::shared_ptr<KeyEvent> keyEvent);

private:
    struct Subscriber {
        Subscriber(int32_t id, SessionPtr sess, std::shared_ptr<KeyOption> keyOption)
            : id_(id), sess_(sess), keyOption_(keyOption), timerId_(-1)
        {
        }
        int32_t id_ { -1 };
        SessionPtr sess_ { nullptr };
        std::shared_ptr<KeyOption> keyOption_ { nullptr };
        int32_t timerId_ { -1 };
        std::shared_ptr<KeyEvent> keyEvent_ { nullptr };
    };
    void InsertSubScriber(std::shared_ptr<Subscriber> subs);

private:
    bool HandleKeyDown(const std::shared_ptr<KeyEvent>& keyEvent);
    bool HandleKeyUp(const std::shared_ptr<KeyEvent>& keyEvent);
    bool HandleKeyCanel(const std::shared_ptr<KeyEvent>& keyEvent);

    bool IsPreKeysMatch(const std::set<int32_t>& preKeys, const std::vector<int32_t>& pressedKeys) const;

    void NotifySubscriber(std::shared_ptr<KeyEvent> keyEvent,
            const std::shared_ptr<Subscriber>& subscriber);

    bool AddTimer(const std::shared_ptr<Subscriber>& subscriber, const std::shared_ptr<KeyEvent>& keyEvent);
    void ClearTimer(const std::shared_ptr<Subscriber>& subscriber);
    void OnTimer(const std::shared_ptr<Subscriber> subscriber);
    void OnSessionDelete(SessionPtr sess);
    bool InitSessionDeleteCallback();

    bool CloneKeyEvent(std::shared_ptr<KeyEvent> keyEvent);

    void RemoveKeyCode(int32_t keyCode, std::vector<int32_t>& keyCodes);

private:
    std::list<std::shared_ptr<Subscriber>> subscribers_ {};
    bool callbackInitialized_ { false };
    std::shared_ptr<KeyEvent> keyEvent_ { nullptr };
};
} // namespace MMI
} // namespace OHOS
#endif  // KEY_EVENT_SUBSCRIBER_H
