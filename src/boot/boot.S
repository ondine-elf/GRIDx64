#include <./config.h>

.set ALIGN,   1 << 0
.set MEMINFO, 1 << 1
.set VIDINFO, 1 << 2

.set MAGIC,      0x1BADB002
.set FLAGS,      ALIGN | MEMINFO | VIDINFO
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0

#ifdef GRAPHICS_MODE
    .long 0
    .long 1024
    .long 768
    .long 32
#else
    .long 1
    .long 80
    .long 25
    .long 0
#endif

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .data
gdt_start:
    .quad 0x0000000000000000
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    .word gdt_end - gdt_start - 1
    .long gdt_start

.section .text
.global _start
.extern kernel_main
.type _start, @function
_start:
    mov $stack_top, %esp

    lgdt gdt_descriptor
    ljmp $0x08, $protected_mode

protected_mode:
    mov $0x10, %cx
    mov %cx,   %ds
    mov %cx,   %es
    mov %cx,   %fs
    mov %cx,   %gs
    mov %cx,   %ss

    cld
    pushl %ebx
    pushl %eax
    call  kernel_main
    addl $8, %esp

    cli
1:  hlt
    jmp 1b

.size _start, . - _start
