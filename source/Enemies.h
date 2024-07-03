#ifndef DUNGEON_CRAWLER_ENEMIES_H
#define DUNGEON_CRAWLER_ENEMIES_H

/**
 * PREFIX: ENM
 */

#include "Entity.h"
#include "Slime.h"

void ENM_MoveEnemyArray(EN_Entity enemies[], int enemies_len,                //
                        SL_SlimeState slime_states[], int slime_states_len,  //
                        EN_Entity players[], int players_len);

void ENM_AnimateEnemyArray(EN_Entity enemies[], int enemies_len, int frame_counter);

#endif