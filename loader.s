.set MAGIC, 0x1badb002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM


.section .text
.extern kernelMain
.extern callConstructors
.global loader

loader:
    cli
    mov $kernel_stack, %esp
    and $0xFFFFFFF0, %esp         # align stack to 16 bytes
    mov %eax, %esi                # preserve Multiboot magic value
    mov %ebx, %edi                # preserve Multiboot info pointer
    call callConstructors
    push %esi                    # push arguments right-to-left (magicNum first)
    push %edi                    # then multibootStruct
    call kernelMain
    add $8, %esp                 # clean up parameters for cdecl

_stop:
    cli
    hlt
    jmp _stop


.section .bss
.align 16
kernel_stack_bottom:
    .space 2*1024*1024

kernel_stack:
    