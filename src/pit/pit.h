#ifndef pith
#define pith

#include "interupts/idt.h"
#include "interupts/interuptRegs.h"

// Channel 0
// Causes IRQ0

// R/W ports
//pitChan0 (0x40)
// pitChan1 (0x41)
// pitChan2 (0x42)

// W only
// pitModeReg (0x43)

#define pitFreq 1193180

void pitHandler(struct InterruptRegisters *regs);

void initPit();
void pauseTicks(uint32_t noTicks);
void pauseTimeMs(uint32_t ms) ;

uint64_t getTickCount();
#endif