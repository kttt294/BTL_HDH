#include "keyboard.h"

static const char normalKeymap[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const char shiftKeymap[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const uint32_t KEYMAP_SIZE = sizeof(normalKeymap) / sizeof(normalKeymap[0]);

KeyboardDriver::KeyboardDriver(InterruptManager* interruptManager)
    : InterruptHandler(0x21, interruptManager),
      dataPort(0x60),
      commandPort(0x64),
      shiftPressed(false),
      head(0),
      tail(0)
{
    // Flush controller buffers and enable IRQ1
    while (commandPort.Read() & 0x01)
        dataPort.Read();

    commandPort.Write(0xAE); // activate keyboard
    commandPort.Write(0x20);
    uint8_t status = dataPort.Read();
    status = (status | 1) & ~0x10;
    commandPort.Write(0x60);
    dataPort.Write(status);

    dataPort.Write(0xF4); // enable scanning
}

KeyboardDriver::~KeyboardDriver()
{
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t keycode = dataPort.Read();

    if (keycode & 0x80)
    {
        uint8_t released = keycode & 0x7F;
        if (released == 0x2A || released == 0x36)
            shiftPressed = false;
        return esp;
    }

    if (keycode == 0x2A || keycode == 0x36)
    {
        shiftPressed = true;
        return esp;
    }

    char translated = Translate(keycode);
    if (translated != 0)
        Enqueue(translated);

    return esp;
}

bool KeyboardDriver::HasKey() const
{
    return head != tail;
}

char KeyboardDriver::GetKey()
{
    if (!HasKey())
        return 0;

    char value = buffer[tail];
    tail = (tail + 1) % BUFFER_SIZE;
    return value;
}

void KeyboardDriver::Enqueue(char c)
{
    uint8_t next = (head + 1) % BUFFER_SIZE;
    if (next == tail)
        tail = (tail + 1) % BUFFER_SIZE; // overwrite oldest if buffer full

    buffer[head] = c;
    head = next;
}

char KeyboardDriver::Translate(uint8_t keycode)
{
    if (keycode >= KEYMAP_SIZE)
        return 0;

    char value = shiftPressed ? shiftKeymap[keycode] : normalKeymap[keycode];
    return value;
}
