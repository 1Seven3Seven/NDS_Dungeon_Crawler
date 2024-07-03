#include "Entity.h"

/*
 * Initialising and setting up an entity.
 */

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

/*
 * Managing entity state.
 */

void EN_SetStateBit(EN_Entity *entity, int n) { entity->state |= 1 << n; }

void EN_ClearStateBit(EN_Entity *entity, int n) { entity->state &= ~(1 << n); }

int EN_GetStateBit(EN_Entity *entity, int n) { return entity->state & (1 << n); }

/*
 * Finding the centre of an entity.
 */

int EN_CentreX(EN_Entity *entity) { return entity->x + entity->w / 2; }

int EN_CentreY(EN_Entity *entity) { return entity->y + entity->h / 2; }

float EN_CentreXf(EN_Entity *entity) { return entity->x + (float)entity->w / 2; }

float EN_CentreYf(EN_Entity *entity) { return entity->y + (float)entity->h / 2; }

/*
 * Getting sides of the collision rectangle.
 */

int EN_Left(EN_Entity *entity) { return entity->x; }

int EN_Right(EN_Entity *entity) { return entity->x + entity->w; }

int EN_Top(EN_Entity *entity) { return entity->y; }

int EN_Bottom(EN_Entity *entity) { return entity->y + entity->h; }

float EN_Leftf(EN_Entity *entity) { return entity->x; }

float EN_Rightf(EN_Entity *entity) { return entity->x + entity->w; }

float EN_Topf(EN_Entity *entity) { return entity->y; }

float EN_Bottomf(EN_Entity *entity) { return entity->y + entity->h; }

/*
 * Collision check between entities.
 */

int EN_Collision(EN_Entity *entity1, EN_Entity *entity2)
{
    if (EN_Leftf(entity1) < EN_Rightf(entity2))
        if (EN_Rightf(entity1) > EN_Leftf(entity2))
            if (EN_Bottomf(entity1) > EN_Topf(entity2))
                if (EN_Topf(entity1) < EN_Bottomf(entity2))  //
                    return 1;

    return 0;
}

/*
 * Setting sides of the collision rectangle.
 */

void EN_SetLeft(EN_Entity *entity, int left) { entity->x = left; }

void EN_SetRight(EN_Entity *entity, int right) { entity->x = right - entity->w; }

void EN_SetTop(EN_Entity *entity, int top) { entity->y = top; }

void EN_SetBottom(EN_Entity *entity, int bottom) { entity->y = bottom - entity->h; }

void EN_SetLeftf(EN_Entity *entity, float left) { entity->x = left; }

void EN_SetRightf(EN_Entity *entity, float right) { entity->x = right - entity->w; }

void EN_SetTopf(EN_Entity *entity, float top) { entity->y = top; }

void EN_SetBottomf(EN_Entity *entity, float bottom) { entity->y = bottom - entity->h; }

/*
 * Moving an entity.
 */

int EN_MoveX(EN_Entity *entity, float x, EN_Entity other_entities[], int other_entities_len, int entity_index)
{
    int hit_a_hitbox = 0;

    if (x == 0) return hit_a_hitbox;

    entity->x += x;

    for (int i = 0; i < other_entities_len; i++)
    {
        if (i == entity_index) continue;

        EN_Entity *other_entity = &other_entities[i];

        if (EN_Collision(entity, other_entity))
        {
            hit_a_hitbox = EN_COLLISION_X;

            if (x > 0)
                EN_SetRightf(entity, EN_Leftf(other_entity));
            else
                EN_SetLeftf(entity, EN_Rightf(other_entity));
        }
    }

    return hit_a_hitbox;
}

int EN_MoveY(EN_Entity *entity, float y, EN_Entity other_entities[], int other_entities_len, int entity_index)
{
    int hit_a_hitbox = 0;

    if (y == 0) return hit_a_hitbox;

    entity->y += y;

    for (int i = 0; i < other_entities_len; i++)
    {
        if (i == entity_index) continue;

        EN_Entity *other_entity = &other_entities[i];

        if (EN_Collision(entity, other_entity))
        {
            hit_a_hitbox = EN_COLLISION_Y;

            if (y > 0)
                EN_SetBottomf(entity, EN_Topf(other_entity));
            else
                EN_SetTopf(entity, EN_Bottomf(other_entity));
        }
    }

    return hit_a_hitbox;
}

int EN_Move(EN_Entity *entity, float x, float y, EN_Entity other_entities[], int other_entities_len, int entity_index)
{
    int x_collision = EN_MoveX(entity, x, other_entities, other_entities_len, entity_index);
    int y_collision = EN_MoveY(entity, y, other_entities, other_entities_len, entity_index);

    return x_collision | y_collision;
}

/*
 * Other.
 */

void EN_TakeDamage(EN_Entity *entity, int damage)
{
    entity->health -= damage;
    if (entity->health <= 0)
    {
        entity->state = 0;
    }
}
