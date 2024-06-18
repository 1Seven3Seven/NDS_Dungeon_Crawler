// NDS stuff
#include <nds.h>
// NDS wifi
// #include <dswifi9.h>

// Standard libraries
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// My libraries
#include "SpriteDrawing.h"
#include "UI.h"

void draw_sprite(u16 *gfx)
{
    // Draw head
    SD_draw_circle(gfx, SpriteSize_32x32, 16, 6, 5, 2);

    // Draw body
    SD_draw_line(gfx, SpriteSize_32x32, 16, 11, 16, 24, 2);

    // Draw left arm
    SD_draw_line(gfx, SpriteSize_32x32, 16, 14, 10, 20, 2);

    // Draw right arm
    SD_draw_line(gfx, SpriteSize_32x32, 16, 14, 22, 20, 2);

    // Draw left leg
    SD_draw_line(gfx, SpriteSize_32x32, 16, 24, 12, 30, 2);

    // Draw right leg
    SD_draw_line(gfx, SpriteSize_32x32, 16, 24, 20, 30, 2);
}

int main(void)
{
    // Setting up the top screen for sprites
    videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    vramSetBankA(VRAM_A_MAIN_SPRITE);
    oamInit(&oamMain, SpriteMapping_1D_128, false);

    // Bottom screen for simple text
    consoleDemoInit();
    UI_ResetDisplayBuffer();

    // A really simple sprite
    u16 *gfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);

    SD_fill(gfx, SpriteSize_32x32, 0);
    draw_sprite(gfx);

    // SD_draw_circle(gfx, SpriteSize_32x32, 10, 10, 5, 2);
    // SD_draw_square(gfx, SpriteSize_32x32, 10, 10, 10, 10, 3);
    // SD_draw_line(gfx, SpriteSize_32x32, 31, 0, 0, 31, 4);

    SPRITE_PALETTE[1] = RGB15(31, 0, 0);
    SPRITE_PALETTE[2] = RGB15(0, 31, 0);
    SPRITE_PALETTE[3] = RGB15(0, 0, 31);
    SPRITE_PALETTE[4] = RGB15(31, 31, 0);

    int my_position[2] = {100, 100};
    int frame_counter = 0;

    oamSet(&oamMain,                        // Oam
           0,                               // id
           my_position[0], my_position[1],  // x, y
           0,                               // priority
           0,                               // palette alpha
           SpriteSize_32x32,                // sprite size
           SpriteColorFormat_256Color,      // colour format
           gfx,                             // graphics pointer
           -1,                              // affine index
           false,                           // size double
           false,                           // hide
           false,                           // h flip
           false,                           // v flip
           false                            // mosaic
    );

    while (1)
    {
        consoleClear();
        scanKeys();
        int keys_held = keysHeld();

        UI_PrintToLine(0, "frame_counter = %d", frame_counter);

        if (keys_held & KEY_LEFT)
        {
            my_position[0] -= 1;
        }
        if (keys_held & KEY_RIGHT)
        {
            my_position[0] += 1;
        }
        if (keys_held & KEY_UP)
        {
            my_position[1] -= 1;
        }
        if (keys_held & KEY_DOWN)
        {
            my_position[1] += 1;
        }

        oamSetXY(&oamMain, 0, my_position[0], my_position[1]);

        frame_counter++;
        UI_PrintDisplayBuffer();
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys_held & KEY_START) break;
    }

    return 0;
}
