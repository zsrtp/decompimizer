#include "rando/rando.h"
#include "rando/seed/seed.h"
#include "rando/tools/tools.h"
#include "rando/tools/verifyItemFunctions.h"
#include "rando/itemWheelMenu.h"
#include "d/d_com_inf_game.h"
#include "SSystem/SComponent/c_math.h"
#include "d/actor/d_a_alink.h"
#include "m_Do/m_Do_controller_pad.h"

randoInfo_c g_randoInfo;

int randoInfo_c::_create() {
    mFrameCounter = 0;
    mInitialized = true;
    isWolfDomeDrawn = false;
    rainbowPhaseAngle = 0.f;
    eventItemStatus = QUEUE_EMPTY;
    g_customMenuRing._initialize();
    g_seedInfo._create();
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
    const uint currentButtons = mDoCPd_c::getHold(PAD_1);

    if (currentButtons != getLastButtonInput())
    {
        // Store before processing since we (potentially) un-set the padInfo values later
        setLastButtonInput(currentButtons);
    }

    if (checkButtonComboAnalog(PAD_TRIGGER_R | PAD_BUTTON_Y))
    {
        handleQuickTransform();
    }

    // Every 300 frames, set rupees to a random value
    mFrameCounter++;
    if (mFrameCounter >= 300) {
        mFrameCounter = 0;
        u16 randomRupees = (u16)cM_rndF(1000.0f);
        dComIfGs_setRupee(randomRupees);
    }

    // Any custom functionality that relies on Link's actor being on a stage
    if (daAlink_getAlinkActorClass())
    {
        if (g_seedInfo.isMidnaHairRainbow())
        {
            adjustMidnaHairColor();
        }
        if (g_seedInfo.isWolfDomeRainbow() && daAlink_getAlinkActorClass()->checkWolfLockAttackChargeState() && isWolfDomeDrawn)
        {
            replaceEquipItemColor();
        }
    }

    // Main code as ran, so update any previous frame variables.
    setPrevFrameAnalogR(mDoCPd_c::getAnalogR(PAD_1));
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

u8 randoInfo_c::getEventItem(u8 flag)
{
    const uint32_t numEventChecks = g_seedInfo.getHeaderPtr()->getEventItemCheckInfoPtr()->getNumEntries();
    const EventItem* eventChecks = g_seedInfo.getEventChecksPtr();

    for (uint32_t i = 0; i < numEventChecks; i++)
    {
        const EventItem* currentEventCheck = &eventChecks[i];
        if (flag == currentEventCheck->getFlag())
        {
            // Return new item
            return currentEventCheck->getItemID();
        }
    }

    // Currently we just use the vanilla item ID as the flag since the scope of these checks are limited at the moment.
    return flag;
}

void randoInfo_c::addItemToEventQueue(u8 item)
{
    for (int i = 0; i < EVENT_ITEM_QUEUE_SIZE; i++)
    {
        if (eventItemQueue[i] == 0)
        {
            eventItemQueue[i] = item;
            break;
        }
    }
}

void randoInfo_c::initGiveItemToPlayer()
{
    switch (daAlink_getAlinkActorClass()->mProcID)
    {
        case daAlink_c::PROC_WAIT:
        case daAlink_c::PROC_TIRED_WAIT:
        case daAlink_c::PROC_MOVE:
        case daAlink_c::PROC_WOLF_WAIT:
        case daAlink_c::PROC_WOLF_TIRED_WAIT:
        case daAlink_c::PROC_WOLF_MOVE:
        case daAlink_c::PROC_ATN_MOVE:
        case daAlink_c::PROC_WOLF_ATN_AC_MOVE:
        {
            // Check if link is currently in a cutscene
            if (daAlink_getAlinkActorClass()->checkEventRun())
            {
                break;
            }

            // Ensure that link is not currently in a message-based event.
            if (daAlink_getAlinkActorClass()->getEventId() != 0)
            {
                break;
            }

            u8 itemToGive = 0xFF;

            for (int i = 0; i < EVENT_ITEM_QUEUE_SIZE; i++)
            {
                const u8 storedItem = eventItemQueue[i];

                if (storedItem)
                {
                    const u8 giveItemToPlayerStatus = getGiveItemToPlayerStatus();

                    // If we have the call to clear the queue, then we want to clear the item and break out.
                    if (giveItemToPlayerStatus == CLEAR_QUEUE)
                    {
                        eventItemQueue[i] = 0;
                        setGiveItemToPlayerStatus(QUEUE_EMPTY);
                        break;
                    }

                    // If the queue is empty and we have an item to give, update the queue state.
                    else if (giveItemToPlayerStatus == QUEUE_EMPTY)
                    {
                        setGiveItemToPlayerStatus(ITEM_IN_QUEUE);
                    }

                    itemToGive = verifyProgressiveItem(storedItem);
                    break;
                }
            }

            // if there is no item to give, break out of the case.
            if (itemToGive == 0xFF)
            {
                break;
            }

            g_dComIfG_gameInfo.play.getEvent()->setGtItm(itemToGive);

            // Set the process value for getting an item to start the "get item" cutscene when next available.
            daAlink_getAlinkActorClass()->mProcID = daAlink_c::PROC_GET_ITEM;

            //  Get the event index for the "Get Item" event.
            const s16 eventIdx = dComIfGp_getEventManager().getEventIdx((fopAc_ac_c*)daAlink_getAlinkActorClass(),"DEFAULT_GETITEM",0xFF);

            // Finally we want to modify the event stack to prioritize our custom event so that it happens next.
            fopAcM_orderChangeEventId(daAlink_getAlinkActorClass(), eventIdx, 1, 0xFFFF);
        }
        default:
        {
            break;
        }
    }
}

void randoInfo_c::handleBonkDamage()
{
    if (!g_seedInfo.bonksDoDamage())
    {
        return;
    }

    u8 currentDamageMultiplier = g_seedInfo.getHeaderPtr()->getDamageMagnification();
    u16 currentHealth = dComIfGs_getLife();
    int newHealth;

    if (dComIfGs_getTransformStatus())
    {
        // Wolf takes double damage
        newHealth = currentHealth - (2 * currentDamageMultiplier);
    }
    else
    {
        newHealth = currentHealth - currentDamageMultiplier;
    }

    // Make sure an underflow doesn't occur
    if (newHealth < 0)
    {
        newHealth = 0;
    }
    dComIfGs_setLife(newHealth);
}
