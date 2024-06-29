#ifndef DUNGEON_CRAWLER_PLAYER_H
#define DUNGEON_CRAWLER_PLAYER_H

/**
 * Functions and definitions regarding the player entities.
 *
 * PREFIX: PL
 */

#include <nds.h>

#include "Entity.h"

/// The width of the player entity.
#define PL_WIDTH 11
/// The height of the player entity.
#define PL_HEIGHT 15

/// The default move speed of the player
#define PL_MOVE_SPEED 1

#define PL_MAX_HEALTH 1
#define PL_ATTACK_DELAY 32

/// Set up the given entity as a player.
void PL_SetupPlayer(EN_Entity *player, int x, int y);

/// Moves the player entity given the state of the held keys.
void PL_Move(EN_Entity *player, uint32 keys_held);

/// Animates the player by modifying its graphics.
void PL_Animate(EN_Entity *player, u16 *player_gfx, int frame_counter);

#endif