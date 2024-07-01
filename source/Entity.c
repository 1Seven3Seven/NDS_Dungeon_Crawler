#include "Entity.h"

void EN_Init(EN_Entity *entity)
{
    entity->type = -1;
    entity->state = 0;

    entity->gfx = NULL;
}

void EN_InitArray(EN_Entity entity_array[], int array_len)
{
    for (int i = 0; i < array_len; i++)
    {
        EN_Init(&entity_array[i]);
    }
}

void EN_Setup(EN_Entity *entity, int x, int y, u8 w, u8 h, s8 type, u16 max_health, u8 attack_delay)
{
    entity->x = (float)x;
    entity->y = (float)y;
    entity->w = w;
    entity->h = h;

    entity->type = type;

    entity->max_health = max_health;
    entity->health = max_health;

    entity->attack_delay = attack_delay;
    entity->current_attack_delay = attack_delay;

    entity->animation_frame_number = 0;

    EN_SetStateBit(entity, EN_STATE_ALIVE);
}

int EN_SetupInSrray(EN_Entity entity_array[], int array_len, int x, int y, u8 w, u8 h, s8 type, u16 max_health,
                    u8 attack_delay)
{
    for (int i = 0; i < array_len; i++)
    {
        if (!entity_array[i].state)
        {
            EN_Setup(&entity_array[i], x, y, w, h, type, max_health, attack_delay);
            return i;
        }
    }

    return -1;
}

void EN_TakeDamage(EN_Entity *entity, int damage)
{
    entity->health -= damage;
    if (entity->health <= 0)
    {
        entity->state = 0;
    }
}

void EN_SetStateBit(EN_Entity *entity, int bit) { entity->state |= 1 << bit; }

void EN_ClearStateBit(EN_Entity *entity, int bit) { entity->state &= ~(1 << bit); }

int EN_GetStateBit(EN_Entity *entity, int bit) { return entity->state & (1 << bit); }

int EN_CentreX(EN_Entity *entity) { return entity->x + entity->w / 2; }

int EN_CentreY(EN_Entity *entity) { return entity->y + entity->h / 2; }

float EN_CentreXf(EN_Entity *entity) { return entity->x + (float)entity->w / 2; }

float EN_CentreYf(EN_Entity *entity) { return entity->y + (float)entity->h / 2; }

int EN_Left(EN_Entity *entity) { return entity->x; }

int EN_Right(EN_Entity *entity) { return entity->x + entity->w; }

int EN_Top(EN_Entity *entity) { return entity->y; }

int EN_Bottom(EN_Entity *entity) { return entity->y + entity->h; }

float EN_Leftf(EN_Entity *entity) { return entity->x; }

float EN_Rightf(EN_Entity *entity) { return entity->x + entity->w; }

float EN_Topf(EN_Entity *entity) { return entity->y; }

float EN_Bottomf(EN_Entity *entity) { return entity->y + entity->h; }
