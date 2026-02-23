#include "rando/tools/tools.h"
#include "rando/tools/memory.h"
#include "rando/rando.h"
#include "rando/seed/seed.h"
#include "d/d_com_inf_game.h"
#include "d/actor/d_a_alink.h"
#include "d/actor/d_a_player.h"
#include "d/actor/d_a_midna.h"
#include "d/d_meter2.h"
#include "d/d_meter2_info.h"
#include "d/d_meter2_draw.h"
#include "d/d_item.h"
#include "f_op/f_op_actor_mng.h"
#include "m_Do/m_Do_controller_pad.h"
#include "dolphin/dvd.h"

bool playerIsInRoomStage(s32 room, const char* stage)
{
    // Only check room if it is valid
    // Room numbers are normally stored as int8_t, so the highest positive value is 127
    if ((room < 0) || (room > 127))
    {
        return false;
    }

    if (room != dStage_roomControl_c::mStayNo)
    {
        return false;
    }

    // Only check stage if it is valid
    if (!stage)
    {
        return false;
    }

    return daAlink_c::checkStageName(stage);
}

void checkTransformFromWolf()
{
    if (dComIfGs_getTransformStatus())
    {
        daAlink_getAlinkActorClass()->procCoMetamorphoseInit();
    }
    return;
}

u8 setNextWarashibeItem()
{
    static const u8 questItemsList[] = {fpcNm_ITEM_LETTER, fpcNm_ITEM_BILL, fpcNm_ITEM_WOOD_STATUE, fpcNm_ITEM_IRIAS_PENDANT, fpcNm_ITEM_HORSE_FLUTE};

    u32 listLength = sizeof(questItemsList) / sizeof(questItemsList[0]);

    u8 newItem = 0xFF; // null by default

    for (u32 i = 0; i < listLength; i++)
    {
        const u32 item = questItemsList[i];
        const u8 slotItem = dComIfGs_getItem(21, 0);
        if (item == slotItem)
        {
            newItem = item;
            u32 j = i;
            do
            {
                j = (j + 1) % listLength; // Move to next index, wrapping around if needed.
                if (checkItemGet(questItemsList[j], 1))
                {
                    newItem = questItemsList[j];
                    break;
                }
            } while (j != i);

            // If the item to switch to is the same as the current item and we don't have the item anymore, null the slot
            if ((newItem == item) && !checkItemGet(item, 1))
            {
                newItem = 0xFF;
            }
            dComIfGs_setItem(21, newItem);

            break;
        }
    }
    return newItem;
}

void offWarashibeItem(u8 item)
{
    g_dComIfG_gameInfo.info.getSavedata().getPlayer().getGetItem().offFirstBit(item);
    setNextWarashibeItem();
}

int initCreatePlayerItem(uint item, uint flag, const cXyz* pos, int roomNo, const csXyz* angle, const cXyz* scale)
{
    uint params = 0xFF0000 | ((flag & 0xFF) << 0x8) | (item & 0xFF);
    return fopAcM_create(539, params, pos, roomNo, angle, scale, -1);
}

bool checkButtonComboAnalog(uint combo)
{
    // Get the buttons pressed this frame and held
    uint buttonsPressedThisFrame = mDoCPd_c::getTrig(PAD_1);
    uint buttonsHeld = mDoCPd_c::getHold(PAD_1);

    // Check if either of the analog buttons are held past a certain threshold
    if (&buttonsPressedThisFrame || &buttonsHeld)
    {
        float analogThreshold = 0.7f;
        // Check if R is included in the combo
        if (combo & PAD_TRIGGER_R)
        {
            // Check if analog R is at 70% or more
            if (mDoCPd_c::getAnalogR(PAD_1) >= analogThreshold)
            {
                if (&buttonsHeld)
                {
                    buttonsHeld |= PAD_TRIGGER_R;
                }

                if (&buttonsPressedThisFrame)
                {
                    // If prevFrameAnalogR is less than 70%,  then 70% was reached this frame
                    if (g_randoInfo.getPrevFrameAnalogR() < analogThreshold)
                    {
                        buttonsPressedThisFrame |= PAD_TRIGGER_R;
                    }
                }
            }
        }
    }

    // Check if the button combo is held
    if ((buttonsHeld & combo) != combo)
    {
        return false;
    }

    // Check if at least one button in the combo was pressed this frame
    return buttonsPressedThisFrame & combo;
}

