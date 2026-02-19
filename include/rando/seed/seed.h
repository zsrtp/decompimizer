#ifndef SEED_H
#define SEED_H

#include "dolphin/types.h"
#include "rando/seed/seedData.h"

enum SeedEnabledFlag
{
    TRANSFORM_ANYWHERE = 0,
    QUICK_TRANSFORM,
    INCREASE_SPINNER_SPEED,
    BONKS_DO_DAMAGE,
    AUTOFILL_WALLETS,
    MODIFY_SHOP_MODELS,
    RAINBOW_LANTERN,
    RAINBOW_MIDNA,
    RAINBOW_LIGHT_SWORD,
    LIGHT_SWORD_ALWAYS_ON
};

bool flagIsEnabled(const uint* bitfieldPtr, uint totalFlags, uint flag);

class EntryInfo
{
    public:
    EntryInfo() {}
    ~EntryInfo() {}

    u16 getNumEntries() const { return numEntries; }
    u16 getDataOffset() const { return dataOffset; }

    private:
    u16 numEntries;
    u16 dataOffset;
};

class seedHeaderInfo_c {
public:

    int _create();
    int _delete();

    const char* getSeedNamePtr() const { return &seedName[0]; }
    u16 getHeaderSize() const { return headerSize; }
    u16 getDataSize() const { return dataSize; }
    uint getTotalSize() const { return totalSize; }

    u16 getMaloShopDonationAmount() const { return maloShopDonationAmount; }
    u8 getCastleRequirements() const { return castleRequirements; }
    u8 getPalaceRequirements() const { return palaceRequirements; }
    u8 getMapClearBits() const { return mapClearBits; }
    u8 getDamageMagnification() const { return damageMagnification; }
    u8 getToTSwordRequirement() const { return totSwordRequirement; }
    u8 getMirrorChamberRequirement() const { return mirrorChamberEntrance; }
    u8 getBarrierReqCount() const { return barrierReqCount; }
    u8 getHcBkReqCount() const { return hcBkReqCount; }
    u8 getHcBkRequirement() const { return hcBkRequirement; }
    u16 getSmallWalletMax() const { return smallWalletMax; }
    u16 getBigWalletMax() const { return bigWalletMax; }
    u16 getGiantWalletMax() const { return giantWalletMax; }

    const EntryInfo* getVolatilePatchInfoPtr() const { return &volatilePatchInfo; }
    const EntryInfo* getOneTimePatchInfoPtr() const { return &oneTimePatchInfo; }
    const EntryInfo* getFlagBitfieldPtr() const { return &flagBitfieldInfo; }
    const EntryInfo* getEventFlagsInfoPtr() const { return &eventFlagsInfo; }
    const EntryInfo* getRegionFlagsInfoPtr() const { return &regionFlagsInfo; }
    const EntryInfo* getBossCheckInfoPtr() const { return &bossCheckInfo; }
    const EntryInfo* getHiddenSkillCheckInfoPtr() const { return &hiddenSkillCheckInfo; }
    const EntryInfo* getBugRewardCheckInfoPtr() const { return &bugRewardCheckInfo; }
    const EntryInfo* getSkyCharacterCheckInfoPtr() const { return &skyCharacterCheckInfo; }
    const EntryInfo* getEventItemCheckInfoPtr() const { return &eventItemCheckInfo; }
    const EntryInfo* getStartingItemCheckInfoPtr() const { return &startingItemInfo; }
    
    /* 0x00 */ char seedName[33];
    /* 0x22 */ u16 headerSize; // Total size of the header in bytes
    /* 0x2A */ u16 dataSize;   // Total number of bytes of seed data
    /* 0x2C */ uint totalSize;  // Total number of bytes in the GCI

    // BitArray where each bit represents a patch/modification to be applied for this playthrough; these
    // patchs/modifications must be applied every time a file is loaded
    /* 0x30 */ EntryInfo volatilePatchInfo;

    // BitArray where each bit represents a patch/modification to be applied for this playthrough; these
    // patchs/modifications must be applied only when a seed is loaded
    /* 0x34 */ EntryInfo oneTimePatchInfo;

    // BitArray where each bit represents an arbitrary flag indicated by the SeedEnabledFlag enum
    /* 0x38 */ EntryInfo flagBitfieldInfo;

