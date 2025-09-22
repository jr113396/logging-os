#ifndef interruptRegsh
#define interruptRegsh
#include "stdint.h"
struct InterruptRegisters {
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t interuptNo, errorCode;
    uint32_t eip, csm, eflags, useresp, ss;
};

#endif