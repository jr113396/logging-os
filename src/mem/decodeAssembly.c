#include "decodeAssembly.h"
#include <stdio.h>
#include <loggingLevel.h>

// *numInstr is a pointer to a memory location storing a number
// if the pointer is null = ignored
// otherwise 
// the nth instruction location is stored in numInstr
// instruction num n is from the value prev in numInstr
uint8_t *matchOpcode(uint8_t *addr, uint32_t numChars, uint32_t numInstr) {
    uint8_t numOperands = 0;
    uint8_t prefix = 0;
    uint8_t found = 0;
    uint8_t unknown = 0;

    // if numInstr = null
    if (!numInstr)
        found = 1;

    uint32_t numBytes = 0;
    uint8_t *instLoc = 0;
    uint32_t numInstructCount = 0;

    for (uint32_t i = 0; i < numChars; i++) {
        uint8_t *inst = addr + i;
        unknown = 0;
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
                    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                        sprintf("SIB");
                    #endif
                    break;
                
                default:
                    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                        sprintf("OP ");
                    #endif
            }
            #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                sprintf(" @ %x - %x\r\n", inst, *inst);
            #endif
            numOperands--;
            continue;
        }

        switch (*inst) {
            case 0x8D:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("LEA");
                #endif
                numOperands = 0;
                break;
            


            case 0xF4:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("HLT");
                #endif
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
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("PUSH");
                #endif
                numOperands = 0; // pushes a register
                break;


            case 0x8B:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("MOV");
                #endif
                numOperands = 1;
                break;


            case 0x45:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("INC");
                #endif
                numOperands = 0;
                break;
            

            case 0xE4:
            case 0xE5:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("IN");
                #endif
                numOperands = 1;
                break;
            
            case 0x68:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("PUSH");
                #endif
                numOperands = 4; // pushes a 32 bit value to the stack
                break;

            case 0xE8:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("CALL");
                #endif
                numOperands = 4; // relative call to this location as 32 bit value
                break;

            case 0xF8:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("CLC");
                #endif
                numOperands = 0;
                break;

            case 0xEC:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("IN");
                #endif
                numOperands = 0;
                break;
            
            case 0x83:
                // maths thing for add, or, adc, and, sub, xor, cmp
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("MAT");
                #endif

                numOperands = 1;
                break;

            case 0xCC:
                // debug - call int 3
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("DBG");
                #endif
                numOperands = 0;
                break;

            case 0xC8:
                // enter
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("ENT"); 
                #endif
                numOperands = 2; // 2 operands for enter
                break;
            
            case 0x90:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("NOP");
                #endif
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
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("ADC");
                #endif
                numOperands = 1;
                break;

            case 0x88:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("MOV");
                #endif
                numOperands = 1;
                break;

            case 0xC6:
            
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("MOV");
                #endif
                numOperands = 2;
                break;
            
            case 0xB6:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("MOV");
                #endif
                numOperands = 1;
                break;

            case 0xE6:
            case 0xE7:
            case 0xEE:
            case 0xEF:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                    sprintf("OUT");
                #endif
                numOperands = 2;
                break;

            case 0xC9:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                sprintf("LVE");
                #endif
                numOperands = 0;
                break;
            
            case 0xC2:
            case 0xC3:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                sprintf("RETN");
                #endif
                numOperands = 0;
                break;

            case 0x0F:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                sprintf("PREF"); // prefix to another byte
                #endif
                prefix = 1;
                break;
            
            case 0xF3:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                sprintf("PREF"); // prefix as well
                #endif
                prefix = 1;
                break;

            case 0x08:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                sprintf("OR");
                #endif
                numOperands = 1;
                break;
            case 0x84:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                sprintf("TEST");
                #endif
                numOperands = 1;
                break;

            case 0x75:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                sprintf("JNE");
                #endif
                numOperands = 1;
                break;

            case 0xEB:
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                sprintf("JMP");
                #endif
                numOperands = 1;
                break;
            
            default:
                // unkown
                #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
                sprintf("UNK");
                #endif
                unknown = 1;
                break;
        }
        #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
        sprintf(" @ %x - %x\r\n", inst, *inst);
        #endif
        if (!prefix && !unknown)
            numInstructCount++;
        prefix = 0;
        if (!found && numInstructCount >= numInstr + 1) {
            numBytes = i;
            #if _SERIAL_LOGGING_LEVEL >= _LOGGING_INFO
            sprintf("**Found the %dth instr at %x - %ud bytes**\r\n", numInstr, inst, numBytes);
            #endif
            found = 1;
            instLoc = inst;
        }
    }
    #if _SERIAL_LOGGING_LEVEL >= _LOGGING_DEBUG
    sprintf("Returning %x\r\n", instLoc);
    #endif
    return instLoc;
}