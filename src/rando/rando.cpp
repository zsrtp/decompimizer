#include "rando/rando.h"
#include "d/d_com_inf_game.h"
#include "SSystem/SComponent/c_math.h"

randoInfo_c g_randoInfo;

int randoInfo_c::_create() {
    mFrameCounter = 0;
    mInitialized = true;
    return 1;
}

int randoInfo_c::_delete() {
    mInitialized = false;
    return 1;
}

int randoInfo_c::execute() {
    if (!mInitialized) {
        return 0;
    }

    // Every 300 frames, set rupees to a random value
    mFrameCounter++;
    if (mFrameCounter >= 300) {
        mFrameCounter = 0;
        u16 randomRupees = (u16)cM_rndF(1000.0f);
        dComIfGs_setRupee(randomRupees);
    }

    return 1;
}

int randoInfo_c::draw() {
    return 1;
}
