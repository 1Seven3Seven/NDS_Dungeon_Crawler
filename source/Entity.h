#ifndef DUNGEON_CRAWLER_ENTITY_H
#define DUNGEON_CRAWLER_ENTITY_H

/**
 * A single place to include to get all entity code.
 *
 * PREFIX: EN
 */

#include <nds.h>

#define EN_STATE_ALIVE 0

typedef struct _EN_Entity
{
    /// Position.
    float x, y;

    /// Size.
    u8 w, h;

    /// The graphics pointer for this entity.
    /// `EN_Init` sets this to NULL, nothing else touches this.
    /// Exists purely to prevent maintaining another graphics array alongside an entity array.
    u16 *gfx;

    /// What type of entity this is.
    /// -1 means unset.
    s8 type;

    /// Health.
    u16 health;
    /// The maximum health.
    u16 max_health;

    /// The delay between attacks.
    u8 attack_delay;
    /// The amount of time to wait till the next attack.
    u8 current_attack_delay;

    /// The current frame of animation to be displayed.
    u8 animation_frame_number;

    /// The current state of the entity.
    /// 0 means dead.
    /// The lowest bit is reserved to mean alive.
    /// Every other bit can be used however.
    u8 state;

} EN_Entity;

/*
 * Initialising and setting up an entity.
 */

/// Sets up the entity to a default, dead, state.
void EN_Init(EN_Entity *entity);

/// Calls Entity_init on every entity in the given array.
void EN_InitArray(EN_Entity entity_array[], int array_len);

/// Sets up the entity.
void EN_Setup(EN_Entity *entity,  //
              int x, int y,       //
              u8 w, u8 h,         //
              s8 type,            //
              u16 max_health,     //
              u8 attack_delay);

/// Finds the first avaliable space in the given array to set up the entity.
/// Returns the index of the setup entity if successful.
/// Returns -1 if not successful.
int EN_SetupInSrray(EN_Entity entity_array[], int array_len,  //
                    int x, int y,                             //
                    u8 w, u8 h,                               //
                    s8 type,                                  //
                    u16 max_health,                           //
                    u8 attack_delay);

/*
 * Managing entity state.
 */

/// Set the `n`th bit of the state.
void EN_SetStateBit(EN_Entity *entity, int n);

/// Clears the `n`th bit of the state.
void EN_ClearStateBit(EN_Entity *entity, int n);

/// Gets the `n`th bit of the state.
int EN_GetStateBit(EN_Entity *entity, int n);

/*
 * Finding the centre of an entity.
 */

/// Calculates and returns the x position of the center of the entity.
int EN_CentreX(EN_Entity *entity);

/// Calculates and returns the y position of the center of the entity.
int EN_CentreY(EN_Entity *entity);

/// Calculates and returns the x position of the center of the entity.
float EN_CentreXf(EN_Entity *entity);

/// Calculates and returns the y position of the center of the entity.
float EN_CentreYf(EN_Entity *entity);

/*
 * Getting sides of the collision rectangle.
 */

/// Returns the x value of the left of the entity.
int EN_Left(EN_Entity *entity);

/// Returns the x value of the right of the entity.
int EN_Right(EN_Entity *entity);

/// Returns the y value of the top of the entity.
int EN_Top(EN_Entity *entity);

/// Returns the y value of the bottom of the entity.
int EN_Bottom(EN_Entity *entity);

/// Returns the x value of the left of the entity.
float EN_Leftf(EN_Entity *entity);

/// Returns the x value of the right of the entity.
float EN_Rightf(EN_Entity *entity);

/// Returns the y value of the top of the entity.
float EN_Topf(EN_Entity *entity);

/// Returns the y value of the bottom of the entity.
float EN_Bottomf(EN_Entity *entity);

/*
 * Collision check between entities.
 */

/// Checks for a collision between the two entities.
/// Returns 1 if there is a collision, 0 if otherwise.
int EN_Collision(EN_Entity *entity1, EN_Entity *entity2);

/*
 * Setting sides of the collision rectangle.
 */

/// Sets the x value of the left of the entity.
void EN_SetLeft(EN_Entity *entity, int left);

/// Sets the x value of the right of the entity.
void EN_SetRight(EN_Entity *entity, int right);

/// Sets the y value of the top of the entity.
void EN_SetTop(EN_Entity *entity, int top);

/// Sets the y value of the bottom of the entity.
void EN_SetBottom(EN_Entity *entity, int bottom);

/// Sets the x value of the left of the entity.
void EN_SetLeftf(EN_Entity *entity, float left);

/// Sets the x value of the right of the entity.
void EN_SetRightf(EN_Entity *entity, float right);

/// Sets the y value of the top of the entity.
void EN_SetTopf(EN_Entity *entity, float top);

/// Sets the y value of the bottom of the entity.
void EN_SetBottomf(EN_Entity *entity, float bottom);

/*
 * Moving an entity.
 */

#define EN_COLLISION_X 1 << 0
#define EN_COLLISION_Y 1 << 1

/// Moves the entity by the given amount along the x-axis.
/// Performs collision checks against entities in the given `other_entities`.
/// Ignores the entity at `entity_index`, this should point to the entity being moved.
/// Returns `EN_COLLISION_X` if there was a collision, 0 if otherwise.
int EN_MoveX(EN_Entity *entity, float x, EN_Entity other_entities[], int other_entities_len, int entity_index);

/// Moves the entity by the given amount along the y-axis.
/// Performs collision checks against entities in the given `other_entities`.
/// Ignores the entity at `entity_index`, this should point to the entity being moved.
/// Returns `EN_COLLISION_Y` if there was a collision, 0 if otherwise.
int EN_MoveY(EN_Entity *entity, float y, EN_Entity other_entities[], int other_entities_len, int entity_index);

/// Moves the entity by the given amount.
/// Performs collision checks against entities in the given `other_entities`.
/// Ignores the entity at `entity_index`, this should point to the entity being moved.
/// Returns the results of moving x and y bitwise or'd with each other.
int EN_Move(EN_Entity *entity, float x, float y, EN_Entity other_entities[], int other_entities_len, int entity_index);

/*
 * Other.
 */

/// Applies the given damage to the entity.
/// If the entity will die, the state is set to 0.
void EN_TakeDamage(EN_Entity *entity, int damage);

#endif