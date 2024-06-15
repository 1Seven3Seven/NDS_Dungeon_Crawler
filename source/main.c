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
    u16 *gfx_red_square = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);

    for (int i = 0; i < 32 * 32 / 2; i++)
    {
        gfx_red_square[i] = 1 | (1 << 8);
    }

    SD_draw_circle(gfx_red_square, SpriteSize_32x32, 10, 10, 5, 2);
    SD_draw_square(gfx_red_square, SpriteSize_32x32, 10, 10, 10, 10, 3);

    SPRITE_PALETTE[1] = RGB15(31, 0, 0);
    SPRITE_PALETTE[2] = RGB15(0, 31, 0);
    SPRITE_PALETTE[3] = RGB15(0, 0, 31);

    int my_position[2] = {100, 100};
    int frame_counter = 0;

    oamSet(&oamMain,                        // Oam
           0,                               // id
           my_position[0], my_position[1],  // x, y
           0,                               // priority
           0,                               // palette alpha
           SpriteSize_32x32,                // sprite size
           SpriteColorFormat_256Color,      // colour format
           gfx_red_square,                  // graphics pointer
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
