// NDS stuff
#include <nds.h>
// NDS wifi
#include <dswifi9.h>

// Standard libraries
#include <stdio.h>
#include <string.h>

//
#define UI_NUM_CHARS 32  // The width of the ui in characters - non-inclusive of '\0'
#define UI_NUM_LINES 24  // The height of the ui in lines
char UIDisplayBuffer[UI_NUM_LINES][UI_NUM_CHARS + 1];

int line_to_write_to = 3;

void packet_handler(int packetID, int packetlength)
{
    char data[1024];

    int data_read = Wifi_RxRawReadPacket(packetID, packetlength, (u16*)data);

    int string_len = strlen(data);

    if (!string_len)
    {
        return;
    }

    if (data[0] == 128)
    {
        return;
    }
    snprintf(UIDisplayBuffer[15], UI_NUM_CHARS + 1, "packet length: %d\n", packetlength);
    snprintf(UIDisplayBuffer[16], UI_NUM_CHARS + 1, "data read: %d\n", data_read);
    snprintf(UIDisplayBuffer[17], UI_NUM_CHARS + 1, "string length: %d\n", string_len);

    UIDisplayBuffer[19 + line_to_write_to][0] = ' ';
    line_to_write_to++;
    line_to_write_to %= 4;
    snprintf(UIDisplayBuffer[19 + (line_to_write_to)], UI_NUM_CHARS + 1, "> %s\n", data);
}

int main(void)
{
    // Setting up the top screen for sprites
    videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    vramSetBankA(VRAM_A_MAIN_SPRITE);
    oamInit(&oamMain, SpriteMapping_1D_128, false);

    // Bottom screen for simple text
    consoleDemoInit();

    for (int i = 0; i < UI_NUM_LINES; i++)
    {
        for (int j = 0; j < UI_NUM_CHARS; j++)
        {
            UIDisplayBuffer[i][j] = ' ';
        }
        UIDisplayBuffer[i][UI_NUM_CHARS] = '\0';
    }

    /*******************
     * Setting up wifi *
     *******************/
    int wifi_works = 0;

    if (!Wifi_InitDefault(INIT_ONLY))
    {
        snprintf(UIDisplayBuffer[0], UI_NUM_CHARS + 1, "Error initialising Wifi library\n");
        goto skip_rest_wifi_stuff;
    }
    else
    {
        wifi_works = 1;
        snprintf(UIDisplayBuffer[0], UI_NUM_CHARS + 1, "Wifi library initialised\n");
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
    int num_sent = 0;

    snprintf(UIDisplayBuffer[1], UI_NUM_CHARS + 1, "Really basic communication demo\n");
    snprintf(UIDisplayBuffer[9], UI_NUM_CHARS + 1, "Most recent data sent:\n");
    snprintf(UIDisplayBuffer[18], UI_NUM_CHARS + 1, "Most recent data received:\n");

    while (1)
    {
        consoleClear();
        scanKeys();
        int keys_held = keysHeld();
        int keys_down = keysDown();

        snprintf(UIDisplayBuffer[2], UI_NUM_CHARS + 1, "Frame counter = %d\n", frame_counter);

        if (keys_down & KEY_A && wifi_works)
        {
            char data[1024];
            for (int i = 0; i < 1024; i++)
            {
                data[i] = '\0';
            }
            snprintf(data, 1024, "Hello, World number %d", num_sent);
            int data_length = strlen(data) + 1;

            Wifi_RawTxFrame(data_length, 0x0014, (u16*)data);

            snprintf(UIDisplayBuffer[8], UI_NUM_CHARS + 1, "chars sent %d\n", data_length);
            snprintf(UIDisplayBuffer[10], UI_NUM_CHARS + 1, "%s\n", data);
            num_sent++;
        }

        for (int i = 0; i < UI_NUM_LINES; i++)
        {
            iprintf("%s", UIDisplayBuffer[i]);
        }

        frame_counter++;
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys_held & KEY_START) break;
    }

    return 0;
}
