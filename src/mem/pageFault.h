#ifndef __mem_pageFault_h
#define __mem_pageFault_h

#include <stdint.h>
#include "paging.h"
#include "physicalMemManager.h"

#include "interupts/idt.h"
#include "interupts/interuptRegs.h"
#include <logging/logging.h>

// logging
uint8_t isThereNewData();
uint32_t readLastAddr();
uint32_t getOldData();
logging_data *createLoggingEvent();

// error debugging
void pageDirFlagsBits(uint32_t dir);
void pageTableFlagsBits(uint32_t tab);
void pageFaultErrorBits(uint32_t error);

// actual fault handler
void pageFaultError(struct InterruptRegisters *regs);
void pageFaultHandler(struct InterruptRegisters *regs);
void loggingPagingHandler(struct InterruptRegisters *regs);
void breakpointHandler(struct InterruptRegisters *regs);
#endif
