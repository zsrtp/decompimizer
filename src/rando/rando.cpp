#include "rando/rando.h"
#include "rando/seed/seed.h"
#include "rando/itemWheelMenu.h"
#include "d/d_com_inf_game.h"
#include "SSystem/SComponent/c_math.h"
#include "d/actor/d_a_alink.h"

randoInfo_c g_randoInfo;

int randoInfo_c::_create() {
    mFrameCounter = 0;
    mInitialized = true;
    g_customMenuRing._initialize();
    return 1;
}

int randoInfo_c::_delete() {
    mInitialized = false;
    return 1;
}

int randoInfo_c::execute() {
    if (!mInitialized) {
        return 0;
    }

    // Every 300 frames, set rupees to a random value
    mFrameCounter++;
    if (mFrameCounter >= 300) {
        mFrameCounter = 0;
        u16 randomRupees = (u16)cM_rndF(1000.0f);
        dComIfGs_setRupee(randomRupees);
    }

    return 1;
}

int randoInfo_c::draw() {
    return 1;
}

bool randoInfo_c::checkValidTransformAnywhere()
{
    if (!g_seedInfo.canTransformAnywhere())
    {
        return false;
    }

    // Check if the player is currently playing the goats minigame.
    if (daAlink_c::checkStageName("F_SP00"))
    {
        switch(g_dComIfG_gameInfo.play.getStartStageLayer())
        {
            // Layer 4 and 5 is used when the minigame is taking place.
            case 4:
            case 5:
            {
                // Return false as the game will crash if the player is in wolf form after the minigame ends.
                return false;
            }
            default:
            {
                break;
            }
        }
    }

    // Return true as the bool is set and there are no conflicting scenerios to prevent transformation
    return true;
}

int randoInfo_c::getBugReward(u8 bugId)
{
    /*
    Once the infrastructure is built the code will look like the following:
    int item = replaceBugReward(bugId); we will probably build the functionality out instead of calling another func though.
    */
    return bugId;
}

u8 randoInfo_c::getSkyCharacterItem()
{
    /*
    Once the infrastructure is built the code will look like the following:
    u8 item = replaceCharacterReward(); we will probably build the functionality out instead of calling another func though.
    */
    return fpcNm_ITEM_ANCIENT_DOCUMENT;
}

u8 randoInfo_c::getPoeItem(u8 bitSw)
{
    /*
    Once the infrastructure is built the code will look like the following:
    u8 item = replacePoeReward(); we will probably build the functionality out instead of calling another func though.
    */
    return fpcNm_ITEM_POU_SPIRIT;
}

void randoInfo_c::handlePoeItem(u8 bitSw)
{
    u8 item = getPoeItem(bitSw);
    // addItemToEventQueue(item);
    daAlink_getAlinkActorClass()->procWolfAtnActorMoveInit();
}
