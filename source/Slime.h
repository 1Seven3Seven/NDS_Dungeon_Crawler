#ifndef DUNGEON_CRAWLER_SLIME_H
#define DUNGEON_CRAWLER_SLIME_H

/**
 * PREFIX: SL
 */

#include <nds.h>

#include "Entity.h"

#define SL_WIDTH 14
#define SL_HEIGHT 8

#define SL_MAX_HEALTH 10
#define SL_ATTACK_DELAY 60

#define SL_INITIAL_VELOCITY 30
#define SL_VELOCITY_REDUCTION 0.1

#define SL_MAX_DISTANCE_ATTACK_PLAYER_SQUARED 72 * 72

typedef enum _SL_State
{
    SL_MOVING = 1,
} SL_State;

typedef struct _SL_SlimeState
{
    /// The direction the slime is moving as a unit vector.
    float vx, vy;

    /// The current velocity the slime is moving multiplied by 10.
    /// So divide by 10 to get the velocity.
    s16 velocity;
} SL_SlimeState;

/// Sets up the given entity as a slime.
/// A `SL_SlimeState` struct must be maintained alongside.
void SL_SetupSlime(EN_Entity *entity, int x, int y);

///
void SL_SetupSlimeState(SL_SlimeState *slime_state);

///
void SL_SetupSlimeStateArray(SL_SlimeState slime_states[], int slime_states_len);

/*
For slimes, the movement counts as an attack
*/
void SL_Move(EN_Entity *slime, SL_SlimeState *slime_state, EN_Entity players[], int players_len);

/// Animates the slime by modifying its graphics
void SL_Animate(EN_Entity *slime, int frame_counter);

#endif