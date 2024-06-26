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
#include "BasicBackground.h"
#include "SpriteSheet.h"

// My libraries
#include "Entity.h"
#include "Skeleton.h"
#include "SpriteDrawing.h"
#include "UI.h"

#define SPRITE_SIZE 16 * 16
#define SPRITES_PER_ROW 8
#define ROW_OFFSET SPRITE_SIZE *SPRITES_PER_ROW

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192

#define MAP_WIDTH 512
#define MAP_HEIGHT 512

#define PLAYER_WIDTH 11
#define PLAYER_HEIGHT 15

#define PLAYER_MOVE_SPEED 1

/// Centre of the player at the centre of the map
#define PLAYER_START_X MAP_WIDTH / 2 - PLAYER_WIDTH / 2
/// Centre of the player at the centre of the map
#define PLAYER_START_Y MAP_HEIGHT / 2 - PLAYER_HEIGHT / 2

/// The normal oam x value when the player is not at the map edges (at the centre of the screen)
#define PLAYER_NORMAL_OAM_X SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2
/// The normal oam y value when the player is not at the map edges (at the centre of the screen)
#define PLAYER_NORMAL_OAM_Y SCREEN_HEIGHT / 2 - PLAYER_HEIGHT / 2

/// The maximum x value the player can travel to
#define PLAYER_MAX_X 512 - PLAYER_WIDTH
/// The maximum y value the player can travel to
#define PLAYER_MAX_Y 512 - PLAYER_HEIGHT

/// The offset from the player centre's x position the scroll should be
#define SCROLL_OFFSET_FROM_PLAYER_X SCREEN_WIDTH / 2
/// The offset from the player centre's y position the scroll should be
#define SCROLL_OFFSET_FROM_PLAYER_Y SCREEN_HEIGHT / 2

/// The maximum x scroll value
#define SCROLL_MAX_X MAP_WIDTH - SCREEN_WIDTH
/// The maximum y scroll value
#define SCROLL_MAX_Y MAP_HEIGHT - SCREEN_HEIGHT

void move_player(Entity *player, int keys)
{
    EN_ClearStateBit(player, EN_MOVING_BIT);

    if (keys & KEY_LEFT)
    {
        player->x -= PLAYER_MOVE_SPEED;
        EN_SetStateBit(player, EN_MOVING_BIT);
    }
    if (keys & KEY_RIGHT)
    {
        player->x += PLAYER_MOVE_SPEED;
        EN_SetStateBit(player, EN_MOVING_BIT);
    }
    if (keys & KEY_UP)
    {
        player->y -= PLAYER_MOVE_SPEED;
        EN_SetStateBit(player, EN_MOVING_BIT);
    }
    if (keys & KEY_DOWN)
    {
        player->y += PLAYER_MOVE_SPEED;
        EN_SetStateBit(player, EN_MOVING_BIT);
    }

    if (player->x < 0) player->x = 0;
    if (player->x > PLAYER_MAX_X) player->x = PLAYER_MAX_X;
    if (player->y < 0) player->y = 0;
    if (player->y > PLAYER_MAX_Y) player->y = PLAYER_MAX_Y;
}

void animate_player(u16 *player_gfx, int frame_counter, Entity *player_entity)
{
    if (EN_GetStateBit(player_entity, EN_MOVING_BIT))
    {
        player_entity->animation_frame_number = 2 + (frame_counter / 10) % 2;
    }
    else
    {
        player_entity->animation_frame_number = (frame_counter / 30) % 2;
    }

    dmaCopy((u8 *)SpriteSheetTiles + SPRITE_SIZE * player_entity->animation_frame_number, player_gfx, SPRITE_SIZE);
}

