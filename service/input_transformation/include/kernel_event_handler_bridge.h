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

#ifndef KERNEL_EVENT_HANDLER_BRIDGE_H
#define KERNEL_EVENT_HANDLER_BRIDGE_H

#include "i_kernel_event_handler.h"
#include "i_seat.h"
#include "i_touch_screen_seat.h"

namespace OHOS {
namespace MMI {
    class KernelEventHandlerBridge : public IKernelEventHandler 
    {
        public:
            static std::shared_ptr<IKernelEventHandler> CreateInstance(const std::shared_ptr<ISeat>& seat, 
                    const std::shared_ptr<ITouchScreenSeat>& touchScreenSeat);
         public:
            // virtual void OnInputEvent(const std::shared_ptr<const RelEvent>& event) override;

            // virtual void OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event) override ;

            virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) override;
        private:
            KernelEventHandlerBridge(const std::shared_ptr<ISeat>& seat, 
                    const std::shared_ptr<ITouchScreenSeat>& touchScreenSeat);
        private:
            std::shared_ptr<ISeat> seat_;
            std::shared_ptr<ITouchScreenSeat> touchScreenSeat_;
    };
} // namespace MMI
} // namespace OHOS
#endif // KERNEL_EVENT_HANDLER_BRIDGE_H