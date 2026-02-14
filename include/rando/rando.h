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
    bool checkValidTransformAnywhere();
    int getBugReward(u8 bugId);
    u8 getSkyCharacterItem();
    u8 getPoeItem(u8 bitSw);
    void handlePoeItem(u8 bitSw);

    bool mInitialized;
    bool transformAnywhere;
    u16 mFrameCounter;
};

extern randoInfo_c g_randoInfo;

#endif  // RANDO_H
