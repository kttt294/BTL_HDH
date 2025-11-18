#include "types.h"

namespace
{
    static uint8_t heap[8 * 1024];
    static uint32_t heapPointer = 0;
}

void* operator new(uint32_t size)
{
    if (size == 0)
        return 0;

    if (heapPointer + size > sizeof(heap))
        return 0;

    void* address = heap + heapPointer;
    heapPointer += size;
    return address;
}

void* operator new[](uint32_t size)
{
    return ::operator new(size);
}

void operator delete(void* ptr)
{
    (void)ptr;
}

void operator delete(void* ptr, uint32_t)
{
    (void)ptr;
}

void operator delete[](void* ptr)
{
    (void)ptr;
}

void operator delete[](void* ptr, uint32_t)
{
    (void)ptr;
}
