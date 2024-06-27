#include "Entity.h"

void EN_Init(Entity *entity)
{
    entity->state = 0;  //
}

void EN_InitArray(Entity entity_array[], int array_len)
{
    for (int i = 0; i < array_len; i++)
    {
        EN_Init(&entity_array[i]);
    }
}

void EN_Setup(Entity *entity, int x, int y, u8 w, u8 h, u16 max_health, u8 attack_delay)
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

    EN_SetStateBit(entity, EN_ALIVE_BIT);
}

int EN_SetupInSrray(Entity entity_array[], int array_len, int x, int y, u8 w, u8 h, u16 max_health, u8 attack_delay)
{
    for (int i = 0; i < array_len; i++)
    {
        if (!entity_array[i].state)
        {
            EN_Setup(&entity_array[i], x, y, w, h, max_health, attack_delay);
            return i;
        }
    }

    return -1;
}

void EN_TakeDamage(Entity *entity, int damage)
{
    entity->health -= damage;
    if (entity->health <= 0)
    {
        entity->state = 0;
    }
}

void EN_SetStateBit(Entity *entity, int bit) { entity->state |= 1 << bit; }

void EN_ClearStateBit(Entity *entity, int bit) { entity->state &= ~(1 << bit); }

int EN_GetStateBit(Entity *entity, int bit) { return entity->state & (1 << bit); }

int EN_CentreX(Entity *entity) { return entity->x + entity->w / 2; }

int EN_CentreY(Entity *entity) { return entity->y + entity->h / 2; }

float EN_CentreXf(Entity *entity) { return entity->x + (float)entity->w / 2; }

float EN_CentreYf(Entity *entity) { return entity->y + (float)entity->h / 2; }
