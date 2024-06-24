#ifndef DUNGEON_CRAWLER_ENTITY_H
#define DUNGEON_CRAWLER_ENTITY_H

#include <nds.h>

#define ENTITY_ALIVE_BIT 0
#define ENTITY_MOVING_BIT 1

typedef struct _Entity
{
    /// Position
    float x, y;

    /// Size
    u8 w, h;

    /// Health
    u16 health;
    /// The maximum health
    u16 max_health;

    /// The delay between attacks
    u8 attack_delay;
    /// The amount of time to wait till the next attack
    u8 current_attack_delay;

    /// The current frame of animation to be displayed
    u8 animation_frame_number;

    /// The current state of the entity
    /// 0 means dead
    /// Lowest bit set means alive
    u8 state;

    /// Some states take time, so this can be used to track that
    u8 state_counter;

} Entity;

/// Sets up the entity to a default, dead, state.
void EN_init(Entity *entity);

/// Calls Entity_init on every entity in the given array.
void EN_init_array(Entity entity_array[], int array_len);

/// Sets up the entity.
void EN_setup(Entity *entity,  //
              int x, int y,    //
              u8 w, u8 h,      //
              u16 max_health,  //
              u8 attack_delay);

/// Finds the first avaliable space in the given array to set up the entity.
/// Returns the index of the setup entity if successful.
/// Returns -1 if not successful.
int EN_setup_in_array(Entity entity_array[], int array_len,  //
                      int x, int y,                          //
                      u8 w, u8 h,                            //
                      u16 max_health,                        //
                      u8 attack_delay);

/// Applies the given damage to the entity.
/// If the entity will die, the state is set to 0.
void EN_take_damage(Entity *entity, int damage);

/// Set the `bit`th bit of the state
inline void EN_set_state_bit(Entity *entity, int bit);

/// Clears the `bit`th bit of the state
inline void EN_clear_state_bit(Entity *entity, int bit);

/// Gets the `bit`th bit of the state
inline int EN_get_state_bit(Entity *entity, int bit);

#endif