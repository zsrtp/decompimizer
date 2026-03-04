#ifndef MEMORY_H
#define MEMORY_H

#include "dolphin/types.h"

void clear_DC_IC_Cache(void* ptr, uint size);
void* clearMemory(void* ptr, int size);

#endif  // MEMORY_H
