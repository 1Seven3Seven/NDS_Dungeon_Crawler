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
#define SCROLL_OFFSET_FROM_ENTITY_CENTRE_X SCREEN_WIDTH / 2
/// The offset from the player centre's y position the scroll should be
#define SCROLL_OFFSET_FROM_ENTITY_CENTRE_Y SCREEN_HEIGHT / 2

/// The maximum x scroll value
#define SCROLL_MAX_X MAP_WIDTH - SCREEN_WIDTH
/// The maximum y scroll value
#define SCROLL_MAX_Y MAP_HEIGHT - SCREEN_HEIGHT

/// The number of entities that are being used in this demo
#define NUM_ENTITIES 3

#define PLAYER_INDEX 0
#define SKELETON_INDEX 1
#define SLIME_INDEX 2

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

void animate_slime(u16 *slime_gfx, int frame_counter, Entity *slime_entity)
{
    if (EN_GetStateBit(slime_entity, EN_MOVING_BIT))
    {
        slime_entity->animation_frame_number = 2 + (frame_counter / 10) % 2;
    }
    else
    {
        slime_entity->animation_frame_number = (frame_counter / 30) % 2;
    }

    dmaCopy((u8 *)SpriteSheetTiles + ROW_OFFSET * 2 + SPRITE_SIZE * slime_entity->animation_frame_number,  //
            slime_gfx,                                                                                     //
            SPRITE_SIZE);
}

void hide_if_not_on_screen(Entity *entity, int oam_id, int offset_x, int offset_y)
{
    if (entity->x > SCREEN_WIDTH + offset_x)
        oamSetHidden(&oamMain, oam_id, true);
    else if (entity->x + entity->w < offset_x)
        oamSetHidden(&oamMain, oam_id, true);
    else if (entity->y > SCREEN_HEIGHT + offset_y)
        oamSetHidden(&oamMain, oam_id, true);
    else if (entity->y + entity->h < offset_y)
        oamSetHidden(&oamMain, oam_id, true);
    else
        oamSetHidden(&oamMain, oam_id, false);
}

void set_oam_positions(Entity entities[], int array_len, int offset_x, int offset_y)
{
    for (int i = 0; i < array_len; i++)
    {
        hide_if_not_on_screen(&entities[i], i, offset_x, offset_y);
        oamSetXY(&oamMain, i, entities[i].x - offset_x, entities[i].y - offset_y);
    }
}

void centre_on_entity(Entity entities[], int entities_len, int centre_index, int bg_ids[], int bg_ids_len)
{
    int scroll_x = EN_CentreX(&entities[centre_index]) - SCROLL_OFFSET_FROM_ENTITY_CENTRE_X;
    int scroll_y = EN_CentreY(&entities[centre_index]) - SCROLL_OFFSET_FROM_ENTITY_CENTRE_Y;

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

    for (int i = 0; i < bg_ids_len; i++)
    {
        bgSetScroll(bg_ids[i], scroll_x, scroll_y);
    }

    set_oam_positions(entities, entities_len, scroll_x, scroll_y);
}

void display_entity_position(int line_number, const char *prepend, Entity *entity)
{
    UI_PrintToLine(line_number, "%s%03d, %03d", prepend, (int)entity->x, (int)entity->y);
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
    int bg_ids[1];
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

    dmaCopy(SpriteSheetPal, SPRITE_PALETTE, SpriteSheetPalLen);

    // Setting up some entites
    Entity entities[NUM_ENTITIES];
    EN_InitArray(entities, NUM_ENTITIES);

    EN_Setup(&entities[PLAYER_INDEX], PLAYER_START_X, PLAYER_START_Y, PLAYER_WIDTH, PLAYER_HEIGHT, 1, 1);
    EN_Setup(&entities[SKELETON_INDEX], 10, 10, 32, 32, 1, 1);
    EN_Setup(&entities[SLIME_INDEX], 10, 50, 32, 32, 1, 1);

    int frame_counter = 0;
    int centre_entity_index = PLAYER_INDEX;

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
        int keys_down = keysDown();

        move_player(&entities[PLAYER_INDEX], keys_held);

        SK_Update(&entities[SKELETON_INDEX], entities[PLAYER_INDEX]);

        animate_player(player_gfx, frame_counter, &entities[PLAYER_INDEX]);
        animate_skeleton(skeleton_gfx, frame_counter, &entities[SKELETON_INDEX]);
        animate_slime(slime_gfx, frame_counter, &entities[SLIME_INDEX]);

        if (keys_down & KEY_L)
        {
            centre_entity_index--;
            if (centre_entity_index < 0) centre_entity_index = NUM_ENTITIES - 1;
        }
        if (keys_down & KEY_R)
        {
            centre_entity_index++;
            if (centre_entity_index >= NUM_ENTITIES) centre_entity_index = 0;
        }

        centre_on_entity(entities, NUM_ENTITIES, centre_entity_index, bg_ids, 1);

        UI_PrintToLine(0, "frame_counter = %d", frame_counter);
        display_entity_position(1, "Player pos   = ", &entities[PLAYER_INDEX]);
        display_entity_position(2, "Skeleton pos = ", &entities[SKELETON_INDEX]);

        frame_counter++;
        UI_PrintDisplayBuffer();
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        bgUpdate();
        if (keys_held & KEY_START) break;
    }

    return 0;
}
