#include "console.h"
#include "keyboard.h"
#include "port.h"

namespace
{
    constexpr double DOUBLE_EPSILON = 1e-6;
}

static Console globalConsole;

Console::Console(uint16_t* videoMemory)
    : videoMemory(videoMemory),
      cursorX(0),
      cursorY(0),
      color(0x0F)
{
    Clear();
}

void Console::SetColor(uint8_t foreground, uint8_t background)
{
    color = (background << 4) | (foreground & 0x0F);
}

void Console::PutChar(char c)
{
    if (c == '\n')
    {
        cursorX = 0;
        ++cursorY;
    }
    else if (c == '\r')
    {
        cursorX = 0;
    }
    else if (c == '\t')
    {
        cursorX = (cursorX + 8) & ~(8 - 1);
    }
    else if (c == '\b')
    {
        Backspace();
        return;
    }
    else
    {
        videoMemory[80 * cursorY + cursorX] = (color << 8) | (uint8_t)c;
        ++cursorX;
    }

    if (cursorX >= 80)
    {
        cursorX = 0;
        ++cursorY;
    }

    Scroll();
    UpdateCursor();
}

void Console::Print(const char* str)
{
    for (uint32_t i = 0; str[i] != '\0'; ++i)
        PutChar(str[i]);
}

void Console::PrintLine(const char* str)
{
    Print(str);
    PutChar('\n');
}

void Console::PrintInt(int32_t value)
{
    if (value == 0)
    {
        PutChar('0');
        return;
    }

    char buffer[12];
    int32_t i = 0;
    bool isNegative = false;

    if (value < 0)
    {
        isNegative = true;
        value = -value;
    }

    while (value > 0)
    {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    if (isNegative)
        PutChar('-');

    while (i > 0)
        PutChar(buffer[--i]);
}

void Console::PrintDouble(double value, uint8_t precision)
{
    bool isNegative = value < 0.0;
    double absValue = isNegative ? -value : value;

    int32_t intPart = (int32_t)absValue;
    double fracPart = absValue - (double)intPart;

    if (fracPart < DOUBLE_EPSILON)
    {
        if (isNegative && intPart != 0)
        PutChar('-');

        if (intPart == 0)
            PutChar('0');
        else
            PrintInt(intPart);

        return;
    }

    if (isNegative)
        PutChar('-');

    if (intPart == 0)
        PutChar('0');
    else
        PrintInt(intPart);

    if (precision > 0)
    {
        PutChar('.');

        for (uint8_t i = 0; i < precision; ++i)
        {
            fracPart *= 10.0;
            int32_t digit = (int32_t)fracPart;
            if (digit < 0) digit = 0;  // Safety check
            if (digit > 9) digit = 9;  // Safety check
            PutChar('0' + digit);
            fracPart -= (double)digit;
        }
    }
}

void Console::Backspace()
{
    if (cursorX == 0 && cursorY == 0)
        return;

    if (cursorX == 0)
    {
        cursorX = 80;
        if (cursorY > 0)
            --cursorY;
    }

    if (cursorX > 0)
        --cursorX;

    videoMemory[80 * cursorY + cursorX] = (color << 8) | ' ';
    UpdateCursor();
}

void Console::Clear()
{
    for (uint32_t y = 0; y < 25; ++y)
        for (uint32_t x = 0; x < 80; ++x)
            videoMemory[80 * y + x] = (color << 8) | ' ';

    cursorX = 0;
    cursorY = 0;
    UpdateCursor();
}

uint32_t Console::ReadLine(KeyboardDriver& keyboard, char* buffer, uint32_t maxLen)
{
    if (maxLen == 0)
        return 0;

    uint32_t length = 0;

    while (true)
    {
        if (!keyboard.HasKey())
            continue;

        char key = keyboard.GetKey();

        if (key == '\r')
            key = '\n';

        if (key == '\n')
        {
            PutChar('\n');
            break;
        }

        if (key == '\b')
        {
            if (length > 0)
            {
                --length;
                buffer[length] = '\0';
                Backspace();
            }
            continue;
        }

        if (key < ' ' || key > '~')
            continue;

        if (length < maxLen - 1)
        {
            buffer[length++] = key;
            buffer[length] = '\0';
            PutChar(key);
        }
    }

    buffer[length] = '\0';
    return length;
}

void Console::Scroll()
{
    if (cursorY < 25)
        return;

    for (uint32_t y = 1; y < 25; ++y)
    {
        for (uint32_t x = 0; x < 80; ++x)
            videoMemory[80 * (y - 1) + x] = videoMemory[80 * y + x];
    }

    for (uint32_t x = 0; x < 80; ++x)
        videoMemory[80 * 24 + x] = (color << 8) | ' ';

    cursorY = 24;
}

void Console::UpdateCursor()
{
    uint16_t position = cursorY * 80 + cursorX;

    Port8Bit command(0x3D4);
    Port8Bit data(0x3D5);

    command.Write(0x0F);
    data.Write(position & 0xFF);
    command.Write(0x0E);
    data.Write((position >> 8) & 0xFF);
}

Console& GetConsole()
{
    return globalConsole;
}

void SetPaletteColor(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    Port8Bit dacWrite(0x3C8);
    Port8Bit dacData(0x3C9);

    dacWrite.Write(index);
    dacData.Write(red);
    dacData.Write(green);
    dacData.Write(blue);
}

void printf(const char* str)
{
    GetConsole().Print(str);
}

void println(const char* str)
{
    GetConsole().PrintLine(str);
}
