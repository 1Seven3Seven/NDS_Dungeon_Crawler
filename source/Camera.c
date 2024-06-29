#include "Camera.h"

CAM_ScrollXY CAM_ScrollOffsetForEntity(EN_Entity *entity)
{
    s32 x = EN_CentreX(entity) - CAM_SCROLL_OFFSET_FROM_ENTITY_CENTRE_X;
    s32 y = EN_CentreY(entity) - CAM_SCROLL_OFFSET_FROM_ENTITY_CENTRE_Y;

    if (x < 0)
        x = 0;
    else if (x > CAM_SCROLL_MAX_X)
        x = CAM_SCROLL_MAX_X;

    if (y < 0)
        y = 0;
    else if (y > CAM_SCROLL_MAX_Y)
        y = CAM_SCROLL_MAX_Y;

    CAM_ScrollXY scroll;

    scroll.x = (u16)x;
    scroll.y = (u16)y;

    return scroll;
}

void CAM_HideIfNotOnScreen(EN_Entity *entity, int oam_id, CAM_ScrollXY scroll)
{
    int is_hidden = (EN_Left(entity) > SCREEN_WIDTH + scroll.x)     //
                    || (EN_Right(entity) < scroll.x)                //
                    || (EN_Top(entity) > SCREEN_HEIGHT + scroll.y)  //
                    || (EN_Bottom(entity) < scroll.y);

    oamSetHidden(&oamMain, oam_id, is_hidden);
}

void CAM_CentreOnPlayer(EN_Entity players[], int players_len, int centre_index, EN_Entity enemies[], int enemies_len,
                        int bg_ids[], int bg_ids_len)
{
    /*
    Making some assumptions:
        The player oam IDs start at 0 and continue to `players_len - 1`.
        The enemy oam IDs start at `players_len` and continue to `players_len + enemies_len - 1`.
    */

    CAM_ScrollXY scroll = CAM_ScrollOffsetForEntity(&players[centre_index]);

    for (int i = 0; i < bg_ids_len; i++)
    {
        bgSetScroll(bg_ids[i], scroll.x, scroll.y);
    }

    for (int i = 0; i < players_len; i++)
    {
        CAM_HideIfNotOnScreen(&players[i], i, scroll);
        oamSetXY(&oamMain, i, players[i].x - scroll.x, players[i].y - scroll.y);
    }

    for (int i = 0; i < enemies_len; i++)
    {
        CAM_HideIfNotOnScreen(&enemies[i], players_len + i, scroll);
        oamSetXY(&oamMain, players_len + i, enemies[i].x - scroll.x, enemies[i].y - scroll.y);
    }
}
