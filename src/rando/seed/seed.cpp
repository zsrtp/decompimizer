#include "rando/seed/seed.h"
#include "rando/tools/tools.h"

seedInfo_c g_seedInfo;

int seedInfo_c::_create() {
    u8* data;
    // Allocate the memory to the back of the heap to avoid possible fragmentation
    const int fileSize = readFile("/mod/seed.bin", false, &data);

    // Make sure the file was successfully read
    if (fileSize <= 0)
    {
        return 0;
    }

    // Get the header data
    // Align to uint as that's the largest variable type in the header class.
    const seedHeaderInfo_c* headerPtr = new seedHeaderInfo_c(data);

    if (!headerPtr->magicIsValid())
    {
        delete[] data;
        delete[] headerPtr;
        return 0;
    }

    // Seed should be valid, so assign the header ptr.
    m_Header = headerPtr;

    // Get the main seed data. align to 0x10 for safety
    const uint dataSize = headerPtr->getDataSize();
    u8* gciDataPtr = new u8[dataSize];
    m_GCIData = gciDataPtr;

    memcpy((u8*)gciDataPtr, &data[headerPtr->getHeaderSize()], dataSize);

    delete[] data;

    return 1;
}

bool flagIsEnabled(const uint* bitfieldPtr, uint totalFlags, uint flag)
{
    // Make sure the flag is valid
    if (flag >= totalFlags)
    {
        return false;
    }

    uint bitsPerWord = sizeof(uint) * 8;
    return (bitfieldPtr[flag / bitsPerWord] >> (flag % bitsPerWord)) & 1U;
}

bool seedInfo_c::flagBitfieldFlagIsEnabled(uint flag) const
{
    const EntryInfo* flagBitfieldPtr = this->getHeaderPtr()->getFlagBitfieldPtr();
    const uint num_bytes = flagBitfieldPtr->getNumEntries();
    const uint gci_offset = flagBitfieldPtr->getDataOffset();

    const uint* bitfieldPtr = reinterpret_cast<const uint*>(&m_GCIData[gci_offset]);
    return flagIsEnabled(bitfieldPtr, num_bytes, flag);
}
