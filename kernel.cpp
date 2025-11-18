#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "console.h"

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors()
{
    for (constructor* i = &start_ctors; i != &end_ctors; ++i)
        (*i)();
}

extern "C" void kernelMain(void* multibootStruct, uint32_t magicNum)
{
    Console& console = GetConsole();
    console.PrintLine("Hello world");
    console.PrintLine("This is my OS");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);
    KeyboardDriver keyboard(&interrupts);

    // Here init hardware

    interrupts.Activate();

    char inputBuffer[128];

    while (true)
    {
        console.Print("\n> ");
        console.ReadLine(keyboard, inputBuffer, sizeof(inputBuffer));
        console.Print("You typed: ");
        console.PrintLine(inputBuffer);
    }
}