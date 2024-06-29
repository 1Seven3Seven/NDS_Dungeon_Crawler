#ifndef DUNGEON_CRAWLER_SKELETON_H
#define DUNGEON_CRAWLER_SKELETON_H

/**
 * PREFIX: SK
 */

#include "Entity.h"

#define SK_WIDTH 32
#define SK_HEIGHT 32

#define SK_MAX_HEALTH 100
#define SK_ATTACK_DELAY 60

///
void SK_SetupSkeleton(EN_Entity *skeleton, int x, int y);

///
void SK_Update(EN_Entity *skeleton, EN_Entity *player);

///
void SK_Animate(EN_Entity *skeleton, u16 *skeleton_gfx, int frame_counter);

#endif