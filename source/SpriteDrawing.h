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

/// @brief Draws a circle on a sprite.
/// @param gfx Pointer to the sprite graphics data in VRAM.
/// @param sprite_size The sprite size value from the SpriteSize enum.
/// @param center_x The x-coordinate of the circle's center within the sprite.
/// @param center_y The y-coordinate of the circle's center within the sprite.
/// @param radius The radius of the circle.
/// @param palette_index The palette index to set the circle's pixels to.
void SD_draw_circle(u16 *gfx, SpriteSize sprite_size, u8 center_x, u8 center_y, u8 radius, u8 palette_index);

/// @brief Draws a square on a sprite.
/// @param gfx Pointer to the sprite graphics data in VRAM.
/// @param sprite_size The sprite size value from the SpriteSize enum.
/// @param x The x-coordinate of the top-left corner of the square within the sprite.
/// @param y The y-coordinate of the top-left corner of the square within the sprite.
/// @param w The width of the square.
/// @param h The height of the square.
/// @param palette_index The palette index to set the square's pixels to.
void SD_draw_square(u16 *gfx, SpriteSize sprite_size, u8 x, u8 y, u8 w, u8 h, u8 palette_index);

/// @brief Draws a line on a sprite using Bresenham's line algorithm.
/// @param gfx Pointer to the sprite graphics data in VRAM.
/// @param sprite_size The sprite size value from the SpriteSize enum.
/// @param x1 The x-coordinate of the start point of the line.
/// @param y1 The y-coordinate of the start point of the line.
/// @param x2 The x-coordinate of the end point of the line.
/// @param y2 The y-coordinate of the end point of the line.
/// @param palette_index The palette index to set the line's pixels to.
void SD_draw_line(u16 *gfx, SpriteSize sprite_size, u8 x1, u8 y1, u8 x2, u8 y2, u8 palette_index);

#endif