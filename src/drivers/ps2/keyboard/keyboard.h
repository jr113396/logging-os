#ifndef ps2keyboarddriverh
#define ps2keyboarddriverh

#include "interupts/interuptRegs.h"

void keyboardHandler(struct InterruptRegisters *regs);
void initKeyboard();

char getChar();

int isKeyPressed(uint8_t c);

uint32_t getStr(char *addr, uint32_t maxLen);
#endif