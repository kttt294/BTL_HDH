#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "types.h"

class KeyboardDriver;

class Console
{
public:
    Console(uint16_t* videoMemory = (uint16_t*)0xb8000);

    void SetColor(uint8_t foreground, uint8_t background);

    void PutChar(char c);
    void Print(const char* str);
    void PrintLine(const char* str);
    void PrintInt(int32_t value);
    void PrintDouble(double value, uint8_t precision = 2);
    void Backspace();
    void Clear();

    uint32_t ReadLine(KeyboardDriver& keyboard, char* buffer, uint32_t maxLen);

private:
    uint16_t* videoMemory;
    uint8_t cursorX;
    uint8_t cursorY;
    uint8_t color;

    void Scroll();
    void UpdateCursor();
};

Console& GetConsole();
void printf(const char* str);
void println(const char* str);
void SetPaletteColor(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);

#endif
