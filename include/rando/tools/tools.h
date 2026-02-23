#ifndef TOOLS_H
#define TOOLS_H

#include "dolphin/types.h"
#include "SSystem/SComponent/c_xyz.h"
#include "SSystem/SComponent/c_sxyz.h"

bool playerIsInRoomStage(s32 room, const char* stage);
void checkTransformFromWolf();
u8 setNextWarashibeItem();
void offWarashibeItem(u8 item);
int initCreatePlayerItem(uint item, uint flag, const cXyz* pos, int roomNo, const csXyz* angle, const cXyz* scale);
bool checkButtonComboAnalog(uint combo);
void handleQuickTransform();
int readFile(const char* file, bool allocFromHead, u8** dataOut);

#endif  // TOOLS_H
