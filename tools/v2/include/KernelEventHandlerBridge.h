#pragma once

#include "IKernelEventHandler.h"
#include "ISeat.h"
#include "ITouchScreenSeat.h"

namespace Input {

    class KernelEventHandlerBridge : public IKernelEventHandler 
    {
        public:
            static std::shared_ptr<IKernelEventHandler> CreateInstance(const std::shared_ptr<ISeat>& seat, 
                    const std::shared_ptr<ITouchScreenSeat>& touchScreenSeat);
         public:
            virtual void OnInputEvent(const std::shared_ptr<const RelEvent>& event) override;

            virtual void OnInputEvent(const std::shared_ptr<const KernelKeyEvent>& event) override ;

            virtual void OnInputEvent(const std::shared_ptr<const AbsEvent>& event) override;
        private:
            KernelEventHandlerBridge(const std::shared_ptr<ISeat>& seat, 
                    const std::shared_ptr<ITouchScreenSeat>& touchScreenSeat);
        private:
            std::shared_ptr<ISeat> seat_;
            std::shared_ptr<ITouchScreenSeat> touchScreenSeat_;
    };
}

