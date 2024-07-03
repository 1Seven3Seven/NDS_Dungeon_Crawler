#include "Enemies.h"

#include "EntityTypes.h"
#include "Skeleton.h"
#include "Slime.h"

void ENM_MoveEnemyArray(EN_Entity enemies[], int enemies_len, SL_SlimeState slime_states[], int slime_states_len,
                        EN_Entity players[], int players_len)
{
    u8 slimes_encountered = 0;

    for (int i = 0; i < enemies_len; i++)
    {
        switch (enemies[i].type)
        {
            case ET_SKELETON:
                SK_Move(&enemies[i], players, players_len);
                break;

            case ET_SLIME:
                SL_Move(&enemies[i], &slime_states[slimes_encountered], players, players_len);
                slimes_encountered++;
                break;
        }
    }
}

void ENM_AnimateEnemyArray(EN_Entity enemies[], int enemies_len, int frame_counter)
{
    for (int i = 0; i < enemies_len; i++)
    {
        switch (enemies[i].type)
        {
            case ET_SKELETON:
                SK_Animate(&enemies[i], frame_counter);
                break;

            case ET_SLIME:
                SL_Animate(&enemies[i], frame_counter);
                break;
        }
    }
}
