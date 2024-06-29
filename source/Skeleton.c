#include "Skeleton.h"

#include "GFX.h"

void SK_Update(Entity *skeleton, Entity *player)
{
    float x_diff = player->x - skeleton->x;
    float y_diff = player->y - skeleton->y;

    if ((abs(x_diff) + abs(y_diff)) < 32)
    {
        EN_ClearStateBit(skeleton, EN_STATE_MOVING);
        return;
    }

    float x_move = x_diff > 0 ? 0.5 : -0.5;
    float y_move = y_diff > 0 ? 0.5 : -0.5;

    if (abs(x_diff) < 0.5)
    {
        x_move = x_diff;
    }
    if (abs(y_diff) < 0.5)
    {
        y_move = y_diff;
    }

    skeleton->x += x_move;
    skeleton->y += y_move;

    EN_SetStateBit(skeleton, EN_STATE_MOVING);
}

void SL_Animate(Entity *skeleton, u16 *skeleton_gfx, int frame_counter)
{
    if (EN_GetStateBit(skeleton, EN_STATE_MOVING))
    {
        skeleton->animation_frame_number = 2 + (frame_counter / 10) % 2;
    }
    else
    {
        skeleton->animation_frame_number = (frame_counter / 30) % 2;
    }

    dmaCopy((u8 *)skeleton + ROW_OFFSET + SPRITE_SIZE * skeleton->animation_frame_number,  //
            skeleton_gfx,                                                                  //
            SPRITE_SIZE);
}
