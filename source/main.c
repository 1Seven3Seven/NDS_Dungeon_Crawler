// NDS stuff
#include <nds.h>
// NDS wifi
#include <dswifi9.h>

// Standard libraries
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// My libraries
#include "UI.h"

void packet_handler(int packetID, int packetLength)
{
    // char data[1024];
    // int data_read = Wifi_RxRawReadPacket(packetID, packetLength, (u16 *)data);
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
    u16 *gfx_red_square = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
    u16 *gfx_green_square = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);

    for (int i = 0; i < 32 * 32 / 2; i++)
    {
        gfx_red_square[i] = 1 | (1 << 8);
        gfx_green_square[i] = 2 | (2 << 8);
    }

    SPRITE_PALETTE[1] = RGB15(31, 0, 0);
    SPRITE_PALETTE[2] = RGB15(0, 31, 0);

    int my_position[2] = {100, 100};

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

    /*******************
     * Setting up wifi *
     *******************/
    // int wifi_works = 0;

    if (!Wifi_InitDefault(INIT_ONLY))
    {
        UI_PrintToLine(0, "Error initialising Wifi library");
        goto skip_rest_wifi_stuff;
    }
    else
    {
        // wifi_works = 1;
        UI_PrintToLine(0, "Wifi library initialised");
    }
    // Use channel 1, can be anything between 1 and 13.
    Wifi_SetChannel(1);
    // We handle all received packets
    Wifi_SetPromiscuousMode(1);
    Wifi_RawSetPacketHandler(packet_handler);
    Wifi_EnableWifi();

skip_rest_wifi_stuff:  // Take a guess

    // Other information to track
    int frame_counter = 0;

    while (1)
    {
        consoleClear();
        scanKeys();
        int keys_held = keysHeld();

        UI_PrintToLine(2, "frame_counter = %d", frame_counter);

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

        UI_PrintToLine(3, "%d, %d", my_position[0], my_position[1]);
        oamSetXY(&oamMain, 0, my_position[0], my_position[1]);

        // if (wifi_works)
        // {
        //     char data[1024];

        //     Wifi_RawTxFrame(/* data_length */ +8, 0x0014, (u16 *)data);
        // }

        frame_counter++;
        UI_PrintDisplayBuffer();
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys_held & KEY_START) break;
    }

    return 0;
}
