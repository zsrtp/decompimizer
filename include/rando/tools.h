#ifndef TOOLS_H
#define TOOLS_H

#include "dolphin/types.h"

bool playerIsInRoomStage(s32 room, const char* stage);
void checkTransformFromWolf();
u8 setNextWarashibeItem();
void offWarashibeItem(u8 item);

#endif  // TOOLS_H
