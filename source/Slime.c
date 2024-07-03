#include "Slime.h"

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "EntityTypes.h"
#include "GFX.h"
#include "Map.h"

void SL_SetupSlime(EN_Entity *slime, int x, int y)
{
    EN_Setup(slime, x, y, SL_WIDTH, SL_HEIGHT, ET_SLIME, SL_MAX_HEALTH, SL_ATTACK_DELAY);
}

void SL_SetupSlimeState(SL_SlimeState *slime_state)
{
    slime_state->velocity = 0;  //
}

void SL_Move(EN_Entity *slime, SL_SlimeState *slime_state, EN_Entity players[], int players_len)
{
    // Wait till we can move
    if (slime->current_attack_delay > 0)
    {
        slime->current_attack_delay--;
        return;
    }

    // We can move, so chose a direction if not moving
    if (slime_state->velocity == -1)
    {
        // Find the closest player to jump to
        float min_distance_squared = MAP_MAX_STRAIGHT_DISTANCE_SQUARED;
        float x_diff = 0, y_diff = 0;

        for (int i = 0; i < players_len; i++)
        {
            float new_x_diff = EN_CentreXf(&players[i]) - EN_CentreXf(slime);
            float new_y_diff = EN_CentreYf(&players[i]) - EN_CentreYf(slime);

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

        // Jump at the closest player if we are close enough to them else a random direction
        float direction;
        if (min_distance_squared < SL_MAX_DISTANCE_ATTACK_PLAYER_SQUARED)
            direction = -atan2f(y_diff, x_diff);
        else
            direction = ((float)rand() / RAND_MAX) * (M_PI * 2);

        // Set the velocity and vector
        slime_state->velocity = SL_INITIAL_VELOCITY;

        slime_state->vx = cosf(direction);
        slime_state->vy = -sinf(direction);

        // We are moving
        EN_SetStateBit(slime, SL_MOVING);
    }

    if (slime_state->velocity == 0)
    {
        // We have finished moving
        EN_ClearStateBit(slime, SL_MOVING);

        // Start the cooldown
        slime->current_attack_delay = slime->attack_delay;

        // So we can choose the direction when cooldown is off
        slime_state->velocity = -1;

        return;
    }

    s16 velocity_10 = slime_state->velocity / 10;

    // Move in the direction
    slime->x += slime_state->vx * velocity_10;
    slime->y += slime_state->vy * velocity_10;

    // Reduce the velocity
    slime_state->velocity--;
}

void SL_Animate(EN_Entity *slime, int frame_counter)
{
    if (EN_GetStateBit(slime, SL_MOVING))
    {
        slime->animation_frame_number = 2 + (frame_counter / 7) % 2;
    }
    else
    {
        slime->animation_frame_number = (frame_counter / 30) % 2;
    }

    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET * 2 + SPRITE_SIZE * slime->animation_frame_number,  //
            slime->gfx,                                                                             //
            SPRITE_SIZE);
}
