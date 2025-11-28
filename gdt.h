#ifndef _GDT_H_
#define _GDT_H_

#include "types.h"

class GlobalDescriptorTable
{
public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    // Lấy độ lệch (offset) của các phân đoạn mã
    uint16_t CodeSegmentDescriptor();
    uint16_t DataSegmentDescriptor();

    class SegmentDescriptor
    {
    public:
        SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t flags);
        uint32_t Base();
        uint32_t Limit();

    private:
        uint16_t limit_low;       // bytes 0-1
        uint16_t base_low;        // bytes 2-3
        uint8_t base_mid;         // byte 4
        uint8_t type;             // byte 5
        uint8_t flags_limit_high; // byte 6
        uint8_t base_high;        // byte 7
    } __attribute__((packed));


    SegmentDescriptor nullSegmentSelector;
    SegmentDescriptor unusedSegmentSelector;
    SegmentDescriptor codeSegmentSelector;
    SegmentDescriptor dataSegmentSelector;
    
};

#endif