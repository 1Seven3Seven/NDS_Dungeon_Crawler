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
#include "Camera.h"
#include "Entity.h"
#include "GFX.h"  // Includes the sprite sheet and backgrounds
#include "Map.h"
#include "Player.h"
#include "Skeleton.h"
#include "Slime.h"
#include "SpriteDrawing.h"
#include "UI.h"

#define KEY_ARROWS (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN)

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192

#define PLAYER_WIDTH 11
#define PLAYER_HEIGHT 15

#define PLAYER_MOVE_SPEED 1

/// Centre of the player at the centre of the map
#define PLAYER_START_X MAP_WIDTH / 2 - PLAYER_WIDTH / 2
/// Centre of the player at the centre of the map
#define PLAYER_START_Y MAP_HEIGHT / 2 - PLAYER_HEIGHT / 2

/// The offset from the player centre's x position the scroll should be
#define SCROLL_OFFSET_FROM_ENTITY_CENTRE_X SCREEN_WIDTH / 2
/// The offset from the player centre's y position the scroll should be
#define SCROLL_OFFSET_FROM_ENTITY_CENTRE_Y SCREEN_HEIGHT / 2

/// The maximum x scroll value
#define SCROLL_MAX_X MAP_WIDTH - SCREEN_WIDTH
/// The maximum y scroll value
#define SCROLL_MAX_Y MAP_HEIGHT - SCREEN_HEIGHT

#define NUM_BACKGROUNDS 1
#define NUM_PLAYERS 1
#define NUM_ENEMIES 2

#define SKELETON_INDEX 0
#define SLIME_INDEX 1

void display_entity_position(int line_number, const char *prepend, Entity *entity)
{
    UI_PrintToLine(line_number, "%s%03d, %03d", prepend, (int)entity->x, (int)entity->y);
}

void draw_pointer(u16 *gfx, SpriteSize sprite_size, int vx, int vy, u8 palette_index)
{
    SD_fill(gfx, sprite_size, 0);

    if (vx == 0 && vy == 0) return;

    u8 x1 = 4 - vx * 4;
    u8 y1 = 4 - vy * 4;

    if (x1 == 8) x1 = 7;
    if (y1 == 8) y1 = 7;

    u8 x2 = 4 + vx * 4;
    u8 y2 = 4 + vy * 4;

    if (x2 == 8) x2 = 7;
    if (y2 == 8) y2 = 7;

    SD_draw_line(gfx, sprite_size, x1, y1, x2, y2, palette_index);
}

s8 x = -1, y = -1, w = -1, h = -1;

void draw_attack_hitbox(u16 *gfx, SpriteSize sprite_size, s8 vx, s8 vy, u8 palette_index)
{
    SD_fill(gfx, SpriteSize_16x16, 0);

    if (vx == 0 && vy == 0) return;

    x = vx < 0 ? 8 : 0;  // If pointing left then x = 8, else x = 0
    y = vy < 0 ? 8 : 0;  // If pointing up then   y = 8, else y = 0

    w = vx ? 8 : 16;  // If pointing left or right w = 8, else w = 16
    h = vy ? 8 : 16;  // If pointing up or down    h = 8, else h = 16

    // Adjust for diagonals
    if (vx && vy)
    {
        x -= vx < 0 ? 4 : 0;
        y -= vy < 0 ? 4 : 0;

        w += 4;
        h += 4;
    }

    SD_draw_square(gfx, SpriteSize_16x16, x, y, w, h, palette_index);
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
    int bg_ids[NUM_BACKGROUNDS];
    bg_ids[0] = bgInit(0, BgType_Text8bpp, BgSize_T_512x512, 0, 1);
    dmaCopy(BasicBackgroundTiles, BG_TILE_RAM(1), BasicBackgroundTilesLen);
    dmaCopy(BasicBackgroundMap, BG_MAP_RAM(0), BasicBackgroundMapLen);
    dmaCopy(BasicBackgroundPal, BG_PALETTE, BasicBackgroundPalLen);

    // Loading some graphics

    u16 *player_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles, player_gfx, SPRITE_SIZE);

    u16 *skeleton_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET, skeleton_gfx, SPRITE_SIZE);

    u16 *slime_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET * 2, slime_gfx, SPRITE_SIZE);

    u16 *attack_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    SD_fill(attack_gfx, SpriteSize_16x16, 0);

    u16 *pointer_gfx = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_256Color);
    SD_fill(pointer_gfx, SpriteSize_8x8, 0);

    dmaCopy(SpriteSheetPal, SPRITE_PALETTE, SpriteSheetPalLen);
    SPRITE_PALETTE[173] = RGB15(31, 31, 31);

    // Setting up a player entity list
    Entity players[NUM_PLAYERS];
    EN_InitArray(players, NUM_PLAYERS);

    // Setting up the players
    PL_SetupPlayer(&players[0], PLAYER_START_X, PLAYER_START_Y);

    // Setting up an enemy entity list
    Entity enemies[NUM_ENEMIES];
    EN_InitArray(players, NUM_ENEMIES);

    // Setting up the enemies

    // The skeleton
    EN_Setup(&enemies[SKELETON_INDEX], 10, 10, 32, 32, 1, 1);

    // The slime
    SL_SetupSlime(&enemies[SLIME_INDEX], 250, 200);
    SlimeState slime_state;
    SL_SetupSlimeState(&slime_state);

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

    oamSet(&oamMain,                    // Oam
           3,                           // id
           0, 0,                        // x, y
           0,                           // priority
           0,                           // palette alpha
           SpriteSize_16x16,            // sprite size
           SpriteColorFormat_256Color,  // colour format
           attack_gfx,                  // graphics pointer
           -1,                          // affine index
           false,                       // size double
           false,                       // hide
           false,                       // h flip
           false,                       // v flip
           false                        // mosaic
    );

    oamSet(&oamMain,                    // Oam
           4,                           // id
           10, 10,                      // x, y
           0,                           // priority
           0,                           // palette alpha
           SpriteSize_8x8,              // sprite size
           SpriteColorFormat_256Color,  // colour format
           pointer_gfx,                 // graphics pointer
           -1,                          // affine index
           false,                       // size double
           false,                       // hide
           false,                       // h flip
           false,                       // v flip
           false                        // mosaic
    );

    int frame_counter = 0;
    int attack_counter = 0;
    s8 vx = 1, vy = 0;

    while (1)
    {
        consoleClear();
        scanKeys();
        uint32 keys_held = keysHeld();
        uint32 keys_down = keysDown();

        PL_Move(&players[0], keys_held);

        SK_Update(&enemies[SKELETON_INDEX], &players[0]);
        SL_Update(&enemies[SLIME_INDEX], &slime_state, &players[0]);

        PL_Animate(&players[0], player_gfx, frame_counter);
        SL_Animate(&enemies[SKELETON_INDEX], skeleton_gfx, frame_counter);
        SL_Animate(&enemies[SLIME_INDEX], slime_gfx, frame_counter);

        CAM_CentreOnPlayer(players, NUM_PLAYERS, 0, enemies, NUM_ENEMIES, bg_ids, NUM_BACKGROUNDS);

        UI_PrintToLine(0, "frame_counter = %d", frame_counter);
        display_entity_position(1, "Player pos   = ", &players[0]);
        display_entity_position(2, "Skeleton pos = ", &enemies[SKELETON_INDEX]);
        display_entity_position(3, "Slime pos    = ", &enemies[SLIME_INDEX]);

        frame_counter++;
        UI_PrintDisplayBuffer();
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        bgUpdate();
        if (keys_held & KEY_START) break;
    }

    return 0;
}
