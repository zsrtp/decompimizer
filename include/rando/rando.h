#ifndef RANDO_H
#define RANDO_H
#define EVENT_ITEM_QUEUE_SIZE 0x10

#include "dolphin/types.h"

enum EventItemStatus
{
    QUEUE_EMPTY,
    ITEM_IN_QUEUE,
    CLEAR_QUEUE,
};

class randoInfo_c {
public:
    randoInfo_c() { mInitialized = false; }

    int _create();
    int _delete();
    int execute();
    int draw();
    bool checkValidTransformAnywhere();
    int getBugReward(u8 bugId);
    u8 getPoeItem(u8 bitSw);
    void handlePoeItem(u8 bitSw);
    u8 getEventItem(u8 flag);
    void addItemToEventQueue(u8 item);
    void initGiveItemToPlayer();
    void handleBonkDamage();

    u8 getGiveItemToPlayerStatus() { return eventItemStatus;}
    void setGiveItemToPlayerStatus(u8 status) { eventItemStatus = status;}
    u16 getLastButtonInput() { return m_LastButtonInput;}
    void setLastButtonInput(u16 buttonInput) { m_LastButtonInput = buttonInput;}
    float getPrevFrameAnalogR() { return prevFrameAnalogR;}
    void setPrevFrameAnalogR(float value) { prevFrameAnalogR = value;}

    bool mInitialized;
    u8 eventItemStatus;
    u16 mFrameCounter;
    u16 m_LastButtonInput;
    float prevFrameAnalogR;
    f32 rainbowPhaseAngle;
    bool isWolfDomeDrawn;
    u8 eventItemQueue[EVENT_ITEM_QUEUE_SIZE];
};

extern randoInfo_c g_randoInfo;

#endif  // RANDO_H
