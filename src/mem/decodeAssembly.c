#include "decodeAssembly.h"
#include <stdio.h>

void matchOpcode(uint8_t *addr, uint32_t numChars) {
    uint8_t numOperands = 0;

    for (uint32_t i = 0; i < numChars; i++) {
        uint8_t *inst = addr + i;
        if (numOperands) {
            sprintf("OP  @ %x - %x\r\n", inst, *inst);
            numOperands--;
            continue;
        }

        switch (*inst) {
            case 0x8D:
                sprintf("LEA @ %x \r\n", inst);
                numOperands = 0;
                break;
            
            case 0x06:
            case 0x0E:
            case 0x16:
            case 0x1E:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
                // push value to stack
                sprintf("PUSH @ %x \r\n", inst);
                break;


            case 0xEC:
                sprintf("IN  @ %x - DX to AL\r\n", inst);
                numOperands = 0;
                break;
            

            case 0xCC:
                // debug - call int 3
                sprintf("DBG @ %x\r\n", inst);
                numOperands = 0;
                break;

            case 0xC8:
                // enter
                sprintf("ENT @ %x\r\n", inst); 
                numOperands = 2; // 2 operands for enter
                break;


            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x80:
            case 0x81:
                sprintf("ADC @ %x\r\n", inst);
                numOperands = 2;
                break;

            case 0x88:
                sprintf("MOV @ %x\r\n", inst);
                break;

            case 0xE6:
            case 0xE7:
            case 0xEE:
            case 0xEF:
                sprintf("OUT @ %x\r\n", inst);
                numOperands = 2;
                break;

            case 0xC9:
                sprintf("LVE @ %x\r\n", inst);
                numOperands = 0;
                break;
            
            case 0xC2:
            case 0xC3:
                sprintf("RETN @ %x\r\n", inst);
                numOperands = 0;
                break;

            default:
                // unkown
                sprintf("UNK @ %x - %x\r\n", inst, *inst);
                break;
        }
    }
}