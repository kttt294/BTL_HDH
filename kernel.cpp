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

// Helper function to parse a number from string
static double ParseNumber(const char* str, int32_t& pos, bool& success)
{
    double value = 0.0;
    bool isNegative = false;
    bool hasDecimal = false;
    double decimalPlace = 0.1;
    bool hasDigit = false;
    
    // Skip spaces
    while (str[pos] == ' ') ++pos;
    
    // Check for negative
    if (str[pos] == '-')
    {
        isNegative = true;
        ++pos;
    }
    
    // Parse digits
    while (str[pos] != '\0' && str[pos] != ' ' && str[pos] != '\n')
    {
        if (str[pos] >= '0' && str[pos] <= '9')
        {
            hasDigit = true;
            if (hasDecimal)
            {
                value += (str[pos] - '0') * decimalPlace;
                decimalPlace *= 0.1;
            }
            else
            {
                value = value * 10.0 + (str[pos] - '0');
            }
        }
        else if (str[pos] == '.')
        {
            hasDecimal = true;
        }
        else
        {
            break;
        }
        ++pos;
    }
    
    if (isNegative) value = -value;
    success = hasDigit;
    return value;
}

// Menu 1: Basic calculations
static void MenuBasicCalculations(Console& console, KeyboardDriver& keyboard)
{
    char inputBuffer[128];
    
    while (true)
    {
        console.SetColor(0x0E, 0x00);  // Yellow
        console.Print("Basic Calc> ");
        console.SetColor(0x0F, 0x00);  // White
        console.PrintLine("(Enter 'back' to return to main menu)");
        console.SetColor(0x0F, 0x00);
        
        console.ReadLine(keyboard, inputBuffer, sizeof(inputBuffer));
        
        // Check if user wants to go back
        if (inputBuffer[0] == 'b' && inputBuffer[1] == 'a' && inputBuffer[2] == 'c' && 
            inputBuffer[3] == 'k' && (inputBuffer[4] == '\0' || inputBuffer[4] == '\n'))
        {
            return;
        }
        
        ExpressionResult result = Calculator::EvaluateExpression(inputBuffer);
        
        if (result.valid)
        {
            console.SetColor(0x0B, 0x00);  // Cyan
            console.Print("Result: ");
            console.PrintDouble(result.value, 2);
            console.PrintLine("");
        }
        else
        {
            console.SetColor(0x0C, 0x00);  // Red
            console.Print("Error: ");
            console.PrintLine(result.errorMessage);
        }
        
        console.SetColor(0x0F, 0x00);
        console.PrintLine("");
    }
}

