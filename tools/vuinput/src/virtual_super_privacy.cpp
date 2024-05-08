#include "virtual_super_privacy.h"

namespace OHOS {
namespace MMI {

VirtualSuperPrivacy::VirtualSuperPrivacy() : VirtualDevice("Virtual SuperPrivacy", 0x19, 0x1, 0x1)
{
    eventTypes_ = { EV_SW };
    properties_ = { INPUT_PROP_DIRECT };
    switches_ = { 17 };
}
} // namespace MMI
} // namespace OHOS