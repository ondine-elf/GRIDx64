.set MAGIC,     0xE85250D6
.set ARCH,      0
.set LENGTH,    (multiboot_header_end - multiboot_header_start)
.set CHECKSUM, -(MAGIC + ARCH + LENGTH)

.section .multiboot
.align 8
multiboot_header_start:
    .long MAGIC
    .long ARCH
    .long LENGTH
    .long CHECKSUM

    // Video mode
    .align 8
    .short 5
    .short 0
    .long  20
    .long  1024
    .long  768
    .long  32

    // EFI amd64 entry address
    .align 8
    .short 9
    .short 0
    .long  12
    .long _start

    // EFI boot services
    .align 8
    .short 7
    .short 0
    .long  8

    .align 8
    .short 0
    .short 0
    .long  0
multiboot_header_end:

/*
.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:
*/

/*
.section .data
gdt_start:
    .quad 0x0000000000000000
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    .word gdt_end - gdt_start - 1
    .long gdt_start
*/

.section .text
.global _start
.extern kernel_main
.type _start, @function
_start:

/*
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
*/

    cld
    mov %eax, %edi
    mov %ebx, %esi
    call  kernel_main

    cli
1:  hlt
    jmp 1b

.size _start, . - _start
