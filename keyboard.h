#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "types.h"
#include "port.h"
#include "interrupts.h"

class KeyboardDriver : public InterruptHandler
{
public:
    KeyboardDriver(InterruptManager* interruptManager);
    ~KeyboardDriver();

    virtual uint32_t HandleInterrupt(uint32_t esp);

    bool HasKey() const;
    char GetKey();

private:
    static const uint8_t BUFFER_SIZE = 128;

    Port8Bit dataPort;
    Port8Bit commandPort;
    bool shiftPressed;
    char buffer[BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;

    void Enqueue(char c);
    char Translate(uint8_t keycode);
};

#endif
