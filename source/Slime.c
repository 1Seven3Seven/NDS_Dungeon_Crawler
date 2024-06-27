#include "Slime.h"

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

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
    if (slime->current_attack_delay > 0)
    {
        slime->current_attack_delay--;
        return;
    }

    // If the slime has stopped moving, then start the attack delay
    // State that the slime should reset its direction
    if (slime_state->velocity == 0)
    {
        slime->current_attack_delay = slime->attack_delay;
        slime_state->velocity = -1;

        // We are no longer moving
        EN_ClearStateBit(slime, EN_MOVING_BIT);
        return;
    }

    // The velocity is unset, so get a direction to jump in
    if (slime_state->velocity == -1)
    {
        slime_state->velocity = SL_INITIAL_VELOCITY;

        float diff_x = EN_CentreXf(player) - EN_CentreXf(slime);
        float diff_y = EN_CentreYf(player) - EN_CentreYf(slime);

        float diff_x_squared = diff_x * diff_x;
        float diff_y_squared = diff_y * diff_y;

        float direction;
        if (diff_x_squared + diff_y_squared < SL_MAX_DISTANCE_ATTACK_PLAYER_SQUARED)
        {
            direction = atan2f(diff_y, diff_x);
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

    s16 velocity_10 = slime_state->velocity / 10;

    // Move in the direction
    slime->x += slime_state->vx * velocity_10;
    slime->y += slime_state->vy * velocity_10;

    // Reduce the velocity
    slime_state->velocity--;
}
