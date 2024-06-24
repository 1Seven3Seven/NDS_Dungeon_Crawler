#include "Entity.h"

void EN_init(Entity *entity)
{
    entity->state = 0;  //
}

void EN_init_array(Entity entity_array[], int array_len)
{
    for (int i = 0; i < array_len; i++)
    {
        EN_init(&entity_array[i]);
    }
}

void EN_setup(Entity *entity, int x, int y, u8 w, u8 h, u16 max_health, u8 attack_delay)
{
    entity->x = (float)x;
    entity->y = (float)y;
    entity->w = w;
    entity->h = h;

    entity->max_health = max_health;
    entity->health = max_health;

    entity->attack_delay = attack_delay;
    entity->current_attack_delay = attack_delay;

    entity->animation_frame_number = 0;

    EN_set_state_bit(entity, ENTITY_ALIVE_BIT);
    entity->state_counter = 0;
}

int EN_setup_in_array(Entity entity_array[], int array_len, int x, int y, u8 w, u8 h, u16 max_health, u8 attack_delay)
{
    for (int i = 0; i < array_len; i++)
    {
        if (!entity_array[i].state)
        {
            EN_setup(&entity_array[i], x, y, w, h, max_health, attack_delay);
            return i;
        }
    }

    return -1;
}

void EN_take_damage(Entity *entity, int damage)
{
    entity->health -= damage;
    if (entity->health <= 0)
    {
        entity->state = 0;
    }
}

extern inline void EN_set_state_bit(Entity *entity, int bit)
{
    entity->state |= 1 << bit;  //
}

extern inline void EN_clear_state_bit(Entity *entity, int bit)
{
    entity->state &= ~(1 << bit);  //
}

extern inline int EN_get_state_bit(Entity *entity, int bit)
{
    return entity->state & (1 << bit);  //
}
