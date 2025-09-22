#include "pit.h"
#include "io/kprintf.h"
#include "io/vga.h"

#include "stdint.h"
#include "io/io.h"

int noPitsHandled = 0;

void pitHandler(struct InterruptRegisters *regs) {
    kprintf("Pit Handler no %d\n", noPitsHandled++);
}

void initPit() {
    install_irq_handler (0, &pitHandler); // install the pit handler function as irq 0
    uint32_t freq = pitFreq; // in hz
    outb(0x43, 0x36); // set mode to be square wave on channel 0
    outb(0x40, (uint8_t) freq & 0xFF);
    outb(0x40, (uint8_t) (freq >> 8) & 0xFF);

} 
