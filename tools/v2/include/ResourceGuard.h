#pragma once

#include <iostream>

#include "IInputDefine.h"

namespace Input {

    template <typename T>
    class ResourceGuard : public NonCopyable {
        public:
            ResourceGuard(T& res, std::function<void(T&)> cleaner)
                : res_(res), cleaner_(cleaner) {
                }
            virtual ~ResourceGuard() {
                if (cleaner_) {
                    cleaner_(res_);
                }
            }

            T& Pop() {
                cleaner_ = std::function<void(T&)>();
                return res_;
            }

        private:
            T& res_;
            std::function<void(T&)> cleaner_;
    };
}
