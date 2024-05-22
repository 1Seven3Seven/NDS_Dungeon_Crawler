// NDS stuff
#include <nds.h>
// NDS wifi
#include <dswifi9.h>

// Standard libraries
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define UI_NUM_CHARS 32  // The width of the ui in characters - non-inclusive of '\0'
#define UI_NUM_LINES 24  // The height of the ui in lines

char DisplayBuffer[UI_NUM_LINES][UI_NUM_CHARS + 1];

void ClearLine(int line_number)
{
    if (line_number < 0 || line_number >= UI_NUM_LINES)
    {
        return;
    }

    memset(DisplayBuffer[line_number], ' ', UI_NUM_CHARS);
    DisplayBuffer[line_number][UI_NUM_CHARS] = '\0';
}

void ResetDisplayBuffer()
{
    for (int i = 0; i < UI_NUM_LINES; ++i)
    {
        memset(DisplayBuffer[i], ' ', UI_NUM_CHARS);
        DisplayBuffer[i][UI_NUM_CHARS] = '\0';
    }
}

void PrintDisplayBuffer()
{
    for (int i = 0; i < UI_NUM_LINES; i++)
    {
        iprintf("%s", DisplayBuffer[i]);
    }
}

void RemoveNullsFromLine(int line_number)
{
    for (int i = 0; i < UI_NUM_CHARS; i++)
    {
        if (DisplayBuffer[line_number][i] == '\0')
        {
            DisplayBuffer[line_number][i] = ' ';
        }
    }
}

int PrintToLine(int line_number, const char *fmt, ...)
{
    if (line_number < 0 || line_number >= UI_NUM_LINES)
    {
        return 0;
    }

    ClearLine(line_number);

    va_list args;
    va_start(args, fmt);
    int return_value = vsnprintf(DisplayBuffer[line_number], UI_NUM_CHARS + 1, fmt, args);
    va_end(args);

    RemoveNullsFromLine(line_number);

    return return_value;
}

void packet_handler(int packetID, int packetLength)
{
    char data[1024];
    int data_read = Wifi_RxRawReadPacket(packetID, packetLength, (u16 *)data);
}

int main(void)
{
    // Setting up the top screen for sprites
    videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    vramSetBankA(VRAM_A_MAIN_SPRITE);
    oamInit(&oamMain, SpriteMapping_1D_128, false);

    // Bottom screen for simple text
    consoleDemoInit();

    ResetDisplayBuffer();

    /*******************
     * Setting up wifi *
     *******************/
    int wifi_works = 0;

    if (!Wifi_InitDefault(INIT_ONLY))
    {
        PrintToLine(0, "Error initialising Wifi library");
        goto skip_rest_wifi_stuff;
    }
    else
    {
        wifi_works = 1;
        PrintToLine(0, "Wifi library initialised");
    }
    // Use channel 1, can be anything between 1 and 13.
    Wifi_SetChannel(1);
    // We handle all received packets
    Wifi_SetPromiscuousMode(1);
    Wifi_RawSetPacketHandler(packet_handler);
    Wifi_EnableWifi();

skip_rest_wifi_stuff:  // Take a guess

    // Other information to track

    while (1)
    {
        consoleClear();
        scanKeys();
        int keys_held = keysHeld();
        int keys_down = keysDown();

        if (keys_down & KEY_A && wifi_works)
        {
            char data[1024];

            Wifi_RawTxFrame(/* data_length */ +8, 0x0014, (u16 *)data);
        }

        PrintDisplayBuffer();

        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys_held & KEY_START) return 0;
    }

    return 0;
}
