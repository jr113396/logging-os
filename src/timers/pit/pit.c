#include "pit.h"

#include <io/kprintf.h>
#include <io/vga.h>
#include <stdint.h>
#include <io/io.h>

uint64_t ticksHandled = 0;
uint8_t secForTicks = 10;

#pragma GCC diagnostic ignored "-Wunused-parameter"
void pitHandler(struct InterruptRegisters *regs) {
    // kprintf("Pit Handler no %d\n", ticksHandled++);
    ticksHandled++;
}

uint64_t getTickCount() {
    return ticksHandled;
}

void setPitFreq(uint8_t secFor1000Ticks) {
    secForTicks = secFor1000Ticks;
    uint32_t desiredHz = (1000/secFor1000Ticks);
    uint32_t freq = pitFreq / desiredHz; // in hz
    outb(0x40, (uint8_t) freq & 0xFF);
    outb(0x40, (uint8_t) (freq >> 8) & 0xFF);
}

void initPit() {
    install_irq_handler (0, &pitHandler); // install the pit handler function as irq 0

    outb(0x43, 0x36); // set mode to be square wave on channel 0

    setPitFreq(secForTicks);
} 

void pauseTicks(uint32_t noTicks) {
    uint64_t startTicks = ticksHandled;
    // wait n ticks 
    while (ticksHandled-startTicks < noTicks) {}
    
    // then return
    return;
}
void pauseTimeMs(uint32_t ms) {
    uint32_t desiredHz = (1000/secForTicks); // 100 hz
    uint64_t noTicks = desiredHz * (ms / 1000); 
    // desiredHz
    uint64_t startTicks = ticksHandled;
    // wait n ticks 
    while (ticksHandled-startTicks < noTicks) {}
    
    // then return
    return;
}


