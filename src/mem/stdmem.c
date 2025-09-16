#include "stdmem.h"

void memset(void *loc, char val, uint32_t count) {
    char *p = (char*) loc;
    for (; count > 0; count--) {
        *p++ = val;
    }
}