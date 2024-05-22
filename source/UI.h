#ifndef UI_H
#define UI_H

#define UI_NUM_CHARS 32  // The width of the ui in characters - non-inclusive of '\0'
#define UI_NUM_LINES 24  // The height of the ui in lines

extern char UI_DisplayBuffer[UI_NUM_LINES][UI_NUM_CHARS + 1];

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

#endif