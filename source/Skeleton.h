#ifndef DUNGEON_CRAWLER_SKELETON_H
#define DUNGEON_CRAWLER_SKELETON_H

#include "Entity.h"

///
void SK_Update(Entity *skeleton, Entity *player);

///
void SL_Animate(Entity *skeleton, u16 *skeleton_gfx, int frame_counter);

#endif