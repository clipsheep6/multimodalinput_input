#include "Log.h"
#include "InputContext.h"

int main(int argc, char* argv[]) {
    LOG_D("Enter");
    auto ctx = Input::InputContext::CreateInstance();
    ctx->Run();
    LOG_D("Leave");
    return 0;
}
