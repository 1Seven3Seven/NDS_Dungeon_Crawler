#ifndef DUNGEON_CRAWLER_SKELETON_H
#define DUNGEON_CRAWLER_SKELETON_H

/**
 * PREFIX: SK
 */

#include "Entity.h"

#define SK_WIDTH 7
#define SK_HEIGHT 15

#define SK_MAX_HEALTH 100
#define SK_ATTACK_DELAY 60

#define SK_MIN_DISTANCE_TO_ATTACK_PLAYER_SQUARED 32 * 32

///
void SK_SetupSkeleton(EN_Entity *skeleton, int x, int y);

///
void SK_Move(EN_Entity *skeleton, EN_Entity players[], int players_len);

///
void SK_Animate(EN_Entity *skeleton, u16 *skeleton_gfx, int frame_counter);

#endif