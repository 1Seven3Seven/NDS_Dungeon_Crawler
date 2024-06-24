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
#include "Entity.h"
#include "SpriteDrawing.h"
#include "UI.h"

#define SPRITE_SIZE 16 * 16
#define SPRITES_PER_ROW 8
#define ROW_OFFSET SPRITE_SIZE *SPRITES_PER_ROW

void animate_player(u16 *player_gfx, int frame_counter, Entity *player_entity)
{
    if (EN_get_state_bit(player_entity, ENTITY_MOVING_BIT))
    {
        player_entity->animation_frame_number = 2 + (frame_counter / 10) % 2;
    }
    else
    {
        player_entity->animation_frame_number = (frame_counter / 30) % 2;
    }

    dmaCopy((u8 *)SpriteSheetTiles + SPRITE_SIZE * player_entity->animation_frame_number, player_gfx, SPRITE_SIZE);
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

    Entity player_entity;
    EN_setup(&player_entity, 100, 100, 32, 32, 1, 1);
    u16 *player_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles, player_gfx, SPRITE_SIZE);

    Entity skeleton_entity;
    EN_init(&skeleton_entity);
    u16 *skeleton_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET, skeleton_gfx, SPRITE_SIZE);

    Entity slime_entity;
    EN_init(&slime_entity);
    u16 *slime_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET * 2, slime_gfx, SPRITE_SIZE);

    dmaCopy(SpriteSheetPal, SPRITE_PALETTE, SpriteSheetPalLen);

    int frame_counter = 0;

    oamSet(&oamMain,                          // Oam
           0,                                 // id
           player_entity.x, player_entity.y,  // x, y
           0,                                 // priority
           0,                                 // palette alpha
           SpriteSize_16x16,                  // sprite size
           SpriteColorFormat_256Color,        // colour format
           player_gfx,                        // graphics pointer
           -1,                                // affine index
           false,                             // size double
           false,                             // hide
           false,                             // h flip
           false,                             // v flip
           false                              // mosaic
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

        // Clear the moving bit
        EN_clear_state_bit(&player_entity, ENTITY_MOVING_BIT);

        if (keys_held & KEY_LEFT)
        {
            player_entity.x -= 1;
            EN_set_state_bit(&player_entity, ENTITY_MOVING_BIT);
        }
        if (keys_held & KEY_RIGHT)
        {
            player_entity.x += 1;
            EN_set_state_bit(&player_entity, ENTITY_MOVING_BIT);
        }
        if (keys_held & KEY_UP)
        {
            player_entity.y -= 1;
            EN_set_state_bit(&player_entity, ENTITY_MOVING_BIT);
        }
        if (keys_held & KEY_DOWN)
        {
            player_entity.y += 1;
            EN_set_state_bit(&player_entity, ENTITY_MOVING_BIT);
        }

        oamSetXY(&oamMain, 0, player_entity.x, player_entity.y);

        animate_player(player_gfx, frame_counter, &player_entity);
        animate_skeleton(skeleton_gfx, 0, frame_counter);
        animate_slime(slime_gfx, 0, frame_counter);

        UI_PrintU16Bits(5, player_entity.state);

        frame_counter++;
        UI_PrintDisplayBuffer();
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys_held & KEY_START) break;
    }

    return 0;
}
