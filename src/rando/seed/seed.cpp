#include "rando/seed/seed.h"
#include "rando/data/flags.h"
#include "rando/tools/verifyItemFunctions.h"
#include "rando/tools/tools.h"
#include "d/d_item.h"
#include "d/d_save.h"
#include "d/d_com_inf_game.h"
#include "d/actor/d_a_alink.h"

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

    // Now that the seed is loaded, set any static values needed.
    setStaticGameValues();

    return 1;
}

void seedInfo_c::initSeed()
{
    /* Copypasta rando code until I get to actually moving it over.
    // (Re)set counters & status
    this->m_AreaFlagsModified = 0;
    this->m_EventFlagsModified = 0;
    this->m_PatchesApplied = 0;

    // getConsole() << "Setting Event Flags... \n";
    this->applyEventFlags();

    // getConsole() << "Setting Region Flags... \n";
    this->applyRegionFlags();
    */
    // Fill small wallet if needed before giving starting items because if the player doesn't start with any wallets,
    // we want to fill the wallet. However if they do then it will be filled anyways.
    if (walletsAreAutoFilled())
    {
        dComIfGs_setRupee(m_Header->getSmallWalletMax());
    }
    giveStartingItems();

    applySeedPatches();
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

void seedInfo_c::giveStartingItems()
{
    const EntryInfo* startingItemInfoPtr = m_Header->getStartingItemCheckInfoPtr();
    const uint num_startingItems = startingItemInfoPtr->getNumEntries();
    const uint gci_offset = startingItemInfoPtr->getDataOffset();

    if (num_startingItems == 0)
    {
        return;
    }

    // Set the pointer as offset into our buffer
    const u8* startingItems = &m_GCIData[gci_offset];

    for (int i = 0; i < num_startingItems; i++)
    {
        execItemGet(startingItems[i]);
    }
}

void seedInfo_c::applySeedPatches()
{
    if (dComIfGs_isEventBit(CLEARED_FARON_TWILIGHT))
    {
        dComIfGs_onDarkClearLV(0);
        dComIfGs_setLightDropNum(0, 0x10);
        execItemGet(fpcNm_ITEM_DROP_CONTAINER);
        if (haveItem(fpcNm_ITEM_WEAR_KOKIRI))
        {
            execItemGet(fpcNm_ITEM_WEAR_KOKIRI);
        }
    }

    if (dComIfGs_isEventBit(CLEARED_ELDIN_TWILIGHT))
    {
        dComIfGs_onDarkClearLV(1);
        dComIfGs_setLightDropNum(1, 0x10);
        execItemGet(fpcNm_ITEM_DROP_CONTAINER02);
    }

    if (dComIfGs_isEventBit(CLEARED_ELDIN_TWILIGHT))
    {
        dComIfGs_onDarkClearLV(2);
        dComIfGs_setLightDropNum(2, 0x10);
        execItemGet(fpcNm_ITEM_DROP_CONTAINER03);
    }

    if (skipMinorCutscenes())
    {
        dComIfGs_onItemFirstBit(fpcNm_ITEM_GREEN_RUPEE);
        dComIfGs_onItemFirstBit(fpcNm_ITEM_BLUE_RUPEE);
        dComIfGs_onItemFirstBit(fpcNm_ITEM_YELLOW_RUPEE);
        dComIfGs_onItemFirstBit(fpcNm_ITEM_RED_RUPEE);
        dComIfGs_onItemFirstBit(fpcNm_ITEM_PURPLE_RUPEE);
        dComIfGs_onItemFirstBit(fpcNm_ITEM_ORANGE_RUPEE);
        dComIfGs_onItemFirstBit(fpcNm_ITEM_SILVER_RUPEE);

        dComIfGs_setAllLetterGet();
        dComIfGs_setAllLetterRead();
    }

    if (dComIfGs_isEventBit(MIDNAS_DESPERATE_HOUR_COMPLETED))
    {
        if (dComIfGs_getDarkClearLV() == 0x7)
        {
            dComIfGs_onDarkClearLV(3);
            dComIfGs_onTransformLV(3);
        }
    }

    if (isMapOpen())
    {
        dComIfGs_setRegionBit(m_Header->getMapClearBits());
    }
}

void setStaticGameValues()
{
    // Update lantern vars
    daAlinkHIO_kandelaar_c1* lv = (daAlinkHIO_kandelaar_c1*)&daAlink_getAlinkActorClass()->mpHIO->mItem.mLantern.m;
    daAlinkHIO_huLight_c1* hlv = (daAlinkHIO_huLight_c1*)&daAlink_getAlinkActorClass()->mpHIO->mItem.mLanternPL.m;
    u8* lanternColorPtr = g_seedInfo.getHeaderPtr()->getLanternColorPtr();
    lv->mColorReg1R = lanternColorPtr[0];
    lv->mColorReg1G = lanternColorPtr[1];
    lv->mColorReg1B = lanternColorPtr[2];
    lv->mColorReg2R = lanternColorPtr[0];
    lv->mColorReg2G = lanternColorPtr[1];
    lv->mColorReg2B = lanternColorPtr[2];
    hlv->mColorR = lanternColorPtr[0];
    hlv->mColorG = lanternColorPtr[1];
    hlv->mColorB = lanternColorPtr[2];
}
