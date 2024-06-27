#ifndef DUNGEON_CRAWLER_GFX_H
#define DUNGEON_CRAWLER_GFX_H

#include "BasicBackground.h"
#include "SpriteSheet.h"

/// The size of the sprite sheet tiles as per the .grit file.
/// Actual sprites may be larger or smaller.
#define SPRITE_SIZE 16 * 16
/// How many sprite tiles per row of the sprite sheet.
#define SPRITES_PER_ROW 8
/// The offset required to go down one row of tiles in the sprite sheet.
#define ROW_OFFSET SPRITE_SIZE *SPRITES_PER_ROW

#endif