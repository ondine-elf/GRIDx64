#ifndef INTERRUPTS_IDT_H
#define INTERRUPTS_IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  attribute;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void idt_set_gate(uint8_t vector, void *isr, uint8_t flags);
void idt_install(void);
void exception_handler(uint32_t vector, uint32_t error_code);

#endif