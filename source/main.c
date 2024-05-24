// NDS stuff
#include <nds.h>
// NDS wifi
#include <dswifi9.h>

// Standard libraries
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// My libraries
#include "UI.h"

// #define DATA_LEN sizeof(char) + sizeof(int) * 2
#define DATA_LEN sizeof(int) * 3
#define CHARS_CAN_PRINT 9 < DATA_LEN ? 9 : DATA_LEN
#define POSITION_INDICATOR 0x17

int other_position[2] = {-32, -32};

int frame_counter = 0;
volatile int frame_save = 0;
volatile int line_to_print_on = 10;
volatile int previous_line_to_print_on = 10;

void PrintByteArrayAsHex(const u8 *array, int array_len, int line_start, int line_end)
{
    if (line_start < 0)
    {
        line_start = 0;
    }
    if (line_end >= UI_NUM_LINES)
    {
        line_end = UI_NUM_LINES - 1;
    }

    int line_number = line_start;
    int hex_number = 0;
    int next_byte = 0;

    while (line_number <= line_end && next_byte < array_len)
    {
        UI_ClearLine(line_number);

        while (hex_number < 8 && next_byte < array_len)
        {
            snprintf(UI_DisplayBuffer[line_number] + hex_number * 3, 3, "%02x", array[next_byte]);
            hex_number++;
            next_byte++;
        }

        UI_RemoveNullsFromLine(line_number);
        line_number++;
        hex_number = 0;
    }
}

void packet_handler(int packetID, int packetLength)
{
    if (frame_counter != frame_save)
    {
        for (int i = 10; i < 20; i++)
        {
            UI_DisplayBuffer[i][0] = ' ';
        }

        frame_save = frame_counter;
        previous_line_to_print_on = line_to_print_on;
    }

    char *data = malloc(sizeof(char) * (packetLength + (packetLength % 2)));

    // int data_read = Wifi_RxRawReadPacket(packetID, packetLength, (u16 *)data);
    Wifi_RxRawReadPacket(packetID, packetLength, (u16 *)data);

    UI_DisplayBuffer[line_to_print_on][0] = '>';
    for (int i = 0; i < (CHARS_CAN_PRINT < packetLength ? CHARS_CAN_PRINT : packetLength); i++)
    {
        snprintf(UI_DisplayBuffer[line_to_print_on] + i * 3 + 2, 3, "%02x", data[i]);
    }
    UI_RemoveNullsFromLine(line_to_print_on);

    line_to_print_on++;

    if (line_to_print_on >= 20)
    {
        line_to_print_on = 10;
    }

    if (data[0] != POSITION_INDICATOR)
    {
        goto packet_handler_cleanup;
    }

    // handling code

packet_handler_cleanup:
    free(data);
}

int setup_wifi()
{
    if (!Wifi_InitDefault(INIT_ONLY))
    {
        UI_PrintToLine(0, "Error initialising Wifi library");
        return 0;
    }
    else
    {
        UI_PrintToLine(0, "Wifi library initialised");
    }
    // Use channel 1, can be anything between 1 and 13.
    Wifi_SetChannel(1);
    // We handle all received packets
    Wifi_SetPromiscuousMode(1);
    Wifi_RawSetPacketHandler(packet_handler);
    Wifi_EnableWifi();

    return 1;
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

    oamSet(&oamMain, 1, other_position[0], other_position[1], 0, 0, SpriteSize_32x32, SpriteColorFormat_256Color,
           gfx_green_square, -1, false, false, false, false, false);

    int wifi_works = setup_wifi();

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

        if (wifi_works)
        {
            u8 data[DATA_LEN] = {'\0'};

            // data[0] = POSITION_INDICATOR;
            data[0] = 'H';
            data[1] = 'e';
            *(int *)(data + sizeof(int)) = htonl(my_position[0]);
            *(int *)(data + sizeof(int) * 2) = htonl(my_position[1]);

            Wifi_RawTxFrame(DATA_LEN, 0x0014, (u16 *)data);

            // char _temp1[100] = {'\0'};
            // char string[] = "He";
            // char _temp2[100] = {'\0'};
            // Wifi_RawTxFrame(sizeof(string) + 100, 0x0014, (u16 *)string);

            PrintByteArrayAsHex(data, DATA_LEN, 22, UI_NUM_LINES);
        }

        frame_counter++;
        UI_PrintDisplayBuffer();
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys_held & KEY_START) break;
    }

    return 0;
}
