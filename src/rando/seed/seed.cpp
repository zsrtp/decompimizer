#include "rando/seed/seed.h"

seedInfo_c g_seedInfo;

int seedInfo_c::_create() {
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
