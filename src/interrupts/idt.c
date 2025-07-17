/*
#include "idt.h"
#include <drivers/console/console.h>
#include <drivers/i8259/i8259.h>

extern void *isr_table[];

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;

void idt_set_gate(uint8_t vector, void *isr, uint8_t flags) {
    idt[vector].offset_low = (uint32_t)isr & 0xFFFF;
    idt[vector].offset_high = ((uint32_t)isr >> 16) & 0xFFFF;
    idt[vector].selector = 0x08;
    idt[vector].attribute = flags;
    idt[vector].zero = 0;
}

void idt_install(void) {
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;

    for (int i = 0; i < 32; i++)
        idt_set_gate(i, isr_table[i], 0x8F);

    for (int i = 32; i < 48; i++)
        idt_set_gate(i, isr_table[i], 0x8E);

    asm volatile ("lidt %0" : : "m" (idtp));
}

void exception_handler(uint32_t vector, uint32_t error_code) {
    if (vector < 32) {
        console_printf("Vector: %d, Error Code: %d\n", vector, error_code);
        asm volatile ("cli; hlt");
    } else if (vector == 32 + 1) {
        console_printf("Key received!\n");
        pic_send_eoi(1);
    }
}
*/
// 8f or 8e?