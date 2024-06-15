#ifndef SPRITE_DRAWING_H
#define SPRITE_DRAWING_H

#include <nds.h>

/// @brief Deconstructs the sprite size to extract the width and height of the sprite.
/// @param sprite_size The sprite size value from the SpriteSize enum.
/// @return A u16 value where the higher byte represents the width and the lower byte represents the height.
u16 SD_deconstruct_sprite_size(SpriteSize sprite_size);

/// @brief Sets a specific pixel in a sprite to a particular palette index.
/// @param gfx Pointer to the sprite graphics data in VRAM.
/// @param sprite_size The sprite size value from the SpriteSize enum.
/// @param x The x-coordinate of the pixel within the sprite.
/// @param y The y-coordinate of the pixel within the sprite.
/// @param palette_index The palette index to set the pixel to.
void SD_set_x_y_to_palette_index(u16 *gfx, SpriteSize sprite_size, u8 x, u8 y, u8 palette_index);

#endif