#include "Skeleton.h"

#include "EntityTypes.h"
#include "GFX.h"
#include "Map.h"

void SK_SetupSkeleton(EN_Entity *skeleton, int x, int y)
{
    EN_Setup(skeleton, x, y, SK_WIDTH, SK_HEIGHT, ET_SKELETON, SK_MAX_HEALTH, SK_ATTACK_DELAY);
}

void SK_Move(EN_Entity *skeleton, EN_Entity players[], int players_len)
{
    /*
    When setting the initila min distance squared, I use the distance from two opposite corners of the map.
    Nothing can be further than this, but I add one for good measure.
    */

    // Find the x and y difference to the closest player
    float min_distance_squared = MAP_MAX_STRAIGHT_DISTANCE_SQUARED;
    float x_diff = 0, y_diff = 0;
    for (int i = 0; i < players_len; i++)
    {
        float new_x_diff = EN_CentreXf(&players[i]) - EN_CentreXf(skeleton);
        float new_y_diff = EN_CentreYf(&players[i]) - EN_CentreYf(skeleton);

        float new_x_diff_squared = new_x_diff * new_x_diff;
        float new_y_diff_squared = new_y_diff * new_y_diff;

        float new_distance_squared = new_x_diff_squared + new_y_diff_squared;

        if (new_distance_squared < min_distance_squared)
        {
            min_distance_squared = new_distance_squared;
            x_diff = new_x_diff;
            y_diff = new_y_diff;
        }
    }

    // Stop moving if close enough
    if (min_distance_squared < SK_MIN_DISTANCE_TO_ATTACK_PLAYER_SQUARED)
    {
        EN_ClearStateBit(skeleton, SK_MOVING);
        return;
    }

    float x_move = x_diff > 0 ? 0.5 : -0.5;
    float y_move = y_diff > 0 ? 0.5 : -0.5;

    if (abs(x_diff) < 0.5) x_move = x_diff;
    if (abs(y_diff) < 0.5) y_move = y_diff;

    skeleton->x += x_move;
    skeleton->y += y_move;

    EN_SetStateBit(skeleton, SK_MOVING);
}

void SK_Animate(EN_Entity *skeleton, u16 *skeleton_gfx, int frame_counter)
{
    if (EN_GetStateBit(skeleton, SK_MOVING))
    {
        skeleton->animation_frame_number = 2 + (frame_counter / 10) % 2;
    }
    else
    {
        skeleton->animation_frame_number = (frame_counter / 30) % 2;
    }

    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET + SPRITE_SIZE * skeleton->animation_frame_number,  //
            skeleton_gfx,                                                                          //
            SPRITE_SIZE);
}
