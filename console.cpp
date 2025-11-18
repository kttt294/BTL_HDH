#include "console.h"
#include "keyboard.h"
#include "port.h"

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

void printf(const char* str)
{
    GetConsole().Print(str);
}

void println(const char* str)
{
    GetConsole().PrintLine(str);
}
