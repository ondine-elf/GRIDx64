#include <tetos/io.h>
#include <drivers/i8259/i8259.h>

/*
 * Remap PIC interrupts to new vector offsets to avoid conflicts
 * with CPU exceptions (0–31). Offsets must be divisible by 8.
 */
void pic_remap(uint8_t master_offset, uint8_t slave_offset) {
    uint8_t master_imr = inb(MASTER_PIC_DATA);
    uint8_t slave_imr = inb(SLAVE_PIC_DATA);

    outb(MASTER_PIC_COMMAND, ICW1_INIT | ICW1_IC4);
    io_wait();
    outb(SLAVE_PIC_COMMAND, ICW1_INIT | ICW1_IC4);
    io_wait();

    outb(MASTER_PIC_DATA, master_offset);
    io_wait();
    outb(SLAVE_PIC_DATA, slave_offset);
    io_wait();

    outb(MASTER_PIC_DATA, 1 << 2);
    io_wait();
    outb(SLAVE_PIC_DATA, 0x02);
    io_wait();

    outb(MASTER_PIC_DATA, ICW4_8086);
    io_wait();
    outb(SLAVE_PIC_DATA, ICW4_8086);
    io_wait();

    outb(MASTER_PIC_DATA, master_imr);
    io_wait();
    outb(SLAVE_PIC_DATA, slave_imr);
    io_wait();
}

/* Mask given IRQ to disable its interrupt */
void pic_set_mask(uint8_t irq) {
    uint16_t port;
    if (irq < 8)
        port = MASTER_PIC_DATA;
    else {
        port = SLAVE_PIC_DATA;
        irq -= 8;
    }
    outb(port, inb(port) | (1 << irq));
}

/* Clear mask on given IRQ to enable its interrupt */
void pic_clear_mask(uint8_t irq) {
    uint16_t port;
    if (irq < 8)
        port = MASTER_PIC_DATA;
    else {
        port = SLAVE_PIC_DATA;
        irq -= 8;
    }
    outb(port, inb(port) & ~(1 << irq));
}

/* Send end-of-interrupt signal after handling IRQ */
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8)
        outb(SLAVE_PIC_COMMAND, OCW2_EOI);
    outb(MASTER_PIC_COMMAND, OCW2_EOI);
}

/* Mask all IRQs, disabling the PIC */
void pic_disable(void) {
    outb(MASTER_PIC_DATA, 0xFF);
    outb(SLAVE_PIC_DATA, 0xFF);
}

/* Read combined 16-bit ISR from master and slave PICs */
uint16_t pic_get_isr(void) {
    outb(MASTER_PIC_COMMAND, OCW3_ISR);
    uint8_t master_isr = inb(MASTER_PIC_COMMAND);

    outb(SLAVE_PIC_COMMAND, OCW3_ISR);
    uint8_t slave_isr = inb(SLAVE_PIC_COMMAND);

    return (slave_isr << 8) | master_isr;
}

/* Read combined 16-bit IRR from master and slave PICS */
uint16_t pic_get_irr(void) {
    outb(MASTER_PIC_COMMAND, OCW3_IRR);
    uint8_t master_irr = inb(MASTER_PIC_COMMAND);

    outb(SLAVE_PIC_COMMAND, OCW3_IRR);
    uint8_t slave_irr = inb(SLAVE_PIC_COMMAND);

    return (slave_irr << 8) | master_irr;
}