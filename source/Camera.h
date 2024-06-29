#ifndef DUNGEON_CRAWLER_CAMERA_H
#define DUNGEON_CRAWLER_CAMERA_H

/**
 * PREFIX: CAM
 */

#include <nds.h>

#include "Entity.h"
#include "Map.h"

#define CAM_WIDTH 256
#define CAM_HEIGHT 192

/// The offset from an entities centre x the scroll should be.
#define CAM_SCROLL_OFFSET_FROM_ENTITY_CENTRE_X CAM_WIDTH / 2
/// The offset from an entities centre y the scroll should be.
#define CAM_SCROLL_OFFSET_FROM_ENTITY_CENTRE_Y CAM_HEIGHT / 2

/// The maximum x scroll value.
#define CAM_SCROLL_MAX_X MAP_WIDTH - CAM_WIDTH
/// The maximum y scroll value.
#define CAM_SCROLL_MAX_Y MAP_HEIGHT - CAM_HEIGHT

typedef struct _CAM_ScrollXY
{
    u16 x;
    u16 y;
} CAM_ScrollXY;

/// Calculates the x and y scroll offsets to centre the camera on the given entity.
CAM_ScrollXY CAM_ScrollOffsetForEntity(Entity *entity);

/// Hides the given oam entry pointed to by `oam_id` if the entity does not appear on screen.
void CAM_HideIfNotOnScreen(Entity *entity, int oam_id, CAM_ScrollXY scroll);

/// Uses oamSetXY to centre all player and enemy entities on the player indicated by `centre_index`.
/// Scrolls the backgrounds to do the same.
void CAM_CentreOnPlayer(Entity players[], int players_len, int centre_index,  //
                        Entity enemies[], int enemies_len,                    //
                        int bg_ids[], int bg_ids_len);

#endif