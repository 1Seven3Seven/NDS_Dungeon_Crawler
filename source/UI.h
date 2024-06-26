#ifndef UI_H
#define UI_H

#include <nds.h>

#define UI_NUM_CHARS 32  // The width of the ui in characters - non-inclusive of '\0'
#define UI_NUM_LINES 24  // The height of the ui in lines

extern char UI_DisplayBuffer[UI_NUM_LINES][UI_NUM_CHARS + 1];

// Return true if the given line number is incorrect
inline int UI_LineNumIncorrect(int line_number) { return line_number < 0 || line_number >= UI_NUM_LINES; }

// Clears the given line in the display buffer.
void UI_ClearLine(int line_number);

// Resets the display buffer to just spaces.
void UI_ResetDisplayBuffer();

// Prints the display buffer.
void UI_PrintDisplayBuffer();

// Removes any null characters from the given line in the display buffer.
void UI_RemoveNullsFromLine(int line_number);

// A wrapper for printf, prints to the given line in the display buffer.
void UI_PrintToLine(int line_number, const char *fmt, ...);

// Prints the bits of a unsigned 16 bit integer
void UI_PrintU16Bits(int line_number, u16 some_u16);

// Prints the bits of a unsigned 32 bit integer
void UI_PrintU32Bits(int line_number, u32 some_u32);

#endif