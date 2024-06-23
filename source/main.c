// NDS stuff
#include <nds.h>
// NDS wifi
// #include <dswifi9.h>

// Standard libraries
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Spritesheet
#include "SpriteSheet.h"

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

    u16 *gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles, gfx, 16 * 16);

    dmaCopy(SpriteSheetPal, SPRITE_PALETTE, SpriteSheetPalLen);

    int my_position[2] = {100, 100};
    int frame_counter = 0;
    u8 moving = 0;

    oamSet(&oamMain,                        // Oam
           0,                               // id
           my_position[0], my_position[1],  // x, y
           0,                               // priority
           0,                               // palette alpha
           SpriteSize_16x16,                // sprite size
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

        moving = 0;

        if (keys_held & KEY_LEFT)
        {
            my_position[0] -= 1;
            moving = 1;
        }
        if (keys_held & KEY_RIGHT)
        {
            my_position[0] += 1;
            moving = 1;
        }
        if (keys_held & KEY_UP)
        {
            my_position[1] -= 1;
            moving = 1;
        }
        if (keys_held & KEY_DOWN)
        {
            my_position[1] += 1;
            moving = 1;
        }

        oamSetXY(&oamMain, 0, my_position[0], my_position[1]);

        if (moving)
        {
            dmaCopy((u8 *)SpriteSheetTiles + 16 * 16 * ((frame_counter / 10) % 2 + 1), gfx, 16 * 16);
        }
        else
        {
            dmaCopy((u8 *)SpriteSheetTiles, gfx, 16 * 16);
        }

        frame_counter++;
        UI_PrintDisplayBuffer();
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys_held & KEY_START) break;
    }

    return 0;
}
