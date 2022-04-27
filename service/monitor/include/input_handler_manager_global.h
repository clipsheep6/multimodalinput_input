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

#ifndef INPUT_HANDLER_MANAGER_GLOBAL_H
#define INPUT_HANDLER_MANAGER_GLOBAL_H

#include <mutex>
#include <set>

#include "nocopyable.h"
#include "singleton.h"

#include "i_input_event_handler.h"
#include "input_handler_type.h"
#include "uds_session.h"

namespace OHOS {
namespace MMI {
class InputHandlerManagerGlobal : public Singleton<InputHandlerManagerGlobal> {
public:
    InputHandlerManagerGlobal() = default;
    DISALLOW_COPY_AND_MOVE(InputHandlerManagerGlobal);
    int32_t AddInputHandler(int32_t handlerId, InputHandlerType handlerType, SessionPtr session);
    void RemoveInputHandler(int32_t handlerId, InputHandlerType handlerType, SessionPtr session);
    void MarkConsumed(int32_t handlerId, int32_t eventId, SessionPtr session);
    bool HandleEvent(std::shared_ptr<KeyEvent> KeyEvent);
    bool HandleEvent(std::shared_ptr<PointerEvent> PointerEvent);

private:
    void InitSessionLostCallback();
    void OnSessionLost(SessionPtr session);

private:
    struct SessionHandler {
        SessionHandler(int32_t id, InputHandlerType handlerType, SessionPtr session)
            : id_(id), handlerType_(handlerType), session_(session) { }
        void SendToClient(std::shared_ptr<KeyEvent> keyEvent) const;
        void SendToClient(std::shared_ptr<PointerEvent> pointerEvent) const;
        bool operator<(const SessionHandler& other) const
        {
            if (id_ != other.id_) {
                return (id_ < other.id_);
            }
            if (handlerType_ != other.handlerType_) {
                return (handlerType_ < other.handlerType_);
            }
            return (session_ < other.session_);
        }
        int32_t id_;
        InputHandlerType handlerType_;
        SessionPtr session_ = nullptr;
    };

    struct MonitorCollection : public IInputEventHandler, protected NoCopyable {
        virtual int32_t GetPriority() const override;
        virtual bool HandleEvent(std::shared_ptr<KeyEvent> KeyEvent) override;
        virtual bool HandleEvent(std::shared_ptr<PointerEvent> PointerEvent) override;

        int32_t AddMonitor(const SessionHandler& mon);
        void RemoveMonitor(const SessionHandler& mon);
        void MarkConsumed(int32_t monitorId, int32_t eventId, SessionPtr session);

        bool HasMonitor(int32_t monitorId, SessionPtr session);
        void UpdateConsumptionState(std::shared_ptr<PointerEvent> pointerEvent);
        void Monitor(std::shared_ptr<PointerEvent> pointerEvent);
        void OnSessionLost(SessionPtr session);

        std::set<SessionHandler> monitors_;
        std::shared_ptr<PointerEvent> lastPointerEvent_ = nullptr;
        int32_t downEventId_ { -1 };
        bool isMonitorConsumed_ { false };
    };

private:
    bool sessionLostCallbackInitialized_ { false };
    MonitorCollection monitors_;
};
} // namespace MMI
} // namespace OHOS
#endif // INPUT_HANDLER_MANAGER_GLOBAL_H