// Menu 2: Solve equations
static void MenuEquations(Console& console, KeyboardDriver& keyboard)
{
    char inputBuffer[128];
    
    // Print initial prompt
    console.SetColor(0x0E, 0x00);  // Yellow
    console.Print("Equations> ");
    console.SetColor(0x0F, 0x00);  // White
    console.PrintLine("(1: Linear, 2: Quadratic, 'back': Main menu)");
    
    while (true)
    {
        console.SetColor(0x0F, 0x00);
        console.ReadLine(keyboard, inputBuffer, sizeof(inputBuffer));
        
        // Check if user wants to go back
        if (inputBuffer[0] == 'b' && inputBuffer[1] == 'a' && inputBuffer[2] == 'c' && 
            inputBuffer[3] == 'k' && (inputBuffer[4] == '\0' || inputBuffer[4] == '\n'))
        {
            return;
        }
        
        // Linear equation
        if (inputBuffer[0] == '1' && (inputBuffer[1] == '\0' || inputBuffer[1] == '\n'))
        {
            console.SetColor(0x0B, 0x00);  // Cyan
            console.PrintLine("Linear equation: ax + b = 0");
            console.Print("Enter a and b: ");
            console.SetColor(0x0F, 0x00);
            
            console.ReadLine(keyboard, inputBuffer, sizeof(inputBuffer));
            
            int32_t pos = 0;
            bool successA = false, successB = false;
            double a = ParseNumber(inputBuffer, pos, successA);
            double b = ParseNumber(inputBuffer, pos, successB);
            
            if (successA && successB)
            {
                LinearSolution solution = Calculator::SolveLinear(a, b);
                console.SetColor(0x0B, 0x00);
                console.Print("Equation: ");
                console.PrintDouble(a, 2);
                console.Print("x + ");
                console.PrintDouble(b, 2);
                console.PrintLine(" = 0");
                
                if (solution.hasSolution)
                {
                    if (solution.infiniteSolutions)
                    {
                        console.PrintLine("Solution: Infinite solutions");
                    }
                    else
                    {
                        console.Print("Solution: x = ");
                        console.PrintDouble(solution.root, 4);
                        console.PrintLine("");
                    }
                }
                else
                {
                    console.PrintLine("Solution: No solution");
                }
                
                console.SetColor(0x0F, 0x00);
                console.PrintLine("");
                // Keep the prompt visible
                console.SetColor(0x0E, 0x00);
                console.Print("Equations> ");
                console.SetColor(0x0F, 0x00);
                console.PrintLine("(1: Linear, 2: Quadratic, 'back': Main menu)");
            }
            else
            {
                console.SetColor(0x0C, 0x00);
                console.PrintLine("Error: Invalid input. Enter two numbers: a b");
                console.SetColor(0x0F, 0x00);
                console.PrintLine("");
                // Keep the prompt visible
                console.SetColor(0x0E, 0x00);
                console.Print("Equations> ");
                console.SetColor(0x0F, 0x00);
                console.PrintLine("(1: Linear, 2: Quadratic, 'back': Main menu)");
            }
        }
        // Quadratic equation
        else if (inputBuffer[0] == '2' && (inputBuffer[1] == '\0' || inputBuffer[1] == '\n'))
        {
            console.SetColor(0x0B, 0x00);  // Cyan
            console.PrintLine("Quadratic equation: ax^2 + bx + c = 0");
            console.Print("Enter a, b, and c: ");
            console.SetColor(0x0F, 0x00);
            
            console.ReadLine(keyboard, inputBuffer, sizeof(inputBuffer));
            
            int32_t pos = 0;
            bool successA = false, successB = false, successC = false;
            double a = ParseNumber(inputBuffer, pos, successA);
            double b = ParseNumber(inputBuffer, pos, successB);
            double c = ParseNumber(inputBuffer, pos, successC);
            
            if (successA && successB && successC)
            {
                QuadraticSolution solution = Calculator::SolveQuadratic(a, b, c);
                console.SetColor(0x0B, 0x00);
                console.Print("Equation: ");
                console.PrintDouble(a, 2);
                console.Print("x^2 + ");
                console.PrintDouble(b, 2);
                console.Print("x + ");
                console.PrintDouble(c, 2);
                console.PrintLine(" = 0");
                
                if (solution.reducedToLinear)
                {
                    console.PrintLine("Note: Reduced to linear equation");
                }
                
                if (solution.hasRealSolutions)
                {
                    console.Print("Solution: x1 = ");
                    console.PrintDouble(solution.root1, 4);
                    console.Print(", x2 = ");
                    console.PrintDouble(solution.root2, 4);
                    console.PrintLine("");
                }
                else
                {
                    console.Print("Solution: No real solutions (discriminant = ");
                    console.PrintDouble(solution.discriminant, 4);
                    console.PrintLine(")");
                }
                
                console.SetColor(0x0F, 0x00);
                console.PrintLine("");
                // Keep the prompt visible
                console.SetColor(0x0E, 0x00);
                console.Print("Equations> ");
                console.SetColor(0x0F, 0x00);
                console.PrintLine("(1: Linear, 2: Quadratic, 'back': Main menu)");
            }
            else
            {
                console.SetColor(0x0C, 0x00);
                console.PrintLine("Error: Invalid input. Enter three numbers: a b c");
                console.SetColor(0x0F, 0x00);
                console.PrintLine("");
                // Keep the prompt visible
                console.SetColor(0x0E, 0x00);
                console.Print("Equations> ");
                console.SetColor(0x0F, 0x00);
                console.PrintLine("(1: Linear, 2: Quadratic, 'back': Main menu)");
            }
        }
        else
        {
            console.SetColor(0x0C, 0x00);
            console.PrintLine("Error: Invalid choice. Enter 1, 2, or 'back'");
            console.SetColor(0x0F, 0x00);
            console.PrintLine("");
            // Keep the prompt visible
            console.SetColor(0x0E, 0x00);
            console.Print("Equations> ");
            console.SetColor(0x0F, 0x00);
            console.PrintLine("(1: Linear, 2: Quadratic, 'back': Main menu)");
        }
    }
}

// Main menu
static void ShowMainMenu(Console& console)
{
    console.SetColor(0x0F, 0x00);
    console.PrintLine("================================================================================");
    console.PrintLine("                             Calculator OS - NHOM 2                             ");
    console.PrintLine("================================================================================");
    console.SetColor(0x0F, 0x00);  // White
    console.PrintLine("1. Basic Calculations (+ - * /)");
    console.PrintLine("2. Solve Equations");
    console.PrintLine("");
    console.SetColor(0x0E, 0x00);  // Yellow
    console.Print("Select menu (1 or 2): ");
    console.SetColor(0x0F, 0x00);
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
    SetPaletteColor(0, 63, 45, 48);
    console.SetColor(0x0F, 0x00);
    console.Clear();
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);
    KeyboardDriver keyboard(&interrupts);

    interrupts.Activate();

    char inputBuffer[128];

    // Main menu loop
    while (true)
    {
        console.Clear();
        ShowMainMenu(console);
        
        console.ReadLine(keyboard, inputBuffer, sizeof(inputBuffer));
        console.PrintLine("");
        
        if (inputBuffer[0] == '1' && (inputBuffer[1] == '\0' || inputBuffer[1] == '\n'))
        {
            console.Clear();
            console.SetColor(0x0F, 0x00);
            console.PrintLine("=== Menu 1: Basic Calculations ===");
            console.SetColor(0x0F, 0x00);
            console.PrintLine("Supported operations: + - * /");
            console.PrintLine("");
            MenuBasicCalculations(console, keyboard);
        }
        else if (inputBuffer[0] == '2' && (inputBuffer[1] == '\0' || inputBuffer[1] == '\n'))
        {
            console.Clear();
            console.SetColor(0x0F, 0x00);
            console.PrintLine("=== Menu 2: Solve Equations ===");
            console.SetColor(0x0F, 0x00);
            console.PrintLine("");
            MenuEquations(console, keyboard);
        }
        else
        {
            console.SetColor(0x0C, 0x00);  // Red
            console.PrintLine("Invalid choice! Please enter 1 or 2.");
            console.SetColor(0x0F, 0x00);
            console.PrintLine("");
            // Wait a bit before showing menu again
            for (volatile int i = 0; i < 10000000; ++i);
        }
    }
}