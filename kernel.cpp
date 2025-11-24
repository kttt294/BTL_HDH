#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "console.h"
#include "calculator.h"

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
    // Validate multiboot magic number
    if (magicNum != 0x2BADB002)
    {
        // Invalid boot, halt
        while (true)
            asm("hlt");
    }

    Console& console = GetConsole();
    console.SetColor(0x0A, 0x00);  // Green text on black background
    console.PrintLine("===================================");
    console.PrintLine("   Simple Calculator OS");
    console.PrintLine("===================================");
    console.SetColor(0x0F, 0x00);  // White text
    console.PrintLine("Enter expressions like: 5 + 3");
    console.PrintLine("Supported: + - * /");
    console.PrintLine("");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);
    KeyboardDriver keyboard(&interrupts);

    interrupts.Activate();

    char inputBuffer[128];

    while (true)
    {
        console.SetColor(0x0E, 0x00);  // Yellow prompt
        console.Print("calc> ");
        console.SetColor(0x0F, 0x00);  // White input
        
        console.ReadLine(keyboard, inputBuffer, sizeof(inputBuffer));
        
        // Evaluate the expression
        ExpressionResult result = Calculator::EvaluateExpression(inputBuffer);
        
        if (result.valid)
        {
            console.SetColor(0x0A, 0x00);  // Green for result
            console.Print("Result: ");
            console.PrintDouble(result.value, 2);
            console.PrintLine("");
        }
        else
        {
            console.SetColor(0x0C, 0x00);  // Red for error
            console.Print("Error: ");
            console.PrintLine(result.errorMessage);
        }
        
        console.SetColor(0x0F, 0x00);  // Reset to white
    }
}