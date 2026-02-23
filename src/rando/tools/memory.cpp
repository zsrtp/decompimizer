#include "dolphin/os/OSCache.h"
#include "rando/tools/memory.h"

void clear_DC_IC_Cache(void* ptr, uint size)
{
    DCFlushRange(ptr, size);
    ICInvalidateRange(ptr, size);
}
