#include "interrupts.h"

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];
InterruptManager* InterruptManager::ActiveInterruptManager = 0;

InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager)
    : interruptNumber(interruptNumber), interruptManager(interruptManager)
{
    if (interruptManager != 0)
        interruptManager->handlers[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler()
{
    if (interruptManager != 0 && interruptManager->handlers[interruptNumber] == this)
        interruptManager->handlers[interruptNumber] = 0;
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp)
{
    return esp;
}

uint32_t InterruptManager::HandleInterrupt(uint8_t interruptNumber, uint32_t esp)
{
    if (ActiveInterruptManager != 0)
        return ActiveInterruptManager->DoHandleInterrupt(interruptNumber, esp);

    return esp;
}

void InterruptManager::SetInterruptDescriptorTableEntry(
        uint8_t interruptNum,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t descriptorPrivilegeLevel,
        uint8_t descriptorType
)
{
    const uint8_t IDT_DESC_PRESENT = 0x80;

    interruptDescriptorTable[interruptNum].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNum].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNum].gdtCodeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNum].reserved = 0;
    interruptDescriptorTable[interruptNum].access = IDT_DESC_PRESENT | descriptorType | (descriptorPrivilegeLevel << 5);
    
}

InterruptManager::InterruptManager(GlobalDescriptorTable* gdt)
    : picMasterCommand(0x20), 
      picMasterData(0x21),
      picSlaveCommand(0xA0),
      picSlaveData(0xA1)
{
    uint32_t codeSegment = gdt->CodeSegmentDescriptor();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for (uint16_t i = 0; i < 256; ++i)
    {
        handlers[i] = 0;
        SetInterruptDescriptorTableEntry(i, codeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }

    SetInterruptDescriptorTableEntry(0x20, codeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x21, codeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);  

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);
    picMasterData.Write(0x20);
    picSlaveData.Write(0x28);

    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);
    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);
    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);


    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t) interruptDescriptorTable;

    asm volatile("lidt %0" : : "m" (idt));
}

InterruptManager::~InterruptManager()
{
    if (ActiveInterruptManager == this)
        ActiveInterruptManager = 0;
}

void InterruptManager::Activate()
{
    ActiveInterruptManager = this;
    asm("sti");
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp)
{
    if (handlers[interruptNumber] != 0)
        esp = handlers[interruptNumber]->HandleInterrupt(esp);

    if (interruptNumber >= 0x28)
        picSlaveCommand.Write(0x20);
    picMasterCommand.Write(0x20);

    return esp;
}