#include "Skeleton.h"

void SK_update(Entity *skeleton, Entity player)
{
    float x_diff = player.x - skeleton->x;
    float y_diff = player.y - skeleton->y;

    if ((abs(x_diff) + abs(y_diff)) < 32)
    {
        EN_clear_state_bit(skeleton, EN_MOVING_BIT);
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

    EN_set_state_bit(skeleton, EN_MOVING_BIT);
}
