// NDS stuff
#include <nds.h>
// NDS wifi
#include <dswifi9.h>

// Standard libraries
#include <stdio.h>
#include <string.h>

typedef struct _Packet
{
    int packetID;
    int packetLength;
    char data[1024];
} Packet;

Packet CreatePacket(int packetID, int packetLength, const char *data, int n)
{
    Packet packet;

    packet.packetID = packetID;
    packet.packetLength = packetLength;
    strncpy(packet.data, data, n < 1024 ? n : 1024);
    packet.data[1023] = '\0';

    return packet;
}

Packet buncha_packets[2048];
int num_used = 0;

void packet_handler(int packetID, int packetlength)
{
    if (num_used >= 1024)
    {
        return;
    }

    char data[1024];

    int data_read = Wifi_RxRawReadPacket(packetID, packetlength, (u16 *)data);

    buncha_packets[num_used++] = CreatePacket(packetID, packetlength, data, data_read);
}

int main(void)
{
    // Setting up the top screen for sprites
    videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    vramSetBankA(VRAM_A_MAIN_SPRITE);
    oamInit(&oamMain, SpriteMapping_1D_128, false);

    // Bottom screen for simple text
    consoleDemoInit();

    /*******************
     * Setting up wifi *
     *******************/
    int wifi_works = 0;

    if (!Wifi_InitDefault(INIT_ONLY))
    {
        iprintf("Error initialising Wifi library\n");
        goto skip_rest_wifi_stuff;
    }
    else
    {
        wifi_works = 1;
        iprintf("Wifi library initialised\n");
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

    while (1)
    {
        consoleClear();
        scanKeys();
        int keys_held = keysHeld();
        int keys_down = keysDown();

        if (keys_down & KEY_A && wifi_works)
        {
            char data[1024];
            for (int i = 0; i < 1024; i++)
            {
                data[i] = '\0';
            }
            int data_length = strlen(data) + 1;

            Wifi_RawTxFrame(data_length, 0x0014, (u16 *)data);

            num_sent++;
        }

        frame_counter++;
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys_held & KEY_START) break;
    }

    return 0;
}
