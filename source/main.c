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

void PrintBuffer(const char *buffer, int buffer_len, int buffer_offset, int start_line, int end_line)
{
    int line = start_line;
    int character = 0;
    int index = 0;

    for (int i = 0; i < (UI_NUM_CHARS < buffer_len ? UI_NUM_CHARS : buffer_len); i++)
    {
        DisplayBuffer[23][i] = buffer[i];
    }

    while (line < end_line && index < buffer_len)
    {
        DisplayBuffer[line][character] = buffer[buffer_offset + index];

        index++;
        character++;
        if (character == UI_NUM_CHARS)
        {
            character = 0;
            line++;
        }
    }
}

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
    memcpy(packet.data, data, n < 1024 ? n : 1024);
    packet.data[1023] = '\0';

    return packet;
}

#define NUM_PACKETS_TO_CAPTURE 64
Packet buncha_packets[NUM_PACKETS_TO_CAPTURE];
int packets_collected = 0;
int line_to_display_on = 8;

void packet_handler(int packetID, int packetLength)
{
    if (packets_collected >= NUM_PACKETS_TO_CAPTURE)
    {
        return;
    }

    char data[1024];

    int data_read = Wifi_RxRawReadPacket(packetID, packetLength, (u16 *)data);

    buncha_packets[packets_collected++] = CreatePacket(packetID, packetLength, data, data_read);

    PrintToLine(2, "Collecting packets, %d", packets_collected);

    DisplayBuffer[line_to_display_on + 0][0] = ' ';
    DisplayBuffer[line_to_display_on + 1][0] = ' ';

    line_to_display_on += 2;

    if (line_to_display_on >= UI_NUM_LINES)
    {
        line_to_display_on = 8;
    }

    PrintToLine(line_to_display_on + 0, ">  ID: %06d, Len: %04D", packetID, packetLength);
    PrintToLine(line_to_display_on + 1, "> %s", data);
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
    int num_sent = 0;

    PrintToLine(4, "Collecting packets");
    PrintToLine(5, "Press a to send one");

    // Packet capture/sending loop
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

            snprintf(data, 1024, "Hello, World number %d", num_sent);

            int data_length = strlen(data) + 1;

            Wifi_RawTxFrame(data_length + 8, 0x0014, (u16 *)data);

            PrintToLine(4, "Sending:");
            PrintToLine(5, "%s", data);

            num_sent++;
        }

        PrintDisplayBuffer();

        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys_held & KEY_START) return 0;
        if (packets_collected == NUM_PACKETS_TO_CAPTURE) break;
    }

    ResetDisplayBuffer();

    PrintToLine(0, "Displaying packets");

    int packet_index = 0;
    int view_window_start = 0;
    int display_type = 0;
    char *display_type_strings[2] = {"ASCII", "HEX"};

    while (1)
    {
        consoleClear();
        scanKeys();
        int keys_held = keysHeld();
        // int keys_down = keysDown();
        int keys_repeat = keysDownRepeat();

        // Adjusting the packet index
        if (keys_repeat & KEY_RIGHT)
        {
            packet_index++;
            if (packet_index >= NUM_PACKETS_TO_CAPTURE)
            {
                packet_index = 0;
            }
        }
        if (keys_repeat & KEY_LEFT)
        {
            packet_index--;
            if (packet_index < 0)
            {
                packet_index = NUM_PACKETS_TO_CAPTURE - 1;
            }
        }

        // Adjusting the display type
        if (keys_repeat & KEY_A)
        {
            display_type = 0;
        }
        if (keys_repeat & KEY_X)
        {
            display_type = 1;
        }

        PrintToLine(1, "Packet index %d", packet_index);
        PrintToLine(2, "Display type %s", display_type_strings[display_type]);

        Packet packet = buncha_packets[packet_index];

        PrintToLine(4, ">  ID: %d, Len: %D", packet.packetID, packet.packetLength);

        if (display_type == 0)
        {
            for (int i = 10; i < 20; i++)
            {
                ClearLine(i);
            }

            PrintBuffer(packet.data, packet.packetLength, 0, 10, 20);
        }
        else if (display_type == 1)
        {
            for (int i = 0; i < 10; i++)
            {
                for (int j = 0; j < 9; j++)
                {
                    sprintf(DisplayBuffer[10 + i] + j * 3, "%02X ", packet.data[i * 10 + j]);
                }
                RemoveNullsFromLine(i);
            }
        }

        PrintToLine(23, "%s", packet.data);

        PrintDisplayBuffer();

        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys_held & KEY_START) return 0;
    }

    return 0;
}
