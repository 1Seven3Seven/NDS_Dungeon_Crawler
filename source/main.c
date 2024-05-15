#include <nds.h>
#include <stdio.h>

int main(void)
{
    // Setting up the top screen for sprites
    videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
    vramSetBankA(VRAM_A_MAIN_SPRITE);
    oamInit(&oamMain, SpriteMapping_1D_128, false);

    // Bottom screen for simple text
    consoleDemoInit();

    // Other information to track
    int frame_counter = 0;

    while (1)
    {
        consoleClear();
        scanKeys();
        int keys = keysHeld();

        iprintf("Hello World!\n");
        iprintf("Frame counter = %d\n", frame_counter);

        frame_counter++;
        swiWaitForVBlank();
        oamUpdate(&oamMain);
        if (keys & KEY_START) break;
    }
}
