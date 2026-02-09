#ifndef RANDO_H
#define RANDO_H

#include "dolphin/types.h"

class randoInfo_c {
public:
    randoInfo_c() { mInitialized = false; }

    int _create();
    int _delete();
    int execute();
    int draw();

    bool mInitialized;
    u16 mFrameCounter;
};

extern randoInfo_c g_randoInfo;

#endif  // RANDO_H
