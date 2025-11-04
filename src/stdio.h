#ifndef __stdio_h
#define __stdio_h

#include <stdint.h>
#include <stdarg.h>

void putchar  ( char   ); // from vga.h

void kprintf  ( char *, ... ); // from kprintf.h
void kfprintf(void (*outChar) (char), char *fmt, va_list ap);  // from kprintf
uint32_t getStr(char *addr, uint32_t maxLen); // from keyboard.h
char getChar();
#endif
