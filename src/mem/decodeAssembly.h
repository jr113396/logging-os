#ifndef __mem_decode_assembly_h
#define __mem_decode_assembly_h

#include <stdint.h>

void matchOpcode(uint8_t *addr, uint32_t numChars, uint32_t *numInstr);
#endif