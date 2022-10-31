#pragma once
#include <functional>

namespace Input {
    class AutoHelper {
        public:
            AutoHelper(std::function<void()> task);
            ~AutoHelper();
            void Cancel();

        private:
            std::function<void()> task_;
    };
}
