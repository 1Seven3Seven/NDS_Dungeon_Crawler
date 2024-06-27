#ifndef DUNGEON_CRAWLER_SLIME_H
#define DUNGEON_CRAWLER_SLIME_H

#include "Entity.h"

#define SL_WIDTH 32
#define SL_HEIGHT 32

#define SL_MAX_HEALTH 10
#define SL_ATTACK_DELAY 60

#define SL_INITIAL_VELOCITY 30
#define SL_VELOCITY_REDUCTION 0.1

#define SL_MAX_DISTANCE_ATTACK_PLAYER_SQUARED 100 * 100

typedef struct _SlimeState
{
    /// The direction the slime is moving as a unit vector.
    float vx, vy;

    /// The current velocity the slime is moving multiplied by 10.
    /// So divide by 10 to get the velocity.
    s16 velocity;
} SlimeState;

/// Sets up the given entity as a slime.
/// A `SlimeState` struct must be maintained alongside.
void SL_SetupSlime(Entity *entity, int x, int y);

///
void SL_SetupSlimeState(SlimeState *slime_state);

/*
For slimes, the movement counts as an attack
*/
void SL_Update(Entity *slime, SlimeState *slime_state, Entity *player);

#endif