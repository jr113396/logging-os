#ifndef ioh
#define ioh

#include <stdint.h>


void     outb    ( uint16_t, uint8_t  );
char inb(uint16_t port);
#endif