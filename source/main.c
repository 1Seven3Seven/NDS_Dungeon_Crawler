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

/// The number of entities that are being used in this demo
#define NUM_ENTITIES 3

#define PLAYER_INDEX 0
#define SKELETON_INDEX 1
#define SLIME_INDEX 2

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

    u16 *attack_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
    SD_fill(attack_gfx, SpriteSize_16x16, 0);

    u16 *pointer_gfx = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_256Color);
    SD_fill(pointer_gfx, SpriteSize_8x8, 0);

    dmaCopy(SpriteSheetPal, SPRITE_PALETTE, SpriteSheetPalLen);
    SPRITE_PALETTE[173] = RGB15(31, 31, 31);

    // Setting up some entites
    Entity entities[NUM_ENTITIES];
    EN_InitArray(entities, NUM_ENTITIES);

    // The player
    PL_SetupPlayer(&entities[PLAYER_INDEX], PLAYER_START_X, PLAYER_START_Y);

    // The skeleton
    EN_Setup(&entities[SKELETON_INDEX], 10, 10, 32, 32, 1, 1);

    // The slime
    SL_SetupSlime(&entities[SLIME_INDEX], 250, 200);
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
    int centre_entity_index = PLAYER_INDEX;
    s8 vx = 1, vy = 0;

    while (1)
    {
        consoleClear();
        scanKeys();
        uint32 keys_held = keysHeld();
        uint32 keys_down = keysDown();

        PL_Move(&entities[PLAYER_INDEX], keys_held);

        // Get a vector in the direction the player is moving
        if (!attack_counter)
        {
            if (keys_held & KEY_ARROWS)
            {
                vx = 0;
                vy = 0;
            }

            if (keys_held & KEY_UP) vy--;
            if (keys_held & KEY_DOWN) vy++;
            if (keys_held & KEY_LEFT) vx--;
            if (keys_held & KEY_RIGHT) vx++;
        }

        // Scroll code copied from the centre on entity function
        // For testing purposes this is here
        int scroll_x = EN_CentreX(&entities[centre_entity_index]) - SCROLL_OFFSET_FROM_ENTITY_CENTRE_X;
        int scroll_y = EN_CentreY(&entities[centre_entity_index]) - SCROLL_OFFSET_FROM_ENTITY_CENTRE_Y;
        if (scroll_x < 0)
            scroll_x = 0;
        else if (scroll_x > SCROLL_MAX_X)
            scroll_x = SCROLL_MAX_X;
        if (scroll_y < 0)
            scroll_y = 0;
        else if (scroll_y > SCROLL_MAX_Y)
            scroll_y = SCROLL_MAX_Y;

        // Draw the pointer given the move direction
        draw_pointer(pointer_gfx, SpriteSize_8x8, vx, vy, 173);
        oamSetXY(
            &oamMain, 4,                                                  //
            EN_CentreX(&entities[PLAYER_INDEX])                           //
                + (entities[PLAYER_INDEX].w / 2 + (vx < 0 ? 8 : 0)) * vx  // Adjust to the left or right of the player
                - (vx == 0 && vy ? 4 : 0)                                 // Adjust left or right if directly up or down
                - scroll_x,                                               //
            EN_CentreY(&entities[PLAYER_INDEX])                           //
                + (entities[PLAYER_INDEX].h / 2 + (vy < 0 ? 8 : 0)) * vy  // Adjust to the top or bottom of the player
                - (vy == 0 && vx ? 4 : 0)                                 // Adjust up or down if directly left or right
                - scroll_y                                                //
        );

        // Player attack cooldown
        if (entities[PLAYER_INDEX].current_attack_delay > 0) entities[PLAYER_INDEX].current_attack_delay--;
        // Player attack
        else if (keys_held & KEY_A && entities[PLAYER_INDEX].current_attack_delay == 0)
        {
            // Draw the attack hitbox
            draw_attack_hitbox(attack_gfx, SpriteSize_16x16, vx, vy, 173);

            // Show the attack hitbox
            oamSetHidden(&oamMain, 3, false);

            //
            attack_counter = 8;
            entities[PLAYER_INDEX].current_attack_delay = entities[PLAYER_INDEX].attack_delay;
        }

        // Maintaining the attack hitbox
        if (attack_counter > 0)
        {
            oamSetXY(&oamMain, 3,                                                                     //
                     (vx > 0 ? EN_Right(&entities[PLAYER_INDEX]) : EN_Left(&entities[PLAYER_INDEX]))  //
                         - (vx < 0 ? 16 : 0)                                                          //
                         - scroll_x,                                                                  //
                     (vy > 0 ? EN_Bottom(&entities[PLAYER_INDEX]) : EN_Top(&entities[PLAYER_INDEX]))  //
                         - (vy < 0 ? 16 : 0)                                                          //
                         - scroll_y                                                                   //
            );
            attack_counter--;
        }
        // Clearing the attack hitbox
        else if (attack_counter == 0)
            oamSetHidden(&oamMain, 3, true);

        SK_Update(&entities[SKELETON_INDEX], &entities[PLAYER_INDEX]);
        SL_Update(&entities[SLIME_INDEX], &slime_state, &entities[PLAYER_INDEX]);

        PL_Animate(&entities[PLAYER_INDEX], player_gfx, frame_counter);
        animate_skeleton(skeleton_gfx, frame_counter, &entities[SKELETON_INDEX]);
        SL_Animate(&entities[SLIME_INDEX], slime_gfx, frame_counter);

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
        display_entity_position(3, "Slime pos    = ", &entities[SLIME_INDEX]);

        UI_PrintToLine(5, "attack vector = %02d, %02d", vx, vy);
        UI_PrintToLine(6, "attack hitbox = %02d, %02d, %02d, %02d", x, y, w, h);

        frame_counter++;
        UI_PrintDisplayBuffer();
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        bgUpdate();
        if (keys_held & KEY_START) break;
    }

    return 0;
}
