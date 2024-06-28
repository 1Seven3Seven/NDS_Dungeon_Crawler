#include "Player.H"

#include "GFX.h"
#include "Map.h"

void PL_SetupPlayer(Entity *player, int x, int y)
{
    EN_Setup(player, x, y, PL_WIDTH, PL_HEIGHT, PL_MAX_HEALTH, PL_ATTACK_DELAY);
}

void PL_Move(Entity *player, uint32 keys_held)
{
    int vx = 0, vy = 0;
    if (keys_held & KEY_LEFT) vx -= PL_MOVE_SPEED;
    if (keys_held & KEY_RIGHT) vx += PL_MOVE_SPEED;
    if (keys_held & KEY_UP) vy -= PL_MOVE_SPEED;
    if (keys_held & KEY_DOWN) vy += PL_MOVE_SPEED;

    if (vx || vy)
        EN_SetStateBit(player, EN_MOVING_BIT);
    else
    {
        EN_ClearStateBit(player, EN_MOVING_BIT);
        return;  // No movement so we can finish here
    }

    player->x += vx;
    player->y += vy;

    // Clamp to stay on the map
    if (player->x < 0) player->x = 0;
    if (player->x > MAP_WIDTH - player->w) player->x = MAP_WIDTH - player->w;
    if (player->y < 0) player->y = 0;
    if (player->y > MAP_HEIGHT - player->h) player->y = MAP_HEIGHT - player->h;
}

void PL_Animate(Entity *player, u16 *player_gfx, int frame_counter)
{
    if (EN_GetStateBit(player, EN_MOVING_BIT))
        player->animation_frame_number = 2 + (frame_counter / 10) % 2;
    else
        player->animation_frame_number = (frame_counter / 30) % 2;

    dmaCopy((u8 *)SpriteSheetTiles + SPRITE_SIZE * player->animation_frame_number, player_gfx, SPRITE_SIZE);
}
