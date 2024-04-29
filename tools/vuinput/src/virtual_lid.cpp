#include "virtual_lid.h"

namespace OHOS {
namespace MMI {

VirtualLid::VirtualLid() : VirtualDevice("Virtual Lid", 0x19, 0x1, 0x1)
{
    eventTypes_ = { EV_SW };
    properties_ = { INPUT_PROP_DIRECT };
    switches_ = { 0 };
}
} // namespace MMI
} // namespace OHOS