#ifndef __stdio_h
#define __stdio_h

#include <stdint.h>

void putchar  ( char   ); // from vga.h

void kprintf  ( char *, ... ); // from kprintf.h

uint32_t getStr(char *addr, uint32_t maxLen); // from keyboard.h
char getChar();
#endif