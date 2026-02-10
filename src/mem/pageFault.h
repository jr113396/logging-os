#ifndef __mem_pageFault_h
#define __mem_pageFault_h

#include <stdint.h>
#include "paging.h"
#include "physicalMemManager.h"

#include "interupts/idt.h"
#include "interupts/interuptRegs.h"


// logging
uint8_t isThereNewData();
uint32_t readLastAddr();
uint32_t getOldData();

// error debugging
void pageDirFlagsBits(uint32_t dir);
void pageTableFlagsBits(uint32_t tab);
void pageFaultErrorBits(uint32_t error);

// actual fault handler
void pageFaultError(struct InterruptRegisters *regs);
void pageFaultHandler(struct InterruptRegisters *regs);
void loggingPagingHandler(struct InterruptRegisters *regs);
#endif
