#include "SpriteDrawing.h"

#define SD_U16_HIGH_BYTE_MASK 0xFF00
#define SD_U16_LOW_BYTE_MASK 0x00FF

#define SD_GET_WIDTH(width_height) ((width_height & SD_U16_HIGH_BYTE_MASK) >> 8)
#define SD_GET_HEIGHT(width_height) (width_height & SD_U16_LOW_BYTE_MASK)

#define LERP(a, b, c) a + c *(b - a)

u16 SD_deconstruct_sprite_size(SpriteSize sprite_size)
{
    /*
    The maximum sprite size is 64x64 thus we can pack this into a single u8 and unpack it when needed.
    So:
        0000 0000 0000 0000
        ^~~~~~~~~ ^~~~~~~~~
        |         | height
        | width
    */

    switch (sprite_size)
    {
        case SpriteSize_8x8:
            return (8 << 8) | 8;
        case SpriteSize_16x16:
            return (16 << 8) | 16;
        case SpriteSize_32x32:
            return (32 << 8) | 32;
        case SpriteSize_64x64:
            return (64 << 8) | 64;

        case SpriteSize_16x8:
            return (16 << 8) | 8;
        case SpriteSize_32x8:
            return (32 << 8) | 8;
        case SpriteSize_32x16:
            return (32 << 8) | 16;
        case SpriteSize_64x32:
            return (64 << 8) | 32;

        case SpriteSize_8x16:
            return (8 << 8) | 16;
        case SpriteSize_8x32:
            return (8 << 8) | 32;
        case SpriteSize_16x32:
            return (16 << 8) | 32;
        case SpriteSize_32x64:
            return (32 << 8) | 64;

        default:
            return 0;
    }
}

void SD_fill(u16 *gfx, SpriteSize sprite_size, u8 palette_index)
{
    u16 width_height = SD_deconstruct_sprite_size(sprite_size);

    u8 sprite_width = SD_GET_WIDTH(width_height);
    u8 sprite_height = SD_GET_HEIGHT(width_height);

    for (int i = 0; i < sprite_width * sprite_height / 2; i++)
    {
        gfx[i] = palette_index | (palette_index << 8);
    }
}

void SD_set_x_y_to_palette_index(u16 *gfx, SpriteSize sprite_size, u8 x, u8 y, u8 palette_index)
{
    u16 width_height = SD_deconstruct_sprite_size(sprite_size);

    u8 sprite_width = SD_GET_WIDTH(width_height);
    u8 sprite_height = SD_GET_HEIGHT(width_height);

    if (x >= sprite_width || y >= sprite_height)
    {
        return;
    }

    // Find the pixel offset
    u16 offset = 0;

    // First we get the x and y offset inside of the given 8x8 square we are in
    u8 x_in_8x8_square = x % 8;
    u8 y_in_8x8_square = y % 8;

    offset += x_in_8x8_square + y_in_8x8_square * 8;

    // Now we offset by the 8x8 square
    u8 x_8x8_square = x / 8;
    u8 y_8x8_square = y / 8;

    offset += 8 * 8 * x_8x8_square;
    offset += 8 * 8 * (sprite_width / 8) * y_8x8_square;

    // Now convert to a u16 offset
    u16 offset_in_u16 = offset / 2;
    u16 byte_to_set = offset % 2;

    // Grab the u16 to be modified
    u16 current_u16 = gfx[offset_in_u16];
    // Remove the data from the byte we want to set
    // Then add in the byte we want to set
    if (byte_to_set == 0)
    {
        current_u16 &= SD_U16_HIGH_BYTE_MASK;
        current_u16 |= ((u16)palette_index);
    }
    else
    {
        current_u16 &= SD_U16_LOW_BYTE_MASK;
        current_u16 |= ((u16)palette_index) << 8;
    }

    // Finally set the value
    gfx[offset_in_u16] = current_u16;
}

void SD_draw_circle(u16 *gfx, SpriteSize sprite_size, u8 center_x, u8 center_y, u8 radius, u8 palette_index)
{
    u16 width_height = SD_deconstruct_sprite_size(sprite_size);

    u8 sprite_width = SD_GET_WIDTH(width_height);
    u8 sprite_height = SD_GET_HEIGHT(width_height);

    if (center_x >= sprite_width || center_y >= sprite_height)
    {
        return;
    }

    u16 radius_squared = radius * radius;

    for (u8 x = 0; x < sprite_width; x++)
    {
        s8 x_diff = x - center_x;

        for (u8 y = 0; y < sprite_height; y++)
        {
            s8 y_diff = y - center_y;

            u16 distance_squared = x_diff * x_diff + y_diff * y_diff;

            if (distance_squared < radius_squared)
            {
                SD_set_x_y_to_palette_index(gfx, sprite_size, x, y, palette_index);
            }
        }
    }
}

void SD_draw_square(u16 *gfx, SpriteSize sprite_size, u8 x, u8 y, u8 w, u8 h, u8 palette_index)
{
    u16 width_height = SD_deconstruct_sprite_size(sprite_size);

    u8 sprite_width = SD_GET_WIDTH(width_height);
    u8 sprite_height = SD_GET_HEIGHT(width_height);

    if (x >= sprite_width || y >= sprite_height)
    {
        return;
    }

    for (u8 pixel_x = x; pixel_x < x + w; pixel_x++)
    {
        if (pixel_x >= sprite_width) continue;

        for (u8 pixel_y = y; pixel_y < y + h; pixel_y++)
        {
            if (pixel_y >= sprite_height) continue;

            SD_set_x_y_to_palette_index(gfx, sprite_size, pixel_x, pixel_y, palette_index);
        }
    }
}

void SD_draw_line(u16 *gfx, SpriteSize sprite_size, u8 x1, u8 y1, u8 x2, u8 y2, u8 palette_index)
{
    u16 width_height = SD_deconstruct_sprite_size(sprite_size);

    u8 sprite_width = SD_GET_WIDTH(width_height);
    u8 sprite_height = SD_GET_HEIGHT(width_height);

    if (x1 >= sprite_width || y1 >= sprite_height || x2 >= sprite_width || y2 >= sprite_height)
    {
        return;
    }

    int x_diff = x1 - x2;
    if (x_diff < 0) x_diff = -x_diff;
    int y_diff = y1 - y2;
    if (y_diff > 0) y_diff = -y_diff;

    int x_step = x1 < x2 ? 1 : -1;
    int y_step = y1 < y2 ? 1 : -1;
    int error = x_diff + y_diff;

    while (1)
    {
        SD_set_x_y_to_palette_index(gfx, sprite_size, x1, y1, palette_index);

        if (x1 == x2 && y1 == y2) break;

        int error2 = error * 2;

        if (error2 >= y_diff)
        {
            error += y_diff;
            x1 += x_step;
        }
        if (error2 <= x_diff)
        {
            error += x_diff;
            y1 += y_step;
        }
    }
}
