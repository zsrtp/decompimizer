#include "rando/tools.h"
#include "d/d_com_inf_game.h"
#include "d/actor/d_a_alink.h"
#include "d/d_item.h"

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
    if (g_dComIfG_gameInfo.info.getSavedata().getPlayer().getPlayerStatusA().getTransformStatus())
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
            const u8 slotItem = g_dComIfG_gameInfo.info.getSavedata().getPlayer().getItem().getItem(21, 0);
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
                g_dComIfG_gameInfo.info.getSavedata().getPlayer().getItem().setItem(21, newItem);

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
