#include "AutoHelper.h"

namespace Input {
    AutoHelper::AutoHelper(std::function<void()> task)
        : task_(task) {
    }

    AutoHelper::~AutoHelper() {
        task_();
    }

    void AutoHelper::Cancel() {
        task_ = std::function<void()>();
    }
}

