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

#define SPRITE_SIZE 16 * 16
#define SPRITES_PER_ROW 8
#define ROW_OFFSET SPRITE_SIZE *SPRITES_PER_ROW

void animate_player(u16 *player_gfx, int moving, int frame_counter)
{
    if (moving)
    {
        dmaCopy((u8 *)SpriteSheetTiles + SPRITE_SIZE * (2 + (frame_counter / 10) % 2), player_gfx, SPRITE_SIZE);
    }
    else
    {
        dmaCopy((u8 *)SpriteSheetTiles + SPRITE_SIZE * ((frame_counter / 30) % 2), player_gfx, SPRITE_SIZE);
    }
}

void animate_skeleton(u16 *skeleton_gfx, int moving, int frame_counter)
{
    if (moving)
    {
        dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET + SPRITE_SIZE * (2 + (frame_counter / 10) % 2),  //
                skeleton_gfx,                                                                        //
                SPRITE_SIZE);
    }
    else
    {
        dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET + SPRITE_SIZE * ((frame_counter / 30) % 2),  //
                skeleton_gfx,                                                                    //
                SPRITE_SIZE);
    }
}

void animate_slime(u16 *slime_gfx, int moving, int frame_counter)
{
    if (moving)
    {
        dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET * 2 + SPRITE_SIZE * (2 + (frame_counter / 10) % 2),  //
                slime_gfx,                                                                               //
                SPRITE_SIZE);
    }
    else
    {
        dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET * 2 + SPRITE_SIZE * ((frame_counter / 30) % 2),  //
                slime_gfx,                                                                           //
                SPRITE_SIZE);
    }
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

    // Loading some graphics

    u16 *player_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles, player_gfx, SPRITE_SIZE);

    u16 *skeleton_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET, skeleton_gfx, SPRITE_SIZE);

    u16 *slime_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET * 2, slime_gfx, SPRITE_SIZE);

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
           player_gfx,                      // graphics pointer
           -1,                              // affine index
           false,                           // size double
           false,                           // hide
           false,                           // h flip
           false,                           // v flip
           false                            // mosaic
    );

    oamSet(&oamMain,                    // Oam
           1,                           // id
           10, 10,                      // x, y
           0,                           // priority
           0,                           // palette alpha
           SpriteSize_16x16,            // sprite size
           SpriteColorFormat_256Color,  // colour format
           skeleton_gfx,                // graphics pointer
           -1,                          // affine index
           false,                       // size double
           false,                       // hide
           false,                       // h flip
           false,                       // v flip
           false                        // mosaic
    );

    oamSet(&oamMain,                    // Oam
           2,                           // id
           10, 50,                      // x, y
           0,                           // priority
           0,                           // palette alpha
           SpriteSize_16x16,            // sprite size
           SpriteColorFormat_256Color,  // colour format
           slime_gfx,                   // graphics pointer
           -1,                          // affine index
           false,                       // size double
           false,                       // hide
           false,                       // h flip
           false,                       // v flip
           false                        // mosaic
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

        animate_player(player_gfx, moving, frame_counter);
        animate_skeleton(skeleton_gfx, 0, frame_counter);
        animate_slime(slime_gfx, 0, frame_counter);

        frame_counter++;
        UI_PrintDisplayBuffer();
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys_held & KEY_START) break;
    }

    return 0;
}
