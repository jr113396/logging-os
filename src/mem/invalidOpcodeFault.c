#include "invalidOpcodeFault.h"
#include <stdio.h>
#include "paging.h"
#include "pageFault.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
void invalidOpcodeHandler(struct InterruptRegisters *regs) {
    sprintf("Invalid opcode handler -- \r\n");
    sprintf("Old data was: %d\r\n", getOldData()); 
    asm ( "hlt;\n\t" );
} 