void handleQuickTransform()
{
    // If the setting isn't enabled, return immediately
    if (!g_seedInfo.canQuickTransform())
    {
        return;
    }

    // Verify Link is loaded on the map.
    if (!daAlink_getAlinkActorClass())
    {
        return;
    }

    // Ensure Link is not in a cutscene
    if (daAlink_getAlinkActorClass()->checkEventRun())
    {
        return;
    }

    // Check if Link has the ability to transform
    if (!dComIfGs_isEventBit(0xD04))
    {
        return;
    }

    // Make sure Link isn't riding anything
    if (daAlink_getAlinkActorClass()->checkRide())
    {
        return;
    }

    if (daAlink_getAlinkActorClass()->checkMagneBootsOn())
    {
        return;
    }

    switch (daAlink_getAlinkActorClass()->mProcID)
    {
        case daAlink_c::PROC_TALK:
        case daAlink_c::PROC_SWIM_UP:
        case daAlink_c::PROC_SWIM_DIVE:
        {
            return;
        }
        // If link is targeting or pulling a chain, we don't want to remove the ability to use items in combat.
        case daAlink_c::PROC_ATN_ACTOR_MOVE:
        case daAlink_c::PROC_ATN_ACTOR_WAIT:
        case daAlink_c::PROC_WOLF_ATN_AC_MOVE:
        {
            break;
        }
        default:
        {
            // Disable the input that was just pressed as sometimes it could cause items to be used or Wolf Link to dig.
            mDoCPd_c::setTrig(PAD_1, 0);
            break;
        }
    }

    // Ensure there are pointers to the mMeterClass and mpMeterDraw structs
    if (!dMeter2Info_getMeterClass())
    {
        return;
    }

    if (!dMeter2Info_getMeterClass()->getMeterDrawPtr())
    {
        return;
    }

    // Make sure Z button isn't dimmed
    if (dMeter2Info_getMeterClass()->getMeterDrawPtr()->getZButtonAlpha() != 1.f)
    {
        return;
    }

    if (daAlink_getAlinkActorClass()->checkIronBallEquip())
    {
        return;
    }

    // Prevent transforming if player has magnetic boots equipped as that can cause a hard lock
    if (daAlink_getAlinkActorClass()->checkNoResetFlg0(daPy_py_c::FLG0_MAGNE_BOOTS_ON))
    {
        return;
    }

    // Make sure player isn't in PoT dark fog
    if (g_env_light.mEvilInitialized & 0x80)
    {
        return;
    }

    if (!g_randoInfo.checkValidTransformAnywhere())
    {
        // Use the game's default checks for if the player can transform.
        if (!daPy_py_c::getMidnaActor()->checkMetamorphoseEnable())
        {
            return;
        }

        // If the player is in castle town and has scared NPCs, they cannot transform
        if (daAlink_c::checkStageName("F_SP116") && dComIfGs_isSaveDunSwitch(0x3C))
        {
            return;
        }

        // Verify npcNear and npcFar
        u32 midnaStateFlg0 = daPy_py_c::getMidnaActor()->getStateFlg0();
        if ((midnaStateFlg0 & 0x100000) || (midnaStateFlg0 & 0x40000))
        {
            return;
        }
    }

    daAlink_getAlinkActorClass()->procCoMetamorphoseInit();
}

int readFile(const char* file, bool allocFromHead, u8** dataOut)
{

    // Init dataOut
    *dataOut = NULL;

    // Open the file
    DVDFileInfo fileInfo;
    if (!DVDOpen(file, &fileInfo))
    {
        return -1;
    }

    // Get the size of the file
    int fileSize = (int)fileInfo.length;

    // Make sure the size is a multiple of DVD_READ_SIZE, as we can only read in and at increments of DVD_READ_SIZE
    fileSize = (fileSize + 0x20 - 1) & ~(0x20 - 1);

    // Allocate memory for the file
    // Buffers that DVDRead uses must be aligned to 0x20 bytes
    uint alignment = 0x20;
    if (!allocFromHead)
    {
        alignment = -0x20;
    }

    u8* fileData = new (alignment) u8[fileSize];

    // Read the file from the disc
    const int result = DVDReadPrio(&fileInfo, fileData, fileSize, 0, 2);

    // Close the file, as it's no longer needed
    DVDClose(&fileInfo);

    // Make sure the file was successfully read
    if (result <= 0)
    {
        delete[] fileData;
        return -1;
    }

    clear_DC_IC_Cache(fileData, fileSize);

    *dataOut = fileData;
    return fileSize;
}