    /* 0x3C */ EntryInfo eventFlagsInfo;  // eventFlags that need to be set for this seed
    /* 0x40 */ EntryInfo regionFlagsInfo; // regionFlags that need to be set, alternating
    /* 0x58 */ EntryInfo bossCheckInfo;
    /* 0x5C */ EntryInfo hiddenSkillCheckInfo;
    /* 0x60 */ EntryInfo bugRewardCheckInfo;
    /* 0x64 */ EntryInfo skyCharacterCheckInfo;
    /* 0x6C */ EntryInfo eventItemCheckInfo;
    /* 0x70 */ EntryInfo startingItemInfo;
    /* 0x8E */ u16 maloShopDonationAmount;
    /* 0x90 */ u8 castleRequirements;
    /* 0x91 */ u8 palaceRequirements;
    /* 0x92 */ u8 mapClearBits;
    /* 0x93 */ u8 damageMagnification;
    /* 0x94 */ u8 totSwordRequirement;
    /* 0x95 */ u8 mirrorChamberEntrance;
    /* 0x96 */ u8 barrierReqCount; // See below for notes
    /* 0x97 */ u8 hcBkRequirement;
    /* 0x98 */ u8 hcBkReqCount; // See below for notes  
    u16 smallWalletMax;
    u16 bigWalletMax;
    u16 giantWalletMax;
};

class seedInfo_c {
    public:
    
        int _create();
        int _delete();
    
        const seedHeaderInfo_c* getHeaderPtr() const { return m_Header; }
        bool seedIsLoaded() const { return m_GCIData; }

        const BossCheck* getBossChecksPtr() const { return m_BossChecks; }
        const HiddenSkillCheck* getHiddenSkillChecksPtr() const { return m_HiddenSkillChecks; }
        const BugReward* getBugRewardChecksPtr() const { return m_BugRewardChecks; }
        const SkyCharacter* getSkyBookChecksPtr() const { return m_SkyBookChecks; }
        const EventItem* getEventChecksPtr() const { return m_EventChecks; }

        const RawRGBTable* getRawRGBTablePtr() const { return m_RawRGBTable; }

        bool flagBitfieldFlagIsEnabled(uint flag) const;

        bool canTransformAnywhere() const { return flagBitfieldFlagIsEnabled(TRANSFORM_ANYWHERE); }
        bool canQuickTransform() const { return flagBitfieldFlagIsEnabled(QUICK_TRANSFORM); }
        bool bonksDoDamage() const { return flagBitfieldFlagIsEnabled(BONKS_DO_DAMAGE); }
        bool walletsAreAutoFilled() const { return flagBitfieldFlagIsEnabled(AUTOFILL_WALLETS); }
        bool shopModelsAreModified() const { return flagBitfieldFlagIsEnabled(MODIFY_SHOP_MODELS); }
        bool isLanternRainbow() const { return flagBitfieldFlagIsEnabled(RAINBOW_LANTERN); }
        bool isMidnaHairRainbow() const { return flagBitfieldFlagIsEnabled(RAINBOW_MIDNA); }
        bool isLightSwordRainbow() const { return flagBitfieldFlagIsEnabled(RAINBOW_LIGHT_SWORD); }
        bool isLightSwordAlwaysOn() const { return flagBitfieldFlagIsEnabled(LIGHT_SWORD_ALWAYS_ON); }

        bool spinnerSpeedIsIncreased() const
        {
            return flagBitfieldFlagIsEnabled(INCREASE_SPINNER_SPEED);
        }

        void loadShopModels();
        void loadShuffledEntrances();
        
       private:
        void applyEventFlags();
        void applyRegionFlags();
        void giveStartingItems();

        const seedHeaderInfo_c* m_Header;
        const u8* m_GCIData;

        const BossCheck* m_BossChecks;
        const HiddenSkillCheck* m_HiddenSkillChecks;
        const BugReward* m_BugRewardChecks;
        const SkyCharacter* m_SkyBookChecks;
        const EventItem* m_EventChecks;

        const RawRGBTable* m_RawRGBTable;
    };

extern seedInfo_c g_seedInfo;

#endif  // SEED_H
