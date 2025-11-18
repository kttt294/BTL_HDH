ENTRY(loader)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    . = 0x0100000;

    .text : ALIGN(4K)
    {
        *(.multiboot)
        *(.text*)
        *(.rodata)
    }

    .data : ALIGN(4K)
    {
        start_ctors = .;
        KEEP(*(.init_array));
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)));
        end_ctors = .;

        *(.data)
    }

    .bss : ALIGN(4K)
    {
        *(.bss)
    }

    /DISCARD/ : 
    {
        *(.fini_array*)
        *(.comment)
    }
}