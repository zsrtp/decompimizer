#include "rando/rando.h"
#include "rando/seed/seed.h"
#include "rando/tools/tools.h"
#include "rando/data/flags.h"
#include "rando/tools/verifyItemFunctions.h"
#include "rando/itemWheelMenu.h"
#include "d/d_com_inf_game.h"
#include "SSystem/SComponent/c_math.h"
#include "d/actor/d_a_alink.h"
#include "m_Do/m_Do_controller_pad.h"
#include "JSystem/J2DGraph/J2DPrint.h"
#include "JSystem/J2DGraph/J2DTextBox.h"
#include "JSystem/J2DGraph/J2DOrthoGraph.h"
#include "m_Do/m_Do_ext.h"

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
    // Set up 2D orthographic projection (608x448 virtual screen)
    J2DOrthoGraph ortho(0.0f, 0.0f, 608.0f, 448.0f, -1.0f, 1.0f);
    ortho.setPort();

    JUtility::TColor white(255, 255, 255, 255);
    JUTFont* font = mDoExt_getMesgFont();

    // J2DPrint: printf-style, draws immediately, no persistent state
    J2DPrint print(font, white, white);
    print.print(0.0f, 50.0f, 255, "J2DPrint Examples");

    // printf formatting
    print.print(0.0f, 70.0f, 255, "Frame: %d  Pos: %.1f, %.1f", mFrameCounter, 1.5f, 2.3f);

    // Per-call alpha (0-255)
    print.print(0.0f, 90.0f, 128, "Half Transparent");

    // Inline escape codes: \x1BCC[RRGGBBAA] changes color mid-string
    print.print(0.0f, 110.0f, 255, "White \x1B" "CC[FF6464FF]Red \x1B" "CC[64FF64FF]Green");

    // Inline escape codes: \x1BFX[n] / \x1BFY[n] changes font scale mid-string
    print.print(0.0f, 130.0f, 255, "Normal \x1B" "FX[30]\x1B" "FY[30]Big");

    // J2DTextBox: persistent string, supports alignment/bounds/pane hierarchy
    JUtility::TColor green(100, 255, 100, 255);
    JUtility::TColor cyan(100, 200, 255, 255);
    JUtility::TColor yellow(255, 255, 100, 255);
    JUtility::TColor red(255, 100, 100, 128);

    // Basic text
    J2DTextBox textbox;
    textbox.setFont(font);
    textbox.setFontSize(24.0f, 24.0f);
    textbox.setString("J2DTextBox Examples");
    textbox.setCharColor(white);
    textbox.setGradColor(white);
    textbox.draw(0.0f, 170.0f, 608.0f, HBIND_LEFT);

    // Gradient color (top green, bottom cyan)
    J2DTextBox gradient;
    gradient.setFont(font);
    gradient.setFontSize(24.0f, 24.0f);
    gradient.setString("Gradient Text");
    gradient.setCharColor(green);
    gradient.setGradColor(cyan);
    gradient.draw(0.0f, 200.0f, 608.0f, HBIND_LEFT);

    // Center-aligned
    J2DTextBox centered;
    centered.setFont(font);
    centered.setFontSize(18.0f, 18.0f);
    centered.setString("Center Aligned");
    centered.setCharColor(yellow);
    centered.setGradColor(yellow);
    centered.draw(0.0f, 230.0f, 608.0f, HBIND_CENTER);

    // Right-aligned
    J2DTextBox right;
    right.setFont(font);
    right.setFontSize(18.0f, 18.0f);
    right.setString("Right Aligned");
    right.setCharColor(yellow);
    right.setGradColor(yellow);
    right.draw(0.0f, 250.0f, 608.0f, HBIND_RIGHT);

    // Large text with transparency
    J2DTextBox large;
    large.setFont(font);
    large.setFontSize(36.0f, 36.0f);
    large.setString("Big & Transparent");
    large.setCharColor(red);
    large.setGradColor(red);
    large.draw(0.0f, 280.0f, 608.0f, HBIND_LEFT);

    // Custom character spacing
    J2DTextBox spaced;
    spaced.setFont(font);
    spaced.setFontSize(20.0f, 20.0f);
    spaced.setCharSpace(8.0f);
    spaced.setString("Wide Spacing");
    spaced.setCharColor(cyan);
    spaced.setGradColor(cyan);
    spaced.draw(0.0f, 320.0f, 608.0f, HBIND_LEFT);

    // Static rotation (tilted 15 degrees, pivot at center of text)
    J2DTextBox tilted;
    tilted.setFont(font);
    tilted.setFontSize(20.0f, 20.0f);
    tilted.setString("Tilted Text");
    tilted.setCharColor(green);
    tilted.setGradColor(green);
    tilted.rotate(60.0f, 10.0f, ROTATE_Z, 15.0f);
    tilted.draw(0.0f, 350.0f);

    // Animated rotation (spins using mFrameCounter)
    J2DTextBox spinning;
    spinning.setFont(font);
    spinning.setFontSize(20.0f, 20.0f);
    spinning.setString("Spinning!");
    spinning.setCharColor(yellow);
    spinning.setGradColor(yellow);
    spinning.rotate(50.0f, 10.0f, ROTATE_Z, (f32)(mFrameCounter * 3 % 360));
    spinning.draw(304.0f, 370.0f);

    // BCK/BRK animations also supported via setAnimation(), but need resources loaded from .arc

    // Animated alpha (pulsing transparency)
    f32 pulse = (cM_ssin((s16)(mFrameCounter * 512)) + 1.0f) * 0.5f;
    u8 pulseAlpha = (u8)(pulse * 255.0f);
    J2DTextBox pulsing;
    pulsing.setFont(font);
    pulsing.setFontSize(24.0f, 24.0f);
    pulsing.setString("Pulsing Alpha");
    pulsing.setCharColor(JUtility::TColor(255, 150, 255, pulseAlpha));
    pulsing.setGradColor(JUtility::TColor(255, 150, 255, pulseAlpha));
    pulsing.draw(0.0f, 400.0f, 608.0f, HBIND_LEFT);

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

void checkSetHCBarrierFlag(u8 req, u8 currentCount)
{
    if (req != g_seedInfo.getHeaderPtr()->getCastleRequirements())
    {
        return;
    }
    
    if (currentCount >= g_seedInfo.getHeaderPtr()->getBarrierReqCount())
    {
        dComIfGs_onEventBit(BARRIER_GONE);
    }
}

void checkSetHCBkFlag(u8 req, u8 currentCount)
{
    if (req != g_seedInfo.getHeaderPtr()->getHcBkRequirement())
    {
        return;
    }
    
    if (currentCount >= g_seedInfo.getHeaderPtr()->getHcBkReqCount())
    {
        dComIfGs_onStageSwitch(0x18, 0x4B);
    }
}
