#include "UI.h"

// NDS stuff
#include <nds.h>

// Standard libraries
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

char UI_DisplayBuffer[UI_NUM_LINES][UI_NUM_CHARS + 1];

void UI_ClearLine(int line_number)
{
    if (line_number < 0 || line_number >= UI_NUM_LINES)
    {
        return;
    }

    memset(UI_DisplayBuffer[line_number], ' ', UI_NUM_CHARS);
    UI_DisplayBuffer[line_number][UI_NUM_CHARS] = '\0';
}

void UI_ResetDisplayBuffer()
{
    for (int i = 0; i < UI_NUM_LINES; ++i)
    {
        memset(UI_DisplayBuffer[i], ' ', UI_NUM_CHARS);
        UI_DisplayBuffer[i][UI_NUM_CHARS] = '\0';
    }
}

void UI_PrintDisplayBuffer()
{
    for (int i = 0; i < UI_NUM_LINES; i++)
    {
        iprintf("%s", UI_DisplayBuffer[i]);
    }
}

void UI_RemoveNullsFromLine(int line_number)
{
    if (line_number < 0 || line_number >= UI_NUM_LINES)
    {
        return;
    }

    for (int i = 0; i < UI_NUM_CHARS; i++)
    {
        if (UI_DisplayBuffer[line_number][i] == '\0')
        {
            UI_DisplayBuffer[line_number][i] = ' ';
        }
    }
}

void UI_PrintToLine(int line_number, const char *fmt, ...)
{
    if (line_number < 0 || line_number >= UI_NUM_LINES)
    {
        return;
    }

    UI_ClearLine(line_number);

    va_list args;
    va_start(args, fmt);
    vsnprintf(UI_DisplayBuffer[line_number], UI_NUM_CHARS + 1, fmt, args);
    va_end(args);

    UI_RemoveNullsFromLine(line_number);
}

void UI_PrintU16Bits(int line_number, u16 some_u16)
{
    if (line_number < 0 || line_number >= UI_NUM_LINES)
    {
        return;
    }

    UI_ClearLine(line_number);

    for (int i = 0; i < 16; i++)
    {
        int bit = (some_u16 >> (15 - i)) & 1;
        UI_DisplayBuffer[line_number][i] = '0' + bit;
    }
}

void UI_PrintU32Bits(int line_number, u32 some_u32)
{
    if (line_number < 0 || line_number >= UI_NUM_LINES)
    {
        return;
    }

    UI_ClearLine(line_number);

    for (int i = 0; i < 32; i++)
    {
        int bit = (some_u32 >> (31 - i)) & 1;
        UI_DisplayBuffer[line_number][i] = '0' + bit;
    }
}
