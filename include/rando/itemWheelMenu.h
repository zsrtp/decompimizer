#ifndef ITEM_WHEEL_MENU_H
#define ITEM_WHEEL_MENU_H

#include "dolphin/types.h"

class customMenuRing_c {
public:

    int _initialize();
    int _delete();
    int execute();
    int draw();
    bool isRingOpen() {return mRingOpen;}
    void setRingOpen(bool val) {mRingOpen = val;}

    bool mRingOpen;
};

extern customMenuRing_c g_customMenuRing;

#endif  // RANDO_H
