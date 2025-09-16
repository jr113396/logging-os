#include "io.h"

inline void
outb (uint16_t port, uint8_t n) {
   asm volatile (
                  "outb %0, %1;\n\t"
                :                       // Outputs
                : "a" (n), "Nd" (port)  // Inputs
                );
}

void outPortB(uint16_t port, uint8_t value) {
    // write a byte to a port
}
