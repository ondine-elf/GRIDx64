#ifndef DRIVERS_I8259_I8259_H
#define DRIVERS_I8259_I8259_H

#include <stdint.h>

#define MASTER_PIC_COMMAND 0x20 /* Command register for master PIC        */
#define MASTER_PIC_DATA    0x21 /* Data register (ICW2–4, IMR) for master */
#define SLAVE_PIC_COMMAND  0xA0 /* Command register for slave PIC         */
#define SLAVE_PIC_DATA     0xA1 /* Data register (ICW2–4, IMR) for slave  */

#define ICW1_IC4  (1 << 0) /* Set if ICW4 will be sent (required on x86)        */
#define ICW1_SNGL (1 << 1) /* Set if single PIC (clear for master/slave setup)  */
#define ICW1_ADI  (1 << 2) /* Address interval: ignored on x86 (8085 mode only) */
#define ICW1_LTIM (1 << 3) /* Set for level-triggered mode (usually unset)      */
#define ICW1_INIT (1 << 4) /* Must be set to begin initialization sequence      */

#define ICW4_8086       (1 << 0) /* 1 = 8086/88 mode (x86), 0 = MCS-80/85 mode        */
#define ICW4_AUTO_EOI   (1 << 1) /* 1 = Auto EOI (no explicit EOI needed), 0 = normal */
#define ICW4_BUF_SLAVE  (1 << 2) /* Buffered mode/slave (only if BUF_MODE is set)     */
#define ICW4_BUF_MASTER (1 << 3) /* Buffered mode/master (only if BUF_MODE is set)    */
#define ICW4_SFNM       (1 << 4) /* 1 = Special Fully Nested Mode, 0 = normal         */

#define OCW2_EOI 0x20 /* End-of-Interrupt (EOI) command for PIC */

#define OCW3_IRR 0x0A /* Read IRR command for PIC */
#define OCW3_ISR 0x0B /* Read ISR command for PIC*/

void pic_remap(uint8_t master_offset, uint8_t slave_offset);
void pic_set_mask(uint8_t irq);
void pic_clear_mask(uint8_t irq);
void pic_send_eoi(uint8_t irq);
void pic_disable(void);
uint16_t pic_get_isr(void);
uint16_t pic_get_irr(void);

#endif