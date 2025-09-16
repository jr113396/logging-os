#include "stdmem.h"

void memset(void *loc, char val, uint32_t count) {
    char *p = (char*dest);
    for (; count > 0; count--) {
        *p++ = val;
    }
}