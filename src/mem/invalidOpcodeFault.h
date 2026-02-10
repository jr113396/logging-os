#ifndef __mem_invalidOpcodeFault_h
#define __mem_invalidOpcodeFault_h
#include <interupts/idt.h>
void invalidOpcodeHandler(struct InterruptRegisters *regs);

#endif