void animate_skeleton(u16 *skeleton_gfx, int frame_counter, Entity *skeleton_entity)
{
    if (EN_GetStateBit(skeleton_entity, EN_MOVING_BIT))
    {
        skeleton_entity->animation_frame_number = 2 + (frame_counter / 10) % 2;
    }
    else
    {
        skeleton_entity->animation_frame_number = (frame_counter / 30) % 2;
    }

    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET + SPRITE_SIZE * skeleton_entity->animation_frame_number,  //
            skeleton_gfx,                                                                                 //
            SPRITE_SIZE);
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
    vramSetBankB(VRAM_B_MAIN_BG_0x06000000);
    oamInit(&oamMain, SpriteMapping_1D_128, false);

    // Bottom screen for simple text
    consoleDemoInit();
    UI_ResetDisplayBuffer();

    // Background
    int bg = bgInit(0, BgType_Text8bpp, BgSize_T_512x512, 0, 1);
    dmaCopy(BasicBackgroundTiles, BG_TILE_RAM(1), BasicBackgroundTilesLen);
    dmaCopy(BasicBackgroundMap, BG_MAP_RAM(0), BasicBackgroundMapLen);
    dmaCopy(BasicBackgroundPal, BG_PALETTE, BasicBackgroundPalLen);

    // Loading some graphics

    Entity player_entity;
    EN_Setup(&player_entity, PLAYER_START_X, PLAYER_START_Y, PLAYER_WIDTH, PLAYER_HEIGHT, 1, 1);
    u16 *player_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles, player_gfx, SPRITE_SIZE);

    Entity skeleton_entity;
    EN_Setup(&skeleton_entity, 10, 10, 32, 32, 1, 1);
    u16 *skeleton_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET, skeleton_gfx, SPRITE_SIZE);

    Entity slime_entity;
    EN_Setup(&slime_entity, 10, 50, 32, 32, 1, 1);
    u16 *slime_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET * 2, slime_gfx, SPRITE_SIZE);

    dmaCopy(SpriteSheetPal, SPRITE_PALETTE, SpriteSheetPalLen);

    int frame_counter = 0;

    // Initial x and y should not matter as we set the x and y each frame
    oamSet(&oamMain,                    // Oam
           0,                           // id
           0, 0,                        // x, y
           0,                           // priority
           0,                           // palette alpha
           SpriteSize_16x16,            // sprite size
           SpriteColorFormat_256Color,  // colour format
           player_gfx,                  // graphics pointer
           -1,                          // affine index
           false,                       // size double
           false,                       // hide
           false,                       // h flip
           false,                       // v flip
           false                        // mosaic
    );

    oamSet(&oamMain,                    // Oam
           1,                           // id
           0, 0,                        // x, y
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
           0, 0,                        // x, y
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

        move_player(&player_entity, keys_held);

        SK_Update(&skeleton_entity, player_entity);

        animate_player(player_gfx, frame_counter, &player_entity);
        animate_skeleton(skeleton_gfx, frame_counter, &skeleton_entity);
        animate_slime(slime_gfx, 0, frame_counter);

        int player_centre_x = EN_CentreX(&player_entity);
        int player_centre_y = EN_CentreY(&player_entity);

        int scroll_x = player_centre_x - SCROLL_OFFSET_FROM_PLAYER_X;
        int scroll_y = player_centre_y - SCROLL_OFFSET_FROM_PLAYER_Y;

        if (scroll_x < 0)
        {
            scroll_x = 0;
        }
        else if (scroll_x > SCROLL_MAX_X)
        {
            scroll_x = SCROLL_MAX_X;
        }

        if (scroll_y < 0)
        {
            scroll_y = 0;
        }
        else if (scroll_y > SCROLL_MAX_Y)
        {
            scroll_y = SCROLL_MAX_Y;
        }

        bgSetScroll(bg, scroll_x, scroll_y);

        oamSetXY(&oamMain, 0, player_entity.x - scroll_x, player_entity.y - scroll_y);
        oamSetXY(&oamMain, 1, skeleton_entity.x - scroll_x, skeleton_entity.y - scroll_y);
        oamSetXY(&oamMain, 2, slime_entity.x - scroll_x, slime_entity.y - scroll_y);

        UI_PrintToLine(0, "frame_counter = %d", frame_counter);
        UI_PrintToLine(1, "Player pos    = %03d, %03d", (int)player_entity.x, (int)player_entity.y);
        UI_PrintToLine(2, "BG scroll     = %03d, %03d", scroll_x, scroll_y);

        UI_PrintToLine(4, "Skeleton pos  = %03d, %03d", (int)skeleton_entity.x, (int)skeleton_entity.y);

        frame_counter++;
        UI_PrintDisplayBuffer();
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        bgUpdate();
        if (keys_held & KEY_START) break;
    }

    return 0;
}
