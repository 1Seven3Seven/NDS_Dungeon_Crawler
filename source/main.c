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

/// Centre of the player at the centre of the map
#define PLAYER_START_X MAP_WIDTH / 2 - PL_WIDTH / 2
/// Centre of the player at the centre of the map
#define PLAYER_START_Y MAP_HEIGHT / 2 - PL_HEIGHT / 2

#define KEY_ARROWS (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN)

#define NUM_BACKGROUNDS 1
#define NUM_PLAYERS 2
#define NUM_ENEMIES 2

#define SKELETON_INDEX 0
#define SLIME_INDEX 1

void display_entity_position(int line_number, const char *prepend, EN_Entity *entity)
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

void draw_attack_hitbox(u16 *gfx, SpriteSize sprite_size, s8 vx, s8 vy, u8 palette_index)
{
    SD_fill(gfx, SpriteSize_16x16, 0);

    if (vx == 0 && vy == 0) return;

    s8 x = -1, y = -1, w = -1, h = -1;

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

void oamSetGFXSizeColour(OamState *oam, int id, SpriteSize size, SpriteColorFormat format, const void *gfxOffset)
{
    oamSet(oam,        // Oam
           id,         // id
           0, 0,       // x, y
           0,          // priority
           0,          // palette alpha
           size,       // sprite size
           format,     // colour format
           gfxOffset,  // graphics pointer
           -1,         // affine index
           false,      // size double
           false,      // hide
           false,      // h flip
           false,      // v flip
           false       // mosaic
    );
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
    // No need to use dmaCopy as the animate functions do that each frame
    u16 *player_gfx[NUM_PLAYERS];
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        player_gfx[i] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    }

    u16 *skeleton_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    u16 *slime_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);

    dmaCopy(SpriteSheetPal, SPRITE_PALETTE, SpriteSheetPalLen);

    // Setting up a player entity list
    EN_Entity players[NUM_PLAYERS];
    EN_InitArray(players, NUM_PLAYERS);

    // Setting up the players
    PL_SetupPlayer(&players[0], PLAYER_START_X, PLAYER_START_Y);
    PL_SetupPlayer(&players[1], PLAYER_START_X + 50, PLAYER_START_Y + 50);

    // Setting up an enemy entity list
    EN_Entity enemies[NUM_ENEMIES];
    EN_InitArray(players, NUM_ENEMIES);

    // Setting up the enemies

    // The skeleton
    SK_SetupSkeleton(&enemies[SKELETON_INDEX], 10, 10);

    // The slime
    SL_SetupSlime(&enemies[SLIME_INDEX], 250, 200);
    SL_SlimeState slime_state;
    SL_SetupSlimeState(&slime_state);

    // Setting oam entries

    // Initial x and y should not matter as we set the x and y each frame
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        oamSetGFXSizeColour(&oamMain, i, SpriteSize_16x16, SpriteColorFormat_256Color, player_gfx[i]);
    }

    // Slime gfx
    oamSetGFXSizeColour(&oamMain, NUM_PLAYERS, SpriteSize_16x16, SpriteColorFormat_256Color, skeleton_gfx);

    // Skeleton gfx
    oamSetGFXSizeColour(&oamMain, NUM_PLAYERS + 1, SpriteSize_16x16, SpriteColorFormat_256Color, slime_gfx);

    int frame_counter = 0;
    int controlling_player_at_index = 0;

    while (1)
    {
        consoleClear();
        scanKeys();
        uint32 keys_held = keysHeld();
        uint32 keys_down = keysDown();

        if (keys_down & KEY_L)
        {
            controlling_player_at_index--;
            if (controlling_player_at_index < 0) controlling_player_at_index = NUM_PLAYERS - 1;
        }
        if (keys_down & KEY_R)
        {
            controlling_player_at_index++;
            if (controlling_player_at_index >= NUM_PLAYERS) controlling_player_at_index = 0;
        }

        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            if (i == controlling_player_at_index)
                PL_Move(&players[i], keys_held);
            else
                PL_Move(&players[i], 0);
        }

        SK_Update(&enemies[SKELETON_INDEX], players, NUM_PLAYERS);
        SL_Update(&enemies[SLIME_INDEX], &slime_state, players, NUM_PLAYERS);

        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            PL_Animate(&players[i], player_gfx[i], frame_counter);
        }
        SK_Animate(&enemies[SKELETON_INDEX], skeleton_gfx, frame_counter);
        SL_Animate(&enemies[SLIME_INDEX], slime_gfx, frame_counter);

        CAM_CentreOnPlayer(players, NUM_PLAYERS, controlling_player_at_index,  //
                           enemies, NUM_ENEMIES,                               //
                           bg_ids, NUM_BACKGROUNDS);

        UI_PrintToLine(0, "frame_counter = %d", frame_counter);
        display_entity_position(1, "Skeleton pos = ", &enemies[SKELETON_INDEX]);
        display_entity_position(2, "Slime pos    = ", &enemies[SLIME_INDEX]);

        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            char prepend[20];
            sprintf(prepend, "Player %d pos = ", i);

            display_entity_position(4 + i, prepend, &players[i]);
        }

        frame_counter++;
        UI_PrintDisplayBuffer();
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        bgUpdate();
        if (keys_held & KEY_START) break;
    }

    return 0;
}
