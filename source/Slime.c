#include "Slime.h"

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "GFX.h"

void SL_SetupSlime(Entity *slime, int x, int y)
{
    EN_Setup(slime, x, y, SL_WIDTH, SL_HEIGHT, SL_MAX_HEALTH, SL_ATTACK_DELAY);
}

void SL_SetupSlimeState(SlimeState *slime_state)
{
    slime_state->velocity = 0;  //
}

void SL_Update(Entity *slime, SlimeState *slime_state, Entity *player)
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
        slime_state->velocity = SL_INITIAL_VELOCITY;

        float diff_x = EN_CentreXf(player) - EN_CentreXf(slime);
        float diff_y = EN_CentreYf(player) - EN_CentreYf(slime);

        float diff_x_squared = diff_x * diff_x;
        float diff_y_squared = diff_y * diff_y;

        // Jump at the player if close enough else random
        float direction;
        if (diff_x_squared + diff_y_squared < SL_MAX_DISTANCE_ATTACK_PLAYER_SQUARED)
        {
            direction = -atan2f(diff_y, diff_x);
        }
        else
        {
            direction = ((float)rand() / RAND_MAX) * (M_PI * 2);
        }

        slime_state->vx = cosf(direction);
        slime_state->vy = -sinf(direction);

        // We are moving
        EN_SetStateBit(slime, EN_MOVING_BIT);
    }

    if (slime_state->velocity == 0)
    {
        // We have finished moving
        EN_ClearStateBit(slime, EN_MOVING_BIT);

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

void SL_Animate(Entity *slime, u16 *slime_gfx, int frame_counter)
{
    if (EN_GetStateBit(slime, EN_MOVING_BIT))
    {
        slime->animation_frame_number = 2 + (frame_counter / 7) % 2;
    }
    else
    {
        slime->animation_frame_number = (frame_counter / 30) % 2;
    }

    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET * 2 + SPRITE_SIZE * slime->animation_frame_number,  //
            slime_gfx,                                                                              //
            SPRITE_SIZE);
}
