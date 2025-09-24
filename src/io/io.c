#include "io.h"

inline void
outb (uint16_t port, uint8_t n) {
   asm volatile (
                  "outb %0, %1;\n\t"
                :                       // Outputs
                : "a" (n), "Nd" (port)  // Inputs
                );
}

inline char inb(uint16_t port) {
  char chr;
  asm volatile (
                  "inb %1, %0;\n\t"
                  : "=a" (chr) 
                  : "Nd" (port)
  );
  return chr;
}