#include "invalidOpcodeFault.h"
#include <stdio.h>
#include "paging.h"
#include "pageFault.h"

void invalidOpcodeHandler(struct InterruptRegisters *regs) {
    sprintf("Invalid opcode handler -- \r\n");
    sprintf("Old data was: %d\r\n", getOldData()); 
    asm ( "hlt;\n\t" );
} 
