#pragma once

#include "IInputDefine.h"

namespace Input {

    class IInputContext;
    class Robot : public NonCopyable {
        public:
            Robot(const IInputContext* context);

            int32_t Start();
            int32_t Stop();

        private:
            int32_t AddDefaultDisplay();

        private:
            const IInputContext* const context_;

    };
}
