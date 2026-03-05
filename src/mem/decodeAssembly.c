#include "decodeAssembly.h"
#include <stdio.h>

// *numInstr is a pointer to a memory location storing a number
// if the pointer is null = ignored
// otherwise 
// the nth instruction location is stored in numInstr
// instruction num n is from the value prev in numInstr
void matchOpcode(uint8_t *addr, uint32_t numChars, uint32_t *numInstr) {
    uint8_t numOperands = 0;
    uint8_t found = 0;

    // if numInstr = null
    if (!numInstr)
        found = 1;

    uint32_t numInstructCount = 0;

    for (uint32_t i = 0; i < numChars; i++) {
        uint8_t *inst = addr + i;
        if (numOperands) {
            switch (*inst) {
                case 0x0:
                case 0x1:
                case 0x2:
                case 0x3:
                case 0x4:
                case 0x5:
                case 0x6:
                case 0x7:
                    // see https://en.wikipedia.org/wiki/ModR/M
                    sprintf("SIB");
                    break;
                
                default:
                    sprintf("OP ");
            }
            
            sprintf(" @ %x - %x\r\n", inst, *inst);
            numOperands--;
            continue;
        }

        switch (*inst) {
            case 0x8D:
                sprintf("LEA");
                numOperands = 0;
                break;
            

            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
                // push value to stack
                sprintf("PUSH");
                break;


            case 0xEC:
                sprintf("IN");
                numOperands = 0;
                break;
            
            case 0x83:
                // maths thing for add, or, adc, and, sub, xor, cmp
                sprintf("MAT");

                numOperands = 1;
                break;

            case 0xCC:
                // debug - call int 3
                sprintf("DBG");
                numOperands = 0;
                break;

            case 0xC8:
                // enter
                sprintf("ENT"); 
                numOperands = 2; // 2 operands for enter
                break;
            
            case 0x90:
                sprintf("NOP");
                numOperands = 0;
                break;


            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x80:
            case 0x81:
                sprintf("ADC");
                numOperands = 1;
                break;

            case 0x88:
                sprintf("MOV");
                numOperands = 1;
                break;

            case 0xE6:
            case 0xE7:
            case 0xEE:
            case 0xEF:
                sprintf("OUT");
                numOperands = 2;
                break;

            case 0xC9:
                sprintf("LVE");
                numOperands = 0;
                break;
            
            case 0xC2:
            case 0xC3:
                sprintf("RETN");
                numOperands = 0;
                break;

            default:
                // unkown
                sprintf("UNK");
                break;
        }
        sprintf(" @ %x - %x\r\n", inst, *inst);
        numInstructCount++;
        if (!found && numInstructCount == *numInstr) {
            sprintf("Found the %dth instr at %x\r\n", inst);
            numInstr = (uint32_t *)inst;
            found = 1;
        }
    }
}