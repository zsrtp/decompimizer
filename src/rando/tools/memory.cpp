#include "dolphin/os/OSCache.h"
#include "rando/tools/memory.h"
#include <string>

void clear_DC_IC_Cache(void* ptr, uint size)
{
    DCFlushRange(ptr, size);
    ICInvalidateRange(ptr, size);
}

void* clearMemory(void* ptr, int size)
{
    return memset(ptr, 0, size);
}
