#include "rando/tools.h"
#include "d/d_com_inf_game.h"
#include "d/actor/d_a_alink.h"

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
