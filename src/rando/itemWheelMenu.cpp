#include "rando/itemWheelMenu.h"

customMenuRing_c g_customMenuRing;

int customMenuRing_c::_initialize() {
    setRingOpen(false);
    return 1;
}
